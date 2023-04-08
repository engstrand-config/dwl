#ifdef DEVELOP
#define BINARY_PATH PREFIX "/bin/dwl-guile-devel"
#define REPL_SOCKET_PATH "/tmp/dwl-guile-devel.socket"
#define WAYLAND_SOCKET_PATH "wayland-dwl-guile-devel"
#else
#define BINARY_PATH PREFIX "/bin/dwl-guile"
#define REPL_SOCKET_PATH "/tmp/dwl-guile.socket"
#define WAYLAND_SOCKET_PATH NULL
#endif
