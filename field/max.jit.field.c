#include "jit.common.h"
#include "commonsyms.h"

typedef struct _max_jit_field 
{
	t_object		ob;
	void			*obex;
	void			*fieldout;
} t_max_jit_field;

t_jit_err jit_field_init(void);

void *max_jit_field_new(t_symbol *s, long argc, t_atom *argv);
void max_jit_field_free(t_max_jit_field *x);
void max_jit_field_bang(t_max_jit_field *x);
void max_jit_field_getcoord(t_max_jit_field *x, t_symbol *s, short argc, t_atom *argv);
void max_jit_field_getpos(t_max_jit_field *x, t_symbol *s, short argc, t_atom *argv);
void max_jit_field_indextoworld(t_max_jit_field *x, t_symbol *s, short argc, t_atom *argv);
void max_jit_field_worldtoindex(t_max_jit_field *x, t_symbol *s, short argc, t_atom *argv);
void max_jit_field_jit_field(t_max_jit_field *x, t_symbol *s, short argc, t_atom *argv);

t_class *max_jit_field_class;

int C74_EXPORT main(void)
{	
	void *p,*q;
	
	common_symbols_init();
	jit_field_init();
	
	setup(&max_jit_field_class, max_jit_field_new, (method)max_jit_field_free, (short)sizeof(t_max_jit_field), 
		0L, A_GIMME, 0);

	p = max_jit_classex_setup(calcoffset(t_max_jit_field,obex));
	q = jit_class_findbyname(gensym("jit_field"));    
	addbang((method)max_jit_field_bang);
	max_addmethod_defer_low((method)max_jit_field_getcoord, "getcoord");
	max_addmethod_defer_low((method)max_jit_field_getpos, "getpos");
	max_addmethod_defer_low((method)max_jit_field_indextoworld, "indextoworld");
	max_addmethod_defer_low((method)max_jit_field_worldtoindex, "worldtoindex");
	addmess((method)max_jit_field_jit_field, "jit_field", A_GIMME,0);
	
	max_jit_classex_standard_wrap(p,q,0);
	return 0;
}

void max_jit_field_bang(t_max_jit_field *x)
{
	void *o = max_jit_obex_jitob_get(x);
	t_atom a[1];
	atom_setsym(a, jit_attr_getsym(o, gensym("name")));
	outlet_anything(x->fieldout, gensym("jit_field"), 1, a);
}

void max_jit_field_getcoord(t_max_jit_field *x, t_symbol *s, short argc, t_atom *argv)
{
	void *o = max_jit_obex_jitob_get(x);
	if (o) {
		long rac = 0;
		t_atom *rav = NULL;
		
		if(jit_object_method(o, gensym("getcoord"), gensym("getcoord"), argc, argv, &rac, &rav)) {
			return;
		}
		
		if(rac && rav) {
			max_jit_obex_dumpout(x, gensym("coord"), rac, rav);	
			jit_freebytes(rav, sizeof(t_atom)*rac);
		}
	}	
}

void max_jit_field_getpos(t_max_jit_field *x, t_symbol *s, short argc, t_atom *argv) {
	void *o = max_jit_obex_jitob_get(x);
	if (o) {
		long rac = 0;
		t_atom *rav = NULL;
		
		if(jit_object_method(o, gensym("getpos"), gensym("getpos"), argc, argv, &rac, &rav)) {
			return;
		}
		
		if(rac && rav) {
			max_jit_obex_dumpout(x, gensym("pos"), rac, rav);	
			jit_freebytes(rav, sizeof(t_atom)*rac);
		}
	}
}

void max_jit_field_indextoworld(t_max_jit_field *x, t_symbol *s, short argc, t_atom *argv)
{
	void *o = max_jit_obex_jitob_get(x);
	if(o && argc >= 3) {
		long ijk[3];
		float xyz[3];
		t_atom a[3];
		
		ijk[0] = jit_atom_getlong(argv);
		ijk[1] = jit_atom_getlong(argv+1);
		ijk[2] = jit_atom_getlong(argv+2);
		jit_object_method(o, gensym("indextoworld"), ijk, xyz);
		
		atom_setfloat(a, xyz[0]);
		atom_setfloat(a+1, xyz[1]);
		atom_setfloat(a+2, xyz[2]);
		max_jit_obex_dumpout(x, gensym("world"), 3, a);
	}
}

void max_jit_field_worldtoindex(t_max_jit_field *x, t_symbol *s, short argc, t_atom *argv)
{
	void *o = max_jit_obex_jitob_get(x);
	if(o && argc >= 3) {
		long ijk[3];
		float xyz[3];
		t_atom a[3];
		
		xyz[0] = jit_atom_getfloat(argv);
		xyz[1] = jit_atom_getfloat(argv+1);
		xyz[2] = jit_atom_getfloat(argv+2);
		jit_object_method(o, gensym("worldtoindex"), xyz, ijk);
		
		atom_setlong(a, ijk[0]);
		atom_setlong(a+1, ijk[1]);
		atom_setlong(a+2, ijk[2]);
		max_jit_obex_dumpout(x, gensym("index"), 3, a);
	}
}

void max_jit_field_jit_field(t_max_jit_field *x, t_symbol *s, short argc, t_atom *argv) {
	void *o = max_jit_obex_jitob_get(x);
	if(o) {
		t_atom a[1];
		jit_object_method(o, s, s, argc, argv);
		atom_setsym(a, jit_attr_getsym(o, gensym("name")));
		outlet_anything(x->fieldout, gensym("jit_field"), 1, a);
	}
}

void max_jit_field_free(t_max_jit_field *x)
{
	jit_object_free(max_jit_obex_jitob_get(x));
	max_jit_obex_free(x);
}

void *max_jit_field_new(t_symbol *s, long argc, t_atom *argv)
{
	t_max_jit_field *x;
	void *o;
	long idx=0;
	long attrstart;

	if ((x=(t_max_jit_field *)max_jit_obex_new(max_jit_field_class,gensym("jit_field")))) {
		if((o=jit_object_new(gensym("jit_field")))) {
			max_jit_obex_dumpout_set(x, outlet_new(x, 0L));
			x->fieldout = outlet_new(x,"jit_field");
			max_jit_obex_jitob_set(x,o);
			
			//get normal args
			attrstart = max_jit_attr_args_offset(argc,argv);
			if(argc&&argv) {
				// field <name>
				if(idx < attrstart && argv[idx].a_type == A_SYM) {
					t_symbol *name = jit_atom_getsym(argv+idx);
					jit_attr_setsym(o, gensym("name"), name);
					++idx;
				}
				
				// field <planecount> <type> <voxelsize>
				if(idx < attrstart && argv[idx].a_type == A_LONG) {
					long planecount = jit_atom_getlong(argv+idx);
					jit_attr_setlong(o, gensym("planecount"), planecount);
					++idx;
					
					if(idx < attrstart && argv[idx].a_type == A_SYM) {
						t_symbol *type = jit_atom_getsym(argv+idx);
						jit_attr_setsym(o, gensym("type"), type);
						++idx;
						
						if(idx < attrstart && argv[idx].a_type == A_FLOAT) {
							float voxelsize = jit_atom_getfloat(argv+idx);
							jit_attr_setfloat(o, gensym("voxelsize"), voxelsize);
							++idx;
						}
					}
				}
			}
			
			max_jit_attr_args(x, argc, argv);
		} else {
			jit_object_error((t_object *)x,"jit.field: could not allocate object");
			freeobject((t_object *) x);
			x = NULL;
		}
	}
	return (x);
}