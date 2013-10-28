#ifndef JIT_FIELD_GRID_H
#define JIT_FIELD_GRID_H

#include "jit.common.h"
#include "JitGrid.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _jit_field_grid 
{
	t_object	ob;
	JitGrid		*grid;
} t_jit_field_grid;


t_jit_field_grid * jit_field_grid_from_atom(t_atom *a);
t_jit_err jit_field_grid_volume_to_mesh_points(t_jit_field_grid *x, t_object *vertex_matrix, float isolevel);
t_jit_err jit_field_grid_volume_to_mesh_quads(t_jit_field_grid *x, t_object *vertex_matrix, t_object *normal_matrix, t_object *index_matrix, float isolevel);

#ifdef __cplusplus
}
#endif

#endif // JIT_FIELD_GRID_H
