jit.field
=========

Volumetric data processing for Jitter
==

The __jitfield__ set of objects are a family of volumetric field processing components.  A field is an object that associates a piece of information with a spatial location.  The core object it jit.field, which represents an infinite 3D voxel grid.  Conceptually, jit.field is a jit.matrix located in space with the exception that the field can have a value at any spatial location, so the number of cells in any given dimension is not fixed.

Since jit.field voxels are located in space, they have both a location and a size.  Position in controlled with the @position and @rotate attributes.  Size is controlled with the @voxelsize attribute.  All voxels in a jit.field object have the exact same size.

Each voxel in a field has a position defined relative to the field, which is in a coordinate system called index space.  Index space coordinates are in integers with the voxel at (0, 0, 0) at the center of the field.  Each voxel also has a corresponding position in world space, which is determined by apply the field’s position, rotate, and voxelsize attributes to the voxel’s index space coordinate.

![Voxel](/images/voxel.png)