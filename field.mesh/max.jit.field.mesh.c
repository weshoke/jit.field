#include "jit.common.h"

typedef struct _max_jit_field_mesh 
{
	t_object	ob;
	void		*obex;		
	void 		*dumpout;
	void 		*out1;

} t_max_jit_field_mesh;

void *max_jit_field_mesh_new(t_symbol *s, long argc, t_atom *argv);
void max_jit_field_mesh_free(t_max_jit_field_mesh *x);

void max_jit_field_mesh_bang(t_max_jit_field_mesh *x);
void max_jit_field_mesh_jit_field(t_max_jit_field_mesh *x, t_symbol *name, long argc, t_atom *argv);
void max_jit_field_mesh_assist(t_max_jit_field_mesh *x, void *b, long io, long index, char *s);
void max_jit_field_mesh_dumpout(t_max_jit_field_mesh *x, t_symbol *s, short argc, t_atom *argv);
void max_jit_field_mesh_output_mesh_geometry(t_max_jit_field_mesh *x);

t_class *max_jit_field_mesh_class;

int C74_EXPORT main(void)
{
	void *p,*q;
	
	t_object *dummy = newinstance(gensym("jit.field"),0,0);
	freeobject(dummy);
	
	setup(&max_jit_field_mesh_class, max_jit_field_mesh_new, (method)max_jit_field_mesh_free, (short)sizeof(t_max_jit_field_mesh), 0L, A_GIMME, 0);
	p = max_jit_classex_setup(calcoffset(t_max_jit_field_mesh,obex));
	q = jit_class_findbyname(gensym("jit_field_mesh"));
	addbang((method)max_jit_field_mesh_bang);
	addmess((method)max_jit_field_mesh_jit_field, "jit_field", A_GIMME,0);
    addmess((method)max_jit_field_mesh_assist, "assist", A_CANT,0);
	max_jit_classex_standard_wrap(p,q,0);
	return 0;
}

void max_jit_field_mesh_bang(t_max_jit_field_mesh *x)
{
	max_jit_field_mesh_output_mesh_geometry(x);
}

void max_jit_field_mesh_jit_field(t_max_jit_field_mesh *x, t_symbol *name, long argc, t_atom *argv)
{
	void *o = max_jit_obex_jitob_get(x);
	jit_object_method(o, name, name, argc, argv);
	max_jit_field_mesh_output_mesh_geometry(x);
}

void max_jit_field_mesh_output_mesh_geometry(t_max_jit_field_mesh *x)
{
	t_atom a[1];
	void *o = max_jit_obex_jitob_get(x);
	void *vm = jit_object_method(o, gensym("get_vertex_matrix"));
	void *im = jit_object_method(o, gensym("get_index_matrix"));
	t_symbol *vname = jit_attr_getsym(vm, gensym("name"));
	t_symbol *iname = jit_attr_getsym(im, gensym("name"));
	
	atom_setsym(a, iname);
	outlet_anything(x->ob.o_outlet, gensym("index_matrix"), 1, a);
	
	if(jit_attr_getlong(o, gensym("normals")) == 1) {
		void *nm = jit_object_method(o, gensym("get_normal_matrix"));
		t_symbol *nname = jit_attr_getsym(nm, gensym("name"));
		atom_setsym(a, nname);
		outlet_anything(x->ob.o_outlet, gensym("normal_matrix"), 1, a);
	}
	
	atom_setsym(a, vname);
	outlet_anything(x->ob.o_outlet, gensym("vertex_matrix"), 1, a);
}

void max_jit_field_mesh_assist(t_max_jit_field_mesh *x, void *b, long io, long index, char *s)
{
	if (io == 1) //input
	{
		if(index == 0) sprintf(s,"messages in");
	}
	else	//output
	{
		if(index == 0) sprintf(s, "mesh matrices");
		else if(index == 1) sprintf(s, "dumpout");
	}
}

void max_jit_field_mesh_dumpout(t_max_jit_field_mesh *x, t_symbol *s, short argc, t_atom *argv)
{
	outlet_anything(x->dumpout,s,argc,argv);	
}
	
void max_jit_field_mesh_free(t_max_jit_field_mesh *x)
{
	jit_object_free(max_jit_obex_jitob_get(x));
	max_jit_obex_free(x);
}

void *max_jit_field_mesh_new(t_symbol *s, long argc, t_atom *argv)
{
	t_max_jit_field_mesh *x;
	
	if (x=(t_max_jit_field_mesh *)max_jit_obex_new(max_jit_field_mesh_class,gensym("jit_field_mesh"))) {
		void *o;
		if ((o=jit_object_new(gensym("jit_field_mesh")))) {
			max_jit_obex_jitob_set(x,o);
			max_jit_obex_dumpout_set(x, x->dumpout=outlet_new(x,0L));
			x->out1 = outlet_new(x, NULL);						
			max_jit_attr_args(x,argc,argv);			
		} 
		else {
			jit_object_error((t_object *)x,"jit.field.mesh: could not allocate object");
			freeobject((t_object *) x);
			x = NULL;
		}
	}
	return (x);
}