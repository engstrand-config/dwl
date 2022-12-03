#pragma once

enum { DSCM_CALL_ARRANGE, DSCM_CALL_ACTION };
typedef struct {
	SCM proc;
	void *args;
} dscm_call_data_t;

static inline SCM
dscm_alist_get(SCM alist, const char* key)
{
	return scm_assoc_ref(alist, scm_from_utf8_string(key));
}

static inline char*
dscm_alist_get_string(SCM alist, const char* key)
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
dscm_alist_get_proc_pointer(SCM alist, const char *key)
{
	scm_t_bits *proc = NULL;
	SCM value = dscm_alist_get(alist, key);
	if (scm_is_false(value))
		return proc;
	SCM eval = scm_primitive_eval(value);
	/* SCM_UNPACK_POINTER is only allowed on expressions where SCM_IMP is 0 */
	if (SCM_IMP(eval) == 1)
		die("dscm: invalid callback procedure. SCM_IMP(proc) = 1");
	if (scm_procedure_p(eval) == SCM_BOOL_T) {
		proc = SCM_UNPACK_POINTER(eval);
		scm_gc_protect_object(eval);
	}
	return proc;
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
	if (proc_ptr == NULL) {
		fprintf(stderr, "dscm: could not call proc that is NULL");
		return;
	}
	SCM proc = SCM_PACK_POINTER(proc_ptr);
	dscm_call_data_t proc_data = {.proc = proc, .args = data};
	void *(*func)(void*) = NULL;
	switch (type) {
	case DSCM_CALL_ARRANGE:
		func = &dscm_call_arrange;
		break;
	case DSCM_CALL_ACTION:
	default:
		func = &dscm_call_action;
	}
	scm_c_with_continuation_barrier(func, &proc_data);
}
