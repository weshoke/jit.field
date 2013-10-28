#include "jit.common.h"
#include "jit.vecmath.h"
#include "jit.field.grid.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _jit_field_particles
{
	t_object	ob;
	float		radius;
	t_object	*field;
} t_jit_field_particles;

void *_jit_field_particles_class;

t_jit_field_particles *jit_field_particles_new(void);
void jit_field_particles_free(t_jit_field_particles *x);
void * jit_field_particles_getfield(t_jit_field_particles *x);
t_jit_err jit_field_particles_jit_matrix(t_jit_field_particles *x, t_symbol *s, int argc, t_atom *argv);

t_jit_err jit_field_particles_init(void)
{
	long attrflags=0;
	t_jit_object *attr;
		
	_jit_field_particles_class = jit_class_new("jit_field_particles",(method)jit_field_particles_new,(method)jit_field_particles_free,
		sizeof(t_jit_field_particles),0L);

	// add attributes	
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW;
	
	jit_class_addmethod(_jit_field_particles_class, (method)jit_field_particles_getfield, "getfield",	A_CANT, 0L);
	jit_class_addmethod(_jit_field_particles_class, (method)jit_field_particles_jit_matrix, "jit_matrix", A_GIMME, 0L);
	
	attr = (t_jit_object *)jit_object_new(_jit_sym_jit_attr_offset, "radius", _jit_sym_float32, attrflags,
				(method)0L, (method)0L, calcoffset(t_jit_field_particles, radius));
    jit_class_addattr(_jit_field_particles_class, (t_jit_object*)attr);

	jit_class_register(_jit_field_particles_class);

	return JIT_ERR_NONE;
}

void * jit_field_particles_getfield(t_jit_field_particles *x)
{
	return x->field;
}

t_jit_err jit_field_particles_jit_matrix(t_jit_field_particles *x, t_symbol *s, int argc, t_atom *argv)
{
	t_jit_err err = JIT_ERR_NONE;
	if(argc && argv) {
		t_symbol *matrix_name = jit_atom_getsym(argv);
		t_jit_object *matrix = NULL;
		
		if((matrix_name == _jit_sym_jit_matrix) && (argc > 1)) {
			matrix_name = jit_atom_getsym(argv+1);
		}
		
		if(argv->a_type == A_OBJ) {
			matrix = (t_jit_object *)jit_atom_getobj(argv);
		}
		else if(matrix_name) {
			matrix = (t_jit_object*)jit_object_findregistered(matrix_name);
			if(!matrix) {
				post ("jit.field.particles: couldn't get matrix object!");
				return JIT_ERR_GENERIC;
			}
		}
		
		if(matrix) {
			jit_object_method(x->field, gensym("particles"), matrix, &x->radius);
		}
	}
	return err;
}

t_jit_field_particles *jit_field_particles_new(void)
{
	t_jit_field_particles *x;
		
	if((x=(t_jit_field_particles *)jit_object_alloc(_jit_field_particles_class))) {
		x->field = (t_object *)jit_object_new(gensym("jit_field"));
		x->radius = 0.5;
	}
	else {
		x = NULL;
	}	
	return x;
}

void jit_field_particles_free(t_jit_field_particles *x)
{
	if(x->field) {
		object_free(x->field);
		x->field = NULL;
	}
}


#ifdef __cplusplus
}
#endif