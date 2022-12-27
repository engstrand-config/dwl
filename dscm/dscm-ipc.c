#include <stdio.h>
#include <string.h>
#include <wayland-client.h>
#include "util.h"
#include "dscm-ipc.h"
#include "../dscm-unstable-v1-protocol-client.h"

static struct dscm_v1 *dscm;
static struct wl_display *display;
static unsigned int done = 0;
static void dscm_layout(void *data, struct dscm_v1 *d, const char *name);
static void dscm_tag(void *data, struct dscm_v1 *d, const char *name);
static void dscm_eval_result(void *data, struct dscm_v1 *d, const char *result,
			uint32_t error);
static void handle_global(void *data, struct wl_registry *registry,
			  uint32_t name, const char *interface, uint32_t version);

struct dscm_v1_listener dscm_listener = {
	.tag = dscm_tag,
	.layout = dscm_layout,
	.eval_result = dscm_eval_result,
};

void
dscm_tag(void *data, struct dscm_v1 *d, const char *name)
{}

void
dscm_layout(void *data, struct dscm_v1 *d, const char *name)
{}

void
dscm_eval_result(void *data, struct dscm_v1 *d, const char *result, uint32_t status)
{
	FILE *fd = status == DSCM_V1_EVAL_STATUS_SUCCESS ? stdout : stderr;
	fprintf(fd, "%s\n", result);
	done = 1;
}

void
handle_global(void *data, struct wl_registry *registry,
	      uint32_t name, const char *interface, uint32_t version)
{
	if (strcmp(interface, dscm_v1_interface.name) == 0) {
		dscm = wl_registry_bind(registry, name, &dscm_v1_interface, 1);
		dscm_v1_add_listener(dscm, &dscm_listener, NULL);
	}
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

	/* Wait for result */
	while (wl_display_dispatch(display) != -1 && !done) {}

	dscm_v1_release(dscm);
	wl_registry_destroy(registry);
	wl_display_disconnect(display);
	return 0;
}
