#include "jit.field.grid.h"


#ifdef __cplusplus
extern "C" {
#endif


#define MIN_VOXEL_SIZE (1e-4)


void *_jit_field_grid_class;

t_jit_field_grid *jit_field_grid_new(void);
void jit_field_grid_free(t_jit_field_grid *x);
t_jit_err jit_field_grid_getattr_planecount(t_jit_field_grid *x, void *attr, long *argc, t_atom **argv);
t_jit_err jit_field_grid_setattr_planecount(t_jit_field_grid *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_field_grid_getattr_type(t_jit_field_grid *x, void *attr, long *argc, t_atom **argv);
t_jit_err jit_field_grid_setattr_type(t_jit_field_grid *x, void *attr, long argc, t_atom *argv);
t_jit_err jit_field_grid_getattr_voxelsize(t_jit_field_grid *x, void *attr, long *argc, t_atom **argv);
t_jit_err jit_field_grid_setattr_voxelsize(t_jit_field_grid *x, void *attr, long argc, t_atom *argv);

t_jit_err jit_field_grid_read(t_jit_field_grid *x, t_object *srcmatrix, t_object *dstmatrix);
t_jit_err jit_field_grid_read_world(
	t_jit_field_grid *x,
	t_jit_matrix_info *pos_minfo, char *pos_data,
	t_jit_matrix_info *field_minfo, char *field_data
);
t_jit_err jit_field_grid_read_index(
	t_jit_field_grid *x,
	t_jit_matrix_info *coord_minfo, char *coord_data,
	t_jit_matrix_info *field_minfo, char *field_data
);

t_jit_err jit_field_grid_write(t_jit_field_grid *x, t_object *posmatrix, t_object *fieldmatrix);
t_jit_err jit_field_grid_write_world(
	t_jit_field_grid *x,
	t_jit_matrix_info *pos_minfo, char *pos_data,
	t_jit_matrix_info *field_minfo, char *field_data
);
t_jit_err jit_field_grid_write_index(
	t_jit_field_grid *x,
	t_jit_matrix_info *coord_minfo, char *coord_data,
	t_jit_matrix_info *field_minfo, char *field_data
);

t_jit_err jit_field_grid_write_relative(t_jit_field_grid *x, t_object *posmatrix, t_object *fieldmatrix);
t_jit_err jit_field_grid_write_relative_world(
	t_jit_field_grid *x,
	t_jit_matrix_info *pos_minfo, char *pos_data,
	t_jit_matrix_info *field_minfo, char *field_data
);
t_jit_err jit_field_grid_write_relative_index(
	t_jit_field_grid *x,
	t_jit_matrix_info *coord_minfo, char *coord_data,
	t_jit_matrix_info *field_minfo, char *field_data
);

t_jit_err jit_field_grid_set_background_value(t_jit_field_grid *x, t_symbol *s, long argc, t_atom *argv);

t_jit_err jit_field_grid_init(void)
{
	long attrflags=0;
	void *attr;
		
	_jit_field_grid_class = jit_class_new("jit_field_grid",(method)jit_field_grid_new,(method)jit_field_grid_free,
		sizeof(t_jit_field_grid),0L);
		
	
	jit_class_addmethod(_jit_field_grid_class, (method)jit_object_register, "register", A_CANT, 0L);
	jit_class_addmethod(_jit_field_grid_class, (method)jit_field_grid_read, "read", A_CANT, 0L);
	jit_class_addmethod(_jit_field_grid_class, (method)jit_field_grid_write, "write", A_CANT, 0L);
	jit_class_addmethod(_jit_field_grid_class, (method)jit_field_grid_write_relative, "write_relative", A_CANT, 0L);
	jit_class_addmethod(_jit_field_grid_class, (method)jit_field_grid_set_background_value, "set_background_value", A_GIMME, 0L);
	
	// add attributes
	attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW;
	attr = jit_object_new(_jit_sym_jit_attr_offset,"planecount",_jit_sym_long,attrflags,
		(method)jit_field_grid_getattr_planecount,(method)jit_field_grid_setattr_planecount,NULL);
	jit_class_addattr(_jit_field_grid_class,(t_jit_object *)attr);
	
	attr = jit_object_new(_jit_sym_jit_attr_offset,"type",_jit_sym_symbol,attrflags,
		(method)jit_field_grid_getattr_type,(method)jit_field_grid_setattr_type,NULL);
	jit_class_addattr(_jit_field_grid_class,(t_jit_object *)attr);
	
	attr = jit_object_new(_jit_sym_jit_attr_offset,"voxelsize",_jit_sym_float32,attrflags,
		(method)jit_field_grid_getattr_voxelsize,(method)jit_field_grid_setattr_voxelsize,NULL);
	jit_class_addattr(_jit_field_grid_class,(t_jit_object *)attr);
	
	jit_class_register(_jit_field_grid_class);

	return JIT_ERR_NONE;
}

t_jit_field_grid * jit_field_grid_from_atom(t_atom *a)
{
	t_jit_field_grid *grid = NULL;
	t_symbol *fname = jit_atom_getsym(a);
	if(fname != _sym_nothing) {
		t_object *o = (t_object *)jit_object_findregistered(fname);
		if(o && jit_object_classname(o) == gensym("jit_field_grid")) {
			grid = (t_jit_field_grid *)o;
		}
	}
	return grid;
}

t_jit_err jit_field_grid_getattr_planecount(t_jit_field_grid *x, void *attr, long *argc, t_atom **argv)
{
	if (*argc && *argv) { ; }
	else {
		*argc = 1;
		if (!(*argv = (t_atom *)jit_getbytes(sizeof(t_atom) * *argc))) {
			*argc = 0;
			return JIT_ERR_OUT_OF_MEM;
		}
	}
	jit_atom_setlong(*argv, x->grid->getPlanecount());
	
	return JIT_ERR_NONE;
}

t_jit_err jit_field_grid_setattr_planecount(t_jit_field_grid *x, void *attr, long argc, t_atom *argv)
{
	if(argc && argv) {
		long planecount = CLAMP(jit_atom_getlong(argv), 1, 3);
		x->grid->setPlanecount(planecount);
	}
	return JIT_ERR_NONE;
}

t_jit_err jit_field_grid_getattr_voxelsize(t_jit_field_grid *x, void *attr, long *argc, t_atom **argv)
{
	if (*argc && *argv) { ; }
	else {
		*argc = 1;
		if (!(*argv = (t_atom *)jit_getbytes(sizeof(t_atom) * *argc))) {
			*argc = 0;
			return JIT_ERR_OUT_OF_MEM;
		}
	}
	jit_atom_setfloat(*argv, x->grid->getVoxelSize());
	
	return JIT_ERR_NONE;
}

t_jit_err jit_field_grid_setattr_voxelsize(t_jit_field_grid *x, void *attr, long argc, t_atom *argv)
{
	if(argc && argv) {
		float voxelSize = MAX(jit_atom_getfloat(argv), MIN_VOXEL_SIZE);
		x->grid->setVoxelSize(voxelSize);
	}
	return JIT_ERR_NONE;
}

t_jit_err jit_field_grid_getattr_type(t_jit_field_grid *x, void *attr, long *argc, t_atom **argv)
{
	if (*argc && *argv) { ; }
	else {
		*argc = 1;
		if (!(*argv = (t_atom *)jit_getbytes(sizeof(t_atom) * *argc))) {
			*argc = 0;
			return JIT_ERR_OUT_OF_MEM;
		}
	}
	switch(x->grid->getGridType()) {
		case GridInt:
			jit_atom_setsym(*argv, _jit_sym_long);
			break;
			
		case GridFloat:
			jit_atom_setsym(*argv, _jit_sym_float32);
			break;
	}
	
	return JIT_ERR_NONE;
}

t_jit_err jit_field_grid_setattr_type(t_jit_field_grid *x, void *attr, long argc, t_atom *argv)
{
	if(argc && argv) {
		t_symbol *type = jit_atom_getsym(argv);
		if(type != _jit_sym_long || type != _jit_sym_float32) {
			type = _jit_sym_float32;
		}
		if(type == _jit_sym_long) {
			x->grid->setGridType(GridInt);
		}
		else if(type == _jit_sym_float32) {
			x->grid->setGridType(GridFloat);
		}
	}
	return JIT_ERR_NONE;
}

t_jit_err jit_field_grid_points_to_vertex_matrix(t_jit_field_grid *x, const vector<Vec3s>& points, t_object *vertex_matrix)
{
	t_jit_matrix_info vertex_info;
	float *vptr;
	unsigned int i;
	
	vertex_info.planecount = 3;
	vertex_info.type = _jit_sym_float32;
	vertex_info.dimcount = 1;
	vertex_info.dim[0] = points.size();
	
	jit_object_method(vertex_matrix, _jit_sym_setinfo, &vertex_info);
	jit_object_method(vertex_matrix, _jit_sym_getinfo, &vertex_info);
	
	jit_object_method(vertex_matrix, _jit_sym_getdata, &vptr);
	for(i=0; i < points.size(); ++i) {
		const Vec3s& point = points[i];
		*vptr++ = point.x();
		*vptr++ = point.y();
		*vptr++ = point.z();
	}
	
	return JIT_ERR_NONE;
}

t_jit_err jit_field_grid_quads_to_index_matrix(t_jit_field_grid *x, const vector<Vec4I>& quads, t_object *index_matrix)
{
	t_jit_matrix_info index_info;
	t_int32	*iptr;
	unsigned int i;
	
	index_info.planecount = 1;
	index_info.type = _jit_sym_long;
	index_info.dimcount = 1;
	index_info.dim[0] = quads.size()*4;
	
	jit_object_method(index_matrix, _jit_sym_setinfo, &index_info);
	jit_object_method(index_matrix, _jit_sym_getinfo, &index_info);
	
	jit_object_method(index_matrix, _jit_sym_getdata, &iptr);
	for(i=0; i < quads.size(); ++i) {
		const Vec4I& quad = quads[i];
		*iptr++ = quad[0];
		*iptr++ = quad[1];
		*iptr++ = quad[2];
		*iptr++ = quad[3];
	}
	
	return JIT_ERR_NONE;
}

t_jit_err jit_field_grid_points_to_normal_matrix(t_jit_field_grid *x, const vector<Vec3s>& points, t_object *normal_matrix)
{
	t_jit_matrix_info normal_info;
	float *nptr;
	unsigned int i;
	JitGrid& grid = *(x->grid);
	
	normal_info.planecount = 3;
	normal_info.type = _jit_sym_float32;
	normal_info.dimcount = 1;
	normal_info.dim[0] = points.size();
	
	jit_object_method(normal_matrix, _jit_sym_setinfo, &normal_info);
	jit_object_method(normal_matrix, _jit_sym_getinfo, &normal_info);
	
	jit_object_method(normal_matrix, _jit_sym_getdata, &nptr);
	for(i=0; i < points.size(); ++i) {
		Vec3s normal = grid.gradient(points[i]);
		normal.normalize();
		*nptr++ = normal.x();
		*nptr++ = normal.y();
		*nptr++ = normal.z();
	}
	
	return JIT_ERR_NONE;
}

t_jit_err jit_field_grid_volume_to_mesh_points(t_jit_field_grid *x, t_object *vertex_matrix, float isolevel)
{
	t_jit_err err = JIT_ERR_NONE;
	
	vector<Vec3s> points;
	vector<Vec4I> quads;

	x->grid->volumeToMesh(points, quads, isolevel);
	jit_field_grid_points_to_vertex_matrix(x, points, vertex_matrix);
	
	return err;
}

t_jit_err jit_field_grid_volume_to_mesh_quads(t_jit_field_grid *x, t_object *vertex_matrix, t_object *normal_matrix, t_object *index_matrix, float isolevel)
{
	t_jit_err err = JIT_ERR_NONE;
	
	vector<Vec3s> points;
	vector<Vec4I> quads;

	x->grid->volumeToMesh(points, quads, isolevel);
	jit_field_grid_points_to_vertex_matrix(x, points, vertex_matrix);
	jit_field_grid_quads_to_index_matrix(x, quads, index_matrix);
	jit_field_grid_points_to_normal_matrix(x, points, normal_matrix);
	
	return err;
}


/*
	Assumes	
		- posmatrix is a 3-plane float32 or long matrix
		- posmatrix and dstmatrix have the same dimensionality
		- dstmatrix has the same type and planecount as the field
*/
t_jit_err jit_field_grid_read(t_jit_field_grid *x, t_object *srcmatrix, t_object *dstmatrix)
{
	t_jit_err err = JIT_ERR_NONE;
	t_jit_matrix_info srcminfo;
	t_jit_matrix_info dstminfo;
	char *srcdata = NULL;
	char *dstdata = NULL;
	
	jit_object_method(srcmatrix, _jit_sym_getinfo, &srcminfo);
	jit_object_method(srcmatrix, _jit_sym_getdata, &srcdata);
	
	jit_object_method(dstmatrix, _jit_sym_getinfo, &dstminfo);
	jit_object_method(dstmatrix, _jit_sym_getdata, &dstdata);
	
	if(srcminfo.planecount != 3) return err;
	
	if(srcminfo.type == _jit_sym_float32) {
		err = jit_field_grid_read_world(x, &srcminfo, srcdata, &dstminfo, dstdata);
	}
	else if(srcminfo.type == _jit_sym_long) {
		err = jit_field_grid_read_index(x, &srcminfo, srcdata, &dstminfo, dstdata);
	}
	
	return err;
}

t_jit_err jit_field_grid_read_world_ndim(
	t_jit_field_grid *x,
	int dimcount,
	t_jit_matrix_info *pos_minfo, char *pos_data,
	t_jit_matrix_info *field_minfo, char *field_data
)
{
	t_jit_err err = JIT_ERR_NONE;
	int i, j;
	int w, h;
	JitGrid &grid = *(x->grid);
	int planecount = field_minfo->planecount;
	
	switch(dimcount) {
		case 1:
		case 2:
			w = pos_minfo->dim[0];
			h = pos_minfo->dimcount > 1 ? pos_minfo->dim[1] : 1;
		
			for(j=0; j < h; ++j) {
				Vec3s *xyz = (Vec3s *)(pos_data + j*pos_minfo->dimstride[1]);
				float *v = (float *)(field_data + j*field_minfo->dimstride[1]);
				
				for(i=0; i < w; ++i) {
					grid.getPos(*xyz, v);
					v += planecount;
					++xyz;
				}
			}
			break;
		
		default:
			for(i=0; i< pos_minfo->dim[dimcount-1]; ++i) {
				char *pos_data_ndim = (char *)pos_data + i*pos_minfo->dimstride[dimcount-1];
				char *field_data_ndim  = (char *)field_data + i*field_minfo->dimstride[dimcount-1];
				jit_field_grid_read_world_ndim(x, dimcount-1, pos_minfo, pos_data_ndim, field_minfo, field_data_ndim);
			}
	}
	return err;
}

t_jit_err jit_field_grid_read_world(
	t_jit_field_grid *x,
	t_jit_matrix_info *pos_minfo, char *pos_data,
	t_jit_matrix_info *field_minfo, char *field_data
)
{
	t_jit_err err = JIT_ERR_NONE;
	err = jit_field_grid_read_world_ndim(x, pos_minfo->dimcount, pos_minfo, pos_data, field_minfo, field_data);
	return err;
}

t_jit_err jit_field_grid_read_index_ndim(
	t_jit_field_grid *x,
	int dimcount,
	t_jit_matrix_info *coord_minfo, char *coord_data,
	t_jit_matrix_info *field_minfo, char *field_data
)
{
	t_jit_err err = JIT_ERR_NONE;
	int i, j;
	int w, h;
	JitGrid &grid = *(x->grid);
	int planecount = field_minfo->planecount;
	
	switch(dimcount) {
		case 1:
		case 2:
			w = coord_minfo->dim[0];
			h = coord_minfo->dimcount > 1 ? coord_minfo->dim[1] : 1;
			
			for(j=0; j < h; ++j) {
				Coord *ijk = (Coord *)(coord_data + j*coord_minfo->dimstride[1]);
				float *v = (float *)(field_data + j*field_minfo->dimstride[1]);
				
				for(i=0; i < w; ++i) {
					grid.getIndex(*ijk, v);
					v += planecount;
					++ijk;
				}
			}
			break;
		
		default:
			for(i=0; i< coord_minfo->dim[dimcount-1]; ++i) {
				char *coord_data_ndim = (char *)coord_data + i*coord_minfo->dimstride[dimcount-1];
				char *field_data_ndim  = (char *)field_data + i*field_minfo->dimstride[dimcount-1];
				jit_field_grid_read_index_ndim(x, dimcount-1, coord_minfo, coord_data_ndim, field_minfo, field_data_ndim);
			}
	}
	return err;
}

t_jit_err jit_field_grid_read_index(
	t_jit_field_grid *x,
	t_jit_matrix_info *coord_minfo, char *coord_data,
	t_jit_matrix_info *field_minfo, char *field_data
)
{
	t_jit_err err = JIT_ERR_NONE;
	err = jit_field_grid_read_index_ndim(x, coord_minfo->dimcount, coord_minfo, coord_data, field_minfo, field_data);
	return err;
}

t_symbol * jit_field_grid_type_to_matrix_type(GridType v) {
	return v == GridInt ? _jit_sym_long : _jit_sym_float32;
}

long jit_field_grid_format_matches_matrix_info(t_jit_field_grid *x, t_jit_matrix_info *minfo)
{
	long planecount = x->grid->getPlanecount();
	t_symbol *type = jit_field_grid_type_to_matrix_type(x->grid->getGridType());
	return planecount == minfo->planecount && type == minfo->type;
}

/*
	Assumes	
		- posmatrix is a 3-plane float32 or long matrix
		- posmatrix and fieldmatrix have the same dimensionality
		- fieldmatrix has the same type and planecount as the field
*/
t_jit_err jit_field_grid_write(t_jit_field_grid *x, t_object *posmatrix, t_object *fieldmatrix)
{
	t_jit_err err = JIT_ERR_NONE;
	t_jit_matrix_info posminfo;
	t_jit_matrix_info fieldminfo;
	char *posdata = NULL;
	char *fielddata = NULL;
	
	jit_object_method(posmatrix, _jit_sym_getinfo, &posminfo);
	jit_object_method(posmatrix, _jit_sym_getdata, &posdata);
	
	jit_object_method(fieldmatrix, _jit_sym_getinfo, &fieldminfo);
	jit_object_method(fieldmatrix, _jit_sym_getdata, &fielddata);
	
	if(posminfo.planecount != 3) return err;
	if(!jit_field_grid_format_matches_matrix_info(x, &fieldminfo)) return err;
	
	if(posminfo.type == _jit_sym_float32) {
		err = jit_field_grid_write_world(x, &posminfo, posdata, &fieldminfo, fielddata);
	}
	else if(posminfo.type == _jit_sym_long) {
		err = jit_field_grid_write_index(x, &posminfo, posdata, &fieldminfo, fielddata);
	}
	
	return err;
}

t_jit_err jit_field_grid_write_world_ndim(
	t_jit_field_grid *x,
	int dimcount,
	t_jit_matrix_info *pos_minfo, char *pos_data,
	t_jit_matrix_info *field_minfo, char *field_data
)
{
	t_jit_err err = JIT_ERR_NONE;
	int i, j;
	int w, h;
	JitGrid &grid = *(x->grid);
	int planecount = field_minfo->planecount;
	
	switch(dimcount) {
		case 1:
		case 2:
			w = pos_minfo->dim[0];
			h = pos_minfo->dimcount > 1 ? pos_minfo->dim[1] : 1;
	
			for(j=0; j < h; ++j) {
				Vec3s *xyz = (Vec3s *)(pos_data + j*pos_minfo->dimstride[1]);
				float *v = (float *)(field_data + j*field_minfo->dimstride[1]);
				
				for(i=0; i < w; ++i) {
					grid.setPos(*xyz, v);
					v += planecount;
					++xyz;
				}
			}
			break;
			
		default:
			for(i=0; i< pos_minfo->dim[dimcount-1]; ++i) {
				char *pos_data_ndim = (char *)pos_data + i*pos_minfo->dimstride[dimcount-1];
				char *field_data_ndim  = (char *)field_data + i*field_minfo->dimstride[dimcount-1];
				jit_field_grid_write_world_ndim(x, dimcount-1, pos_minfo, pos_data_ndim, field_minfo, field_data_ndim);
			}
	}
			
	return err;
}

t_jit_err jit_field_grid_write_world(
	t_jit_field_grid *x,
	t_jit_matrix_info *pos_minfo, char *pos_data,
	t_jit_matrix_info *field_minfo, char *field_data
)
{
	t_jit_err err = JIT_ERR_NONE;
	err = jit_field_grid_write_world_ndim(x, pos_minfo->dimcount, pos_minfo, pos_data, field_minfo, field_data);
	return err;
}

t_jit_err jit_field_grid_write_index_ndim(
	t_jit_field_grid *x,
	int dimcount,
	t_jit_matrix_info *coord_minfo, char *coord_data,
	t_jit_matrix_info *field_minfo, char *field_data
)
{
	t_jit_err err = JIT_ERR_NONE;
	int i, j;
	int w, h;
	JitGrid &grid = *(x->grid);
	int planecount = field_minfo->planecount;
	
	switch(dimcount) {
		case 1:
		case 2:
			w = coord_minfo->dim[0];
			h = coord_minfo->dimcount > 1 ? coord_minfo->dim[1] : 1;
	
			for(j=0; j < h; ++j) {
				Coord *ijk = (Coord *)(coord_data + j*coord_minfo->dimstride[1]);
				float *v = (float *)(field_data + j*field_minfo->dimstride[1]);
				
				for(i=0; i < w; ++i) {
					grid.setIndex(*ijk, v);
					v += planecount;
					++ijk;
				}
			}
			break;
			
		default:
			for(i=0; i< coord_minfo->dim[dimcount-1]; ++i) {
				char *coord_data_ndim = (char *)coord_data + i*coord_minfo->dimstride[dimcount-1];
				char *field_data_ndim  = (char *)field_data + i*field_minfo->dimstride[dimcount-1];
				jit_field_grid_write_index_ndim(x, dimcount-1, coord_minfo, coord_data_ndim, field_minfo, field_data_ndim);
			}
	}
	return err;
}

t_jit_err jit_field_grid_write_index(
	t_jit_field_grid *x,
	t_jit_matrix_info *coord_minfo, char *coord_data,
	t_jit_matrix_info *field_minfo, char *field_data
)
{
	t_jit_err err = JIT_ERR_NONE;
	err = jit_field_grid_write_index_ndim(x, coord_minfo->dimcount, coord_minfo, coord_data, field_minfo, field_data);
	return err;
}

t_jit_err jit_field_grid_write_relative(t_jit_field_grid *x, t_object *posmatrix, t_object *fieldmatrix)
{
	t_jit_err err = JIT_ERR_NONE;
	t_jit_matrix_info posminfo;
	t_jit_matrix_info fieldminfo;
	char *posdata = NULL;
	char *fielddata = NULL;
	
	jit_object_method(posmatrix, _jit_sym_getinfo, &posminfo);
	jit_object_method(posmatrix, _jit_sym_getdata, &posdata);
	
	jit_object_method(fieldmatrix, _jit_sym_getinfo, &fieldminfo);
	jit_object_method(fieldmatrix, _jit_sym_getdata, &fielddata);
	
	if(posminfo.planecount != 3) return err;
	if(!jit_field_grid_format_matches_matrix_info(x, &fieldminfo)) return err;
	
	if(posminfo.type == _jit_sym_float32) {
		err = jit_field_grid_write_relative_world(x, &posminfo, posdata, &fieldminfo, fielddata);
	}
	else if(posminfo.type == _jit_sym_long) {
		err = jit_field_grid_write_relative_index(x, &posminfo, posdata, &fieldminfo, fielddata);
	}
	
	return err;
}

t_jit_err jit_field_grid_write_relative_world_ndim(
	t_jit_field_grid *x,
	int dimcount,
	t_jit_matrix_info *pos_minfo, char *pos_data,
	t_jit_matrix_info *field_minfo, char *field_data
)
{
	t_jit_err err = JIT_ERR_NONE;
	int i, j;
	int w, h;
	JitGrid &grid = *(x->grid);
	int planecount = field_minfo->planecount;
	
	switch(dimcount) {
		case 1:
		case 2:
			w = pos_minfo->dim[0];
			h = pos_minfo->dimcount > 1 ? pos_minfo->dim[1] : 1;
	
			for(j=0; j < h; ++j) {
				Vec3s *xyz = (Vec3s *)(pos_data + j*pos_minfo->dimstride[1]);
				float *v = (float *)(field_data + j*field_minfo->dimstride[1]);
				
				for(i=0; i < w; ++i) {
					float cval[3];
					grid.getPos(*xyz, cval);
					v[0] += cval[0];
					v[1] += cval[1];
					v[2] += cval[2];
					grid.setPos(*xyz, v);
					v += planecount;
					++xyz;
				}
			}
			break;
			
		default:
			for(i=0; i< pos_minfo->dim[dimcount-1]; ++i) {
				char *pos_data_ndim = (char *)pos_data + i*pos_minfo->dimstride[dimcount-1];
				char *field_data_ndim  = (char *)field_data + i*field_minfo->dimstride[dimcount-1];
				jit_field_grid_write_relative_world_ndim(x, dimcount-1, pos_minfo, pos_data_ndim, field_minfo, field_data_ndim);
			}
	}
			
	return err;
}

t_jit_err jit_field_grid_write_relative_world(
	t_jit_field_grid *x,
	t_jit_matrix_info *pos_minfo, char *pos_data,
	t_jit_matrix_info *field_minfo, char *field_data
)
{
	t_jit_err err = JIT_ERR_NONE;
	err = jit_field_grid_write_relative_world_ndim(x, pos_minfo->dimcount, pos_minfo, pos_data, field_minfo, field_data);
	return err;
}

t_jit_err jit_field_grid_write_relative_index_ndim(
	t_jit_field_grid *x,
	int dimcount,
	t_jit_matrix_info *coord_minfo, char *coord_data,
	t_jit_matrix_info *field_minfo, char *field_data
)
{
	t_jit_err err = JIT_ERR_NONE;
	int i, j;
	int w, h;
	JitGrid &grid = *(x->grid);
	int planecount = field_minfo->planecount;
	
	switch(dimcount) {
		case 1:
		case 2:
			w = coord_minfo->dim[0];
			h = coord_minfo->dimcount > 1 ? coord_minfo->dim[1] : 1;
	
			for(j=0; j < h; ++j) {
				Coord *ijk = (Coord *)(coord_data + j*coord_minfo->dimstride[1]);
				float *v = (float *)(field_data + j*field_minfo->dimstride[1]);
				
				for(i=0; i < w; ++i) {
					float cval[3];
					grid.getIndex(*ijk, cval);
					v[0] += cval[0];
					v[1] += cval[1];
					v[2] += cval[2];
					grid.setIndex(*ijk, v);
					v += planecount;
					++ijk;
				}
			}
			break;
			
		default:
			for(i=0; i< coord_minfo->dim[dimcount-1]; ++i) {
				char *coord_data_ndim = (char *)coord_data + i*coord_minfo->dimstride[dimcount-1];
				char *field_data_ndim  = (char *)field_data + i*field_minfo->dimstride[dimcount-1];
				jit_field_grid_write_relative_index_ndim(x, dimcount-1, coord_minfo, coord_data_ndim, field_minfo, field_data_ndim);
			}
	}
	return err;
}

t_jit_err jit_field_grid_write_relative_index(
	t_jit_field_grid *x,
	t_jit_matrix_info *coord_minfo, char *coord_data,
	t_jit_matrix_info *field_minfo, char *field_data
)
{
	t_jit_err err = JIT_ERR_NONE;
	err = jit_field_grid_write_relative_index_ndim(x, coord_minfo->dimcount, coord_minfo, coord_data, field_minfo, field_data);
	return err;
}

t_jit_err jit_field_grid_set_background_value(t_jit_field_grid *x, t_symbol *s, long argc, t_atom *argv) {
	t_jit_err err = JIT_ERR_NONE;
	if(argc && argv) {
		float v[3];
		for(int i=0; i < argc; ++i) {
			v[i] = jit_atom_getfloat(argv+i);
		}
		x->grid->setBackgroundValue(v);
	}
	return err;
}


t_jit_field_grid *jit_field_grid_new(void)
{
	t_jit_field_grid *x;
		
	if((x=(t_jit_field_grid *)jit_object_alloc(_jit_field_grid_class))) {
		x->grid = new JitGrid(GridFloat, 1, 0.05);
	}
	else {
		x = NULL;
	}	
	return x;
}

void jit_field_grid_free(t_jit_field_grid *x)
{
	if(x->grid) {
		delete x->grid;
		x->grid = NULL;
	}
}


#ifdef __cplusplus
}
#endif