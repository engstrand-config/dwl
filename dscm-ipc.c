#include <string.h>
#include <wayland-client.h>
#include "util.h"
#include "dscm-ipc.h"
#include "dscm-unstable-v1-protocol-client.h"

static struct dscm_v1 *dscm;
static struct wl_display *display;

void
handle_global(void *data, struct wl_registry *registry,
	      uint32_t name, const char *interface, uint32_t version)
{
	if (strcmp(interface, dscm_v1_interface.name) == 0)
		dscm = wl_registry_bind(registry, name, &dscm_v1_interface, 1);
}

int dscm_ipc_evaluate(char *exp)
{
	if (!exp)
		die("missing expression");

	display = wl_display_connect(NULL);
	if (!display)
		die("could not connect to display, is dwl-guile running?");

	struct wl_registry *registry = wl_display_get_registry(display);
	struct wl_registry_listener registry_listener = {.global = handle_global};
	wl_registry_add_listener(registry, &registry_listener, NULL);
	wl_display_roundtrip(display);

	if (!dscm)
		die("compositor does not support the dscm protocol");

	dscm_v1_eval(dscm, exp);
	wl_display_roundtrip(display);

	wl_registry_destroy(registry);
	wl_display_disconnect(display);
	return 0;
}
