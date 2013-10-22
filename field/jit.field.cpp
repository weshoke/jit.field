#include "jit.common.h"
#include "jit.field.grid.h"

#ifdef __cplusplus
extern "C" {
#endif

t_jit_err jit_field_grid_init(void);
t_jit_err jit_field_sphere_init(void);
t_jit_err jit_field_mesh_init(void);
t_jit_err jit_field_op_init(void);
t_jit_err jit_field_particles_init(void);

typedef struct _jit_field 
{
	t_object			ob;
	t_symbol			*name;
	t_jit_field_grid	*grid;
} t_jit_field;

void *_jit_field_class;

void jit_field_wrapit(t_class *c);
t_max_err jit_field_wrapper_attrwrap_set(t_jit_field *x, void *attr, long ac, t_atom *av);
t_max_err jit_field_wrapper_attrwrap_get(t_jit_field *x, void *attr, long *ac, t_atom **av);
t_jit_field *jit_field_new(void);
void jit_field_free(t_jit_field *x);
t_jit_err jit_field_setattr_name(t_jit_field *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_field_sphere(t_jit_field *x, float *center, float *radius, float *halfwidth);
t_jit_err jit_field_max(t_jit_field *x, t_jit_field_grid *in1, t_jit_field_grid *in2);
t_jit_err jit_field_min(t_jit_field *x, t_jit_field_grid *in1, t_jit_field_grid *in2);
t_jit_err jit_field_add(t_jit_field *x, t_jit_field_grid *in1, t_jit_field_grid *in2);
t_jit_err jit_field_mul(t_jit_field *x, t_jit_field_grid *in1, t_jit_field_grid *in2);
t_jit_err jit_field_normalize(t_jit_field *x, t_jit_field_grid *in1);
t_jit_err jit_field_magnitude(t_jit_field *x, t_jit_field_grid *in1);
t_jit_err jit_field_curvature(t_jit_field *x, t_jit_field_grid *in1);
t_jit_err jit_field_laplacian(t_jit_field *x, t_jit_field_grid *in1);
t_jit_err jit_field_gradient(t_jit_field *x, t_jit_field_grid *in1);
t_jit_err jit_field_divergence(t_jit_field *x, t_jit_field_grid *in1);
t_jit_err jit_field_curl(t_jit_field *x, t_jit_field_grid *in1);
t_jit_err jit_field_particles(t_jit_field *x, void *position_matrix, float *radius);
t_jit_err jit_field_setcoord(t_jit_field *x, t_symbol *s, long argc, t_atom *argv);
t_jit_err jit_field_getcoord(t_jit_field *x, t_symbol *s, long argc, t_atom *argv, long *rac, t_atom **rav);
t_jit_err jit_field_setpos(t_jit_field *x, t_symbol *s, long argc, t_atom *argv);
t_jit_err jit_field_getpos(t_jit_field *x, t_symbol *s, long argc, t_atom *argv, long *rac, t_atom **rav);
t_jit_err jit_field_indextoworld(t_jit_field *x, long *index, float *world);
t_jit_err jit_field_worldtoindex(t_jit_field *x, float *world, long *index);
t_jit_err jit_field_jit_field(t_jit_field *x, t_symbol *s, long argc, t_atom *argv);
void * jit_field_getgrid(t_jit_field *x);

void * jit_field_getmatrix(t_jit_field *x);

t_jit_err jit_field_init(void) 
{
	long attrflags=0;
	t_jit_object *attr;
	
		
	_jit_field_class = jit_class_new("jit_field",(method)jit_field_new,(method)jit_field_free,
		sizeof(t_jit_field),0L);
		
		
	jit_class_addmethod(_jit_field_class, (method)jit_object_register, "register", A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_sphere, "sphere", A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_max, "max", A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_min, "min", A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_add, "add", A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_mul, "mul", A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_normalize, "normalize", A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_magnitude, "magnitude", A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_curvature, "curvature", A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_laplacian, "laplacian", A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_gradient, "gradient", A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_divergence, "divergence", A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_curl, "curl", A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_particles, "particles", A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_setcoord, "setcoord" , A_GIMME, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_getcoord, "getcoord" , A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_setpos, "setpos" , A_GIMME, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_getpos, "getpos" , A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_indextoworld, "indextoworld" , A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_worldtoindex, "worldtoindex" , A_CANT, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_jit_field, "jit_field" , A_GIMME, 0L);
	jit_class_addmethod(_jit_field_class, (method)jit_field_getgrid, "getgrid" , A_CANT, 0L);
	
	//add attributes	
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW;
	
	attr = (t_jit_object *)jit_object_new(_jit_sym_jit_attr_offset, "name", _jit_sym_symbol, attrflags,
				(method)0L, (method)jit_field_setattr_name, calcoffset(t_jit_field, name));
    jit_class_addattr(_jit_field_class, (t_jit_object*)attr);

	jit_field_grid_init();
	jit_field_sphere_init();
	jit_field_mesh_init();
	jit_field_op_init();
	jit_field_particles_init();
	
	jit_field_wrapit((t_class *)_jit_field_class);
	jit_class_register(_jit_field_class);

	return JIT_ERR_NONE;
}


void jit_field_wrapit(t_class *c)
{
	t_class *jc;
	long attrcount=0;
	t_symbol **attrarray=NULL;
	long canset,canget,flags;
//	t_messlist *mp;
//	t_messlist *mp2;
	long i;
	void *attr=NULL;

	jc = class_findbyname(gensym("jitter"),gensym("jit_field_grid"));
	if (!jc) {
		error("jit.field: could not find jit_field_grid class");
		return;
	}
	 
	// for each public attribute add a wrapper attribute
	hashtab_getkeys((t_hashtab *)jc->c_attributes,&attrcount,&attrarray);
	for (i=0;i<attrcount;i++) {
		if ((attr=class_attr_get(jc,attrarray[i]))) {
			canset = (long) object_method(attr,gensym("usercanset"));
			canget = (long) object_method(attr,gensym("usercanget"));
			if (canset||canget) {
				flags = 0;
				if (!canset) flags |= ATTR_SET_OPAQUE_USER;
				if (!canget) flags |= ATTR_GET_OPAQUE_USER;
				attr = attribute_new(attrarray[i]->s_name,(t_symbol *)object_method(attr,_jit_sym_gettype),flags,
					(method)jit_field_wrapper_attrwrap_get,(method)jit_field_wrapper_attrwrap_set);
				class_addattr(c,(t_jit_object*)attr);
				
				if(attrarray[i] == gensym("type")) {
					CLASS_ATTR_ENUM(c,"type",0, "long float32");
					CLASS_ATTR_DEFAULT(c, "type", 0, "float32");
				}
				else if(attrarray[i] == gensym("planecount")) {
					CLASS_ATTR_FILTER_MIN(c, "type", 1);
					CLASS_ATTR_FILTER_MAX(c, "type", 3);
				}
			}
		}
	}
	freebytes(attrarray, sizeof(t_symbol *)*attrcount);
}

t_max_err jit_field_wrapper_attrwrap_set(t_jit_field *x, void *attr, long ac, t_atom *av)
{
	if (x->grid&&attr) {
		//MAKE THIS FUNCTION AND EXPORT FOR 1.5 GUARANTEE
		//jit_object_attr_setvalueof(x->matrix,object_method(attr,_jit_sym_getname),ac,av); 
		return object_attr_setvalueof(x->grid,(t_symbol *)object_method(attr,_jit_sym_getname),ac,av); 
	}
	return MAX_ERR_GENERIC;
}

t_max_err jit_field_wrapper_attrwrap_get(t_jit_field *x, void *attr, long *ac, t_atom **av)
{
	if (x->grid&&attr) {
		//MAKE THIS FUNCTION AND EXPORT FOR 1.5 GUARANTEE
		//return jit_object_attr_getvalueof(x->matrix,object_method(attr,_jit_sym_getname),ac,av);
		return object_attr_getvalueof(x->grid,(t_symbol *)object_method(attr,_jit_sym_getname),ac,av);
	}
	return MAX_ERR_GENERIC;
}

t_jit_err jit_field_setattr_name(t_jit_field *x, void *attr, long argc, t_atom *argv)
{
	t_symbol *name = _sym_nothing;
	if(argc && argv) {
		name = atom_getsym(argv);
	}
	if(name == _jit_sym_nothing) name = jit_symbol_unique();
	
	if(x->name != name) {
		void *o = NULL;
		jit_object_detach(x->name, x);
		x->name = name;
		o = x->grid; 
		o = jit_object_method(o, _jit_sym_register, x->name);
		x->grid = (t_jit_field_grid *)o;
		jit_object_attach(x->name, x);
	}
	
	return JIT_ERR_NONE;
}

t_jit_err jit_field_sphere(t_jit_field *x, float *center, float *radius, float *halfwidth)
{
	t_jit_err err = JIT_ERR_NONE;
	x->grid->grid->sphere(Vec3s(center), *radius, *halfwidth);
	return err;
}

t_jit_err jit_field_max(t_jit_field *x, t_jit_field_grid *in1, t_jit_field_grid *in2)
{
	t_jit_err err = JIT_ERR_NONE;
	if(in1 && in2) {
		x->grid->grid->compMax(*(in1->grid), *(in2->grid));
	}
	return err;
}

t_jit_err jit_field_min(t_jit_field *x, t_jit_field_grid *in1, t_jit_field_grid *in2)
{
	t_jit_err err = JIT_ERR_NONE;
	if(in1 && in2) {
		x->grid->grid->compMin(*(in1->grid), *(in2->grid));
	}
	return err;
}

t_jit_err jit_field_add(t_jit_field *x, t_jit_field_grid *in1, t_jit_field_grid *in2)
{
	t_jit_err err = JIT_ERR_NONE;
	if(in1 && in2) {
		x->grid->grid->compSum(*(in1->grid), *(in2->grid));
	}
	return err;
}

t_jit_err jit_field_mul(t_jit_field *x, t_jit_field_grid *in1, t_jit_field_grid *in2)
{
	t_jit_err err = JIT_ERR_NONE;
	if(in1 && in2) {
		x->grid->grid->compMul(*(in1->grid), *(in2->grid));
	}
	return err;
}

t_jit_err jit_field_normalize(t_jit_field *x, t_jit_field_grid *in1)
{
	t_jit_err err = JIT_ERR_NONE;
	if(in1) {
		x->grid->grid->normalize(*(in1->grid));
	}
	return err;
}

t_jit_err jit_field_magnitude(t_jit_field *x, t_jit_field_grid *in1)
{
	t_jit_err err = JIT_ERR_NONE;
	if(in1) {
		x->grid->grid->magnitude(*(in1->grid));
	}
	return err;
}

t_jit_err jit_field_curvature(t_jit_field *x, t_jit_field_grid *in1)
{
	t_jit_err err = JIT_ERR_NONE;
	if(in1) {
		if(in1->grid->getPlanecount() == 1 && in1->grid->getGridType() == GridFloat) {
			x->grid->grid->curvature(*(in1->grid));
		}
		else {
			object_error((t_object *)x, "jit.field: curvature input muset be 1-plane float32");
		}
	}
	return err;
}

t_jit_err jit_field_laplacian(t_jit_field *x, t_jit_field_grid *in1)
{
	t_jit_err err = JIT_ERR_NONE;
	if(in1) {
		if(in1->grid->getPlanecount() == 1 && in1->grid->getGridType() == GridFloat) {
			x->grid->grid->laplacian(*(in1->grid));
		}
		else {
			object_error((t_object *)x, "jit.field: laplacian input muset be 1-plane float32");
		}
	}
	return err;
}

t_jit_err jit_field_gradient(t_jit_field *x, t_jit_field_grid *in1)
{
	t_jit_err err = JIT_ERR_NONE;
	if(in1) {
		if(in1->grid->getPlanecount() == 1 && in1->grid->getGridType() == GridFloat) {
			x->grid->grid->gradient(*(in1->grid));
		}
		else {
			object_error((t_object *)x, "jit.field: gradient input muset be 1-plane float32");
		}
	}
	return err;
}

t_jit_err jit_field_divergence(t_jit_field *x, t_jit_field_grid *in1)
{
	t_jit_err err = JIT_ERR_NONE;
	if(in1) {
		x->grid->grid->divergence(*(in1->grid));
	}
	return err;
}

t_jit_err jit_field_curl(t_jit_field *x, t_jit_field_grid *in1)
{
	t_jit_err err = JIT_ERR_NONE;
	if(in1) {
		if(in1->grid->getPlanecount() == 3) {
			x->grid->grid->curl(*(in1->grid));
		}
		else {
			object_error((t_object *)x, "jit.field: curl input muset be 3-plane");
		}
	}
	return err;
}

t_jit_err jit_field_particles(t_jit_field *x, void *position_matrix, float *radius)
{
	t_jit_err err = JIT_ERR_NONE;
	if(position_matrix) {
		t_jit_matrix_info minfo;
		char *ptr;
		// TODO: lock data
		object_method(position_matrix, _jit_sym_getinfo, &minfo);
		object_method(position_matrix, _jit_sym_getdata, &ptr);
		x->grid->grid->rasterizeParticles(&minfo, ptr, *radius);
	}
	return err;
}

t_jit_err jit_field_setcoord(t_jit_field *x, t_symbol *s, long argc, t_atom *argv)
{
	t_jit_err err = JIT_ERR_NONE;
	if(argc >= 5) {
		Coord ijk(
			jit_atom_getlong(argv),
			jit_atom_getlong(argv+1),
			jit_atom_getlong(argv+2)
		);
		float v[3];
		for(int i=4; i < MIN(8, argc); ++i) {
			v[i] = jit_atom_getfloat(argv+i);
		}
		x->grid->grid->setIndex(ijk, v);
	}
	return err;
}

t_jit_err jit_field_getcoord(t_jit_field *x, t_symbol *s, long argc, t_atom *argv, long *rac, t_atom **rav)
{
	t_jit_err err = JIT_ERR_NONE;
	if(argc >= 3) {
		Coord ijk(
			jit_atom_getlong(argv),
			jit_atom_getlong(argv+1),
			jit_atom_getlong(argv+2)
		);
		float v;
		x->grid->grid->getIndex(ijk, &v);
		
		*rac = 1;
		*rav = (t_atom *)jit_getbytes(sizeof(t_atom)*1);
		if (!*rav) {
			return JIT_ERR_OUT_OF_MEM;
		}
		atom_setfloat(*rav, v);
	}
	return err;
}

t_jit_err jit_field_setpos(t_jit_field *x, t_symbol *s, long argc, t_atom *argv)
{
	t_jit_err err = JIT_ERR_NONE;
	if(argc >= 5) {
		Vec3s xyz(
			jit_atom_getfloat(argv),
			jit_atom_getfloat(argv+1),
			jit_atom_getfloat(argv+2)
		);
		float v[3];
		for(int i=4; i < MIN(8, argc); ++i) {
			v[i] = jit_atom_getfloat(argv+i);
		}
		x->grid->grid->setPos(xyz, v);
	}
	return err;
}

t_jit_err jit_field_getpos(t_jit_field *x, t_symbol *s, long argc, t_atom *argv, long *rac, t_atom **rav)
{
	t_jit_err err = JIT_ERR_NONE;
	if(argc >= 3) {
		Vec3s xyz(
			jit_atom_getfloat(argv),
			jit_atom_getfloat(argv+1),
			jit_atom_getfloat(argv+2)
		);
		float v;
		x->grid->grid->getPos(xyz, &v);
		*rac = 1;
		*rav = (t_atom *)jit_getbytes(sizeof(t_atom)*1);
		if (!*rav) {
			return JIT_ERR_OUT_OF_MEM;
		}
		atom_setfloat(*rav, v);
	}
	return err;
}

t_jit_err jit_field_indextoworld(t_jit_field *x, long *index, float *world)
{
	t_jit_err err = JIT_ERR_NONE;
	Coord ijk(index[0], index[1], index[2]);
	Vec3s xyz = x->grid->grid->indexToWorld(ijk);
	world[0] = xyz[0];
	world[1] = xyz[1];
	world[2] = xyz[2];
	return err;
}

t_jit_err jit_field_worldtoindex(t_jit_field *x, float *world, long *index)
{
	t_jit_err err = JIT_ERR_NONE;
	Vec3s xyz(world[0], world[1], world[2]);
	Coord ijk = x->grid->grid->worldToIndex(xyz);
	index[0] = ijk[0];
	index[1] = ijk[1];
	index[2] = ijk[2];
	return err;
}

t_jit_err jit_field_jit_field(t_jit_field *x, t_symbol *s, long argc, t_atom *argv)
{
	t_jit_err err = JIT_ERR_NONE;
	if(argc && argv) {
		t_jit_field_grid *src = jit_field_grid_from_atom(argv);
		x->grid->grid->shallowCopy(*(src->grid));
	}
	return err;
}

void * jit_field_getgrid(t_jit_field *x)
{
	return x->grid;
}

t_jit_field *jit_field_new(void)
{
	t_jit_field *x;
		
	if((x=(t_jit_field *)jit_object_alloc(_jit_field_class))) {
		void *o;
		
		x->name = _jit_sym_nothing;
		if(x->name == _jit_sym_nothing) x->name = jit_symbol_unique();
		
		o = jit_object_new(gensym("jit_field_grid"));
		o = jit_object_method(o, _jit_sym_register, x->name);
		
		if (!o) {
			jit_object_post((t_object *)x, (char *)"error could not create jit.field.grid %s", x->name->s_name);
			x->name = NULL; //no, no, no...we're not bound to this guy
			freeobject((t_object *)x);
			x = NULL;
			goto out;
		}
		
		x->grid = (t_jit_field_grid *)o;
		jit_object_attach(x->name, x);
	}
	else {
		x = NULL;
	}
out:
	return x;
}

void jit_field_free(t_jit_field *x)
{
	if(x->grid) {
		object_free(x->grid);
		x->grid = NULL;
	}
}


#ifdef __cplusplus
}
#endif