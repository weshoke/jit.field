#include "jit.common.h"
#include "jit.vecmath.h"
#include "jit.field.grid.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _jit_field_filter
{
	t_object	ob;
	t_symbol	*filter;
	float		val;
} t_jit_field_filter;

void *_jit_field_filter_class;

t_jit_field_filter *jit_field_filter_new(void);
void jit_field_filter_free(t_jit_field_filter *x);
t_jit_err jit_field_filter_jit_field(t_jit_field_filter *x, t_symbol *name, long argc, t_atom *argv);

t_jit_err jit_field_filter_init(void)
{
	long attrflags=0;
	t_jit_object *attr;
		
	_jit_field_filter_class = jit_class_new("jit_field_filter",(method)jit_field_filter_new,(method)jit_field_filter_free,
		sizeof(t_jit_field_filter),0L);

	// add attributes	
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW;
	
	jit_class_addmethod(_jit_field_filter_class, (method)jit_field_filter_jit_field, "jit_field", A_GIMME, 0L);
	
	attr = (t_jit_object *)jit_object_new(_jit_sym_jit_attr_offset, "filter", _jit_sym_symbol, attrflags,
				(method)0L, (method)0L, calcoffset(t_jit_field_filter, filter));
    jit_class_addattr(_jit_field_filter_class, (t_jit_object*)attr);
	CLASS_ATTR_ENUM((t_class *)_jit_field_filter_class, "filter", 0, "cuvature laplacian gaussian offset median mean");

	attr = (t_jit_object *)jit_object_new(_jit_sym_jit_attr_offset, "val", _jit_sym_float32, attrflags,
				(method)0L, (method)0L, calcoffset(t_jit_field_filter, val));
    jit_class_addattr(_jit_field_filter_class, (t_jit_object*)attr);

	jit_class_register(_jit_field_filter_class);
	
	return JIT_ERR_NONE;
}

t_jit_err jit_field_filter_jit_field(t_jit_field_filter *x, t_symbol *name, long argc, t_atom *argv)
{
	t_jit_err err = JIT_ERR_NONE;

	if(argc && argv) {
		t_jit_field_grid *grid = (t_jit_field_grid *)jit_field_grid_from_atom(argv);
		if(!grid->grid->isLevelset()) {
			jit_object_error((t_object *)x, "filter inputs must be 1-plane float32 level set fields");
			return err;
		}
		
		if(x->filter == gensym("curvature")) {
			grid->grid->meanCurvatureFilter();
		}
		else if(x->filter == gensym("laplacian")) {
			grid->grid->laplacianFilter();
		}
		else if(x->filter == gensym("gaussian")) {
			grid->grid->gaussianFilter(x->val);
		}
		else if(x->filter == gensym("offset")) {
			grid->grid->offsetFilter(x->val);
		}
		else if(x->filter == gensym("median")) {
			grid->grid->medianFilter(x->val);
		}
		else if(x->filter == gensym("mean")) {
			grid->grid->meanFilter(x->val);
		}
	}
	
	return err;
}


t_jit_field_filter *jit_field_filter_new(void)
{
	t_jit_field_filter *x;
		
	if((x=(t_jit_field_filter *)jit_object_alloc(_jit_field_filter_class))) {
		x->filter = gensym("curvature");
		x->val = 1.;
	}
	else {
		x = NULL;
	}	
	return x;
}

void jit_field_filter_free(t_jit_field_filter *x)
{}


#ifdef __cplusplus
}
#endif