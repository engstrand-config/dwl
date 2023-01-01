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
		(CVAR) = DEFVAL;				\
		DSCM_DEFINE_P(CVAR, KEY, SETTER, RELOADER);	\
	}

SCM metadata;

/* Set to 0 after the initial config load */
static unsigned int firstload = 1;
static unsigned int inhibdef;

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
setter_bool(void *cvar, SCM value)
{
	DSCM_ASSERT_TYPE(scm_is_bool(value), value, "set", DSCM_ARG2, "bool");
	(*((unsigned int*)cvar)) = scm_to_bool(value);
}

static inline void
setter_uint(void *cvar, SCM value)
{
	DSCM_ASSERT_TYPE(scm_is_integer(value), value, "set", DSCM_ARG2, "unsigned int");
	(*((unsigned int*)cvar)) = scm_to_unsigned_integer(value, 0, UINT_MAX);
}

static inline void
setter_uint32(void *cvar, SCM value)
{
	DSCM_ASSERT_TYPE(scm_is_integer(value), value, "set", DSCM_ARG2, "uint32");
	(*((uint32_t*)cvar)) = scm_to_uint32(value);
}

static inline void
setter_double(void *cvar, SCM value)
{
	DSCM_ASSERT_TYPE(scm_is_number(value), value, "set", DSCM_ARG2, "double");
	(*((double*)cvar)) = scm_to_double(value);
}

static inline void
setter_color(void *cvar, SCM value)
{
	DSCM_ASSERT_TYPE((scm_is_true(scm_list_p(value)) || scm_is_string(value)),
			 value, "set", DSCM_ARG2, "float[4] or string");

	float color[4];
	if (scm_is_string(value)) {
		char *str, *ptr, *str_orig = scm_to_locale_string(value);
		str = str_orig;
		/* Color parsing logic adapted from [sway] */
		if (*str == '#')
			str++;
		int len = strlen(str);

		scm_dynwind_begin(0);
		scm_dynwind_free(str_orig);

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
		scm_dynwind_end();
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
	DSCM_ASSERT_TYPE(scm_is_true(scm_list_p(value)),
			 value, "set", DSCM_ARG2, "list[string]");
	DSCM_ASSERT(!scm_is_null(value),
		    "Invalid tag list, there must be at least one tag: ~a", value);
	int length = scm_to_int(scm_length(value));
	char **newtags = calloc(length, sizeof(char*));
	for (int i = 0; i < length; i++) {
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
	DSCM_ASSERT_TYPE(scm_is_true(scm_list_p(value)),
			 value, "set-xkb-rules", DSCM_ARG1, "alist");
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

	xkb->rules = dscm_assoc_ref_string(value, "rules");
	xkb->model = dscm_assoc_ref_string(value, "model");
	xkb->layout = dscm_assoc_ref_string(value, "layout");
	xkb->variant = dscm_assoc_ref_string(value, "variant");
	xkb->options = dscm_assoc_ref_string(value, "options");
}

static inline void
setter_binding(void *cvar, SCM value)
{
	Binding tmp, *b;
	struct wl_list *lst = cvar;

	SCM sequence = scm_car(value);
	SCM action = scm_cadr(value);

	DSCM_ASSERT_TYPE(scm_is_string(sequence),
			 value, "bind", DSCM_ARG2, "string");
	DSCM_ASSERT_TYPE(dscm_is_callback(action),
			 value, "bind", DSCM_ARG3, "symbol or procedure");

	/* Attempt to parse before any allocation */
	dscm_parse_binding_sequence(&tmp, scm_to_locale_string(sequence));

	wl_list_for_each(b, lst, link) {
		if (b->key == tmp.key && b->mod == tmp.mod) {
			b->key = tmp.key;
			b->mod = tmp.mod;
			b->action = dscm_get_pointer(action);
			return;
		}
	}

	b = calloc(1, sizeof(Binding));
	b->key = tmp.key;
	b->mod = tmp.mod;
	b->action = dscm_get_pointer(action);
	wl_list_insert(lst, &b->link);
}

static inline void
setter_layout(void *cvar, SCM value)
{
	SCM id = scm_car(value);
	SCM symbol = scm_cadr(value);
	SCM arrange = scm_caddr(value);

	DSCM_ASSERT_TYPE(scm_is_symbol(id),
			 value, "set-layouts", DSCM_ARG1, "symbol");
	DSCM_ASSERT_TYPE(scm_is_string(symbol),
			 value, "set-layouts", DSCM_ARG2, "string");
	DSCM_ASSERT_TYPE(scm_is_symbol(arrange),
			 value, "set-layouts", DSCM_ARG3, "symbol");

	char *idstr = scm_to_locale_string(scm_symbol_to_string(id));
	char *symbolstr = scm_to_locale_string(symbol);

	Layout *l;
	struct wl_list *lst = cvar;
	wl_list_for_each(l, lst, link) {
		if (!strcmp(l->id, idstr)) {
			free(l->symbol);
			free(idstr);
			l->symbol = symbolstr;
			l->arrange = dscm_get_pointer(arrange);
			return;
		}
	}

	/* not found */
	l = calloc(1, sizeof(Layout));
	l->id = idstr;
	l->symbol = symbolstr;
	l->arrange = dscm_get_pointer(arrange);
	wl_list_insert(lst, &l->link);
}

static inline void
setter_rule(void *cvar, SCM value)
{
	DSCM_ASSERT_TYPE(scm_is_true(scm_list_p(value)),
			 value, "set-rules", DSCM_ARG1, "alist");
	Rule *r;
	int found = 0;
	struct wl_list *lst = cvar;

	char *id = dscm_assoc_ref_string(value, "id");
	char *title = dscm_assoc_ref_string(value, "title");

	DSCM_ASSERT((id || title), "Missing id and/or title in rule: ~s", value);

	SCM tags = dscm_assoc_ref(value, "tags");
	SCM floating = dscm_assoc_ref(value, "floating?");
	SCM monitor = dscm_assoc_ref(value, "monitor");
	SCM alpha = dscm_assoc_ref(value, "alpha");

	wl_list_for_each(r, lst, link) {
		if (((!r->id && !id) || (r->id && id && !strcmp(r->id, id))) &&
		    ((!r->title && !title) || (r->title && title && !strcmp(r->title, title)))) {
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
		r->monitor = -1;
	}

	scm_dynwind_begin(0);
	scm_dynwind_unwind_handler(free, r, 0);
	if (!found && id) scm_dynwind_unwind_handler(free, id, 0);
	if (!found && title) scm_dynwind_unwind_handler(free, title, 0);

	DSCM_ASSERT_TYPE(scm_is_bool(floating),
			 value, "set-rules", "floating?", "bool");
	r->isfloating = scm_to_bool(floating);

	if (!scm_is_false(tags)) {
		if (scm_is_true(scm_list_p(tags))) {
			SCM ref;
			for (int i = 0; i < scm_to_int(scm_length(tags)); i++) {
				ref = scm_list_ref(tags, scm_from_int(i));
				DSCM_ASSERT_TYPE(
					scm_is_unsigned_integer(ref, 0, UINT_MAX),
					value, "set-rules", "tags",
					"list[unsigned int]");
				r->tags |= (1 << dscm_get_tag(ref, numtags));
			}
		} else {
			DSCM_ASSERT_TYPE(scm_is_unsigned_integer(tags, 0, UINT_MAX),
					 value, "set-rules", "tags", "unsigned int");
			r->tags = dscm_get_tag(tags, numtags);
		}
	}
	if (!scm_is_false(monitor)) {
		DSCM_ASSERT_TYPE(scm_is_integer(monitor),
				 value, "set-rules", "monitor", "int");
		r->monitor = scm_to_int(monitor);
	}
	if (!scm_is_false(alpha)) {
		DSCM_ASSERT_TYPE(scm_is_number(alpha),
				 value, "set-rules", "alpha", "double");
		r->alpha = scm_to_double(alpha);
	}

	if (!found)
		wl_list_insert(lst, &r->link);

	scm_dynwind_end();
}

static inline void
setter_monrule(void *cvar, SCM value)
{
	DSCM_ASSERT_TYPE(scm_is_true(scm_list_p(value)),
			 value, "set-monitor-rules", DSCM_ARG1, "alist");
	MonitorRule *r;
	int found = 0;
	struct wl_list *lst = cvar;

	char *name = dscm_assoc_ref_string(value, "name");
	SCM mfact = dscm_assoc_ref(value, "master-factor");
	SCM nmaster = dscm_assoc_ref(value, "masters");
	SCM scale = dscm_assoc_ref(value, "scale");
	SCM rr = dscm_assoc_ref(value, "transform");
	SCM x = dscm_assoc_ref(value, "x");
	SCM y = dscm_assoc_ref(value, "y");
	SCM lt = dscm_assoc_ref(value, "layout");

	wl_list_for_each(r, lst, link) {
		if ((!r->name && !name) || (r->name && name && !strcmp(r->name, name))) {
			found = 1;
			if (name != NULL) free(name);
			break;
		}
	}

	if (!found) {
		r = calloc(1, sizeof(MonitorRule));
		r->name = name;

		/* Set sensible defaults just in case some parameters are not set */
		r->scale = 1;
		r->rr = WL_OUTPUT_TRANSFORM_NORMAL;
		r->nmaster = 1;
		r->mfact = 0.55;
	}

	scm_dynwind_begin(0);
	scm_dynwind_unwind_handler(free, r, 0);
	if (!found && name) scm_dynwind_unwind_handler(free, name, 0);

	if (!scm_is_false(mfact)) {
		DSCM_ASSERT_TYPE(scm_is_number(mfact),
				 value, "set-monitor-rules", "master-factor", "float");
		r->mfact = (float)scm_to_double(mfact);
	}
	if (!scm_is_false(scale)) {
		DSCM_ASSERT_TYPE(scm_is_number(scale),
				 value, "set-monitor-rules", "scale", "float");
		r->scale = (float)scm_to_double(scale);
	}
	if (!scm_is_false(nmaster)) {
		DSCM_ASSERT_TYPE(scm_is_integer(nmaster),
				 value, "set-monitor-rules", "masters", "int");
		r->nmaster = scm_to_int(nmaster);
	}
	if (!scm_is_false(x)) {
		DSCM_ASSERT_TYPE(scm_is_integer(x),
				 value, "set-monitor-rules", "x", "int");
		r->x = scm_to_int(x);
	}
	if (!scm_is_false(y)) {
		DSCM_ASSERT_TYPE(scm_is_integer(y),
				 value, "set-monitor-rules", "y", "int");
		r->y = scm_to_int(y);
	}
	if (!scm_is_false(rr)) {
		DSCM_ASSERT_TYPE(scm_is_symbol(rr),
				 value, "set-monitor-rules", "transform", "symbol");
		SCM rreval = scm_primitive_eval(rr);
		DSCM_ASSERT(scm_is_integer(rreval),
			    "Unbound transform ~s in monitor rule: ~a", rr, value);
		r->rr = scm_to_int(rreval);
	}
	if (!scm_is_false(lt)) {
		DSCM_ASSERT_TYPE(scm_is_symbol(lt),
				 value, "set-monitor-rules", "layout",
				 "symbol (matching id of defined layouts)");
		Layout *l;
		char *id = scm_to_locale_string(scm_symbol_to_string(lt));
		wl_list_for_each(l, &layouts, link) {
			if (!strcmp(l->id, id)) {
				free(id);
				r->lt = l;
				break;
			}
		}
	} else {
		r->lt = wl_container_of(layouts.next, r->lt, link);
	}

	if (!found)
		wl_list_insert(lst, &r->link);

	scm_dynwind_end();
}

static inline void
reload_libinput()
{
	Pointer *p;
	wl_list_for_each(p, &pointers, link)
		applylibinputrules(p->dev);
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
		keymap = xkb_keymap_new_from_names(
			context, xkb_rules, XKB_KEYMAP_COMPILE_NO_FLAGS);
		wlr_keyboard_set_keymap(kb->wlr_keyboard, keymap);
		xkb_keymap_unref(keymap);
		xkb_context_unref(context);
	}
}

static inline void
reload_kbrepeat()
{
	Keyboard *kb;
	wl_list_for_each(kb, &keyboards, link)
		wlr_keyboard_set_repeat_info(
			kb->wlr_keyboard, repeat_rate, repeat_delay);
}

static inline void
reload_layouts()
{
	Layout *l;
	DscmClient *c;
	wl_list_for_each(c, &dscm_clients, link)
	    wl_list_for_each(l, &layouts, link)
		    dscm_v1_send_layout(c->resource, l->symbol);
}

static inline void
reload_rules()
{
	Client *c;
	Monitor *m;
	wl_list_for_each(c, &clients, link)
		applyrules(c);
	wl_list_for_each(m, &mons, link) {
		arrangelayers(m);
		arrange(m);
	}

	printstatus();
}

static inline void
reload_monrules()
{
	Monitor *m;
	wl_list_for_each(m, &mons, link)
		applymonrules(m);
}

static inline void
reload_tags()
{
	Client *c;
	Monitor *m;
	DscmClient *dc;
	unsigned int newtags, update = 0;
	wl_list_for_each(dc, &dscm_clients, link)
	    for (int i = 0; i < numtags; i++)
		    dscm_v1_send_tag(dc->resource, tags[i]);
	/* TODO: setmon will call printstatus, add flag to disable? */
	wl_list_for_each(c, &clients, link) {
		newtags = c->tags & TAGMASK;
		/* If the client was previosly rendered on a tag outside
		   of the new tag range, reassign it to the last tag in the
		   new tag set. */
		if (c->tags > TAGMASK) {
			update = 1;
			c->tags = newtags | (1 << (numtags - 1));
		}
	}

	if (update) {
		wl_list_for_each(m, &mons, link) {
			newtags = m->tagset[m->seltags] & TAGMASK;
			if (m->tagset[m->seltags] > TAGMASK)
				newtags |= (1 << (numtags - 1));
			m->tagset[m->seltags] = newtags;
			arrange(m);
		}
		printstatus();
	}
}

static inline void
dscm_config_load()
{
	if (firstload) scm_c_primitive_load(PREFIX "/share/dwl-guile/init.scm");
	scm_c_primitive_load(config_file);

	if (firstload) {
		if (!inhibdef)
			scm_c_primitive_load(PREFIX "/share/dwl-guile/defaults.scm");

		/* Ensure that all required settings have been set */
		int found = 0;
		MonitorRule *r;
		wl_list_for_each(r, &monrules, link)
			if (r->name == NULL)
				found = 1;
		if (!found)
			die("dscm: no default monitor rule with name = NULL set\n\n\
Consider adding something like the following to your config:\n\
(set-monitor-rules '((masters . 1)\n\
		     (master-factor . 0.55)\n\
		     (scale . 1)\n\
		     (rr . TRANSFORM-NORMAL)\n\
		     (layout . tile)))");
		if (!wl_list_length(&layouts))
			die("dscm: no layouts found, please add at least one layout\n\n\
Consider adding the following to your config:\n\
(set-layouts 'tile \"[]=\" 'dwl:tile)\n\
(set-layouts 'monocle \"|M|\" 'dwl:monocle)");

		firstload = 0;
	}
}

static inline void
dscm_config_initialize()
{
	wl_list_init(&keys);
	wl_list_init(&buttons);
	wl_list_init(&layouts);
	wl_list_init(&rules);
	wl_list_init(&monrules);

	metadata = scm_make_hash_table(scm_from_int(1));

	/* Populate keycode hash table */
	dscm_keycodes_initialize();

	DSCM_DEFINE(inhibdef, "inhibit-defaults?", 0, &setter_bool, NULL);
	DSCM_DEFINE(borderpx, "border-px", 1, &setter_uint, &reload_borderpx);
	DSCM_DEFINE(gappih, "gap-ih", 0, &setter_uint, &reload_gaps);
	DSCM_DEFINE(gappiv, "gap-iv", 0, &setter_uint, &reload_gaps);
	DSCM_DEFINE(gappoh, "gap-oh", 0, &setter_uint, &reload_gaps);
	DSCM_DEFINE(gappov, "gap-ov", 0, &setter_uint, &reload_gaps);

	DSCM_DEFINE(default_alpha, "default-alpha", 1.0, &setter_double, NULL);
	DSCM_DEFINE(bypass_surface_visibility, "bypass-surface-visibility", 1,
		     &setter_uint, NULL);

	DSCM_DEFINE(repeat_rate, "repeat-rate", 50, &setter_uint, &reload_kbrepeat);
	DSCM_DEFINE(repeat_delay, "repeat-delay", 300, &setter_uint, &reload_kbrepeat);
	DSCM_DEFINE(xkb_rules, "xkb-rules", NULL, &setter_xkb_rules, &reload_xkb_rules);

	DSCM_DEFINE(sloppyfocus, "sloppy-focus?", 1, &setter_bool, NULL);
	DSCM_DEFINE(smartgaps, "smart-gaps?", 0, &setter_bool, NULL);
	DSCM_DEFINE(smartborders, "smart-borders?", 0, &setter_bool, NULL);
	DSCM_DEFINE(tap_to_click, "tap-to-click?", 1, &setter_bool, &reload_libinput);
	DSCM_DEFINE(tap_and_drag, "tap-and-drag?", 1, &setter_uint, &reload_libinput);
	DSCM_DEFINE(drag_lock, "drag-lock?", 1, &setter_bool, &reload_libinput);
	DSCM_DEFINE(left_handed, "left-handed?", 0, &setter_bool, &reload_libinput);
	DSCM_DEFINE(natural_scrolling, "natural-scrolling?", 0,
		    &setter_bool, &reload_libinput);
	DSCM_DEFINE(disable_while_typing, "disable-while-typing?", 1,
		    &setter_bool, &reload_libinput);
	DSCM_DEFINE(middle_button_emulation, "middle-button-emulation?", 0,
		    &setter_bool, &reload_libinput);

	DSCM_DEFINE(accel_speed, "accel-speed", 0.0,
		    &setter_double, &reload_libinput);
	DSCM_DEFINE(accel_profile, "accel-profile",
		    LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE,
		    &setter_uint32, &reload_libinput);
	DSCM_DEFINE(send_events_mode, "send-events-mode",
		    LIBINPUT_CONFIG_SEND_EVENTS_ENABLED,
		    &setter_uint32, &reload_libinput);
	DSCM_DEFINE(scroll_method, "scroll-method",
		    LIBINPUT_CONFIG_SCROLL_2FG,
		    &setter_uint32, &reload_libinput);
	DSCM_DEFINE(click_method, "click-method",
		    LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS,
		    &setter_uint32, &reload_libinput);
	DSCM_DEFINE(button_map, "button-map",
		    LIBINPUT_CONFIG_TAP_MAP_LRM,
		    &setter_uint32, &reload_libinput);

	DSCM_DEFINE_P(rootcolor, "root-color", &setter_color, &reload_rootcolor);
	DSCM_DEFINE_P(bordercolor, "border-color", &setter_color, &reload_bordercolor);
	DSCM_DEFINE_P(focuscolor, "focus-color", &setter_color, &reload_bordercolor);
	DSCM_DEFINE_P(fullscreen_bg, "fullscreen-color",
		      &setter_color, &reload_fullscreen_bg);
	DSCM_DEFINE_P(lockscreen_bg, "lockscreen-color",
		      &setter_color, &reload_lockscreen_bg);

	DSCM_DEFINE_P(keys, "keys", &setter_binding, NULL);
	DSCM_DEFINE_P(buttons, "buttons", &setter_binding, NULL);
	DSCM_DEFINE_P(tags, "tags", &setter_tags, &reload_tags);
	DSCM_DEFINE_P(layouts, "layouts", &setter_layout, &reload_layouts);
	DSCM_DEFINE_P(rules, "rules", &setter_rule, &reload_rules);
	DSCM_DEFINE_P(monrules, "monrules", &setter_monrule, &reload_monrules);
}

static inline void
dscm_config_cleanup()
{
	Rule *r, *rtmp;
	Layout *l, *ltmp;
	Binding *b, *btmp;
	MonitorRule *mr, *mrtmp;
	wl_list_for_each_safe(r, rtmp, &rules, link) {
		if (r->id) free(r->id);
		if (r->title) free(r->title);
		free(r);
	}
	wl_list_for_each_safe(mr, mrtmp, &monrules, link) {
		if (mr->name) free(mr->name);
		free(mr);
	}
	wl_list_for_each_safe(l, ltmp, &layouts, link) {
		if (l->id) free(l->id);
		if (l->symbol) free(l->symbol);
		free(l);
	}
	wl_list_for_each_safe(b, btmp, &keys, link)
		free(b);
	wl_list_for_each_safe(b, btmp, &buttons, link)
		free(b);
	for (int i = 0; i < numtags; i++) free(tags[i]);
	if (xkb_rules != NULL) {
		if (xkb_rules->rules) free((char*)xkb_rules->rules);
		if (xkb_rules->model) free((char*)xkb_rules->model);
		if (xkb_rules->layout) free((char*)xkb_rules->layout);
		if (xkb_rules->variant) free((char*)xkb_rules->variant);
		if (xkb_rules->options) free((char*)xkb_rules->options);
		free(xkb_rules);
	}
}
