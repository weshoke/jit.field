#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object
#include "jpatcher_api.h"
#include "jit.common.h"

typedef struct _max_jit_field_particles 
{
	t_object	ob;
	void		*obex;		
	void 		*dumpout;
	void 		*out1;

} t_max_jit_field_particles;

void *max_jit_field_particles_new(t_symbol *s, long argc, t_atom *argv);
void max_jit_field_particles_free(t_max_jit_field_particles *x);

void max_jit_field_particles_jit_matrix(t_max_jit_field_particles *x, t_symbol *name, long argc, t_atom *argv);
void max_jit_field_particles_assist(t_max_jit_field_particles *x, void *b, long io, long index, char *s);
void max_jit_field_particles_dumpout(t_max_jit_field_particles *x, t_symbol *s, short argc, t_atom *argv);

t_class *max_jit_field_particles_class;

int C74_EXPORT main(void)
{
	void *p,*q;
	
	// instantiate jit.phys.world object
	t_object *dummy = newinstance(gensym("jit.field"),0,0);
	freeobject(dummy);
	
	setup(&max_jit_field_particles_class, max_jit_field_particles_new, (method)max_jit_field_particles_free, (short)sizeof(t_max_jit_field_particles), 0L, A_GIMME, 0);

	p = max_jit_classex_setup(calcoffset(t_max_jit_field_particles,obex));
	q = jit_class_findbyname(gensym("jit_field_particles"));
	addmess((method)max_jit_field_particles_jit_matrix, "jit_matrix", A_GIMME,0);
    addmess((method)max_jit_field_particles_assist, "assist", A_CANT,0);
	max_jit_classex_standard_wrap(p,q,0);
	
	return 0;
}

void max_jit_field_particles_jit_matrix(t_max_jit_field_particles *x, t_symbol *name, long argc, t_atom *argv)
{
	void *o = max_jit_obex_jitob_get(x);
	void *field = NULL;
	t_atom a[1];
	
	if(argc && argv) {
		jit_object_method(o, name, name, argc, argv);
		
		field = object_method(o, gensym("getfield"));
		if(!field) object_error((t_object *)x, "unable to get field object");
		
		atom_setsym(a, jit_attr_getsym(field, gensym("name")));
		outlet_anything(x->ob.o_outlet, gensym("jit_field"), 1, a);
	}
}

void max_jit_field_particles_assist(t_max_jit_field_particles *x, void *b, long io, long index, char *s)
{
	if (io == 1) //input
	{
		if(index == 0) sprintf(s,"messages in");
	}
	else	//output
	{
		if(index == 0) sprintf(s, "jitter matrix");
		else if(index == 1) sprintf(s, "dumpout");
	}
}

void max_jit_field_particles_dumpout(t_max_jit_field_particles *x, t_symbol *s, short argc, t_atom *argv)
{
	outlet_anything(x->dumpout,s,argc,argv);	
}
	
void max_jit_field_particles_free(t_max_jit_field_particles *x)
{
	jit_object_free(max_jit_obex_jitob_get(x));
	max_jit_obex_free(x);
}

void *max_jit_field_particles_new(t_symbol *s, long argc, t_atom *argv)
{
	t_max_jit_field_particles *x;
	void *o;
//	long attrstart;

	if (x=(t_max_jit_field_particles *)max_jit_obex_new(max_jit_field_particles_class,gensym("jit_field_particles"))) {
		//attrstart = max_jit_attr_args_offset(argc,argv);
		if ((o=jit_object_new(gensym("jit_field_particles")))) {
			max_jit_obex_jitob_set(x,o);
			max_jit_obex_dumpout_set(x, x->dumpout=outlet_new(x,0L));
			x->out1 = outlet_new(x, NULL);
			max_jit_attr_args(x,argc,argv);			
		} 
		else {
			jit_object_error((t_object *)x,"jit.field.particles: could not allocate object");
			freeobject((t_object *) x);
			x = NULL;
		}
	}
	return (x);
}