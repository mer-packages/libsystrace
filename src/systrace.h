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
#define SYSTRACE_H

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 * Determine whether or not libsystrace should trace a given \a module &
 * \a tracepoint.
 *
 * Returns 1 if the parameters provided should be traced, 0 otherwise.
 */
extern int systrace_should_trace(const char *module, const char *tracepoint);

/*!
 * Record the start of a duration event in a given \a module and \a tracepoint.
 *
 * Optionally use \a fmt to provide more context about the event.
 *
 * Duration events *must* be strictly nested (such as in a call stack, for
 * example). If you wish to trace events that cannot be nested, see
 * systrace_async_begin.
 *
 * You must call systrace_duration_end with the same parameters to end the event.
 */
extern void systrace_duration_begin(const char *module, const char *tracepoint, const char *fmt, ...);

/*!
 * Record the end of a duration event in a given \a module and \a tracepoint.
 *
 * Optionally use \a fmt to provide more context about the event.
 *
 * Duration events *must* be strictly nested (such as in a call stack, for
 * example). If you wish to trace events that cannot be nested, see
 * systrace_async_begin.
 *
 * systrace_duration_end must have been preceeded by a systrace_duration_begin call
 * with the same parameters.
 */
extern void systrace_duration_end(const char *module, const char *tracepoint, const char *fmt, ...);

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
extern void systrace_record_counter(const char *module, const char *tracepoint, const char *fmt, ...);

/*!
 * Record the start of an asynchronous event for the given \a module and \a tracepoint.
 *
 * An asynchronous event is an occurrence that is not necessarily nested like
 * duration events, e.g. an ongoing network transfer.
 *
 * \a cookie is a pointer representing a unique instance for this
 * asynchronous event.
 *
 * When the event completes, systrace_async_end must be invoked with the same
 * parameters.
 */
extern void systrace_async_begin(const char *module, const char *tracepoint, void *cookie, const char *fmt, ...);

/*!
 * Record the end of an asynchronous event for the given \a module and \a tracepoint.
 *
 * An asynchronous event is an occurrence that is not necessarily nested like
 * duration events, e.g. an ongoing network transfer.
 *
 * \a cookie is a pointer representing a unique instance for this
 * asynchronous event.
 *
 * systrace_async_end must have been preceeded at some point by a
 * systrace_async_begin call with the same parameters.
 */
extern void systrace_async_end(const char *module, const char *tracepoint, void *cookie, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // SYSTRACE_H
