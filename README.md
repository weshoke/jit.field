jit.field
=========

##### Volumetric data processing for Jitter

The __jit.field__ set of objects are a family of volumetric field processing components.  A field is an object that associates a piece of information with a spatial location.  The core object is __jit.field__, which represents an infinite 3D voxel grid.  Conceptually, __jit.field__ is a __jit.matrix__ located in space with the exception that the field can have a value at any spatial location, so the number of cells in any given dimension is not fixed.

Since __jit.field__ voxels are located in space, they have both a location and a size.  Position in controlled with the __@position__ and __@rotate__ attributes.  Size is controlled with the __@voxelsize__ attribute.  All voxels in a __jit.field__ object have the exact same size.

Each voxel in a field has a position defined relative to the field, which is in a coordinate system called _index space_.  Index space coordinates are in integers with the voxel at (0, 0, 0) at the center of the field.  Each voxel also has a corresponding position in _world space_, which is determined by apply the field’s position, rotate, and voxelsize attributes to the voxel’s index space coordinate.

![Voxel](/images/voxel.png)
![Voxel Grid](/images/voxel.grid.png)

Fields are useful for placing and retrieving values from spatial locations.  Two common uses for fields are as level sets and vector fields.  Level sets describe surfaces through a distance field.  The surface of the level set is located where the distance field is zero.  This is no different from how jit.gl.isosurface works.  The difference is that in jit.field, not every voxel needs to have a value explicitly defined.  Only those voxels near the level set surface matter since they mark the location of the surface.  Level sets defined this way are called narrow-band level sets.

### Narrow-Band Level Set Objects
__jit.field.sphere__ and __jit.field.particles__ generate narrow-band level sets.  The size of the narrow band is determined by the __@halfwidth__ attribute, which dictates home many voxels on either side of the level set to calculate the distance function.