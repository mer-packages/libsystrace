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

#include "systrace.h"

pthread_mutex_t systrace_trace_mutex;
FILE *systrace_trace_target;

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

