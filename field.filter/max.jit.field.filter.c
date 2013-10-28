#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object
#include "jpatcher_api.h"
#include "jit.common.h"

#define MAX_FIELD_PLANES (3)

typedef struct _max_jit_field_filter 
{
	t_object	ob;
	void		*obex;		
	void 		*dumpout;
	void 		*out1;
	
} t_max_jit_field_filter;

void *max_jit_field_filter_new(t_symbol *s, long argc, t_atom *argv);
void max_jit_field_filter_free(t_max_jit_field_filter *x);


void max_jit_field_filter_jit_field(t_max_jit_field_filter *x, t_symbol *name, long argc, t_atom *argv);
void max_jit_field_filter_assist(t_max_jit_field_filter *x, void *b, long io, long index, char *s);
void max_jit_field_filter_dumpout(t_max_jit_field_filter *x, t_symbol *s, short argc, t_atom *argv);

t_class *max_jit_field_filter_class;


int C74_EXPORT main(void)
{
	void *p,*q;
	//long attrflags;
	//void *attr;
	
	t_object *dummy = newinstance(gensym("jit.field"),0,0);
	freeobject(dummy);
	
	setup(&max_jit_field_filter_class, max_jit_field_filter_new, (method)max_jit_field_filter_free, (short)sizeof(t_max_jit_field_filter), 0L, A_GIMME, 0);
	p = max_jit_classex_setup(calcoffset(t_max_jit_field_filter,obex));
	q = jit_class_findbyname(gensym("jit_field_filter"));
	addmess((method)max_jit_field_filter_jit_field, "jit_field", A_GIMME,0);
	addmess((method)max_jit_field_filter_assist, "assist", A_CANT,0);
	
	//attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW;
	
    max_jit_classex_standard_wrap(p,q,0);
	
	return 0;
}

void max_jit_field_filter_jit_field(t_max_jit_field_filter *x, t_symbol *name, long argc, t_atom *argv)
{
	void *o = max_jit_obex_jitob_get(x);
	
	if(argc && argv) {
		jit_object_method(o, name, name, argc, argv);
		outlet_anything(x->ob.o_outlet, gensym("jit_field"), 1, argv);
	}
}

void max_jit_field_filter_assist(t_max_jit_field_filter *x, void *b, long io, long index, char *s)
{
	if (io == 1) //input
	{
		if(index == 0) sprintf(s,"messages in");
	}
	else	//output
	{
		if(index == 0) sprintf(s, "processed field");
		else if(index == 1) sprintf(s, "dumpout");
	}
}

void max_jit_field_filter_dumpout(t_max_jit_field_filter *x, t_symbol *s, short argc, t_atom *argv)
{
	outlet_anything(x->dumpout,s,argc,argv);	
}
	
void max_jit_field_filter_free(t_max_jit_field_filter *x)
{
	jit_object_free(max_jit_obex_jitob_get(x));
	max_jit_obex_free(x);
}

void *max_jit_field_filter_new(t_symbol *s, long argc, t_atom *argv)
{
	t_max_jit_field_filter *x;
	
	if (x=(t_max_jit_field_filter *)max_jit_obex_new(max_jit_field_filter_class,gensym("jit_field_filter"))) {
		void *o;
		if ((o=jit_object_new(gensym("jit_field_filter")))) {
			max_jit_obex_jitob_set(x,o);
			max_jit_obex_dumpout_set(x, x->dumpout=outlet_new(x,0L));
			max_jit_attr_args(x,argc,argv);
		} 
		else {
			jit_object_error((t_object *)x,"jit.field.filter: could not allocate object");
			freeobject((t_object *) x);
			x = NULL;
		}
	}
	return (x);
}