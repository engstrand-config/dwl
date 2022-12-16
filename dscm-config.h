#pragma once

SCM config;

/* Config variable definitions. */
/* These will be automatically set from the guile config. */
static int repeat_rate               = 25;
static int repeat_delay              = 600;
static double accel_speed            = 0.0;
static int sloppyfocus               = 1;
static int bypass_surface_visibility = 0;
static unsigned int borderpx         = 1;
static double default_alpha          = 1.0;
static unsigned int gappih           = 0;
static unsigned int gappiv           = 0;
static unsigned int gappoh           = 0;
static unsigned int gappov           = 0;
static int smartgaps                 = 0;
static int smartborders              = 0;
static float *rootcolor              = NULL;
static float *bordercolor            = NULL;
static float *focuscolor             = NULL;
static float *textcolor              = NULL;
static float *fullscreen_bg          = NULL;
static char **tags                   = NULL;
static char **termcmd                = NULL;
static char **menucmd                = NULL;
static Layout *layouts               = NULL;
static MonitorRule *monrules         = NULL;
static Rule *rules                   = NULL;
static Key *keys                     = NULL;
static Button *buttons               = NULL;
static struct xkb_rule_names *xkb_rules = NULL;
static enum libinput_config_accel_profile accel_profile = LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE;

static unsigned int numtags     = 0;
static unsigned int numkeys     = 0;
static unsigned int numrules    = 0;
static unsigned int numlayouts  = 0;
static unsigned int nummonrules = 0;
static unsigned int numbuttons  = 0;
static unsigned int TAGMASK     = 0;

/* Trackpad */
static int tap_to_click = 1;
static int tap_and_drag = 1;
static int drag_lock = 1;
static int natural_scrolling = 0;
static int disable_while_typing = 1;
static int left_handed = 0;
static int middle_button_emulation = 0;
static uint32_t send_events_mode = LIBINPUT_CONFIG_SEND_EVENTS_ENABLED;
static enum libinput_config_scroll_method scroll_method = LIBINPUT_CONFIG_SCROLL_2FG;
static enum libinput_config_click_method click_method = LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS;
static enum libinput_config_tap_button_map button_map = LIBINPUT_CONFIG_TAP_MAP_LRM;

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
dscm_config_parse(char *config_file)
{
	scm_c_primitive_load(config_file);
	config = dscm_get_variable("config");

	sloppyfocus = dscm_alist_get_int(config, "sloppy-focus");
	borderpx = dscm_alist_get_unsigned_int(config, "border-px", 25);
	repeat_rate = dscm_alist_get_unsigned_int(config, "repeat-rate", 5000);
	repeat_delay = dscm_alist_get_unsigned_int(config, "repeat-delay", 5000);
	default_alpha = dscm_alist_get_double(config, "default-alpha");
	gappih = dscm_alist_get_unsigned_int(config, "gaps-horizontal-inner", -1);
	gappiv = dscm_alist_get_unsigned_int(config, "gaps-vertical-inner", -1);
	gappoh = dscm_alist_get_unsigned_int(config, "gaps-horizontal-outer", -1);
	gappov = dscm_alist_get_unsigned_int(config, "gaps-vertical-outer", -1);
	smartgaps = dscm_alist_get_int(config, "smart-gaps");
	smartborders = dscm_alist_get_int(config, "smart-borders");

	accel_speed = dscm_alist_get_double(config, "acceleration-speed");
	tap_to_click = dscm_alist_get_int(config, "tap-to-click");
	tap_and_drag = dscm_alist_get_int(config, "tap-and-drag");
	drag_lock = dscm_alist_get_int(config, "drag-lock");
	natural_scrolling = dscm_alist_get_int(config, "natural-scrolling");
	disable_while_typing = dscm_alist_get_int(config, "disable-while-typing");
	left_handed = dscm_alist_get_int(config, "left-handed");
	middle_button_emulation = dscm_alist_get_int(config, "middle-button-emulation");

	accel_profile = scm_to_int(dscm_alist_get_eval(config, "acceleration-profile"));
	send_events_mode = scm_to_int(dscm_alist_get_eval(config, "send-events-mode"));
	scroll_method = scm_to_int(dscm_alist_get_eval(config, "scroll-method"));
	click_method = scm_to_int(dscm_alist_get_eval(config, "click-method"));
	button_map = scm_to_int(dscm_alist_get_eval(config, "button-map"));

	SCM colors = dscm_alist_get(config, "colors");
	rootcolor = dscm_iterate_list(dscm_alist_get(colors, "root"),
				      sizeof(float), 0, &dscm_parse_color, NULL);
	bordercolor = dscm_iterate_list(dscm_alist_get(colors, "border"),
					sizeof(float), 0, &dscm_parse_color, NULL);
	focuscolor = dscm_iterate_list(dscm_alist_get(colors, "focus"),
				       sizeof(float), 0, &dscm_parse_color, NULL);
	textcolor = dscm_iterate_list(dscm_alist_get(colors, "text"),
				      sizeof(float), 0, &dscm_parse_color, NULL);
	fullscreen_bg = dscm_iterate_list(dscm_alist_get(colors, "fullscreen"),
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

static inline void
dscm_config_cleanup()
{
	fprintf(stdout, "dscm: starting cleanup\n");
	int i;
	char **str;
	for (i = 0; i < numtags; i++) free(tags[i]);
	for (str = termcmd; *str != NULL; str++) free(*str);
	for (str = menucmd; *str != NULL; str++) free(*str);
	for (i = 0; i < numlayouts; i++) free(layouts[i].symbol);
	for (i = 0; i < nummonrules; i++) free(monrules[i].name);
	for (i = 0; i < numrules; i++) {
		Rule r = rules[i];
		free(r.id);
		free(r.title);
	}
	free(layouts);
	free(monrules);
	free(keys);
	free(buttons);
	free(rootcolor);
	free(bordercolor);
	free(focuscolor);
	free(textcolor);
	free(fullscreen_bg);
	free((char*)xkb_rules->rules);
	free((char*)xkb_rules->model);
	free((char*)xkb_rules->layout);
	free((char*)xkb_rules->variant);
	free((char*)xkb_rules->options);
	free(xkb_rules);
}
