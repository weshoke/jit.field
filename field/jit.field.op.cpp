#include "jit.common.h"
#include "jit.vecmath.h"
#include "jit.field.grid.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _jit_field_op
{
	t_object	ob;
	t_symbol	*op;
	t_object	*in2field;
	t_object	*field;
} t_jit_field_op;

void *_jit_field_op_class;
t_symbol *ps_jit_field_op_max;
t_symbol *ps_jit_field_op_min;
t_symbol *ps_jit_field_op_add;
t_symbol *ps_jit_field_op_mul;
t_symbol *ps_jit_field_op_plus;
t_symbol *ps_jit_field_op_star;

t_jit_field_op *jit_field_op_new(void);
void jit_field_op_free(t_jit_field_op *x);
void * jit_field_op_getfield(t_jit_field_op *x);
t_jit_err jit_field_op_jit_field(t_jit_field_op *x, t_symbol *name, long argc, t_atom *argv);

t_jit_err jit_field_op_init(void)
{
	long attrflags=0;
	t_jit_object *attr;
		
	_jit_field_op_class = jit_class_new("jit_field_op",(method)jit_field_op_new,(method)jit_field_op_free,
		sizeof(t_jit_field_op),0L);

	//add attributes	
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW;
	
	jit_class_addmethod(_jit_field_op_class, (method)jit_field_op_getfield, "getfield",	A_CANT, 0L);
	jit_class_addmethod(_jit_field_op_class, (method)jit_field_op_jit_field, "jit_field", A_GIMME, 0L);
	
	attr = (t_jit_object *)jit_object_new(_jit_sym_jit_attr_offset, "op", _jit_sym_symbol, attrflags,
				(method)0L, (method)0L, calcoffset(t_jit_field_op, op));
    jit_class_addattr(_jit_field_op_class, (t_jit_object*)attr);
	CLASS_ATTR_ENUM((t_class *)_jit_field_op_class, "op", 0, "max min + *");

	jit_class_register(_jit_field_op_class);
	
	ps_jit_field_op_max = gensym("max");
	ps_jit_field_op_min = gensym("min");
	ps_jit_field_op_add = gensym("add");
	ps_jit_field_op_mul = gensym("mul");
	ps_jit_field_op_plus = gensym("+");
	ps_jit_field_op_star = gensym("*");

	return JIT_ERR_NONE;
}

void * jit_field_op_getfield(t_jit_field_op *x)
{
	return x->field;
}

t_jit_err jit_field_op_jit_field(t_jit_field_op *x, t_symbol *name, long argc, t_atom *argv)
{
	t_jit_err err = JIT_ERR_NONE;
	t_object *o1 = NULL;
	t_object *o2 = NULL;
	
	if(argc && argv) {
		o1 = (t_object *)jit_field_grid_from_atom(argv);
		if(argc >= 2) {
			if(argv[1].a_type == A_FLOAT || argv[1].a_type == A_LONG) {
				o2 = (t_object *)jit_object_method(x->in2field, gensym("getgrid"));
				jit_attr_setlong(o2, gensym("planecount"), jit_attr_getlong(o1, gensym("planecount")));
				jit_attr_setsym(o2, gensym("type"), jit_attr_getsym(o1, gensym("type")));
				jit_object_method(o2, gensym("set_background_value"), gensym("set_background_value"), argc-1, argv+1);
			}
			else {
				o2 = (t_object *)jit_field_grid_from_atom(argv+1);
			}
		}
	}
	
	if(o1 && o2) {
		long done = 1;
		if(x->op == ps_jit_field_op_max) {
			object_method(x->field, ps_jit_field_op_max, o1, o2);
		}
		else if(x->op == ps_jit_field_op_min) {
			object_method(x->field, ps_jit_field_op_min, o1, o2);
		}
		else if(x->op == ps_jit_field_op_plus) {
			object_method(x->field, ps_jit_field_op_add, o1, o2);
		}
		else if(x->op == ps_jit_field_op_star) {
			object_method(x->field, ps_jit_field_op_mul, o1, o2);
		}
		else {
			done = 0;
		}
		if(done == 1) return JIT_ERR_NONE;
	}
	
	if(o1) {
		if(x->op == gensym("normalize")) {
			object_method(x->field, gensym("normalize"), o1);
		}
		else if(x->op == gensym("magnitude")) {
			object_method(x->field, gensym("magnitude"), o1);
		}
		else if(x->op == gensym("curvature")) {
			object_method(x->field, gensym("curvature"), o1);
		}
		else if(x->op == gensym("laplacian")) {
			object_method(x->field, gensym("laplacian"), o1);
		}
		else if(x->op == gensym("gradient")) {
			object_method(x->field, gensym("gradient"), o1);
		}
		else if(x->op == gensym("divergence")) {
			object_method(x->field, gensym("divergence"), o1);
		}
		else if(x->op == gensym("curl")) {
			object_method(x->field, gensym("curl"), o1);
		}
	}
	return err;
}


t_jit_field_op *jit_field_op_new(void)
{
	t_jit_field_op *x;
		
	if((x=(t_jit_field_op *)jit_object_alloc(_jit_field_op_class))) {
		x->in2field = (t_object *)jit_object_new(gensym("jit_field"));
		x->field = (t_object *)jit_object_new(gensym("jit_field"));
		x->op = gensym("max");
	}
	else {
		x = NULL;
	}	
	return x;
}

void jit_field_op_free(t_jit_field_op *x)
{
	if(x->in2field) {
		object_free(x->in2field);
		x->in2field = NULL;
	}
	if(x->field) {
		object_free(x->field);
		x->field = NULL;
	}
}


#ifdef __cplusplus
}
#endif