#ifndef JIT_GRID_H
#define JIT_GRID_H

#include "jit.common.h"
#include <openvdb/openvdb.h>
#include <openvdb/Grid.h>
#include <openvdb/math/Stencils.h>
#include <openvdb/tools/Composite.h>
#include <openvdb/tools/GridOperators.h>
#include <openvdb/tools/LevelSetFilter.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/MeshToVolume.h>
#include <openvdb/tools/ParticlesToLevelSet.h>
#include <openvdb/tools/VolumeToMesh.h>
#include <openvdb/tools/ValueTransformer.h>
#include <openvdb/util/NullInterrupter.h>
#include <openvdb/util/Util.h>
#include <openvdb/math/Math.h>
#include <vector>

typedef openvdb::Grid<openvdb::Vec2STree> Vec2SGrid;
typedef openvdb::Grid<openvdb::Vec2ITree> Vec2IGrid;
typedef openvdb::Grid<openvdb::Vec3ITree> Vec3IGrid;

using openvdb::FloatGrid;
using openvdb::Vec3SGrid;
using openvdb::Int32Grid;
using openvdb::Vec2i;
using openvdb::Vec3i;
using openvdb::Vec2s;
using openvdb::Vec3s;
using openvdb::Vec3R;
using openvdb::Vec3I;
using openvdb::Vec4I;
using openvdb::Coord;
using std::vector;

enum GridType{
	GridInt=0,
	GridFloat
};


class JitGrid {
public:
	JitGrid(GridType gridType, int planecount, float voxelSize)
	:	gridType(gridType),
		planecount(planecount),
		voxelSize(voxelSize)
	{
		createGrid();
	}
	
	virtual ~JitGrid() {}
	
	bool operator==(const JitGrid& other);
	void shallowCopy(const JitGrid& src);
	
	GridType getGridType() const { return gridType; }
	void setGridType(GridType v);
	int getPlanecount() const { return planecount; }
	void setPlanecount(int v);
	float getVoxelSize() const { return voxelSize; }
	void setVoxelSize(float v);
	
	void setBackgroundValue(float *v);
	void setBackgroundValue(int *v);
	void sphere(const Vec3s& center, float radius, float halfWidth);
	void meshToVolume(const vector<Vec3s>& points, const vector<Vec3I>& triangles, float halfwidth);
	void volumeToMesh(vector<Vec3s>& points, vector<Vec4I>& indices, float isovalue);
	void rasterizeParticles(t_jit_matrix_info *minfo, char *ptr, float radius);
	
	template<typename T>
	void compMax(JitGrid& in1, JitGrid& in2);
	void compMax(JitGrid& in1, JitGrid& in2);
	
	template<typename T>
	void compMin(JitGrid& in1, JitGrid& in2);
	void compMin(JitGrid& in1, JitGrid& in2);
	
	template<typename T>
	void compSum(JitGrid& in1, JitGrid& in2);
	void compSum(JitGrid& in1, JitGrid& in2);
	
	template<typename T>
	void compMul(JitGrid& in1, JitGrid& in2);
	void compMul(JitGrid& in1, JitGrid& in2);
	
	template<typename T>
	void normalize(JitGrid& in1);
	void normalize(JitGrid& in1);
	
	template<typename T>
	void magnitude(JitGrid& in1);
	void magnitude(JitGrid& in1);
	
	template<typename T>
	void curvature(JitGrid& in1);
	void curvature(JitGrid& in1);
	
	template<typename T>
	void laplacian(JitGrid& in1);
	void laplacian(JitGrid& in1);
	
	template<typename T>
	void gradient(JitGrid& in1);
	void gradient(JitGrid& in1);
	
	template<typename T>
	void divergence(JitGrid& in1);
	void divergence(JitGrid& in1);
	
	template<typename T>
	void curl(JitGrid& in1);
	void curl(JitGrid& in1);
	
	void meanCurvatureFilter();
	void laplacianFilter();
	void gaussianFilter(float width);
	void offsetFilter(float offset);
	void medianFilter(float width);
	void meanFilter(float width);
	
	Vec3s gradient(const Vec3s& xyz) const;
	
	Vec3s indexToWorld(const Coord& ijk) const;
	Coord worldToIndex(const Vec3s& xyz) const;
	
	void setIndex(const Coord& ijk, int *v);
	void getIndex(const Coord& ijk, int *v) const;
	void setPos(const Vec3s& xyz, int *v);
	void getPos(const Vec3s& xyz, int *v) const;
	
	void setIndex(const Coord& ijk, float *v);
	void getIndex(const Coord& ijk, float *v) const;
	void setPos(const Vec3s& xyz, float *v);
	void getPos(const Vec3s& xyz, float *v) const;
	
	bool isLevelset() const;

protected:
	void rasterizeParticlesND(openvdb::tools::ParticlesToLevelSet<FloatGrid>& particleRasterizer, int dimcount, t_jit_matrix_info *minfo, char *ptr, float radius);


	void createGrid();
	void destroyGrid();
	
	template<typename T>
	void setGrid(const typename T::Ptr& ptr);
	
	bool equivalentFormats(const JitGrid& grid) const;

	GridType gridType;
	int planecount;
	float voxelSize;
	
	Int32Grid::Ptr int1Grid;
	Vec2IGrid::Ptr int2Grid;
	Vec3IGrid::Ptr int3Grid;
	FloatGrid::Ptr float1Grid;
	Vec2SGrid::Ptr float2Grid;
	Vec3SGrid::Ptr float3Grid;
};

#endif // JIT_GRID_H
