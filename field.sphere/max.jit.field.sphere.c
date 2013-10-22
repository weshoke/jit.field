#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object
#include "jpatcher_api.h"
#include "jit.common.h"

typedef struct _max_jit_field_sphere 
{
	t_object	ob;
	void		*obex;		
	void 		*dumpout;
	void 		*out1;

} t_max_jit_field_sphere;

void *max_jit_field_sphere_new(t_symbol *s, long argc, t_atom *argv);
void max_jit_field_sphere_free(t_max_jit_field_sphere *x);

void max_jit_field_sphere_bang(t_max_jit_field_sphere *x);
void max_jit_field_sphere_assist(t_max_jit_field_sphere *x, void *b, long io, long index, char *s);
void max_jit_field_sphere_dumpout(t_max_jit_field_sphere *x, t_symbol *s, short argc, t_atom *argv);

t_class *max_jit_field_sphere_class;

int C74_EXPORT main(void)
{
	void *p,*q;
	
	t_object *dummy = newinstance(gensym("jit.field"),0,0);
	freeobject(dummy);
	
	setup(&max_jit_field_sphere_class, max_jit_field_sphere_new, (method)max_jit_field_sphere_free, (short)sizeof(t_max_jit_field_sphere), 0L, A_GIMME, 0);
	p = max_jit_classex_setup(calcoffset(t_max_jit_field_sphere,obex));
	q = jit_class_findbyname(gensym("jit_field_sphere"));
	addbang((method)max_jit_field_sphere_bang);
    addmess((method)max_jit_field_sphere_assist, "assist", A_CANT,0);
	max_jit_classex_standard_wrap(p,q,0);
	
	return 0;
}

void max_jit_field_sphere_bang(t_max_jit_field_sphere *x)
{
	void *o = max_jit_obex_jitob_get(x);
	void *field = NULL;
	t_atom a[1];
	
	jit_object_method(o, gensym("create"));
	field = object_method(o, gensym("getfield"));
	if(!field) object_error((t_object *)x, "unable to get field object");
	
	atom_setsym(a, jit_attr_getsym(field, gensym("name")));
	outlet_anything(x->ob.o_outlet, gensym("jit_field"), 1, a);
}

void max_jit_field_sphere_assist(t_max_jit_field_sphere *x, void *b, long io, long index, char *s)
{
	if (io == 1) //input
	{
		if(index == 0) sprintf(s,"messages in");
	}
	else	//output
	{
		if(index == 0) sprintf(s, "field containing a sphere");
		else if(index == 1) sprintf(s, "dumpout");
	}
}

void max_jit_field_sphere_dumpout(t_max_jit_field_sphere *x, t_symbol *s, short argc, t_atom *argv)
{
	outlet_anything(x->dumpout,s,argc,argv);	
}
	
void max_jit_field_sphere_free(t_max_jit_field_sphere *x)
{
	jit_object_free(max_jit_obex_jitob_get(x));
	max_jit_obex_free(x);
}

void *max_jit_field_sphere_new(t_symbol *s, long argc, t_atom *argv)
{
	t_max_jit_field_sphere *x;
	void *o;
//	long attrstart;

	if (x=(t_max_jit_field_sphere *)max_jit_obex_new(max_jit_field_sphere_class,gensym("jit_field_sphere"))) {
		//attrstart = max_jit_attr_args_offset(argc,argv);
		if((o=jit_object_new(gensym("jit_field_sphere")))) {
			max_jit_obex_jitob_set(x,o);
			max_jit_obex_dumpout_set(x, x->dumpout=outlet_new(x,0L));
			x->out1 = outlet_new(x, NULL);						
			max_jit_attr_args(x,argc,argv);			
		} 
		else {
			jit_object_error((t_object *)x,"jit.field.sphere: could not allocate object");
			freeobject((t_object *) x);
			x = NULL;
		}
	}
	return (x);
}