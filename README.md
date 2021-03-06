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

![Narrow-Band Level Set](/images/narrowband.levelset.png)

### Transforming Fields
Fields can be transformed using the __jit.field.op__ object.  It’s just like __jit.op__ except for fields.  It supports a number of binary and unary operators:

#### Binary Operators
* __min__: Take the minimum of two fields.  The minimum on level sets is equivalent to a boolean union.
* __max__: Take the maximum of two fields.  The maximum on level sets is equivalent to a boolean intersection.
* __+__: Sum two fields per-voxel
* __*__: Multiply two fields per-voxel

####Unary Operators
Not all unary operators work for all field formats.  Furthermore, some unary operators take inputs of one format and output a different format.  The accepted input and output types of an operator are indicated in the following format: (\<planecount\> \<type\>) → (\<planecount\> \<type\>).  The format to the left of the arrow is the input format.  The format to the right is the corresponding output format.  The keyword *any* refers to any planecount or type depending on location.
* __normalize__: (any any) → (any any) Normalize a vector field.  If the field is scalar, no operation is performed.
* __magnitude__: (any any) → (1 any) Calculate the magnitude of a field.
* __curvature__: (1 float32) → (1 float32) Calculate the mean curvature of a level set
* __laplacian__: (1 float32) → (1 float32) Calculate the Laplacian of a level set
* __gradient__: (1 float32) → (3 float32) Calculate the gradient of a level set
* __divergence__: (any any) → (1 any) Calculate the divergence of a field.
* __curl__: (3 float32) → (3 float32) Calculate the curl of a vector field.

### Read/Write to a Field
To read and write to a field with a matrix, use the __jit.field.read__ and __jit.field.write__ objects.  Both objects can index fields in either world or index coordinates.  If the position matrix is type float32, the position matrix will be assumed to be in world coordinates.  If the matrix is type long, it will be assumed to be in index space.  When writing to a matrix, the values to be written need to have the same format as the field in terms of planecount and type.

### Mesh-Volume Conversion
In addition to directly reading from and writing to a field, level set fields can be generated from mesh and converted to meshes.  To convert a mesh to a field, use the __jit.field.volume__ object.  It can use the matrixoutput of a __jit.gl.model__ object for reading meshes stored on disk.  It can also convert jitter matrices representing geometry data like __jit.gl.mesh__.  For example, with __@drawmode__ set to trigrid, it will interpret a matrix of position values as a grid of triangles.

By contrast, the __jit.field.mesh__ object will convert a level set to a mesh.  It functions just like __jit.gl.isosurface__, using an __@isolevel__ parameter to select which contour of the level set to render as geometry.  The output can be fed directly into __jit.gl.mesh__.  If __@mode__ is set to mesh, the output will consist of vertex, normal, and index matrices.  If __@mode__ is set to particles, it will only output the vertex matrix.