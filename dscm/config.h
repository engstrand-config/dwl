#pragma once

#define DSCM_DEFINE(CVAR, KEY, DEFVAL, SETTER, RELOADER)		\
	{								\
	(CVAR) = DEFVAL;						\
	SCM m1 = scm_from_pointer(&(CVAR), NULL);			\
	SCM m2 = scm_from_pointer(SETTER, NULL);			\
	SCM m3 = SCM_BOOL_F;						\
	scm_gc_protect_object(m1);					\
	scm_gc_protect_object(m2);					\
	if (RELOADER != NULL) {						\
		m3 = scm_from_pointer(RELOADER, NULL);			\
		scm_gc_protect_object(m3);				\
	}								\
	scm_hash_set_x(							\
		metadata,						\
		scm_string_to_symbol(scm_from_locale_string(KEY)),	\
		scm_list_3(m1, m2, m3));				\
	}

#define DSCM_ASSERT_OPTION(RESULT, KEY)					\
	if (scm_is_false(RESULT)) scm_misc_error(			\
		"dwl:set", "Invalid option: '~a", scm_list_1(KEY))

#define DSCM_ASSERT_TYPE(PRED, VALUE, TYPE)				\
	SCM_ASSERT_TYPE(PRED, VALUE, SCM_ARG2, "", TYPE);

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

static float *rootcolor              = NULL;
static float *bordercolor            = NULL;
static float *focuscolor             = NULL;
static float *fullscreen_bg          = NULL;
static float *lockscreen_bg          = NULL;

static char **tags                   = NULL;
static char **termcmd                = NULL;
static char **menucmd                = NULL;
static Layout *layouts               = NULL;
static MonitorRule *monrules         = NULL;
static Rule *rules                   = NULL;
static Key *keys                     = NULL;
static Button *buttons               = NULL;
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
dscm_config_cleanup()
{}

static inline void
dscm_parse_color(unsigned int index, SCM value, void *data)
{
	((float*)data)[index] = (float)scm_to_double(value);
}

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

static inline struct xkb_rule_names *
dscm_parse_xkb_rules(SCM config)
{
	SCM xkb = dscm_alist_get(config, "xkb-rules");
	struct xkb_rule_names *dest = calloc(1, sizeof(struct xkb_rule_names));
	*dest = (struct xkb_rule_names){
		.rules = dscm_alist_get_string(xkb, "rules"),
		.model = dscm_alist_get_string(xkb, "model"),
		.layout = dscm_alist_get_string(xkb, "layouts"),
		.variant = dscm_alist_get_string(xkb, "variants"),
		.options = dscm_alist_get_string(xkb, "options"),
	};
	return dest;
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

	DSCM_DEFINE (repeat_rate, "repeat-rate", 50, &setter_uint, &reload_inputdevice);
	DSCM_DEFINE (repeat_delay, "repeat-delay", 300, &setter_uint, &reload_inputdevice);
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

	scm_c_primitive_load(config_file);

	config = dscm_get_variable("config");

	SCM colors = dscm_alist_get(config, "colors");
	rootcolor = dscm_iterate_list(dscm_alist_get(colors, "root"),
				      sizeof(float), 0, &dscm_parse_color, NULL);
	bordercolor = dscm_iterate_list(dscm_alist_get(colors, "border"),
					sizeof(float), 0, &dscm_parse_color, NULL);
	focuscolor = dscm_iterate_list(dscm_alist_get(colors, "focus"),
				       sizeof(float), 0, &dscm_parse_color, NULL);
	fullscreen_bg = dscm_iterate_list(dscm_alist_get(colors, "fullscreen"),
					  sizeof(float), 0, &dscm_parse_color, NULL);
	lockscreen_bg = dscm_iterate_list(dscm_alist_get(colors, "lockscreen"),
					  sizeof(float), 0, &dscm_parse_color, NULL);
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
	xkb_rules = dscm_parse_xkb_rules(config);
	TAGMASK = ((1 << numtags) - 1);
}
