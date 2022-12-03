_VERSION = 0.4-rc1
VERSION  = `git describe --long --tags --dirty 2>/dev/null || echo $(_VERSION)`

PKG_CONFIG = pkg-config

# paths
PREFIX = /usr/local
MANDIR = $(PREFIX)/share/man

# Default compile flags (overridable by environment)
CFLAGS ?= -g -Wall -Wextra -Werror -Wno-unused-parameter -Wno-sign-compare -Wno-unused-function -Wno-unused-variable -Wno-unused-result -Wdeclaration-after-statement

XWAYLAND =
XLIBS =
# Uncomment to build XWayland support
#XWAYLAND = -DXWAYLAND
#XLIBS = xcb xcb-icccm
