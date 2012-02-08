//===-- tsan_test_util.h ----------------------------------------*- C++ -*-===//
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
// Test utils.
//===----------------------------------------------------------------------===//

// A location of memory on which a race may be detected.
class MemLoc {
 public:
  explicit MemLoc(int offset_from_aligned = 0);
  ~MemLoc();
  void *loc() const { return loc_; }
 private:
  void *loc_;
};

// A thread is started in CTOR and joined in DTOR.
// The events (like Read, Write, etc) are passed to ScopedThread via a global
// queue, so that the order of the events observed by different threads is
// fixed.
class ScopedThread {
 public:
  ScopedThread();
  ~ScopedThread();
  void Access(const MemLoc &ml, bool is_write, int size, bool expect_race);
  void Read(const MemLoc &ml, int size, bool expect_race = false) {
    Access(ml, false, size, expect_race);
  }
  void Write(const MemLoc &ml, int size, bool expect_race = false) {
    Access(ml, true, size, expect_race);
  }
  void Read1(const MemLoc &ml, bool expect_race = false) {
    Read(ml, 1, expect_race); }
  void Read2(const MemLoc &ml, bool expect_race = false) {
    Read(ml, 2, expect_race); }
  void Read4(const MemLoc &ml, bool expect_race = false) {
    Read(ml, 4, expect_race); }
  void Read8(const MemLoc &ml, bool expect_race = false) {
    Read(ml, 8, expect_race); }
  void Write1(const MemLoc &ml, bool expect_race = false) {
    Write(ml, 1, expect_race); }
  void Write2(const MemLoc &ml, bool expect_race = false) {
    Write(ml, 2, expect_race); }
  void Write4(const MemLoc &ml, bool expect_race = false) {
    Write(ml, 4, expect_race); }
  void Write8(const MemLoc &ml, bool expect_race = false) {
    Write(ml, 8, expect_race); }
 private:
  struct Impl *impl_;
  ScopedThread(const ScopedThread&);  // Not implemented.
  void operator = (const ScopedThread&);  // Not implemented.
};