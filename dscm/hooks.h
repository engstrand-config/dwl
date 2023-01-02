static SCM hook_quit;
static SCM hook_startup;

static inline void
dscm_hooks_run(SCM hook)
{
	scm_c_run_hook(hook, scm_list_n(SCM_UNDEFINED));
}

static inline SCM
dscm_hooks_run_async_caller(void *hook)
{
	scm_t_bits *ptr = hook;
	dscm_hooks_run(SCM_PACK_POINTER(ptr));
	return SCM_BOOL_T;
}

static inline void
dscm_hooks_run_async(SCM hook)
{
	scm_spawn_thread(dscm_hooks_run_async_caller,
			 SCM_UNPACK_POINTER(hook), NULL, NULL);
}

static inline void
dscm_hooks_initialize()
{
	hook_quit = scm_make_hook(scm_from_int(0));
	hook_startup = scm_make_hook(scm_from_int(0));

	/* Expose to Guile context */
	scm_c_define("dwl:hook-quit", hook_quit);
	scm_c_define("dwl:hook-startup", hook_startup);
}
