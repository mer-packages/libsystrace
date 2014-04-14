# libsystrace

libsystrace is a light-weight C library for writing data to Linux's trace_marker
in the format used by the Android systrace framework.

See also:
* [kernel.org ftrace documentation](https://www.kernel.org/doc/Documentation/trace/ftrace.txt)
* [Android documentation about systrace](https://developer.android.com/tools/help/systrace.html)

## General Terminology

A "module" is a collective group of trace events. Currently valid modules are:
  - io
  - graphics
  - dbus
  - sql
  - touch

Other modules may be added at will.

A "tracepoint" is an instantly recognisable string describing the origin of a
trace event inside a module. A common good choice might be a method name (e.g.
QImage::fill).

## Use

Traces are created using [the Android atrace tool](https://github.com/mer-tools/atrace).

Traces are then turned into human-readable output using
[the 'chromium-viewer' tool from Android](https://github.com/mer-tools/chromium-trace)

and the HTML output may be subsequently viewed using Chromium.

One can impact which modules are logged based on setting the SYSTRACE_AREA
environment variable. It is expected that this variable be set to a comma
seperated list of modules to trace, e.g:

    SYSTRACE_AREA=io,touch

would enable tracing for the io & touch modules.

