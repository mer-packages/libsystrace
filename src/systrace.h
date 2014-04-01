/*
 * Copyright (C) 2014 Jolla Ltd. <robin.burchell@jollamobile.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
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
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#ifndef SYSTRACE_H

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern pthread_mutex_t systrace_trace_mutex;
extern FILE *systrace_trace_target;
extern int systrace_should_trace(const char *module, const char *tracepoint);

#ifdef __cplusplus
}
#endif

#define SYSTRACE_RECORD(module, what, tracepoint, message) do { \
          if (systrace_should_trace(module, tracepoint)) { \
              pthread_mutex_lock(&systrace_trace_mutex); \
              if (what == 'B') \
                 fprintf(systrace_trace_target, "B|%i|%s::%s%s", \
                    getpid(), tracepoint, module, message); \
              else if (what == 'E') \
                 fprintf(systrace_trace_target, "E"); \
              else \
                 fprintf(systrace_trace_target, "C|%i|%s::%s-%i|%s", \
                    getpid(), tracepoint, module, getpid(), message); \
              fflush(systrace_trace_target); \
              pthread_mutex_unlock(&systrace_trace_mutex); \
          } \
      } while(0)
#define SYSTRACE_BEGIN(module, tracepoint, message) SYSTRACE_RECORD(module, 'B', tracepoint, message)
#define SYSTRACE_END(module, tracepoint, message) SYSTRACE_RECORD(module, 'E', tracepoint, message)
#define SYSTRACE_COUNTER(module, tracepoint, message) SYSTRACE_RECORD(module, 'C', tracepoint, message)

#endif // SYSTRACE_H
