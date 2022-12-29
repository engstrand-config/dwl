#pragma once

#define DSCM_ASSERT(PRED, MSG, ...)					\
	if (!PRED) scm_misc_error(					\
		"", MSG, scm_list_n(__VA_ARGS__, SCM_UNDEFINED))

#define DSCM_ASSERT_TYPE(PRED, VALUE, TYPE)				\
	SCM_ASSERT_TYPE(PRED, VALUE, SCM_ARG2, "", TYPE)

#define DSCM_ASSERT_OPTION(RESULT, KEY)					\
	if (scm_is_false(RESULT)) scm_misc_error(			\
		"dwl:set", "Invalid option: '~a", scm_list_1(KEY))

enum { DSCM_CALL_ARRANGE, DSCM_CALL_ACTION };
typedef struct {
	SCM proc;
	void *args;
} dscm_call_data_t;

typedef struct {
	char *exp;
	struct wl_resource *resource;
	void (*callback)(struct wl_resource*, char*, uint32_t);
} dscm_eval_call_data_t;

typedef void(*dscm_reloader_t)();
typedef void(*dscm_setter_t)(void*, SCM);

static inline SCM
dscm_alist_get(SCM alist, const char* key)
{
	return scm_assoc_ref(alist, scm_from_utf8_string(key));
}

static inline char*
dscm_alist_get_string(SCM alist, const char *key)
{
	SCM value = dscm_alist_get(alist, key);
	if (scm_is_string(value))
		return scm_to_locale_string(value);
	return NULL;
}

static inline int
dscm_alist_get_int(SCM alist, const char* key)
{
	SCM value = dscm_alist_get(alist, key);
	if (scm_is_bool(value))
		return scm_is_true(value) ? 1 : 0;
	return scm_to_int(value);
}

static inline unsigned int
dscm_alist_get_unsigned_int(SCM alist, const char* key, int max)
{
	return scm_to_unsigned_integer(dscm_alist_get(alist, key), 0, max);
}

static inline double
dscm_alist_get_double(SCM alist, const char* key)
{
	SCM value = dscm_alist_get(alist, key);
	if (scm_is_bool(value))
		return scm_is_true(value) ? 1 : 0;
	return scm_to_double(value);
}

static inline float
dscm_alist_get_float(SCM alist, const char* key)
{
	return (float)dscm_alist_get_double(alist, key);
}

static inline scm_t_bits *
dscm_get_pointer(SCM action)
{
	SCM actionref = scm_primitive_eval(action);
	DSCM_ASSERT((scm_procedure_p(actionref) == SCM_BOOL_T),
		    "Invalid action callback: ~s", action);
	scm_gc_protect_object(actionref);
	return SCM_UNPACK_POINTER(actionref);
}

static inline SCM
dscm_get_variable(const char *name)
{
	return scm_variable_ref(scm_c_lookup(name));
}

static inline unsigned int
dscm_get_list_length(SCM list)
{
	return scm_to_unsigned_integer(scm_length(list), 0, -1);
}

static inline SCM
dscm_get_list_item(SCM list, unsigned int index)
{
	return scm_list_ref(list, scm_from_unsigned_integer(index));
}

static inline uint32_t
dscm_alist_get_modifiers(SCM alist, const char *key)
{
	SCM modifiers = dscm_alist_get(alist, key);
	uint32_t mod = 0;
	unsigned int i = 0, length = dscm_get_list_length(modifiers);
	for (; i < length; i++) {
		SCM item = dscm_get_list_item(modifiers, i);
		SCM eval = scm_primitive_eval(item);
		mod |= scm_to_uint32(eval);
	}
	return mod;
}

static inline unsigned int
dscm_get_tag(SCM tag, unsigned int tags)
{
	unsigned int target_tag = scm_to_unsigned_integer(tag, 1, tags) - 1;
	return (1 << (target_tag));
}

static inline void *
dscm_iterate_list(SCM list, size_t elem_size, int append_null,
		  void (*iterator)(unsigned int, SCM, void*), unsigned int *length_var)
{
	SCM item;
	unsigned int i = 0, length = 0;
	length = dscm_get_list_length(list);
	void *allocated = calloc(append_null ? length + 1 : length, elem_size);
	for (; i < length; i++) {
		item = dscm_get_list_item(list, i);
		(*iterator)(i, item, allocated);
	}
	if (append_null)
		((void**)allocated)[i] = NULL;
	if (length_var)
		*length_var = length;
	return allocated;
}

static inline void
dscm_modify_list(SCM list, void *target, void (*iterator)(unsigned int, SCM, void*))
{
	SCM item;
	unsigned int length = dscm_get_list_length(list);
	for (unsigned int i = 0; i < length; i++) {
		item = dscm_get_list_item(list, i);
		(*iterator)(i, item, target);
	}
}

static inline void
dscm_parse_binding_sequence(Binding *b, char *sequence)
{
	char *token, *next, *key, *ptr;
	if ((key = strpbrk(sequence, "<"))) {
		SCM keycode = scm_hash_ref(
			keycodes, scm_from_locale_string(key), SCM_UNDEFINED);
		DSCM_ASSERT((!scm_is_false(keycode)),
			    "Invalid keysym in bind sequence: ~s",
			    scm_from_locale_string(sequence));
		b->key = (xkb_keycode_t)scm_to_uint32(keycode);
	} else if ((key = strpbrk(sequence, "["))) {
		for (ptr = &key[1]; !(*ptr == '\0' || *ptr == ']'); ptr++)
			DSCM_ASSERT(isdigit(*ptr),
				    "Invalid keycode in bind sequence: ~s",
				    scm_from_locale_string(sequence));
		DSCM_ASSERT((*ptr == ']'), "Trailing ']' in bind sequence: ~s",
			    scm_from_locale_string(sequence));
		*ptr = '\0';
		b->key = atoi(&key[1]);
	}

	/* Replace < or [ with NULL to make sure that it is not included
	   when parsing the modifiers. */
	if (key != NULL)
		key[0] = '\0';

	b->mod = 0;
	token = strtok_r(sequence, "-", &sequence);
	while (token) {
		if ((next = strtok_r(sequence, "-", &sequence)) || key != NULL) {
			if (!strcmp(token, "C"))
				b->mod |= WLR_MODIFIER_CTRL;
			else if (!strcmp(token, "M"))
				b->mod |= WLR_MODIFIER_ALT;
			else if (!strcmp(token, "S"))
				b->mod |= WLR_MODIFIER_SHIFT;
			else if (!strcmp(token, "s"))
				b->mod |= WLR_MODIFIER_LOGO; // Super (Mod4)
			else
				DSCM_ASSERT(0,
					    "Invalid modifier '~s' in bind sequence: ~s",
					    scm_from_locale_string(token),
					    scm_from_locale_string(sequence));
		} else {
			SCM keycode = scm_hash_ref(
				keycodes, scm_from_locale_string(token), SCM_UNDEFINED);
			DSCM_ASSERT((!scm_is_false(keycode)),
				    "Invalid keysym in bind sequence: ~s",
				    scm_from_locale_string(sequence));
			b->key = (xkb_keycode_t)scm_to_uint32(keycode);
		}
		token = next;
	}
}

static inline SCM
dscm_call_eval(void *data)
{
	dscm_eval_call_data_t *call_data = data;
	SCM eval = scm_c_eval_string(call_data->exp);
	SCM evalstr = scm_object_to_string(eval, SCM_UNDEFINED);
	char *result = scm_to_locale_string(evalstr);
	(*(call_data->callback))(call_data->resource, result,
				 DSCM_V1_EVAL_STATUS_SUCCESS);
	free(call_data->exp);
	free(call_data);
	free(result);
	return SCM_BOOL_T;
}

static inline SCM
dscm_call_thread_handler(void *data, SCM key, SCM args)
{
	dscm_eval_call_data_t *call_data = data;
	SCM error = scm_apply_2(
		scm_c_public_ref("guile", "format"),
		SCM_BOOL_F,
		scm_cadr(args),
		scm_caddr(args));
	char *errorstr = scm_to_locale_string(error);
	fprintf(stderr, "dscm: error in eval of %s\n=> %s\n",
		call_data->exp, errorstr);
	(*(call_data->callback))(call_data->resource, errorstr,
				 DSCM_V1_EVAL_STATUS_ERROR);
	free(errorstr);
	free(call_data->exp);
	free(call_data);
	return SCM_BOOL_T;
}

static inline void*
dscm_call_action(void *data)
{
	return scm_call_0(((dscm_call_data_t*)data)->proc);
}

static inline void*
dscm_call_arrange(void *data)
{
	dscm_call_data_t *proc_data = (dscm_call_data_t*)data;
	SCM mon = scm_from_pointer(proc_data->args, NULL);
	return scm_call_1(proc_data->proc, mon);
}

static inline void
dscm_safe_call(unsigned int type, scm_t_bits *proc_ptr, void *data)
{
	if (proc_ptr == NULL)
		die("dscm: could not call proc that is NULL");
	SCM proc = SCM_PACK_POINTER(proc_ptr);
	void*(*func)(void*) = type == DSCM_CALL_ARRANGE ?
		&dscm_call_arrange : &dscm_call_action;
	scm_c_with_continuation_barrier(
		func, &((dscm_call_data_t){.proc = proc, .args = data}));
}

static inline void
dscm_thread_eval(struct wl_resource *resource,
		void (*callback)(struct wl_resource*, char*, uint32_t),
		char *exp)
{
	dscm_eval_call_data_t *proc_data = ecalloc(1, sizeof(dscm_eval_call_data_t));
	proc_data->exp = exp;
	proc_data->resource = resource;
	proc_data->callback = callback;
	scm_spawn_thread(dscm_call_eval, proc_data,
			 dscm_call_thread_handler, proc_data);
}
