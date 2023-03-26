#ifdef DEVELOP
#define REPL_SOCKET_PATH "/tmp/dwl-guile-devel.socket"
#define WAYLAND_SOCKET_PATH "wayland-dwl-guile-devel"
#else
#define REPL_SOCKET_PATH "/tmp/dwl-guile.socket"
#define WAYLAND_SOCKET_PATH NULL
#endif
