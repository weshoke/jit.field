#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object
#include "jpatcher_api.h"
#include "jit.common.h"
#include "jit.matrix.util.h"

typedef struct _max_jit_field_write 
{
	t_object	ob;
	void		*obex;		
	void 		*dumpout;
	long		lastin;
	void		*proxy;
	void 		*out1;
	t_symbol	*field_name;
	t_symbol	*mode;
	t_symbol	*in2name;
	void		*in1matrix;
	void		*in2matrix;
} t_max_jit_field_write;

void *max_jit_field_write_new(t_symbol *s, long argc, t_atom *argv);
void max_jit_field_write_free(t_max_jit_field_write *x);

void max_jit_field_write_jit_matrix(t_max_jit_field_write *x, t_symbol *name, long argc, t_atom *argv);
void max_jit_field_write_assist(t_max_jit_field_write *x, void *b, long io, long index, char *s);
void max_jit_field_write_dumpout(t_max_jit_field_write *x, t_symbol *s, short argc, t_atom *argv);

t_class *max_jit_field_write_class;


int C74_EXPORT main(void)
{
	void *p;
	long attrflags;
	void *attr;
	
	t_object *dummy = newinstance(gensym("jit.field"),0,0);
	freeobject(dummy);
	
	setup(&max_jit_field_write_class, max_jit_field_write_new, (method)max_jit_field_write_free, (short)sizeof(t_max_jit_field_write), 0L, A_GIMME, 0);
	p = max_jit_classex_setup(calcoffset(t_max_jit_field_write,obex));
	addmess((method)max_jit_field_write_jit_matrix, "jit_matrix", A_GIMME,0);
	addmess((method)max_jit_field_write_assist, "assist", A_CANT,0);
	
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_DEFER_LOW ;
	attr = jit_object_new(_jit_sym_jit_attr_offset,"field_name",_jit_sym_symbol,attrflags,
		(method)0L,(method)0L,calcoffset(t_max_jit_field_write,field_name));
	max_jit_classex_addattr(p,attr);
	
	attr = jit_object_new(_jit_sym_jit_attr_offset,"mode",_jit_sym_symbol,attrflags,
		(method)0L,(method)0L,calcoffset(t_max_jit_field_write,mode));
	max_jit_classex_addattr(p,attr);
	//CLASS_ATTR_ENUM((t_class *)p, "mode", 0, "absolute relative");
	
    max_jit_classex_standard_wrap(p,NULL,0);
	return 0;
}

void * max_jit_field_write_adapt_matrix_to_pos(t_max_jit_field_write *x, void *matrix)
{
	t_jit_matrix_info minfo;
	jit_object_method(matrix, _jit_sym_getinfo, &minfo);
	if(!(minfo.type == _jit_sym_float32 || minfo.type == _jit_sym_long) || minfo.planecount != 3) {
		if(minfo.type == _jit_sym_float64) minfo.type = _jit_sym_float32;
		else if(minfo.type == _jit_sym_char) minfo.type = _jit_sym_long;
		minfo.planecount = 3;
	
		jit_object_method(x->in1matrix, _jit_sym_setinfo, &minfo);
		jit_object_method(x->in1matrix, _jit_sym_getinfo, &minfo);
		return x->in1matrix;
	}
	return matrix;
}

void * max_jit_field_write_adapt_matrix_to_field(t_max_jit_field_write *x, void *field, void *posmatrix, void *matrix)
{
	t_jit_matrix_info posminfo;
	t_jit_matrix_info minfo;
	long field_planecount =object_attr_getlong(field, gensym("planecount"));
	t_symbol *field_type = object_attr_getsym(field, gensym("type"));
	jit_object_method(posmatrix, _jit_sym_getinfo, &posminfo);
	jit_object_method(matrix, _jit_sym_getinfo, &minfo);
	if(
		minfo.type != field_type ||
		minfo.planecount != field_planecount ||
		jit_matrix_info_uniform_dim(&posminfo, 1, &minfo) != JIT_ERR_NONE
	) {
		minfo = posminfo;
		minfo.type = field_type;
		minfo.planecount = field_planecount;
		
		jit_object_method(x->in2matrix, _jit_sym_setinfo, &minfo);
		jit_object_method(x->in2matrix, _jit_sym_getinfo, &minfo);
		jit_object_method(x->in2matrix, _jit_sym_frommatrix, matrix, NULL);
		return x->in2matrix;
	}
	return matrix;
}

void max_jit_field_write_jit_matrix(t_max_jit_field_write *x, t_symbol *name, long argc, t_atom *argv)
{
	if(argc && argv) {
		t_symbol *name = jit_atom_getsym(argv);
		long innum = proxy_getinlet((t_object *)x);
		if(innum == 1) {
			x->in2name = name;
		}
		else {
			void *posmatrix = jit_object_findregistered(name);
			if(posmatrix && jit_object_method(posmatrix, _jit_sym_class_jit_matrix)) {
				void *fieldmatrix = jit_object_findregistered(x->in2name);
				if(fieldmatrix && jit_object_method(fieldmatrix, _jit_sym_class_jit_matrix)) {
					void *field = jit_object_findregistered(x->field_name);
					if(field && jit_object_classname(field) == gensym("jit_field_grid")) {
						t_atom a[1];
						
						posmatrix = max_jit_field_write_adapt_matrix_to_pos(x, posmatrix);
						fieldmatrix = max_jit_field_write_adapt_matrix_to_field(x, field, posmatrix, fieldmatrix);
						if(x->mode == gensym("absolute")) {
							jit_object_method(field, gensym("write"), posmatrix, fieldmatrix);
						}
						else {
							jit_object_method(field, gensym("write_relative"), posmatrix, fieldmatrix);
						}
						
						atom_setsym(a, x->field_name);
						outlet_anything(x->ob.o_outlet, gensym("jit_field"), 1, a);
					}
					else {
						object_error((t_object *)x, "jit.field.write: object %s is not a field", x->field_name->s_name);
					}
				}
			}
		}
	}
}

void max_jit_field_write_assist(t_max_jit_field_write *x, void *b, long io, long index, char *s)
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

void max_jit_field_write_dumpout(t_max_jit_field_write *x, t_symbol *s, short argc, t_atom *argv)
{
	outlet_anything(x->dumpout,s,argc,argv);
}
	
void max_jit_field_write_free(t_max_jit_field_write *x)
{
	if(x->in1matrix) {
		jit_object_free(x->in1matrix);
		x->in1matrix = NULL;
	}
	if(x->in2matrix) {
		jit_object_free(x->in2matrix);
		x->in2matrix = NULL;
	}

	max_jit_obex_free(x);
}

void *max_jit_field_write_new(t_symbol *s, long argc, t_atom *argv)
{
	t_max_jit_field_write *x;
	if ((x=(t_max_jit_field_write *)max_jit_obex_new(max_jit_field_write_class,gensym("jit_field_write")))) {
		long attrstart;
		
		max_jit_obex_dumpout_set(x, x->dumpout=outlet_new(x,0L));
		x->lastin = 0;
		x->proxy = proxy_new((t_object *)x, 1, &x->lastin);
		x->out1 = outlet_new(x, NULL);
		x->field_name = _jit_sym_nothing;
		x->mode = gensym("absolute");
		x->in2name = _jit_sym_nothing;
		x->in1matrix = jit_object_new(gensym("jit_matrix"), NULL);
		x->in2matrix = jit_object_new(gensym("jit_matrix"), NULL);
		
		attrstart = max_jit_attr_args_offset(argc,argv);
		if (attrstart&&argv) {
			jit_atom_arg_getsym(&x->field_name, 0, attrstart, argv);
		}
		
		max_jit_attr_args(x,argc,argv);
	}
	return (x);
}