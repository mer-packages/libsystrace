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

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "systrace.h"

static pthread_mutex_t systrace_trace_mutex;
static FILE *systrace_trace_target;

__attribute__((constructor)) void systrace_init()
{
    pthread_mutex_init(&systrace_trace_mutex, NULL);
    systrace_trace_target = fopen("/sys/kernel/debug/tracing/trace_marker", "a");
    if (!systrace_trace_target)
        perror("can't open /sys/kernel/debug/tracing/trace_marker");
}

__attribute__((destructor)) void systrace_deinit() {
    pthread_mutex_destroy(&systrace_trace_mutex);
    if (systrace_trace_target)
        fclose(systrace_trace_target);
}

static char systrace_got_areas = 0;
static char *systrace_areas = NULL;

int systrace_should_trace(const char *module, const char *tracepoint)
{
    (void)tracepoint; /* not used at present */
    if (!systrace_got_areas) {
        systrace_got_areas = 1;
        systrace_areas = getenv("SYSTRACE_AREA");
    }

    if (!systrace_areas || !module) {
        /* no areas enabled */
        return 0;
    }

    /* TODO: caching of areas would prevent constant rescanning, if we care. */
    return strstr(systrace_areas, module) != NULL;
}

// begin and end (must nest)
// B|pid|message
// E
//
// async range:
// S|pid|msg|cookie
// F|pid|msg|cookie

/*! \internal
 */
#define SYSTRACE_RECORD(module, what, tracepoint, buffer) do { \
          if (systrace_should_trace(module, tracepoint)) { \
              pthread_mutex_lock(&systrace_trace_mutex); \
              if (what == 'B') \
                 fprintf(systrace_trace_target, "B|%i|%s::%s%s", \
                    getpid(), tracepoint, module, buffer); \
              else if (what == 'E') \
                 fprintf(systrace_trace_target, "E"); \
              else \
                 fprintf(systrace_trace_target, "C|%i|%s::%s-%i|%s", \
                    getpid(), tracepoint, module, getpid(), buffer); \
              fflush(systrace_trace_target); \
              pthread_mutex_unlock(&systrace_trace_mutex); \
          } \
      } while(0)

void systrace_duration_begin(const char *module, const char *tracepoint, const char *fmt, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    SYSTRACE_RECORD(module, 'B', tracepoint, buffer);
}

void systrace_duration_end(const char *module, const char *tracepoint, const char *fmt, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    SYSTRACE_RECORD(module, 'E', tracepoint, buffer);
}

void systrace_record_counter(const char *module, const char *tracepoint, const char *fmt, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    SYSTRACE_RECORD(module, 'C', tracepoint, buffer);
}

/*! \internal
 */
#define SYSTRACE_RECORD_ASYNC(module, what, tracepoint, cookie, message) do { \
          if (systrace_should_trace(module, tracepoint)) { \
              pthread_mutex_lock(&systrace_trace_mutex); \
              if (what == 'S') \
                 fprintf(systrace_trace_target, "S|%i|%s::%s%s|%p", \
                    getpid(), tracepoint, module, message, cookie); \
              else if (what == 'F') \
                 fprintf(systrace_trace_target, "F|%i|%s::%s%s|%p", \
                    getpid(), tracepoint, module, message, cookie); \
              fflush(systrace_trace_target); \
              pthread_mutex_unlock(&systrace_trace_mutex); \
          } \
      } while(0)

void systrace_async_begin(const char *module, const char *tracepoint, void *cookie, const char *fmt, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    SYSTRACE_RECORD_ASYNC(module, 'S', tracepoint, cookie, buffer);
}

void systrace_async_end(const char *module, const char *tracepoint, void *cookie, const char *fmt, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    SYSTRACE_RECORD_ASYNC(module, 'F', tracepoint, cookie, buffer);
}

