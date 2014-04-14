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

/*!
 * Determine whether or not libsystrace should trace a given \a module &
 * \a tracepoint.
 *
 * Returns 1 if the parameters provided should be traced, 0 otherwise.
 */
extern int systrace_should_trace(const char *module, const char *tracepoint);

#ifdef __cplusplus
}
#endif

// begin and end (must nest)
// B|pid|message
// E
//
// async range:
// S|pid|msg|cookie
// F|pid|msg|cookie

/*! \internal
 */
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

/*!
 * Record the start of a duration event in a given \a module and \a tracepoint.
 *
 * Optionally use \a message to provide more context about the event.
 *
 * Duration events *must* be strictly nested (such as in a call stack, for
 * example). If you wish to trace events that cannot be nested, see
 * SYSTRACE_ASYNC_BEGIN.
 *
 * You must call SYSTRACE_END with the same parameters to end the event.
 */
#define SYSTRACE_BEGIN(module, tracepoint, message) SYSTRACE_RECORD(module, 'B', tracepoint, message)

/*!
 * Record the end of a duration event in a given \a module and \a tracepoint.
 *
 * Optionally use \a message to provide more context about the event.
 *
 * Duration events *must* be strictly nested (such as in a call stack, for
 * example). If you wish to trace events that cannot be nested, see
 * SYSTRACE_ASYNC_BEGIN.
 *
 * SYSTRACE_END must have been preceeded by a SYSTRACE_BEGIN call
 * with the same parameters.
 */
#define SYSTRACE_END(module, tracepoint, message) SYSTRACE_RECORD(module, 'E', tracepoint, message)

/*!
 * Record a counter event for the given \a module and \a tracepoint.
 *
 * A counter is a method of recording the count of resources over execution. For
 * instance, one might want to count the number of available free buffers when
 * rendering graphics.
 *
 * In this particular case, \a tracepoint is most likely most useful to
 * represent a variable rather than a code location, and 'message' should refer
 * to a string buffer containing the number of items.
 */
#define SYSTRACE_COUNTER(module, tracepoint, message) SYSTRACE_RECORD(module, 'C', tracepoint, message)

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

/*!
 * Record the start of an asynchronous event for the given \a module and \a tracepoint.
 *
 * An asynchronous event is an occurrence that is not necessarily nested like
 * duration events, e.g. an ongoing network transfer.
 *
 * \a cookie is a pointer representing a unique instance for this
 * asynchronous event.
 *
 * When the event completes, SYSTRACE_ASYNC_END must be invoked with the same
 * parameters.
 */
#define SYSTRACE_ASYNC_BEGIN(module, tracepoint, cookie, message) SYSTRACE_RECORD_ASYNC(module, 'S', tracepoint, cookie, message)

/*!
 * Record the end of an asynchronous event for the given \a module and \a tracepoint.
 *
 * An asynchronous event is an occurrence that is not necessarily nested like
 * duration events, e.g. an ongoing network transfer.
 *
 * \a cookie is a pointer representing a unique instance for this
 * asynchronous event.
 *
 * SYSTRACE_ASYNC_END must have been preceeded at some point by a SYSTRACE_ASYNC_BEGIN call
 * with the same parameters.
 */
#define SYSTRACE_ASYNC_END(module, tracepoint, cookie, message) SYSTRACE_RECORD_ASYNC(module, 'F', tracepoint, cookie, message)

#endif // SYSTRACE_H
