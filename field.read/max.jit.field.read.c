#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object
#include "jpatcher_api.h"
#include "jit.common.h"
#include "jit.matrix.util.h"

typedef struct _max_jit_field_read 
{
	t_object	ob;
	void		*obex;		
	void 		*dumpout;
	void 		*out1;
	t_symbol	*field_name;
	void		*in1matrix;
	void		*out1matrix_wrapper;
} t_max_jit_field_read;

void *max_jit_field_read_new(t_symbol *s, long argc, t_atom *argv);
void max_jit_field_read_free(t_max_jit_field_read *x);

void max_jit_field_read_jit_matrix(t_max_jit_field_read *x, t_symbol *name, long argc, t_atom *argv);
void max_jit_field_read_assist(t_max_jit_field_read *x, void *b, long io, long index, char *s);
void max_jit_field_read_dumpout(t_max_jit_field_read *x, t_symbol *s, short argc, t_atom *argv);

t_class *max_jit_field_read_class;

int C74_EXPORT main(void)
{
	long attrflags;
	void *attr;
	void *p;
	
	t_object *dummy = newinstance(gensym("jit.field"),0,0);
	freeobject(dummy);
	
	setup(&max_jit_field_read_class, max_jit_field_read_new, (method)max_jit_field_read_free, (short)sizeof(t_max_jit_field_read), 0L, A_GIMME, 0);
	p = max_jit_classex_setup(calcoffset(t_max_jit_field_read,obex));
	addmess((method)max_jit_field_read_jit_matrix, "jit_matrix", A_GIMME,0);	
	addmess((method)max_jit_field_read_assist, "assist", A_CANT,0);
	
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_DEFER_LOW ;
	attr = jit_object_new(_jit_sym_jit_attr_offset,"field_name",_jit_sym_symbol,attrflags,
		(method)0L,(method)0L,calcoffset(t_max_jit_field_read,field_name));
	max_jit_classex_addattr(p,attr);
	
    max_jit_classex_standard_wrap(p,NULL,0);
	return 0;
}

void * max_jit_field_read_adapt_matrix_to_pos(t_max_jit_field_read *x, void *matrix)
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

void * max_jit_field_read_adapt_matrix_to_field(t_max_jit_field_read *x, void *field, void *posmatrix)
{
	t_jit_matrix_info posminfo;
	t_jit_matrix_info minfo;
	long field_planecount =object_attr_getlong(field, gensym("planecount"));
	t_symbol *field_type = object_attr_getsym(field, gensym("type"));
	void *out1matrix = jit_object_method(x->out1matrix_wrapper, gensym("getmatrix"));
	jit_object_method(posmatrix, _jit_sym_getinfo, &posminfo);
	jit_object_method(out1matrix, _jit_sym_getinfo, &minfo);
	if(
		minfo.type != field_type ||
		minfo.planecount != field_planecount ||
		jit_matrix_info_uniform_dim(&posminfo, 1, &minfo) != JIT_ERR_NONE
	) {
		minfo = posminfo;
		minfo.type = field_type;
		minfo.planecount = field_planecount;
		
		jit_object_method(out1matrix, _jit_sym_setinfo, &minfo);
		jit_object_method(out1matrix, _jit_sym_getinfo, &minfo);
	}
	return out1matrix;
}

void max_jit_field_read_jit_matrix(t_max_jit_field_read *x, t_symbol *name, long argc, t_atom *argv)
{
	if(argc && argv) {
		t_symbol *name = jit_atom_getsym(argv);
		void *posmatrix = jit_object_findregistered(name);
		if(posmatrix && jit_object_method(posmatrix, _jit_sym_class_jit_matrix)) {
			void *field = jit_object_findregistered(x->field_name);
			if(field && jit_object_classname(field) == gensym("jit_field_grid")) {
				t_atom a[1];
				void *dstmatrix;
				posmatrix = max_jit_field_read_adapt_matrix_to_pos(x, posmatrix);
				dstmatrix = max_jit_field_read_adapt_matrix_to_field(x, field, posmatrix);
				jit_object_method(field, gensym("read"), posmatrix, dstmatrix);
				
				atom_setsym(a, jit_attr_getsym(x->out1matrix_wrapper, gensym("name")));
				outlet_anything(x->ob.o_outlet, gensym("jit_matrix"), 1, a);
			}
			else {
				object_error((t_object *)x, "jit.field.read: object %s is not a field", x->field_name->s_name);
			}
		}
	}
}

void max_jit_field_read_assist(t_max_jit_field_read *x, void *b, long io, long index, char *s)
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

void max_jit_field_read_dumpout(t_max_jit_field_read *x, t_symbol *s, short argc, t_atom *argv)
{
	outlet_anything(x->dumpout,s,argc,argv);	
}
	
void max_jit_field_read_free(t_max_jit_field_read *x)
{
	if(x->in1matrix) {
		object_free(x->in1matrix);
		x->in1matrix = NULL;
	}
	if(x->out1matrix_wrapper) {
		object_free(x->out1matrix_wrapper);
		x->out1matrix_wrapper = NULL;
	}
	max_jit_obex_free(x);
}

void *max_jit_field_read_new(t_symbol *s, long argc, t_atom *argv)
{
	t_max_jit_field_read *x;
	if ((x=(t_max_jit_field_read *)max_jit_obex_new(max_jit_field_read_class,gensym("jit_field_read")))) {
		long attrstart;
		
		max_jit_obex_dumpout_set(x, x->dumpout=outlet_new(x,0L));
		x->field_name = _jit_sym_nothing;
		x->out1 = outlet_new(x, NULL);
		x->in1matrix = jit_object_new(gensym("jit_matrix"), NULL);
		x->out1matrix_wrapper = jit_object_new(gensym("jit_matrix_wrapper"), jit_symbol_unique(), 0, NULL);
		
		attrstart = max_jit_attr_args_offset(argc,argv);
		if (attrstart&&argv) {
			jit_atom_arg_getsym(&x->field_name, 0, attrstart, argv);
		}
		
		max_jit_attr_args(x,argc,argv);
	}
	return (x);
}