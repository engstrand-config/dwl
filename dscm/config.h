#pragma once

#define DSCM_DEFINE_P(CVAR, KEY, SETTER, RELOADER)			\
	{								\
		SCM m1 = scm_from_pointer(&(CVAR), NULL);		\
		SCM m2 = scm_from_pointer(SETTER, NULL);		\
		SCM m3 = SCM_BOOL_F;					\
		scm_gc_protect_object(m1);				\
		scm_gc_protect_object(m2);				\
		if (RELOADER != NULL) {					\
			m3 = scm_from_pointer(RELOADER, NULL);		\
			scm_gc_protect_object(m3);			\
		}							\
		scm_hash_set_x(						\
			metadata,					\
			scm_string_to_symbol(scm_from_locale_string(KEY)), \
			scm_list_3(m1, m2, m3));			\
	}

#define DSCM_DEFINE(CVAR, KEY, DEFVAL, SETTER, RELOADER)	\
	{							\
		if (DEFVAL) (CVAR) = DEFVAL;			\
		DSCM_DEFINE_P(CVAR, KEY, SETTER, RELOADER);	\
	}

SCM config;
SCM metadata;

/* Set to 0 after the initial config load */
static unsigned int firstload = 1;

/* Config variable definitions. */
/* These will be automatically set from the guile config. */
static double default_alpha;
static unsigned int repeat_rate;
static unsigned int repeat_delay;
static unsigned int sloppyfocus;
static unsigned int bypass_surface_visibility;
static unsigned int borderpx;
static unsigned int gappih;
static unsigned int gappiv;
static unsigned int gappoh;
static unsigned int gappov;
static unsigned int smartgaps;
static unsigned int smartborders;

/* No easy way to set these default values in a macro */
static float rootcolor[4]     = {0.0, 0.0, 0.0, 1.0};
static float bordercolor[4]   = {0.5, 0.5, 0.5, 1.0};
static float focuscolor[4]    = {1.0, 0.0, 0.0, 1.0};
static float fullscreen_bg[4] = {0.1, 0.1, 0.1, 1.0};
static float lockscreen_bg[4] = {0.1, 0.1, 0.1, 1.0};

static char **tags                      = NULL;
static struct xkb_rule_names *xkb_rules = NULL;

static struct wl_list keys;
static struct wl_list buttons;
static struct wl_list layouts;
static struct wl_list rules;
static struct wl_list monrules;

static unsigned int numtags     = 0;
static unsigned int TAGMASK     = 0;

/* Trackpad and mouse */
static double accel_speed;
static unsigned int tap_to_click;
static unsigned int tap_and_drag;
static unsigned int drag_lock;
static unsigned int natural_scrolling;
static unsigned int disable_while_typing;
static unsigned int left_handed;
static unsigned int middle_button_emulation;
static uint32_t send_events_mode;
static enum libinput_config_scroll_method scroll_method;
static enum libinput_config_click_method click_method;
static enum libinput_config_tap_button_map button_map;
static enum libinput_config_accel_profile accel_profile;

static inline void
setter_uint(void *cvar, SCM value)
{
	DSCM_ASSERT_TYPE(scm_is_integer(value), value, "unsigned int");
	(*((unsigned int*)cvar)) = scm_to_unsigned_integer(value, 0, UINT_MAX);
}

static inline void
setter_uint32(void *cvar, SCM value)
{
	DSCM_ASSERT_TYPE(scm_is_integer(value), value, "uint32_t");
	(*((uint32_t*)cvar)) = scm_to_uint32(value);
}

static inline void
setter_double(void *cvar, SCM value)
{
	DSCM_ASSERT_TYPE(scm_is_number(value), value, "double");
	(*((double*)cvar)) = scm_to_double(value);
}

static inline void
setter_color(void *cvar, SCM value)
{
	DSCM_ASSERT_TYPE(scm_list(value) || scm_is_string(value),
			 value, "float[4] or string");

	float color[4];
	if (scm_is_string(value)) {
		char *str, *ptr, *str_orig = scm_to_locale_string(value);
		str = str_orig;
		/* Color parsing logic adapted from [sway] */
		if (*str == '#')
			str++;
		int len = strlen(str);

		/* Disallows "0x" prefix that strtoul would ignore */
		DSCM_ASSERT(((len == 6 || len == 8) &&
			     isxdigit(str[0]) && isxdigit(str[1])),
			    "Invalid hex color: ~s", value);

		uint32_t parsed = strtoul(str, &ptr, 16);
		DSCM_ASSERT(!(*ptr), "Invalid hex color: ~s", value);

		if (len == 8) {
			color[3] = ((parsed >> 0) & 0xff) / 255.0;
			parsed >>= 8;
		} else {
			color[3] = 1.0;
		}

		color[0] = ((parsed >> 16) & 0xff) / 255.0;
		color[1] = ((parsed >>  8) & 0xff) / 255.0;
		color[2] = ((parsed >>  0) & 0xff) / 255.0;
		free(str_orig);
	} else {
		int length = scm_to_int(scm_length(value));
		DSCM_ASSERT(((length >= 3) && (length <= 4)),
			    "Invalid float color: ~s, expected RGB(A) format", value);
		color[0] = (float)scm_to_double(scm_car(value));
		color[1] = (float)scm_to_double(scm_cadr(value));
		color[2] = (float)scm_to_double(scm_caddr(value));
		color[3] = length == 4 ? (float)scm_to_double(scm_cadddr(value)) : 1.0;
	}
	memcpy(cvar, color, sizeof(color));
}

static inline void
setter_tags(void *cvar, SCM value)
{
	DSCM_ASSERT_TYPE((scm_list_p(value) == SCM_BOOL_T), value, "list");
	DSCM_ASSERT(!scm_is_null(value),
		    "Invalid tag list, there must be at least one tag: ~a", value);
	unsigned int length = scm_to_unsigned_integer(scm_length(value), 0, UINT_MAX);
	char **newtags = calloc(length, sizeof(char*));
	for (size_t i = 0; i < length; i++) {
		SCM ref = scm_list_ref(value, scm_from_int(i));
		DSCM_ASSERT(scm_is_string(ref),
			    "Invalid tag ~s, expected string: ~a", ref, value);
		newtags[i] = scm_to_locale_string(ref);
	}

	char **tmp = tags, **end = tags + numtags;
	tags = newtags;
	numtags = length;
	TAGMASK = ((1 << numtags) - 1);

	if (tmp != NULL) {
		for (char **ptr = tmp; ptr < end; ptr++)
			free(*ptr);
		free(tmp);
	}
}

static inline void
setter_xkb_rules(void *cvar, SCM value)
{
	DSCM_ASSERT_TYPE(scm_list_p(value) == SCM_BOOL_T, value, "alist");
	struct xkb_rule_names *xkb = (*((struct xkb_rule_names**)cvar));
	if (xkb) {
		if (xkb->rules) free((char*)xkb->rules);
		if (xkb->model) free((char*)xkb->model);
		if (xkb->layout) free((char*)xkb->layout);
		if (xkb->variant) free((char*)xkb->variant);
		if (xkb->options) free((char*)xkb->options);
	} else {
		xkb = calloc(1, sizeof(struct xkb_rule_names));
	}

	xkb->rules = dscm_alist_get_string(value, "rules");
	xkb->model = dscm_alist_get_string(value, "model");
	xkb->layout = dscm_alist_get_string(value, "layout");
	xkb->variant = dscm_alist_get_string(value, "variant");
	xkb->options = dscm_alist_get_string(value, "options");
}

static inline void
setter_binding(void *cvar, SCM id, SCM value)
{
	DSCM_ASSERT_TYPE((scm_list_p(value) == SCM_BOOL_T), value, "list");
	SCM sequence = scm_car(value);
	scm_t_bits *action = dscm_get_pointer(scm_cadr(value));
	DSCM_ASSERT_TYPE(scm_is_string(sequence), sequence, "string");

	Binding tmp, *b;
	struct wl_list *lst = cvar;

	/* Attempt to parse before any allocation */
	/* TODO: Use dynwind? */
	dscm_parse_binding_sequence(&tmp, scm_to_locale_string(sequence));

	wl_list_for_each(b, lst, link) {
		if (b->key == tmp.key && b->mod == tmp.mod) {
			b->key = tmp.key;
			b->mod = tmp.mod;
			b->action = action;
			return;
		}
	}

	b = calloc(1, sizeof(Binding));
	b->key = tmp.key;
	b->mod = tmp.mod;
	b->action = action;
	wl_list_insert(lst, &b->link);
}

static inline void
setter_layout(void *cvar, SCM value)
{
	char *id = scm_to_locale_string(scm_symbol_to_string(scm_car(value)));
	char *symbol = scm_to_locale_string(scm_cadr(value));
	SCM action = scm_caddr(value);

	Layout *l;
	struct wl_list *lst = cvar;
	wl_list_for_each(l, lst, link) {
		if (!strcmp(l->id, id)) {
			free(l->symbol);
			free(id);
			l->symbol = symbol;
			l->arrange = dscm_get_pointer(action);
			return;
		}
	}

	/* not found */
	l = calloc(1, sizeof(Layout));
	l->id = id;
	l->symbol = symbol;
	l->arrange = dscm_get_pointer(action);
	wl_list_insert(lst, &l->link);
}

static inline void
setter_rule(void *cvar, SCM value)
{
	DSCM_ASSERT_TYPE((scm_list_p(value) == SCM_BOOL_T), value, "alist");
	char *id = dscm_alist_get_string(value, "id");
	char *title = dscm_alist_get_string(value, "title");

	DSCM_ASSERT(id || title, "Missing id and/or title in rule: ~s", value);

	Rule *r;
	int found = 0;
	struct wl_list *lst = cvar;
	SCM tags = dscm_alist_get(value, "tag");
	SCM isfloating = dscm_alist_get(value, "is-floating");
	SCM monitor = dscm_alist_get(value, "monitor");
	SCM alpha = dscm_alist_get(value, "alpha");

	DSCM_ASSERT_TYPE(scm_is_unsigned_integer(tags, 0, UINT_MAX),
			 tags, "unsigned int");
	DSCM_ASSERT_TYPE(scm_is_integer(isfloating), tags, "int");
	DSCM_ASSERT_TYPE(scm_is_integer(monitor), tags, "int");
	DSCM_ASSERT_TYPE(scm_is_number(alpha), tags, "double");

	wl_list_for_each(r, lst, link) {
		if ((id && !strcmp(r->id, id)) && (title && !strcmp(r->title, title))) {
			found = 1;
			if (id) free(id);
			if (title) free(title);
			break;
		}
	}

	if (!found) {
		r = calloc(1, sizeof(Rule));
		r->id = id;
		r->title = title;
		wl_list_insert(lst, &r->link);
	}

	r->tags = scm_to_unsigned_integer(tags, 0, UINT_MAX);
	r->isfloating = scm_to_int(isfloating);
	r->monitor = scm_to_int(monitor);
	r->alpha = scm_to_double(alpha);
}

static inline void
setter_monrule(void *cvar, SCM value)
{
	DSCM_ASSERT_TYPE((scm_list_p(value) == SCM_BOOL_T), value, "alist");
	MonitorRule *r;
	int found = 0;
	struct wl_list *lst = cvar;
	char *name = dscm_alist_get_string(value, "name");
	char *lt = scm_to_locale_string(scm_symbol_to_string(dscm_alist_get(value, "layout")));

	SCM mfact = dscm_alist_get(value, "master-factor");
	SCM nmaster = dscm_alist_get(value, "masters");
	SCM scale = dscm_alist_get(value, "scale");
	SCM rr = dscm_alist_get(value, "transform");
	SCM x = dscm_alist_get(value, "x");
	SCM y = dscm_alist_get(value, "y");

	/* TODO: add function for getting value and asserting type */
	/* DSCM_ASSERT_TYPE(scm_is_unsigned_integer(tags, 0, UINT_MAX),
	   tags, "unsigned int"); */
	/* DSCM_ASSERT_TYPE(scm_is_float(mfact), mfact, "float"); */
	/* DSCM_ASSERT_TYPE(scm_is_float(scale), scale, "float"); */
	/* DSCM_ASSERT_TYPE(scm_is_symbol(rr), rr, "symbol"); */
	/* DSCM_ASSERT_TYPE(scm_is_symbol(lt), lt, "symbol"); */
	/* DSCM_ASSERT_TYPE(scm_is_integer(nmaster), nmaster, "int"); */
	/* DSCM_ASSERT_TYPE(scm_is_integer(x), x, "int"); */
	/* DSCM_ASSERT_TYPE(scm_is_integer(y), y, "int"); */

	wl_list_for_each(r, lst, link) {
		if ((r->name == NULL && name == NULL) || (r->name && name && !strcmp(r->name, name))) {
			found = 1;
			if (name != NULL) free(name);
			break;
		}
	}

	if (!found) {
		r = calloc(1, sizeof(MonitorRule));
		r->name = name;
		wl_list_insert(lst, &r->link);
	}

	if (!scm_is_false(mfact))
		r->mfact = (float)scm_to_double(mfact);
	if (!scm_is_false(scale))
		r->scale = (float)scm_to_double(scale);
	if (!scm_is_false(nmaster))
		r->nmaster = scm_to_int(nmaster);
	if (!scm_is_false(x))
		r->x = scm_to_int(x);
	if (!scm_is_false(y))
		r->y = scm_to_int(y);
	if (!scm_is_false(rr)) {
		SCM rreval = scm_primitive_eval(rr);
		DSCM_ASSERT_TYPE(scm_is_integer(rreval), rreval, "int");
		r->rr = scm_to_int(rreval);
	}

	if (lt != NULL) {
		Layout *l;
		wl_list_for_each(l, &layouts, link) {
			if (!strcmp(l->id, lt)) {
				free(lt);
				r->lt = l;
				break;
			}
		}
		DSCM_ASSERT((r->lt != NULL),
			    "Invalid layout reference in monitor rule: ~a", value);
	}
}
static inline void
dscm_parse_string(unsigned int index, SCM str, void *data)
{
       ((char**)data)[index] = scm_to_locale_string(str);
}

static inline void
reload_inputdevice()
{
	/* reload libinput settings for the current input device */
}

static inline void
reload_borderpx()
{
	Client *c;
	int draw_borders;
	wl_list_for_each(c, &clients, link) {
		draw_borders = c->bw;
		resize(c, c->geom, 0, draw_borders);
	}
}

static inline void
reload_gaps()
{
	Monitor *m;
	/* Rearrange clients on all monitors */
	wl_list_for_each(m, &mons, link) {
		defaultgaps(NULL);
		arrange(m);
	}
}

static inline void
reload_rootcolor()
{
	/* Not set by upstream dwl in v0.4 */
}

static inline void
reload_bordercolor()
{
	Client *c;
	float *color;
	wl_list_for_each(c, &clients, link) {
		color = focustop(selmon) == c ? focuscolor : bordercolor;
		for (int i = 0; i < 4; i++)
			wlr_scene_rect_set_color(c->border[i], color);
	}
}

static inline void
reload_fullscreen_bg()
{
	Monitor *m;
	wl_list_for_each(m, &mons, link)
		wlr_scene_rect_set_color(m->fullscreen_bg, fullscreen_bg);
}

static inline void
reload_lockscreen_bg()
{
	wlr_scene_rect_set_color(locked_bg, lockscreen_bg);
}

static inline void
reload_xkb_rules()
{
	Keyboard *kb;
	struct xkb_keymap *keymap;
	struct xkb_context *context;
	wl_list_for_each(kb, &keyboards, link) {
		context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
		keymap = xkb_keymap_new_from_names(context, xkb_rules,
						   XKB_KEYMAP_COMPILE_NO_FLAGS);

		wlr_keyboard_set_keymap(kb->wlr_keyboard, keymap);
		xkb_keymap_unref(keymap);
		xkb_context_unref(context);
	}
}

static inline void
reload_keyboard_repeat_info()
{
	Keyboard *kb;
	wl_list_for_each(kb, &keyboards, link)
		wlr_keyboard_set_repeat_info(
			kb->wlr_keyboard, repeat_rate, repeat_delay);
}

static inline void
reload_layouts()
{}

static inline void
reload_rules()
{}

static inline void
reload_monrules()
{}

static inline void
reload_tags()
{
	/* TODO: Move clients to new tag if the tag they were on have been removed */
}

static inline void
dscm_config_load()
{
	scm_c_primitive_load(config_file);
	firstload = 0;
}

static inline void
dscm_config_initialize()
{
	wl_list_init(&keys);
	wl_list_init(&buttons);
	wl_list_init(&layouts);
	wl_list_init(&rules);
	wl_list_init(&monrules);

	const char *id = "tile";
	const char *symbol = "[]=";
	Layout *l = calloc(1, sizeof(Layout));
	l->id = strdup("tile");
	l->symbol = strdup("[]=");
	l->arrange = dscm_get_pointer(
		scm_string_to_symbol(scm_from_locale_string("dwl:tile")));
	wl_list_insert(&layouts, &l->link);

	MonitorRule *r = calloc(1, sizeof(MonitorRule));
	r->name = NULL;
	r->nmaster = 1;
	r->mfact = 0.55;
	r->scale = 1;
	r->lt = l;
	r->rr = WL_OUTPUT_TRANSFORM_NORMAL;
	r->x = 0;
	r->y = 0;
	wl_list_insert(&monrules, &r->link);

	metadata = scm_make_hash_table(scm_from_int(1));

	/* Populate keycode hash table */
	dscm_keycodes_initialize();

	DSCM_DEFINE (borderpx, "border-px", 1, &setter_uint, &reload_borderpx);
	DSCM_DEFINE (sloppyfocus, "sloppy-focus", 1, &setter_uint, NULL);
	DSCM_DEFINE (gappih, "gap-ih", 0, &setter_uint, &reload_gaps);
	DSCM_DEFINE (gappiv, "gap-iv", 0, &setter_uint, &reload_gaps);
	DSCM_DEFINE (gappoh, "gap-oh", 0, &setter_uint, &reload_gaps);
	DSCM_DEFINE (gappov, "gap-ov", 0, &setter_uint, &reload_gaps);

	DSCM_DEFINE (smartgaps, "smart-gaps", 0, &setter_uint, NULL);
	DSCM_DEFINE (smartborders, "smart-borders", 0, &setter_uint, NULL);
	DSCM_DEFINE (default_alpha, "default-alpha", 1.0, &setter_double, NULL);
	DSCM_DEFINE (bypass_surface_visibility, "bypass-surface-visibility", 1,
		     &setter_uint, NULL);

	DSCM_DEFINE (repeat_rate, "repeat-rate", 50,
		     &setter_uint, &reload_keyboard_repeat_info);
	DSCM_DEFINE (repeat_delay, "repeat-delay", 300,
		     &setter_uint, &reload_keyboard_repeat_info);
	DSCM_DEFINE (xkb_rules, "xkb-rules", NULL, &setter_xkb_rules, &reload_xkb_rules);

	DSCM_DEFINE (tap_to_click, "tap-to-click", 1, &setter_uint, &reload_inputdevice);
	DSCM_DEFINE (tap_and_drag, "tap-and-drag", 1, &setter_uint, &reload_inputdevice);
	DSCM_DEFINE (drag_lock, "drag-lock", 1, &setter_uint, &reload_inputdevice);
	DSCM_DEFINE (left_handed, "left-handed", 0, &setter_uint, &reload_inputdevice);
	DSCM_DEFINE (accel_speed, "accel-speed", 0.0,
		     &setter_double, &reload_inputdevice);
	DSCM_DEFINE (natural_scrolling, "natural-scrolling", 0,
		     &setter_uint, &reload_inputdevice);
	DSCM_DEFINE (disable_while_typing, "disable-while-typing", 1,
		     &setter_uint, &reload_inputdevice);
	DSCM_DEFINE (middle_button_emulation, "middle-button-emulation", 0,
		     &setter_uint, &reload_inputdevice);
	DSCM_DEFINE (accel_profile, "accel-profile", LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE,
		     &setter_uint32, &reload_inputdevice);
	DSCM_DEFINE (send_events_mode, "send-events-mode", LIBINPUT_CONFIG_SEND_EVENTS_ENABLED,
		     &setter_uint32, &reload_inputdevice);
	DSCM_DEFINE (scroll_method, "scroll-method", LIBINPUT_CONFIG_SCROLL_2FG,
		     &setter_uint32, &reload_inputdevice);
	DSCM_DEFINE (click_method, "click-method", LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS,
		     &setter_uint32, &reload_inputdevice);
	DSCM_DEFINE (button_map, "button-map", LIBINPUT_CONFIG_TAP_MAP_LRM,
		     &setter_uint32, &reload_inputdevice);

	DSCM_DEFINE_P (rootcolor, "root-color",
		       &setter_color, &reload_rootcolor);
	DSCM_DEFINE_P (bordercolor, "border-color",
		       &setter_color, &reload_bordercolor);
	DSCM_DEFINE_P (focuscolor, "focus-color",
		       &setter_color, &reload_bordercolor);
	DSCM_DEFINE_P (fullscreen_bg, "fullscreen-color",
		       &setter_color, &reload_fullscreen_bg);
	DSCM_DEFINE_P (lockscreen_bg, "lockscreen-color",
		       &setter_color, &reload_lockscreen_bg);

	DSCM_DEFINE_P (keys, "keys", &setter_binding, NULL);
	DSCM_DEFINE_P (buttons, "buttons", &setter_binding, NULL);
	DSCM_DEFINE_P (tags, "tags", &setter_tags, &reload_tags);
	DSCM_DEFINE_P (layouts, "layouts", &setter_layout, &reload_layouts);
	DSCM_DEFINE_P (layouts, "rules", &setter_rule, &reload_rules);
	DSCM_DEFINE_P (layouts, "monrules", &setter_monrule, &reload_monrules);
}

static inline void
dscm_config_cleanup()
{}
