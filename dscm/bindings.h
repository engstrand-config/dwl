static inline SCM
dscm_binding_monocle(SCM monitor)
{
	Monitor *m = (Monitor*)scm_to_pointer(monitor);
	monocle(m);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_tile(SCM monitor)
{
	Monitor *m = (Monitor*)scm_to_pointer(monitor);
	tile(m);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_spawn(SCM args)
{
	if (scm_is_null(args))
		return SCM_BOOL_F;
	unsigned int i = 0, length = scm_to_unsigned_integer(
		scm_length(args), 0, UINT_MAX);
	char *cmd_args[length + 1];
	for (; i < length; i++) {
		SCM arg_exp = scm_list_ref(args, scm_from_unsigned_integer(i));
		char *arg = scm_to_locale_string(arg_exp);
		cmd_args[i] = arg;
	}
	cmd_args[i] = NULL;
	Arg a = {.v = cmd_args};
	spawn(&a);
	for (char **iter = cmd_args; *iter != NULL; iter++)
		free(*iter);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_shcmd(SCM args)
{
	SCM sh_args = scm_string_join(args, scm_from_utf8_string(" "),
				      scm_string_to_symbol(scm_from_utf8_string("infix")));
	SCM extended = scm_list_3(scm_from_utf8_string("/bin/sh"),
				  scm_from_utf8_string("-c"),
				  sh_args);
	return dscm_binding_spawn(extended);
}

static inline SCM
dscm_binding_focusstack(SCM value)
{
	Arg a = {.i = scm_to_int(value)};
	focusstack(&a);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_setmfact(SCM value)
{
	Arg a = {.f = scm_to_double(value)};
	setmfact(&a);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_togglefloating()
{
	togglefloating(NULL);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_zoom()
{
	zoom(NULL);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_setlayout(SCM value)
{
	char *id = scm_to_locale_string(value);
	Layout *l, *layout;
	wl_list_for_each(l, &layouts, link)
		if (strcmp(l->id, id) == 0)
			layout = l;
	if (layout == NULL)
		return SCM_BOOL_F;
	Arg a = {.v = layout};
	setlayout(&a);
	free(id);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_togglefullscreen()
{
	togglefullscreen(NULL);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_focusmon(SCM value)
{
	SCM eval = scm_primitive_eval(value);
	Arg a = {.i = scm_to_int(eval)};
	focusmon(&a);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_tagmon(SCM value)
{
	SCM eval = scm_primitive_eval(value);
	Arg a = {.i = scm_to_int(eval)};
	tagmon(&a);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_chvt(SCM tty)
{
	if (!scm_is_number(tty))
		return SCM_BOOL_F;
	int target_tty = scm_to_int(tty);
	if (target_tty <= 0 || target_tty > 12)
		return SCM_BOOL_F;
	Arg a = {.ui = (unsigned int)target_tty};
	chvt(&a);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_changealpha(SCM value)
{
	Arg a = {.f = scm_to_double(value)};
	changealpha(&a);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_view(SCM value)
{
	Arg a = {.ui = dscm_get_tag(value, numtags)};
	view(&a);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_viewprev(SCM value)
{
	viewprev(NULL);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_toggleview(SCM value)
{
	Arg a = {.ui = dscm_get_tag(value, numtags)};
	toggleview(&a);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_tag(SCM value)
{
	Arg a = {.ui = dscm_get_tag(value, numtags)};
	tag(&a);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_cyclelayout(SCM direction)
{
	Arg a = {.i = 1};
	if (scm_is_number(direction))
		a.i = scm_to_int(direction);
	cyclelayout(&a);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_incnmaster(SCM value)
{
	Arg a = {.i = scm_to_int(value)};
	incnmaster(&a);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_toggletag(SCM value)
{
	Arg a = {.ui = dscm_get_tag(value, numtags)};
	toggletag(&a);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_move()
{
	Arg a = {.ui = CurMove};
	moveresize(&a);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_resize()
{
	Arg a = {.ui = CurResize};
	moveresize(&a);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_killclient()
{
	killclient(NULL);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_quit()
{
	quit(NULL);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_incrgaps(SCM value)
{
	Arg a = {.i = scm_to_int(value)};
	incrgaps(&a);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_incrigaps(SCM value)
{
	Arg a = {.i = scm_to_int(value)};
	incrigaps(&a);
	return SCM_BOOL_T;
}
static inline SCM
dscm_binding_incrogaps(SCM value)
{
	Arg a = {.i = scm_to_int(value)};
	incrogaps(&a);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_togglegaps()
{
	togglegaps(NULL);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_defaultgaps()
{
	defaultgaps(NULL);
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_set(SCM rest)
{
	DSCM_SET_REST("set", rest, 2) {
		SCM key = dscm_list_ref(rest, i);
		SCM meta = scm_hash_ref(metadata, key, SCM_UNDEFINED);
		DSCM_ASSERT(!scm_is_false(meta), "Invalid key in setter: ~a", key);

		dscm_reloader_t func;
		void *cvar = scm_to_pointer(scm_car(meta));
		dscm_setter_t setter = (dscm_setter_t)scm_to_pointer(scm_cadr(meta));
		(*setter)(cvar, dscm_list_ref(rest, i + 1));

		/* When parsing the config for the first time, there is no need to
		   manually reload the updated configuration parameter, since this
		   will be done automatically right after.*/
		if (!firstload) {
			SCM reloader = scm_caddr(meta);
			if (!scm_is_false(reloader)) {
				func = (dscm_reloader_t)scm_to_pointer(reloader);
				(*func)();
			}
		}
	}
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_setkeys(SCM rest)
{
	DSCM_SET_REST("set-keys", rest, 2)
		dscm_binding_set(
			scm_list_2(dscm_string_to_symbol("keys"),
				   scm_list_2(dscm_list_ref(rest, i),
					      dscm_list_ref(rest, i + 1))));
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_setlayouts(SCM rest)
{
	DSCM_SET_REST("set-layouts", rest, 3)
		dscm_binding_set(
			scm_list_2(dscm_string_to_symbol("layouts"),
				   scm_list_3(dscm_list_ref(rest, i),
					      dscm_list_ref(rest, i + 1),
					      dscm_list_ref(rest, i + 2))));
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_setrules(SCM rest)
{
	DSCM_SET_REST("set-rules", rest, 1)
		dscm_binding_set(scm_list_2(dscm_string_to_symbol("rules"),
					    dscm_list_ref(rest, i)));
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_setmonrules(SCM rest)
{
	DSCM_SET_REST("set-monitor-rules", rest, 1)
		dscm_binding_set(scm_list_2(dscm_string_to_symbol("monrules"),
					    dscm_list_ref(rest, i)));
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_setxkbrules(SCM xkb)
{
	dscm_binding_set(scm_list_2(dscm_string_to_symbol("xkb-rules"), xkb));
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_reloadconfig()
{
	dscm_config_load();
	return SCM_BOOL_T;
}

static inline SCM
dscm_binding_movestack(SCM direction)
{
	Arg a = {.i = scm_to_int(direction)};
	movestack(&a);
	return SCM_BOOL_T;
}

static inline void
dscm_register()
{
	scm_c_define("DIRECTION-LEFT", scm_from_int(WLR_DIRECTION_LEFT));
	scm_c_define("DIRECTION-RIGHT", scm_from_int(WLR_DIRECTION_RIGHT));
	scm_c_define("DIRECTION-UP", scm_from_int(WLR_DIRECTION_UP));
	scm_c_define("DIRECTION-DOWN", scm_from_int(WLR_DIRECTION_DOWN));
	scm_c_define("TRANSFORM-NORMAL",
		     scm_from_int(WL_OUTPUT_TRANSFORM_NORMAL));
	scm_c_define("TRANSFORM-ROTATE-90",
		     scm_from_int(WL_OUTPUT_TRANSFORM_90));
	scm_c_define("TRANSFORM-ROTATE-180",
		     scm_from_int(WL_OUTPUT_TRANSFORM_180));
	scm_c_define("TRANSFORM-ROTATE-270",
		     scm_from_int(WL_OUTPUT_TRANSFORM_270));
	scm_c_define("TRANSFORM-FLIPPED",
		     scm_from_int(WL_OUTPUT_TRANSFORM_FLIPPED));
	scm_c_define("TRANSFORM-FLIPPED-90",
		     scm_from_int(WL_OUTPUT_TRANSFORM_FLIPPED_90));
	scm_c_define("TRANSFORM-FLIPPED-180",
		     scm_from_int(WL_OUTPUT_TRANSFORM_FLIPPED_180));
	scm_c_define("TRANSFORM-FLIPPED-270",
		     scm_from_int(WL_OUTPUT_TRANSFORM_FLIPPED_270));

	/* libinput */
	scm_c_define("SCROLL-NO-SCROLL",
		     scm_from_int(LIBINPUT_CONFIG_SCROLL_NO_SCROLL));
	scm_c_define("SCROLL-2FG",
		     scm_from_int(LIBINPUT_CONFIG_SCROLL_2FG));
	scm_c_define("SCROLL-EDGE",
		     scm_from_int(LIBINPUT_CONFIG_SCROLL_EDGE));
	scm_c_define("SCROLL-ON-BUTTON-DOWN",
		     scm_from_int(LIBINPUT_CONFIG_SCROLL_ON_BUTTON_DOWN));

	scm_c_define("CLICK-METHOD-NONE",
		     scm_from_int(LIBINPUT_CONFIG_CLICK_METHOD_NONE));
	scm_c_define("CLICK-METHOD-BUTTON-AREAS",
		     scm_from_int(LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS));
	scm_c_define("CLICK-METHOD-CLICKFINGER",
		     scm_from_int(LIBINPUT_CONFIG_CLICK_METHOD_CLICKFINGER));

	scm_c_define("TAP-MAP-LRM",
		     scm_from_int(LIBINPUT_CONFIG_TAP_MAP_LRM));
	scm_c_define("TAP-MAP-LMR",
		     scm_from_int(LIBINPUT_CONFIG_TAP_MAP_LMR));

	scm_c_define("SEND-EVENTS-ENABLED",
		     scm_from_int(LIBINPUT_CONFIG_SEND_EVENTS_ENABLED));
	scm_c_define("SEND-EVENTS-DISABLED",
		     scm_from_int(LIBINPUT_CONFIG_SEND_EVENTS_DISABLED));
	scm_c_define("SEND-EVENTS-DISABLED-ON-EXTERNAL-MOUSE",
		     scm_from_int(LIBINPUT_CONFIG_SEND_EVENTS_DISABLED_ON_EXTERNAL_MOUSE));

	scm_c_define("ACCEL-PROFILE-FLAT",
		     scm_from_int(LIBINPUT_CONFIG_ACCEL_PROFILE_FLAT));
	scm_c_define("ACCEL-PROFILE-ADAPTIVE",
		     scm_from_int(LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE));

	/* dwl bindings */
	scm_c_define_gsubr("dwl:chvt", 1, 0, 0, &dscm_binding_chvt);
	scm_c_define_gsubr("dwl:quit", 0, 0, 0, &dscm_binding_quit);
	scm_c_define_gsubr("dwl:kill-client", 0, 0, 0, &dscm_binding_killclient);
	scm_c_define_gsubr("dwl:tile", 1, 0, 0, &dscm_binding_tile);
	scm_c_define_gsubr("dwl:monocle", 1, 0, 0, &dscm_binding_monocle);
	scm_c_define_gsubr("dwl:spawn", 0, 0, 1, &dscm_binding_spawn);
	scm_c_define_gsubr("dwl:view", 1, 0, 0, &dscm_binding_view);
	scm_c_define_gsubr("dwl:view-previous", 0, 0, 0, &dscm_binding_viewprev);
	scm_c_define_gsubr("dwl:toggle-view", 1, 0, 0, &dscm_binding_toggleview);
	scm_c_define_gsubr("dwl:tag", 1, 0, 0, &dscm_binding_tag);
	scm_c_define_gsubr("dwl:cycle-layout", 0, 1, 0, &dscm_binding_cyclelayout);
	scm_c_define_gsubr("dwl:toggle-tag", 1, 0, 0, &dscm_binding_toggletag);
	scm_c_define_gsubr("dwl:focus-stack", 1, 0, 0, &dscm_binding_focusstack);
	scm_c_define_gsubr("dwl:zoom", 0, 0, 0, &dscm_binding_zoom);
	scm_c_define_gsubr("dwl:set-layout", 1, 0, 0, &dscm_binding_setlayout);
	scm_c_define_gsubr("dwl:toggle-fullscreen", 0, 0, 0, &dscm_binding_togglefullscreen);
	scm_c_define_gsubr("dwl:toggle-floating", 0, 0, 0, &dscm_binding_togglefloating);
	scm_c_define_gsubr("dwl:focus-monitor", 1, 0, 0, &dscm_binding_focusmon);
	scm_c_define_gsubr("dwl:tag-monitor", 1, 0, 0, &dscm_binding_tagmon);
	scm_c_define_gsubr("dwl:move", 0, 0, 0, &dscm_binding_move);
	scm_c_define_gsubr("dwl:resize", 0, 0, 0, &dscm_binding_resize);
	scm_c_define_gsubr("dwl:change-masters", 1, 0, 0, &dscm_binding_incnmaster);
	scm_c_define_gsubr("dwl:change-master-factor", 1, 0, 0, &dscm_binding_setmfact);
	scm_c_define_gsubr("dwl:shcmd", 0, 0, 1, &dscm_binding_shcmd);
	scm_c_define_gsubr("dwl:change-alpha", 1, 0, 0, &dscm_binding_changealpha);
	scm_c_define_gsubr("dwl:toggle-gaps", 0, 0, 0, &dscm_binding_togglegaps);
	scm_c_define_gsubr("dwl:default-gaps", 0, 0, 0, &dscm_binding_defaultgaps);
	scm_c_define_gsubr("dwl:change-gaps", 1, 0, 0, &dscm_binding_incrgaps);
	scm_c_define_gsubr("dwl:change-gaps-inner", 1, 0, 0, &dscm_binding_incrigaps);
	scm_c_define_gsubr("dwl:change-gaps-outer", 1, 0, 0, &dscm_binding_incrogaps);
	scm_c_define_gsubr("dwl:move-stack", 1, 0, 0, &dscm_binding_movestack);

	/* dwl-guile specific bindings */
	scm_c_define_gsubr("dwl:reload-config", 0, 0, 0, &dscm_binding_reloadconfig);
	scm_c_define_gsubr("set", 0, 0, 1, &dscm_binding_set);
	scm_c_define_gsubr("set-keys", 0, 0, 1, &dscm_binding_setkeys);
	scm_c_define_gsubr("set-layouts", 0, 0, 1, &dscm_binding_setlayouts);
	scm_c_define_gsubr("set-rules", 0, 0, 1, &dscm_binding_setrules);
	scm_c_define_gsubr("set-monitor-rules", 0, 0, 1, &dscm_binding_setmonrules);
	scm_c_define_gsubr("set-xkb-rules", 1, 0, 0, &dscm_binding_setxkbrules);
}
