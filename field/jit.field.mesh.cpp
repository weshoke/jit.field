#include "jit.common.h"
#include "jit.vecmath.h"
#include "jit.field.grid.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _jit_field_mesh
{
	t_object	ob;
	t_object	*vertex_matrix;
	t_object	*normal_matrix;
	t_object	*index_matrix;
	float		isolevel;
	t_symbol	*mode;
} t_jit_field_mesh;

void *_jit_field_mesh_class;

t_jit_field_mesh *jit_field_mesh_new(void);
void jit_field_mesh_free(t_jit_field_mesh *x);
t_jit_err jit_field_mesh_jit_field(t_jit_field_mesh *x, t_symbol *name, long argc, t_atom *argv);
t_jit_err jit_field_mesh_volume_to_mesh(t_jit_field_mesh *x, t_jit_field_grid *grid);
void * jit_field_mesh_get_vertex_matrix(t_jit_field_mesh *x);
void * jit_field_mesh_get_normal_matrix(t_jit_field_mesh *x);
void * jit_field_mesh_get_index_matrix(t_jit_field_mesh *x);

t_jit_err jit_field_mesh_init(void)
{
	long attrflags=0;
	t_jit_object *attr;
		
	_jit_field_mesh_class = jit_class_new("jit_field_mesh",(method)jit_field_mesh_new,(method)jit_field_mesh_free,
		sizeof(t_jit_field_mesh),0L);

	// add attributes	
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW;
	
	jit_class_addmethod(_jit_field_mesh_class, (method)jit_field_mesh_jit_field, "jit_field", A_GIMME, 0L);
	jit_class_addmethod(_jit_field_mesh_class, (method)jit_field_mesh_get_vertex_matrix, "get_vertex_matrix", A_CANT, 0L);
	jit_class_addmethod(_jit_field_mesh_class, (method)jit_field_mesh_get_normal_matrix, "get_normal_matrix", A_CANT, 0L);
	jit_class_addmethod(_jit_field_mesh_class, (method)jit_field_mesh_get_index_matrix, "get_index_matrix",	A_CANT, 0L);
	jit_class_addmethod(_jit_field_mesh_class, (method)jit_field_mesh_volume_to_mesh, "volume_to_mesh",	A_CANT, 0L);
	
	
	attr = (t_jit_object *)jit_object_new(_jit_sym_jit_attr_offset, "isolevel", _jit_sym_float32, attrflags,
				(method)0L, (method)0L, calcoffset(t_jit_field_mesh, isolevel));
    jit_class_addattr(_jit_field_mesh_class, (t_jit_object*)attr);
	
	attr = (t_jit_object *)jit_object_new(_jit_sym_jit_attr_offset, "mode", _jit_sym_symbol, attrflags,
				(method)0L, (method)0L, calcoffset(t_jit_field_mesh, mode));
    jit_class_addattr(_jit_field_mesh_class, (t_jit_object*)attr);
	CLASS_ATTR_ENUM((t_class *)_jit_field_mesh_class, "mode", 0, "mesh particles");

	jit_class_register(_jit_field_mesh_class);

	return JIT_ERR_NONE;
}

t_jit_err jit_field_mesh_jit_field(t_jit_field_mesh *x, t_symbol *name, long argc, t_atom *argv)
{
	t_jit_err err = JIT_ERR_NONE;
	if(argc && argv) {
		t_jit_field_grid *o = (t_jit_field_grid *)jit_field_grid_from_atom(argv);
		if(o) {
			err = jit_field_mesh_volume_to_mesh(x, o);
		}
	}
	return err;
}

t_jit_err jit_field_mesh_volume_to_mesh(t_jit_field_mesh *x, t_jit_field_grid *grid)
{
	t_object *vertex_matrix = (t_object *)jit_object_method(x->vertex_matrix, gensym("getmatrix"));
	if(x->mode == gensym("particle")) {
		jit_field_grid_volume_to_mesh_points(grid, vertex_matrix, x->isolevel);
	}
	else {	// mode == gensym("mesh")
		t_object *normal_matrix = (t_object *)jit_object_method(x->normal_matrix, gensym("getmatrix"));
		t_object *index_matrix = (t_object *)jit_object_method(x->index_matrix, gensym("getmatrix"));
		jit_field_grid_volume_to_mesh_quads(grid, vertex_matrix, normal_matrix, index_matrix, x->isolevel);
	}
	
	return JIT_ERR_NONE;
}



void * jit_field_mesh_get_vertex_matrix(t_jit_field_mesh *x)
{
	return x->vertex_matrix;
}

void * jit_field_mesh_get_normal_matrix(t_jit_field_mesh *x)
{
	return x->normal_matrix;
}

void * jit_field_mesh_get_index_matrix(t_jit_field_mesh *x)
{
	return x->index_matrix;
}


t_jit_field_mesh *jit_field_mesh_new(void)
{
	t_jit_field_mesh *x;
		
	if((x=(t_jit_field_mesh *)jit_object_alloc(_jit_field_mesh_class))) {
		x->vertex_matrix = (t_jit_object *)jit_object_new(gensym("jit_matrix_wrapper"), jit_symbol_unique(), 0, NULL);
		x->normal_matrix = (t_jit_object *)jit_object_new(gensym("jit_matrix_wrapper"), jit_symbol_unique(), 0, NULL);
		x->index_matrix = (t_jit_object *)jit_object_new(gensym("jit_matrix_wrapper"), jit_symbol_unique(), 0, NULL);
		x->mode = gensym("mesh");
		x->isolevel = 0;
	}
	else {
		x = NULL;
	}	
	return x;
}

void jit_field_mesh_free(t_jit_field_mesh *x)
{
	if(x->vertex_matrix) {
		jit_object_free(x->vertex_matrix);
		x->vertex_matrix = NULL;
	}
	if(x->normal_matrix) {
		jit_object_free(x->normal_matrix);
		x->normal_matrix = NULL;
	}
	if(x->index_matrix) {
		jit_object_free(x->index_matrix);
		x->index_matrix = NULL;
	}
}


#ifdef __cplusplus
}
#endif