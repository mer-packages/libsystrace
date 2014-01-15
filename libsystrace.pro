VERSION = 0.0.0

TEMPLATE = lib
TARGET = systrace
CONFIG -= qt

SOURCES = \
    src/systrace.c

HEADERS = \
    src/systrace.h

headers.path = $$[QT_INSTALL_PREFIX]/include
headers.files += $$HEADERS
INSTALLS += headers

target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
