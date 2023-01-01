static SCM hook_quit;

static inline void
dscm_hooks_initialize()
{
	hook_quit = scm_make_hook(scm_from_int(0));

	/* Expose to Guile context */
	scm_c_define("dwl:hook-quit", hook_quit);
}
