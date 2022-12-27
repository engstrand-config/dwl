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

#define DSCM_ASSERT(PRED, MSG, ...)					\
	if (!PRED) scm_misc_error(					\
		"", MSG, scm_list_n(__VA_ARGS__, SCM_UNDEFINED))

#define DSCM_ASSERT_TYPE(PRED, VALUE, TYPE)				\
	SCM_ASSERT_TYPE(PRED, VALUE, SCM_ARG2, "", TYPE)

#define DSCM_ASSERT_OPTION(RESULT, KEY)					\
	if (scm_is_false(RESULT)) scm_misc_error(			\
		"dwl:set", "Invalid option: '~a", scm_list_1(KEY))

SCM config;
SCM metadata;

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
static char **termcmd                   = NULL;
static char **menucmd                   = NULL;
static Layout *layouts                  = NULL;
static MonitorRule *monrules            = NULL;
static Rule *rules                      = NULL;
static Key *keys                        = NULL;
static Button *buttons                  = NULL;
static struct xkb_rule_names *xkb_rules = NULL;

static unsigned int numtags     = 0;
static unsigned int numkeys     = 0;
static unsigned int numrules    = 0;
static unsigned int numlayouts  = 0;
static unsigned int nummonrules = 0;
static unsigned int numbuttons  = 0;
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
	(*((uint32_t*)cvar)) = scm_to_unsigned_integer(value, 0, UINT32_MAX);
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
setter_xkb_rules(void *cvar, SCM value)
{
	DSCM_ASSERT_TYPE(scm_list(value), value, "alist");
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
		wlr_keyboard_set_repeat_info(kb->wlr_keyboard, repeat_rate,
					     repeat_delay);
}

static inline void
dscm_config_cleanup()
{}

static inline void
dscm_parse_string(unsigned int index, SCM str, void *data)
{
	((char**)data)[index] = scm_to_locale_string(str);
}

static inline void
dscm_parse_layout(unsigned int index, SCM layout, void *data)
{
	((Layout*)data)[index] = (Layout){
		.id = dscm_alist_get_string(layout, "id"),
		.symbol = dscm_alist_get_string(layout, "symbol"),
		.arrange = dscm_alist_get_proc_pointer(layout, "arrange")
	};
}

static inline void
dscm_parse_monitor_rule(unsigned int index, SCM rule, void *data)
{
	enum wl_output_transform rr = scm_to_int(dscm_alist_get_eval(rule, "transform"));
	((MonitorRule*)data)[index] = (MonitorRule){
		.name = dscm_alist_get_string(rule, "name"),
		.mfact = dscm_alist_get_float(rule, "master-factor"),
		.nmaster = dscm_alist_get_int(rule, "masters"),
		.scale = dscm_alist_get_float(rule, "scale"),
		.lt = &layouts[dscm_alist_get_int(rule, "layout")],
		.rr = rr,
		.x = dscm_alist_get_int(rule, "x"),
		.y = dscm_alist_get_int(rule, "y"),
	};
}

static inline void
dscm_parse_rule(unsigned int index, SCM rule, void *data)
{
	((Rule*)data)[index] = (Rule){
		.id = dscm_alist_get_string(rule, "id"),
		.title = dscm_alist_get_string(rule, "title"),
		.tags = dscm_alist_get_unsigned_int(rule, "tag", -1),
		.isfloating = dscm_alist_get_int(rule, "floating"),
		.monitor = dscm_alist_get_int(rule, "monitor"),
		.alpha = dscm_alist_get_double(rule, "alpha")
	};
}

static inline void
dscm_parse_key(unsigned int index, SCM key, void *data)
{
	xkb_keycode_t keycode = dscm_alist_get_unsigned_int(key, "key", -1);
	/* Should we use `xkb_keycode_is_legal_x11`? */
	if (!xkb_keycode_is_legal_x11(keycode)
	    || !xkb_keycode_is_legal_ext(keycode))
		die("dscm: keycode '%d' is not a legal keycode\n", keycode);
	((Key*)data)[index] = (Key){
		.mod = dscm_alist_get_modifiers(key, "modifiers"),
		.keycode = keycode,
		.func = dscm_alist_get_proc_pointer(key, "action")
	};
}

static inline void
dscm_parse_button(unsigned int index, SCM button, void *data)
{
	unsigned int key = scm_to_unsigned_integer(
		dscm_alist_get_eval(button, "button"), 0, -1);
	((Button*)data)[index] = (Button){
		.mod = dscm_alist_get_modifiers(button, "modifiers"),
		.button = key,
		.func = dscm_alist_get_proc_pointer(button, "action")
	};
}

static inline void
dscm_config_initialize()
{
	metadata = scm_make_hash_table(scm_from_int(1));
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
	DSCM_DEFINE (accel_speed, "accel-speed", 0.0, &setter_double, &reload_inputdevice);
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

	scm_c_primitive_load(config_file);

	config = dscm_get_variable("config");

	tags = dscm_iterate_list(dscm_alist_get(config, "tags"),
				 sizeof(char*), 0, &dscm_parse_string, &numtags);
	termcmd = dscm_iterate_list(dscm_alist_get(config, "terminal"),
				    sizeof(char*), 1, &dscm_parse_string, NULL);
	menucmd = dscm_iterate_list(dscm_alist_get(config, "menu"),
				    sizeof(char*), 1, &dscm_parse_string, NULL);
	layouts = dscm_iterate_list(dscm_alist_get(config, "layouts"),
				    sizeof(Layout), 0, &dscm_parse_layout, &numlayouts);
	rules = dscm_iterate_list(dscm_alist_get(config, "rules"),
				  sizeof(Rule), 0, &dscm_parse_rule, &numrules);
	monrules = dscm_iterate_list(dscm_alist_get(config, "monitor-rules"),
				     sizeof(MonitorRule), 0, &dscm_parse_monitor_rule, &nummonrules);
	keys = dscm_iterate_list(dscm_alist_get(config, "keys"),
				 sizeof(Key), 0, &dscm_parse_key, &numkeys);
	buttons = dscm_iterate_list(dscm_alist_get(config, "buttons"),
				    sizeof(Button), 0, &dscm_parse_button, &numbuttons);
	TAGMASK = ((1 << numtags) - 1);
}
