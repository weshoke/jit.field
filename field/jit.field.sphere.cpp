#include "jit.common.h"
#include "jit.vecmath.h"
#include "jit.field.grid.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _jit_field_sphere
{
	t_object	ob;
	
	float		center[3];
	float		radius;
	float		halfwidth;

	t_object	*field;
} t_jit_field_sphere;

void *_jit_field_sphere_class;

t_jit_field_sphere *jit_field_sphere_new(void);
void jit_field_sphere_free(t_jit_field_sphere *x);
t_jit_err jit_field_sphere_create(t_jit_field_sphere *x, t_symbol *name, long arc, t_atom *argv);
void * jit_field_sphere_getfield(t_jit_field_sphere *x);

t_jit_err jit_field_sphere_init(void)
{
	long attrflags=0;
	t_jit_object *attr;
		
	_jit_field_sphere_class = jit_class_new("jit_field_sphere",(method)jit_field_sphere_new,(method)jit_field_sphere_free,
		sizeof(t_jit_field_sphere),0L);

	// add attributes	
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW;
	
	jit_class_addmethod(_jit_field_sphere_class, (method)jit_field_sphere_create, "create",	A_GIMME, 0L);
	jit_class_addmethod(_jit_field_sphere_class, (method)jit_field_sphere_getfield, "getfield",	A_CANT, 0L);
	
	attr = (t_jit_object *)jit_object_new(_jit_sym_jit_attr_offset_array, "center", _jit_sym_float32, 3, attrflags,
		(method)0L, (method)0L, NULL, calcoffset(t_jit_field_sphere, center));
	jit_class_addattr(_jit_field_sphere_class, attr);
	
	attr = (t_jit_object *)jit_object_new(_jit_sym_jit_attr_offset, "radius", _jit_sym_float32, attrflags,
				(method)0L, (method)0L, calcoffset(t_jit_field_sphere, radius));
    jit_class_addattr(_jit_field_sphere_class, (t_jit_object*)attr);
	
	attr = (t_jit_object *)jit_object_new(_jit_sym_jit_attr_offset, "halfwidth", _jit_sym_float32, attrflags,
				(method)0L, (method)0L, calcoffset(t_jit_field_sphere, halfwidth));
    jit_class_addattr(_jit_field_sphere_class, (t_jit_object*)attr);

	jit_class_register(_jit_field_sphere_class);

	return JIT_ERR_NONE;
}

t_jit_err jit_field_sphere_create(t_jit_field_sphere *x, t_symbol *name, long arc, t_atom *argv)
{
	t_jit_err err = JIT_ERR_NONE;
	jit_object_method(x->field, gensym("sphere"), x->center, &x->radius, &x->halfwidth);
	return err;
}

void * jit_field_sphere_getfield(t_jit_field_sphere *x)
{
	return x->field;
}


t_jit_field_sphere *jit_field_sphere_new(void)
{
	t_jit_field_sphere *x;
		
	if((x=(t_jit_field_sphere *)jit_object_alloc(_jit_field_sphere_class))) {
		x->field = (t_object *)jit_object_new(gensym("jit_field"));
		
		x->center[0] = 0;
		x->center[1] = 0;
		x->center[2] = 0;
		x->radius = 1;
		x->halfwidth = openvdb::OPENVDB_VERSION_NAME::LEVEL_SET_HALF_WIDTH;
	}
	else {
		x = NULL;
	}	
	return x;
}

void jit_field_sphere_free(t_jit_field_sphere *x)
{
	if(x->field) {
		object_free(x->field);
		x->field = NULL;
	}
}


#ifdef __cplusplus
}
#endif