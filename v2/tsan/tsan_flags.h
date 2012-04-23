//===-- tsan_flags.h --------------------------------------------*- C++ -*-===//
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

#ifndef TSAN_FLAGS_H
#define TSAN_FLAGS_H

#include "tsan_defs.h"

namespace __tsan {

struct Flags {
  // Enable dynamic annotations, otherwise they are no-ops.
  bool enable_annotations;
  // Supress a race reports if we've already output another race report
  // with the same stacks.
  bool suppress_equal_stacks;
  // Supress a race reports if we've already output another race report
  // on the same address.
  bool suppress_equal_addresses;
  // Report thread leaks at exit?
  bool report_thread_leaks;
  // Report violations of async signal-safety
  // (e.g. malloc() call from a signal handler).
  bool report_signal_unsafe;
  // If set, all atomics are effectively sequentially consistent (seq_sct)
  // regardless of what a user actually specified.
  bool force_seq_cst_atomics;
  // Strip that prefix from file paths in reports.
  const char *strip_path_prefix;
  // Suppressions filename.
  const char *suppressions;
  // Override exit status if something was reported.
  int exit_status;
  // Log fileno (1 - stdout, 2 - stderr).
  int log_fileno;
  // Sleep in main thread before exiting for that many ms
  // (useful to catch "at exit" races).
  int atexit_sleep_ms;
};

Flags *flags();
void InitializeFlags(Flags *flags, const char *env);
}

#endif  // #ifndef TSAN_FLAGS_H