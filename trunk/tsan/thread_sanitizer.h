/*
  This file is part of ThreadSanitizer, a dynamic data race detector.

  Copyright (C) 2008-2009 Google Inc
     opensource@google.com

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307, USA.

  The GNU General Public License is contained in the file COPYING.
*/

// Author: Konstantin Serebryany.
//--------- Head ------------------- {{{1
#ifndef __THREAD_SANITIZER_H__
#define __THREAD_SANITIZER_H__

#include "ts_util.h"

//--------- Utils ------------------- {{{1
#include "ts_util.h"

void Report(const char *format, ...);
void PcToStrings(uintptr_t pc, bool demangle,
                string *img_name, string *rtn_name,
                string *file_name, int *line_no);
string PcToRtnNameAndFilePos(uintptr_t pc);
string PcToRtnName(uintptr_t pc, bool demangle);
string Demangle(const char *str);


//--------- FLAGS ---------------------------------- {{{1
struct FLAGS {
  bool             ignore_stack;
  intptr_t         verbosity;
  bool             show_stats;
  bool             show_expected_races;
  uintptr_t        trace_addr;
  uintptr_t        segment_set_recycle_queue_size;
  vector<string>   file_prefix_to_cut;
  vector<string>   ignore;
  vector<string>   cut_stack_below;
  string           summary_file;
  string           log_file;
  intptr_t         max_n_threads;
  bool             compress_cache_lines;
  bool             unlock_on_mutex_destroy;

  intptr_t         sample_events;
  intptr_t         sample_events_depth;

  intptr_t         num_callers;

  intptr_t    keep_history;
  bool        fast_mode;
  bool        pure_happens_before;
  bool        ignore_in_dtor;
  bool        exit_after_main;
  bool        demangle;
  bool        announce_threads;
  bool        full_output;
  bool        show_states;
  bool        show_proc_self_status;
  bool        show_valgrind_context;  // debug-only
  bool        suggest_happens_before_arcs;
  bool        show_pc;
  bool        color;  // Colorify terminal output.
  bool        html;  // Output in html format.

  intptr_t  debug_level;
  intptr_t  trace_level;

  intptr_t     dry_run;
  intptr_t     max_sid;
  intptr_t     max_mem_in_mb;
  intptr_t     num_callers_in_history;

  intptr_t     literace_sampling;

  bool         report_races;
  bool         thread_coverage;
  bool         dump_events;
  bool         symbolize;
  bool         attach_mode;

  string       offline_syntax;

  string       tsan_program_name;
  string       tsan_url;
};

extern FLAGS *G_flags;

//--------- TS Exports ----------------- {{{1
enum EventType {
  NOOP,
  READ,
  WRITE,
  LOCK_BEFORE,
  READER_LOCK,
  WRITER_LOCK,
  UNLOCK,
  UNLOCK_OR_INIT,
  LOCK_CREATE,
  LOCK_DESTROY,
  BUS_LOCK_ACQUIRE,
  BUS_LOCK_RELEASE,
  THR_SET_PTID,
  THR_START,
  THR_FIRST_INSN,
  THR_END,
  THR_JOIN_BEFORE,
  THR_JOIN_AFTER,
  RTN_EXIT,
  RTN_CALL,
  SBLOCK_ENTER,
  BBLOCK_ENTER,
  SIGNAL,
  WAIT_BEFORE,
  WAIT_AFTER,
  TWAIT_AFTER,
  BARRIER_BEFORE,
  BARRIER_AFTER,
  PCQ_CREATE,
  PCQ_DESTROY,
  PCQ_PUT,
  PCQ_GET,
  SP_CHANGE,
  STACK_MEM_NEW,
  STACK_MEM_DIE,
  MALLOC,
  FREE,
  PUBLISH_RANGE,
  UNPUBLISH_RANGE,
  HB_LOCK,
  IGNORE_READS_BEG,
  IGNORE_READS_END,
  IGNORE_WRITES_BEG,
  IGNORE_WRITES_END,
  SET_THREAD_NAME,
  SET_LOCK_NAME,
  TRACE_MEM,
  EXPECT_RACE,
  VERBOSITY,
  STACK_TRACE,
  LAST_EVENT
};


static const char *kEventNames[] = {
  "NOOP",
  "READ",
  "WRITE",
  "LOCK_BEFORE",
  "READER_LOCK",
  "WRITER_LOCK",
  "UNLOCK",
  "UNLOCK_OR_INIT",
  "LOCK_CREATE",
  "LOCK_DESTROY",
  "BUS_LOCK_ACQUIRE",
  "BUS_LOCK_RELEASE",
  "THR_SET_PTID",
  "THR_START",
  "THR_FIRST_INSN",
  "THR_END",
  "THR_JOIN_BEFORE",
  "THR_JOIN_AFTER",
  "RTN_EXIT",
  "RTN_CALL",
  "SBLOCK_ENTER",
  "BBLOCK_ENTER",
  "SIGNAL",
  "WAIT_BEFORE",
  "WAIT_AFTER",
  "TWAIT_AFTER",
  "BARRIER_BEFORE",
  "BARRIER_AFTER",
  "PCQ_CREATE",
  "PCQ_DESTROY",
  "PCQ_PUT",
  "PCQ_GET",
  "SP_CHANGE",
  "STACK_MEM_NEW",
  "STACK_MEM_DIE",
  "MALLOC",
  "FREE",
  "PUBLISH_RANGE",
  "UNPUBLISH_RANGE",
  "HB_LOCK",
  "IGNORE_READS_BEG",
  "IGNORE_READS_END",
  "IGNORE_WRITES_BEG",
  "IGNORE_WRITES_END",
  "SET_THREAD_NAME",
  "SET_LOCK_NAME",
  "TRACE_MEM",
  "EXPECT_RACE",
  "VERBOSITY",
  "STACK_TRACE",
  "LAST_EVENT"
};


class Event;

struct ThreadSanitizerReport {
  // Types of reports.
  enum ReportType {
    DATA_RACE,
    UNLOCK_FOREIGN,
    UNLOCK_NONLOCKED,
    INVALID_LOCK
  };

  ReportType type;

  virtual ~ThreadSanitizerReport() { }
};

extern void ThreadSanitizerInit();
extern void ThreadSanitizerFini();
extern void ThreadSanitizerHandleOneEvent(Event *event);
extern void ThreadSanitizerHandleMemoryAccess(int32_t tid,
                                              uintptr_t addr, uintptr_t size,
                                              bool is_w);
extern void ThreadSanitizerHandleStackMemChange(int32_t tid, uintptr_t addr,
                                                uintptr_t size, bool is_new);
extern void ThreadSanitizerParseFlags(vector<string>* args);
extern bool ThreadSanitizerWantToInstrumentSblock(uintptr_t pc);
extern bool ThreadSanitizerWantToCreateSegmentsOnSblockEntry(uintptr_t pc);
extern bool ThreadSanitizerIgnoreAccessesBelowFunction(uintptr_t pc);

extern void ThreadSanitizerEnterSblock(int32_t tid, uintptr_t pc);
extern void ThreadSanitizerHandleRtnCall(int32_t tid, uintptr_t call_pc,
                                         uintptr_t target_pc);
extern void ThreadSanitizerHandleRtnExit(int32_t tid);
extern void ThreadSanitizerPrintReport(ThreadSanitizerReport *report);
extern void ThreadSanitizerPrintUsage();

extern bool g_so_far_only_one_thread;
extern bool g_has_entered_main;
extern bool g_has_exited_main;

//--------- Event ------------------------- {{{1
class Event {
 public:
  Event(EventType type, int32_t tid, uintptr_t pc, uintptr_t a, uintptr_t info)
      : type_(type),
      tid_(tid),
      pc_(pc),
      a_(a),
      info_(info) {
      }
  Event() {}  // Not initialized.

  void Init(EventType type, int32_t tid, uintptr_t pc, uintptr_t a, uintptr_t info) {
    type_ = type;
    tid_  = tid;
    pc_   = pc;
    a_    = a;
    info_ = info;
  }


  EventType type()  const { return type_; }
  int32_t   tid()   const { return tid_; }
  uintptr_t a()     const { return a_; }
  uintptr_t pc()    const { return pc_; }
  uintptr_t info()  const { return info_; }
  void      Print() const {
    Printf("T%d: %s [pc=%p; a=%p; i=%p]\n",
           tid(), TypeString(type()), pc(), a(), info());

  }
  static const char *TypeString(EventType type) {
    return kEventNames[type];
  }
 private:
  EventType      type_;
  int32_t   tid_;
  uintptr_t pc_;
  uintptr_t a_;
  uintptr_t info_;
};

// end. {{{1
#endif  //  __THREAD_SANITIZER_H__
// vim:shiftwidth=2:softtabstop=2:expandtab
