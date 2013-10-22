#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object
#include "jpatcher_api.h"
#include "jit.common.h"

#define MAX_FIELD_PLANES (3)

typedef struct _max_jit_field_op 
{
	t_object	ob;
	void		*obex;		
	void 		*dumpout;
	long		lastin;
	void		*proxy;
	void 		*out1;
	long		usename;
	t_symbol	*in2name;
	float		val[MAX_FIELD_PLANES];
	
} t_max_jit_field_op;

void *max_jit_field_op_new(t_symbol *s, long argc, t_atom *argv);
void max_jit_field_op_free(t_max_jit_field_op *x);


void max_jit_field_op_int(t_max_jit_field_op *x, long c);
void max_jit_field_op_float(t_max_jit_field_op *x, double f);
t_jit_err max_jit_field_op_setattr_val(t_max_jit_field_op *x, void *attr, long argc, t_atom *argv);
void max_jit_field_op_list(t_max_jit_field_op *x, t_symbol *s, short argc, t_atom *argv);
void max_jit_field_op_jit_field(t_max_jit_field_op *x, t_symbol *name, long argc, t_atom *argv);
void max_jit_field_op_assist(t_max_jit_field_op *x, void *b, long io, long index, char *s);
void max_jit_field_op_dumpout(t_max_jit_field_op *x, t_symbol *s, short argc, t_atom *argv);

t_class *max_jit_field_op_class;


int C74_EXPORT main(void)
{
	void *p,*q;
	long attrflags;
	void *attr;
	
	t_object *dummy = newinstance(gensym("jit.field"),0,0);
	freeobject(dummy);
	
	setup(&max_jit_field_op_class, max_jit_field_op_new, (method)max_jit_field_op_free, (short)sizeof(t_max_jit_field_op), 0L, A_GIMME, 0);
	p = max_jit_classex_setup(calcoffset(t_max_jit_field_op,obex));
	q = jit_class_findbyname(gensym("jit_field_op"));
	addmess((method)max_jit_field_op_jit_field, "jit_field", A_GIMME,0);
	addint((method)max_jit_field_op_int);
 	addfloat((method)max_jit_field_op_float);
	addmess((method)max_jit_field_op_list, "list", A_GIMME,0);
	addmess((method)max_jit_field_op_assist, "assist", A_CANT,0);
	
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW;
	attr = jit_object_new(_jit_sym_jit_attr_offset_array,"val",_jit_sym_float32,MAX_FIELD_PLANES,attrflags,
		(method)0L,(method)max_jit_field_op_setattr_val,0/*fix*/,calcoffset(t_max_jit_field_op,val));
	max_jit_classex_addattr(p,attr);
	
    max_jit_classex_standard_wrap(p,q,0);
	
	return 0;
}

void max_jit_field_op_int(t_max_jit_field_op *x, long c)
{
	x->val[0] = (float)c;
	x->val[1] = x->val[0];
	x->val[2] = x->val[0];
	x->usename = 0;
}

void max_jit_field_op_float(t_max_jit_field_op *x, double f)
{
	x->val[0] = f;
	x->val[1] = f;
	x->val[2] = f;
	x->usename = 0;
}
void max_jit_field_op_list(t_max_jit_field_op *x, t_symbol *s, short argc, t_atom *argv)
{
	long i;
	for(i=0; i < MIN(MAX_FIELD_PLANES, argc); ++i) {
		x->val[i] = atom_getfloat(argv+i);
	}
	x->usename = 0;
}

t_jit_err max_jit_field_op_setattr_val(t_max_jit_field_op *x, void *attr, long argc, t_atom *argv)
{
	if(argc == 1) {
		float v = jit_atom_getfloat(argv);
		x->val[0] = v;
		x->val[1] = v;
		x->val[2] = v;
	}
	else {
		long i;
		for(i=0; i < argc; ++i) {
			x->val[i] = jit_atom_getfloat(argv+i);
		}
	}
	return JIT_ERR_NONE;
}

void max_jit_field_op_jit_field(t_max_jit_field_op *x, t_symbol *name, long argc, t_atom *argv)
{
	void *o = max_jit_obex_jitob_get(x);
	void *field = NULL;
	t_atom a[1+MAX_FIELD_PLANES];
	long innum = proxy_getinlet((t_object *)x);
	
	if(innum == 1) {
		if(argc && argv) {
			x->in2name = jit_atom_getsym(argv);
			x->usename = 1;
		}
		else {
			x->in2name = _jit_sym_nothing;
			x->usename = 0;
		}
	}
	else if(argc && argv) {
		long argc = 0;
		a[0] = argv[0];
		if(x->usename) {
			jit_atom_setsym(a+1, x->in2name);
			argc = 2;
		}
		else {
			jit_atom_setfloat(a+1, x->val[0]);
			jit_atom_setfloat(a+2, x->val[1]);
			jit_atom_setfloat(a+3, x->val[2]);
			argc = 1+MAX_FIELD_PLANES;
		}
		jit_object_method(o, name, name, argc, a);
		
		field = object_method(o, gensym("getfield"));
		if(!field) object_error((t_object *)x, "unable to get field object");
		
		atom_setsym(a, jit_attr_getsym(field, gensym("name")));
		outlet_anything(x->ob.o_outlet, gensym("jit_field"), 1, a);
	}
}

void max_jit_field_op_assist(t_max_jit_field_op *x, void *b, long io, long index, char *s)
{
	if (io == 1) //input
	{
		if(index == 0) sprintf(s,"messages in");
	}
	else	//output
	{
		if(index == 0) sprintf(s, "field containing a op");
		else if(index == 1) sprintf(s, "dumpout");
	}
}

void max_jit_field_op_dumpout(t_max_jit_field_op *x, t_symbol *s, short argc, t_atom *argv)
{
	outlet_anything(x->dumpout,s,argc,argv);	
}
	
void max_jit_field_op_free(t_max_jit_field_op *x)
{
	jit_object_free(max_jit_obex_jitob_get(x));
	max_jit_obex_free(x);
}

void *max_jit_field_op_new(t_symbol *s, long argc, t_atom *argv)
{
	t_max_jit_field_op *x;
	
	if (x=(t_max_jit_field_op *)max_jit_obex_new(max_jit_field_op_class,gensym("jit_field_op"))) {
		void *o;
		if ((o=jit_object_new(gensym("jit_field_op")))) {
			max_jit_obex_jitob_set(x,o);
			max_jit_obex_dumpout_set(x, x->dumpout=outlet_new(x,0L));
			x->lastin = 0;
			x->proxy = proxy_new((t_object *)x, 1, &x->lastin);
			x->out1 = outlet_new(x, NULL);
			x->usename = 0;
			x->in2name = _jit_sym_nothing;
			x->val[0] = 0;
			x->val[1] = 0;
			x->val[2] = 0;
			max_jit_attr_args(x,argc,argv);			
		} 
		else {
			jit_object_error((t_object *)x,"jit.field.op: could not allocate object");
			freeobject((t_object *) x);
			x = NULL;
		}
	}
	return (x);
}