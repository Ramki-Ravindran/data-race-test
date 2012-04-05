//===-- tsan_slab.cc --------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a part of ThreadSanitizer (TSan), a race detector.
//
//===----------------------------------------------------------------------===//
#include "tsan_slab.h"

#include "tsan_platform.h"
#include "tsan_rtl.h"

namespace __tsan {

uptr const kBatch = 64;
uptr const kAllocSize = 1024*1024;

SlabAlloc::SlabAlloc(uptr size)
  : mtx_(MutexTypeSlab, StatMtxSlab)
  , size_(size >= sizeof(head_) ? size : sizeof(head_))
  , count_()
  , allocated_()
  , head_()
  , superblocks_() {
  CHECK_EQ(size_ % sizeof(head_), 0);
  CHECK_LE(size_, kAllocSize / kBatch);
}

SlabAlloc::~SlabAlloc() {
  CHECK_EQ(allocated_, 0);
  // Return all superblocks back to system.
  while (superblocks_) {
    void** tmp = superblocks_;
    superblocks_ = (void**)*tmp;
    virtual_free(tmp, kAllocSize);
  }
}

void* SlabAlloc::Alloc(uptr *n) {
  Lock l(&mtx_);
  // If we are out of block, allocate a superblock from system.
  if (count_ == 0) {
    CHECK_EQ(head_, 0);
    char *mem = (char*)virtual_alloc(kAllocSize);
    char *end = mem + kAllocSize;
    // Take one block to maintain superblock list.
    *(void**)mem = superblocks_;
    superblocks_ = (void**)mem;
     // Split it into blocks.
    for (char* p = mem + size_; p + size_ <= end; p += size_) {
      *(void**)p = head_;
      head_ = (void**)p;
      count_++;
    }
  }

  // If we have very few blocks, give them all away.
  if (count_ <= kBatch) {
    *n += count_;
    allocated_  += count_;
    count_ = 0;
    void **p = head_;
    head_ = 0;
    return p;
  }

  // Count off kBatch blocks and return them.
  void **p = head_;
  for (uptr i = 0; i < kBatch-1 && p; i++)
    p = (void**)*p;
  void **next = (void**)*p;
  *p = 0;
  p = head_;
  head_ = next;
  *n += kBatch;
  count_ -= kBatch;
  allocated_ += kBatch;
  return p;
}

void SlabAlloc::Free(void *first, void *last, uptr n) {
  Lock l(&mtx_);
  CHECK_LE(n, allocated_);
  count_ += n;
  allocated_ -= n;
  *(void**)last = head_;
  head_ = (void**)first;
}

uptr SlabAlloc::Size() const {
  return size_;
}

SlabCache::SlabCache(SlabAlloc* parent)
  : parent_(parent)
  , head_()
  , count_() {
}

SlabCache::~SlabCache() {
  if (count_) {
    void **last = head_;
    while (*last)
      last = (void**)*last;
    parent_->Free(head_, last, count_);
  }
  head_ = (void**)0xdeadbeef;
}

void* SlabCache::Alloc() {
  if (LIKELY(count_)) {
    count_--;
    void **p = head_;
    head_ = (void**)*p;
    return p;
  }
  return AllocSlow();
}

void* NOINLINE SlabCache::AllocSlow() {
  CHECK_EQ(count_, 0);
  CHECK_EQ(head_, 0);
  head_ = (void**)parent_->Alloc(&count_);
  CHECK_NE(count_, 0);
  CHECK_NE(head_, 0);
  return Alloc();
}

void SlabCache::Free(void* p) {
  *(void**)p = head_;
  head_ = (void**)p;
  count_++;
  if (count_ == 2*kBatch)
    Drain();
}

void NOINLINE SlabCache::Drain() {
  CHECK_EQ(count_, 2*kBatch);
  CHECK_NE(head_, 0);
  void **p = head_;
  for (uptr i = 0; i < kBatch-1; i++)
    p = (void**)*p;
  void **next = (void**)*p;
  parent_->Free(head_, p, kBatch);
  head_ = next;
  count_ -= kBatch;
}

uptr SlabCache::Size() const {
  return parent_->Size();
}

RegionAlloc::RegionAlloc(void *mem, uptr size)
  : mem_((char*)mem)
  , end_((char*)mem + size) {
  CHECK_NE(mem, 0);
  CHECK_GT(size, 0);
}

void *RegionAlloc::Alloc(uptr size) {
  char *p = mem_;
  if (p + size > end_)
    return 0;
  mem_ += size;
  return p;
}

}  // namespace __tsan
