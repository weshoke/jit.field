#include "jit.common.h"

typedef struct _max_jit_field_volume
{
	t_object	ob;
	void		*obex;
	long		lastin;
	void		*proxy;
	void 		*dumpout;
	void 		*out1;
	t_symbol	*index_matrix_name;
} t_max_jit_field_volume;

void *max_jit_field_volume_new(t_symbol *s, long argc, t_atom *argv);
void max_jit_field_volume_free(t_max_jit_field_volume*x);

void max_jit_field_volume_bang(t_max_jit_field_volume*x);
void max_jit_field_volume_jit_matrix(t_max_jit_field_volume*x, t_symbol *name, long argc, t_atom *argv);
void max_jit_field_volume_assist(t_max_jit_field_volume*x, void *b, long io, long index, char *s);
void max_jit_field_volume_dumpout(t_max_jit_field_volume*x, t_symbol *s, short argc, t_atom *argv);

t_class *max_jit_field_volume_class;

int C74_EXPORT main(void)
{
	void *p,*q;
	
	t_object *dummy = newinstance(gensym("jit.field"),0,0);
	freeobject(dummy);
	
	setup(&max_jit_field_volume_class, max_jit_field_volume_new, (method)max_jit_field_volume_free, (short)sizeof(t_max_jit_field_volume), 0L, A_GIMME, 0);
	p = max_jit_classex_setup(calcoffset(t_max_jit_field_volume,obex));
	q = jit_class_findbyname(gensym("jit_field_volume"));
	addbang((method)max_jit_field_volume_bang);
	addmess((method)max_jit_field_volume_jit_matrix, "jit_matrix", A_GIMME,0);
    addmess((method)max_jit_field_volume_assist, "assist", A_CANT,0);
	max_jit_classex_standard_wrap(p,q,0);
	return 0;
}

void max_jit_field_volume_bang(t_max_jit_field_volume *x)
{
	t_atom a[1];
	void *o = max_jit_obex_jitob_get(x);
	void *field = jit_object_method(o, gensym("getfield"));
	jit_atom_setsym(a, jit_attr_getsym(field, gensym("name")));
	outlet_anything(x->ob.o_outlet, gensym("jit_matrix"), 1, a);
}

void * max_jit_field_volume_get_matrix(t_symbol *name) {
	if(name != _jit_sym_nothing) {
		return jit_object_findregistered(name);
	}
	return NULL;
}

void max_jit_field_volume_jit_matrix(t_max_jit_field_volume *x, t_symbol *name, long argc, t_atom *argv)
{
	t_atom a[1];
	long innum = proxy_getinlet((t_object *)x);
	if(innum == 1) {
		if(argc && argv) {
			x->index_matrix_name = jit_atom_getsym(argv);
		}
		else {
			x->index_matrix_name = _jit_sym_nothing;
		}
	}
	else if(argc && argv) {
		void *o = max_jit_obex_jitob_get(x);
		t_symbol *vertex_matrix_name = jit_atom_getsym(argv);
		void *index_matrix = max_jit_field_volume_get_matrix(x->index_matrix_name);
		void *vertex_matrix = max_jit_field_volume_get_matrix(vertex_matrix_name);
		
		if(vertex_matrix) {
			void *field = jit_object_method(o, gensym("getfield"));
			jit_object_method(o, gensym("mesh_to_volume"), vertex_matrix, index_matrix);
			jit_atom_setsym(a, jit_attr_getsym(field, gensym("name")));
			outlet_anything(x->ob.o_outlet, gensym("jit_field"), 1, a);
		}
		else {
			if(!vertex_matrix) object_error((t_object *)x, "couldn't get vertex matrix %s", vertex_matrix_name->s_name);
		}
	}
}

void max_jit_field_volume_assist(t_max_jit_field_volume *x, void *b, long io, long index, char *s)
{
	if (io == 1) //input
	{
		if(index == 0) sprintf(s,"messages in");
	}
	else	//output
	{
		if(index == 0) sprintf(s, "field");
		else if(index == 1) sprintf(s, "dumpout");
	}
}

void max_jit_field_volume_dumpout(t_max_jit_field_volume *x, t_symbol *s, short argc, t_atom *argv)
{
	outlet_anything(x->dumpout,s,argc,argv);	
}
	
void max_jit_field_volume_free(t_max_jit_field_volume*x)
{
	jit_object_free(max_jit_obex_jitob_get(x));
	max_jit_obex_free(x);
}

void *max_jit_field_volume_new(t_symbol *s, long argc, t_atom *argv)
{
	t_max_jit_field_volume *x;
	
	if (x=(t_max_jit_field_volume*)max_jit_obex_new(max_jit_field_volume_class,gensym("jit_field_volume"))) {
		void *o;
		if ((o=jit_object_new(gensym("jit_field_volume")))) {
			max_jit_obex_jitob_set(x,o);
			max_jit_obex_dumpout_set(x, x->dumpout=outlet_new(x,0L));
			x->lastin = 0;
			x->proxy = proxy_new((t_object *)x, 1, &x->lastin);
			x->out1 = outlet_new(x, NULL);
			x->index_matrix_name = _jit_sym_nothing;
			max_jit_attr_args(x,argc,argv);			
		} 
		else {
			jit_object_error((t_object *)x,"jit.field.volume: could not allocate object");
			freeobject((t_object *) x);
			x = NULL;
		}
	}
	return (x);
}