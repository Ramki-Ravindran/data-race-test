/* Relite
 * Copyright (c) 2011, Google Inc.
 * All rights reserved.
 * Author: Dmitry Vyukov (dvyukov@google.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef RELITE_RT_H_INCLUDED
#define RELITE_RT_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include "relite_pthread.h"
#include "relite_stdlib.h"

void    relite_enter                  (void const volatile*);
void    relite_leave                  ();
void    relite_store                  (void const volatile*);
void    relite_load                   (void const volatile*);


struct relite_call_desc_t {
  char const*                   func;
  char const*                   file;
  int                           line;
  int                           pos;
};

struct relite_mop_desc_t {
  char const*                   var;
  char const*                   file;
  int                           line;
  int                           pos;
};

struct relite_func_desc_t {
  int                               call_count;
  struct relite_call_desc_t const*  calls;
  int                               mop_count;
  struct relite_mop_desc_t const*   mops;
};

static struct relite_call_desc_t relite_calls [] = {};
static struct relite_mop_desc_t relite_mops [] = {};
static struct relite_func_desc_t relite_func_desc = {};


#ifdef __cplusplus
}
#endif
#endif
