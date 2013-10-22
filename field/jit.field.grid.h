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

#ifdef __cplusplus
}
#endif

#endif // JIT_FIELD_GRID_H
