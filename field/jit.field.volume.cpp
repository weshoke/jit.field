#include "jit.common.h"
#include "jit.vecmath.h"
#include "jit.field.grid.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _jit_field_volume
{
	t_object	ob;
	t_object	*field;
	float		halfwidth;
	t_symbol	*drawmode;
} t_jit_field_volume;

void *_jit_field_volume_class;

t_jit_field_volume *jit_field_volume_new(void);
void jit_field_volume_free(t_jit_field_volume *x);
void * jit_field_volume_getfield(t_jit_field_volume *x);
t_jit_err jit_field_volume_mesh_to_volume(t_jit_field_volume *x, t_object *vertex_matrix, t_object *index_matrix);

t_jit_err jit_field_volume_init(void)
{
	long attrflags=0;
	t_jit_object *attr;
		
	_jit_field_volume_class = jit_class_new("jit_field_volume",(method)jit_field_volume_new,(method)jit_field_volume_free,
		sizeof(t_jit_field_volume),0L);

	// add attributes	
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW;
	
	jit_class_addmethod(_jit_field_volume_class, (method)jit_field_volume_mesh_to_volume, "mesh_to_volume",	A_CANT, 0L);
	jit_class_addmethod(_jit_field_volume_class, (method)jit_field_volume_getfield, "getfield",	A_CANT, 0L);
	
	
	attr = (t_jit_object *)jit_object_new(_jit_sym_jit_attr_offset, "halfwidth", _jit_sym_float32, attrflags,
				(method)0L, (method)0L, calcoffset(t_jit_field_volume, halfwidth));
    jit_class_addattr(_jit_field_volume_class, (t_jit_object*)attr);
	
	attr = (t_jit_object *)jit_object_new(_jit_sym_jit_attr_offset, "drawmode", _jit_sym_symbol, attrflags,
				(method)0L, (method)0L, calcoffset(t_jit_field_volume, drawmode));
    jit_class_addattr(_jit_field_volume_class, (t_jit_object*)attr);
	CLASS_ATTR_ENUM((t_class *)_jit_field_volume_class, "drawmode", 0, "trigrid triangles");

	jit_class_register(_jit_field_volume_class);

	return JIT_ERR_NONE;
}

void * jit_field_volume_getfield(t_jit_field_volume *x)
{
	return x->field;
}

t_jit_err jit_field_volume_triangles_nd(t_jit_field_volume *x, int dimcount, t_jit_matrix_info *index_minfo, char *index_ptr, vector<Vec3I> &triangles)
{
	int w, h;
	int i, j;
	int ntri;

	switch(dimcount) {
		case 1:
		case 2:
			w = index_minfo->dim[0];
			h = dimcount == 1 ? 1 : index_minfo->dim[1];
			ntri = w/3;
			for(j=0; j < h; ++j) {
				int32_t *iptr = (int32_t *)(index_ptr + j*index_minfo->dimstride[1]);
				for(i=0; i < ntri; ++i) {
					uint32_t i1 = *iptr++;
					uint32_t i2 = *iptr++;
					uint32_t i3 = *iptr++;
					triangles.push_back(Vec3I(i1, i2, i3));
				}
			}
			break;
		
		default:
			for(i=0; i < index_minfo->dim[dimcount-1]; ++i) {
				char *iptr = index_ptr + i*index_minfo->dimstride[dimcount-1];
				jit_field_volume_triangles_nd(x, dimcount-1, index_minfo, iptr, triangles);
			}
	}
	return JIT_ERR_NONE;
}

t_jit_err jit_field_volume_points_nd(t_jit_field_volume *x, int dimcount, t_jit_matrix_info *vertex_minfo, char *vertex_ptr, vector<Vec3s> &points)
{
	int w, h;
	int i, j;
	int planecount;

	switch(dimcount) {
		case 1:
		case 2:
			w = vertex_minfo->dim[0];
			h = dimcount == 1 ? 1 : vertex_minfo->dim[1];
			planecount = vertex_minfo->planecount;
			for(j=0; j < h; ++j) {
				float *vptr = (float *)(vertex_ptr + j*vertex_minfo->dimstride[1]);
				for(i=0; i < w; ++i) {
					float x = vptr[0];
					float y = vptr[1];
					float z = vptr[2];
					points.push_back(Vec3s(x, y, z));
					vptr += planecount;
				}
			}
			break;
		
		default:
			for(i=0; i < vertex_minfo->dim[dimcount-1]; ++i) {
				char *vptr = vertex_ptr + i*vertex_minfo->dimstride[dimcount-1];
				jit_field_volume_points_nd(x, dimcount-1, vertex_minfo, vptr, points);
			}
	}
	return JIT_ERR_NONE;
}

t_jit_err jit_field_volume_mesh_to_volume(t_jit_field_volume *x, t_object *vertex_matrix, t_object *index_matrix)
{
	t_jit_matrix_info vertex_minfo;
	char *vertex_ptr;
	t_jit_field_grid *grid = (t_jit_field_grid *)jit_object_method(x->field, gensym("getgrid"));
	
	jit_object_method(vertex_matrix, _jit_sym_getinfo, &vertex_minfo);
	if(vertex_minfo.type != _jit_sym_float32) {
		jit_object_error((t_object *)x, "vertex matrix must be float32");
		return JIT_ERR_NONE;
	}
	jit_object_method(vertex_matrix, _jit_sym_getdata, &vertex_ptr);
	
	vector<Vec3s> points;
	vector<Vec3I> triangles;
	jit_field_volume_points_nd(x, vertex_minfo.dimcount, &vertex_minfo, vertex_ptr, points);
	
	if(index_matrix) {
		t_jit_matrix_info index_minfo;
		char *index_ptr;
	
		jit_object_method(index_matrix, _jit_sym_getinfo, &index_minfo);
		if(index_minfo.type != _jit_sym_float32) {
			jit_object_error((t_object *)x, "vertex matrix must be long");
			return JIT_ERR_NONE;
		}
		if(index_minfo.planecount != 1) {
			jit_object_error((t_object *)x, "vertex matrix must be 1 planes");
			return JIT_ERR_NONE;
		}
		jit_object_method(index_matrix, _jit_sym_getdata, &index_ptr);
		
		jit_field_volume_triangles_nd(x, index_minfo.dimcount, &index_minfo, index_ptr, triangles);
	}
	else {
		// only 2D matrices
		if(x->drawmode == gensym("trigrid")) {
			int w = vertex_minfo.dim[0];
			int h = vertex_minfo.dimcount > 1 ? vertex_minfo.dim[1] : 1;
			for(int j=0; j < h-1; ++j) {
				for(int i=0; i < w-1; ++i) {
					triangles.push_back(Vec3I(i+j*w, i+(j+1)*w, i+1+j*w));
					triangles.push_back(Vec3I(i+1+j*w, i+(j+1)*w, i+1+(j+1)*w));
				}
			}
		}
		else {
			int ntri = points.size()/3;
			for(int i=0; i < ntri; ++i) {
				triangles.push_back(Vec3I(3*i, 3*i+1, 3*i+2));
			}
		}
	}
	grid->grid->meshToVolume(points, triangles, x->halfwidth);
	
	return JIT_ERR_NONE;
}


t_jit_field_volume *jit_field_volume_new(void)
{
	t_jit_field_volume *x;
		
	if((x=(t_jit_field_volume *)jit_object_alloc(_jit_field_volume_class))) {
		x->field = (t_object *)jit_object_new(gensym("jit_field"));
		x->halfwidth = openvdb::OPENVDB_VERSION_NAME::LEVEL_SET_HALF_WIDTH;
		x->drawmode = gensym("trigrid");
	}
	else {
		x = NULL;
	}	
	return x;
}

void jit_field_volume_free(t_jit_field_volume *x)
{
	if(x->field) {
		object_free(x->field);
		x->field = NULL;
	}
}


#ifdef __cplusplus
}
#endif