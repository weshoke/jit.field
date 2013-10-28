#include "JitGrid.h"
#include <iostream>

using std::cout;

class ParticlePositionList {
public:

	ParticlePositionList(openvdb::Real radius, size_t n, Vec3s *points)
	:	radius(radius), n(n), points(points)
	{}

	// Return the total number of particles in list.
	// Always required!
	size_t size() const { return n; }

	// Get the world space position of n'th particle.
	// Required by ParticledToLevelSet::rasterizeSphere(*this,radius).
	void getPos(size_t n, Vec3R& xyz) const {
		points[n].toV(xyz.asPointer());
	}

	// Get the world space position and radius of n'th particle.
	// Required by ParticledToLevelSet::rasterizeSphere(*this).
	void getPosRad(size_t n, Vec3R& xyz, openvdb::Real& rad) const {
		points[n].toV(xyz.asPointer());
		rad = radius;
	}

	// Get the world space position, radius and velocity of n'th particle.
	// Required by ParticledToLevelSet::rasterizeSphere(*this,radius).
	void getPosRadVel(size_t n, Vec3R& xyz, openvdb::Real& rad, Vec3R& vel) const {
		points[n].toV(xyz.asPointer());
		rad = radius;
		vel.setZero();
	}

	// Get the attribute of the n'th particle. AttributeType is user-defined!
	// Only required if attribute transfer is enabled in ParticledToLevelSet.
	//void getAtt(AttributeType& att) const {}
	
protected:
	openvdb::Real radius;
	size_t n;
	Vec3s *points;
};

bool JitGrid::operator==(const JitGrid& other) {
	if(planecount == other.planecount && gridType == other.gridType) {
		switch(gridType) {
			case GridInt:
				switch(planecount) {
					case 1: return int1Grid == other.int1Grid;
					case 2: return int2Grid == other.int2Grid;
					case 3: return int3Grid == other.int3Grid;
				}
				break;
			
			case GridFloat:
				switch(planecount) {
					case 1: return float1Grid == other.float1Grid;
					case 2: return float2Grid == other.float2Grid;
					case 3: return float3Grid == other.float3Grid;
				}
				break;
		}
	}
	return false;
}

void JitGrid::shallowCopy(const JitGrid& src) {
	if(*this == src) return;

	destroyGrid();
	gridType = src.gridType;
	planecount = src.planecount;
	voxelSize = src.voxelSize;
	
	switch(gridType) {
		case GridInt:
			switch(planecount) {
				case 1: int1Grid = src.int1Grid; break;
				case 2: int2Grid = src.int2Grid; break;
				case 3: int3Grid = src.int3Grid; break;
			}
			break;
		
		case GridFloat:
			switch(planecount) {
				case 1: float1Grid = src.float1Grid; break;
				case 2: float2Grid = src.float2Grid; break;
				case 3: float3Grid = src.float3Grid; break;
			}
			break;
	}
}

void JitGrid::setGridType(GridType v) {
	destroyGrid();
	gridType = v;
	createGrid();
}

void JitGrid::setPlanecount(int v) {
	destroyGrid();
	planecount = v;
	createGrid();
}

void JitGrid::setVoxelSize(float v) {
	destroyGrid();
	voxelSize = v;
	createGrid();
}

template<>
void JitGrid::setGrid<Int32Grid>(const Int32Grid::Ptr& ptr) {
	if(gridType != GridInt || planecount != 1) {
		destroyGrid();
		gridType = GridInt;
		planecount = 1;
	}
	int1Grid = ptr;
	voxelSize = ptr->transform().voxelSize()[0];
}

template<>
void JitGrid::setGrid<Vec2IGrid>(const Vec2IGrid::Ptr& ptr) {
	if(gridType != GridInt || planecount != 2) {
		destroyGrid();
		gridType = GridInt;
		planecount = 2;
	}
	int2Grid = ptr;
	voxelSize = ptr->transform().voxelSize()[0];
}

template<>
void JitGrid::setGrid<Vec3IGrid>(const Vec3IGrid::Ptr& ptr) {
	if(gridType != GridInt || planecount != 3) {
		destroyGrid();
		gridType = GridInt;
		planecount = 3;
	}
	int3Grid = ptr;
	voxelSize = ptr->transform().voxelSize()[0];
}

template<>
void JitGrid::setGrid<FloatGrid>(const FloatGrid::Ptr& ptr) {
	if(gridType != GridFloat || planecount != 1) {
		destroyGrid();
		gridType = GridFloat;
		planecount = 1;
	}
	float1Grid = ptr;
	voxelSize = ptr->transform().voxelSize()[0];
}

template<>
void JitGrid::setGrid<Vec2SGrid>(const Vec2SGrid::Ptr& ptr) {
	if(gridType != GridFloat || planecount != 2) {
		destroyGrid();
		gridType = GridFloat;
		planecount = 2;
	}
	float2Grid = ptr;
	voxelSize = ptr->transform().voxelSize()[0];
}

template<>
void JitGrid::setGrid<Vec3SGrid>(const Vec3SGrid::Ptr& ptr) {
	if(gridType != GridFloat || planecount != 3) {
		destroyGrid();
		gridType = GridFloat;
		planecount = 3;
	}
	float3Grid = ptr;
	voxelSize = ptr->transform().voxelSize()[0];
}


void JitGrid::setBackgroundValue(float *v) {
	switch(gridType) {
		case GridFloat:
			switch(planecount) {
				case 1: float1Grid->setBackground(*v); break;
				case 2: float2Grid->setBackground(Vec2s(v)); break;
				case 3: float3Grid->setBackground(Vec3s(v)); break;
			}
			break;
		default: break;
	}
}

void JitGrid::setBackgroundValue(int *v) {
	switch(gridType) {
		case GridInt:
			switch(planecount) {
				case 1: int1Grid->setBackground(*v); break;
				case 2: int2Grid->setBackground(Vec2i(v)); break;
				case 3: int3Grid->setBackground(Vec3i(v)); break;
			}
			break;
		default: break;
	}
}

void JitGrid::sphere(const Vec3s& center, float radius, float halfWidth) {
	if(!isLevelset()) {
		destroyGrid();
	}
	float1Grid = openvdb::tools::createLevelSetSphere<FloatGrid>(radius, center, voxelSize, halfWidth);
}

void JitGrid::meshToVolume(const vector<Vec3s>& points, const vector<Vec3I>& triangles, float halfwidth) {
	if(!isLevelset()) {
		destroyGrid();
	}
	openvdb::math::Transform::Ptr xform = openvdb::math::Transform::createLinearTransform(voxelSize);
	float1Grid = openvdb::tools::meshToLevelSet<FloatGrid>(*xform, points, triangles, halfwidth);
}

void JitGrid::volumeToMesh(vector<Vec3s>& points, vector<Vec4I>& indices, float isovalue) {
	if(isLevelset()) {
		openvdb::tools::volumeToMesh(*float1Grid, points, indices, isovalue);
	}
}


void JitGrid::rasterizeParticles(t_jit_matrix_info *minfo, char *ptr, float radius) {
	if(!isLevelset()) {
		destroyGrid();
		createGrid();
	}

	if(minfo->type == _jit_sym_float32 && minfo->planecount == 3) {
		// The width in voxel units of the generated narrow band level set is given by 2*background/dx
		openvdb::tools::ParticlesToLevelSet<FloatGrid> particleRasterizer(*float1Grid);
		float1Grid->clear();
		rasterizeParticlesND(particleRasterizer, minfo->dimcount, minfo, ptr, radius);
	}
}

void JitGrid::rasterizeParticlesND(openvdb::tools::ParticlesToLevelSet<FloatGrid>& particleRasterizer, int dimcount, t_jit_matrix_info *minfo, char *ptr, float radius) {
	int w, h;
	
	switch(dimcount) {
		case 1:
		case 2:
			w = minfo->dim[0];
			h = minfo->dimcount >= 2 ? minfo->dim[1] : 1;
			for(int j=0; j < h; ++j) {
				Vec3s *v = (Vec3s *)(ptr + j*minfo->dimstride[1]);
				ParticlePositionList particles(radius, w, v);
				particleRasterizer.rasterizeSpheres(particles, radius);
			}
			break;
		
		default:
			for(int i=0; i< minfo->dim[dimcount-1]; ++i) {
				char *ptr_ndim = (char *)ptr + i*minfo->dimstride[dimcount-1];
				rasterizeParticlesND(particleRasterizer, dimcount-1, minfo, ptr_ndim, radius);
			}
	}
}


template<>
void JitGrid::compMax<Int32Grid>(JitGrid& in1, JitGrid& in2) {
	Int32Grid::Ptr
		copyOfIn1 = in1.int1Grid->deepCopy(),
		copyOfIn2 = in2.int1Grid->deepCopy();

	openvdb::tools::compMax(*copyOfIn1, *copyOfIn2);
	setGrid<Int32Grid>(copyOfIn1);
}

template<>
void JitGrid::compMax<Vec2IGrid>(JitGrid& in1, JitGrid& in2) {
	Vec2IGrid::Ptr
		copyOfIn1 = in1.int2Grid->deepCopy(),
		copyOfIn2 = in2.int2Grid->deepCopy();

	openvdb::tools::compMax(*copyOfIn1, *copyOfIn2);
	setGrid<Vec2IGrid>(copyOfIn1);
}

template<>
void JitGrid::compMax<Vec3IGrid>(JitGrid& in1, JitGrid& in2) {
	Vec3IGrid::Ptr
		copyOfIn1 = in1.int3Grid->deepCopy(),
		copyOfIn2 = in2.int3Grid->deepCopy();

	openvdb::tools::compMax(*copyOfIn1, *copyOfIn2);
	setGrid<Vec3IGrid>(copyOfIn1);
}

template<>
void JitGrid::compMax<FloatGrid>(JitGrid& in1, JitGrid& in2) {
	FloatGrid::Ptr
		copyOfIn1 = in1.float1Grid->deepCopy(),
		copyOfIn2 = in2.float1Grid->deepCopy();

	openvdb::tools::compMax(*copyOfIn1, *copyOfIn2);
	setGrid<FloatGrid>(copyOfIn1);
}

template<>
void JitGrid::compMax<Vec2SGrid>(JitGrid& in1, JitGrid& in2) {
	Vec2SGrid::Ptr
		copyOfIn1 = in1.float2Grid->deepCopy(),
		copyOfIn2 = in2.float2Grid->deepCopy();

	openvdb::tools::compMax(*copyOfIn1, *copyOfIn2);
	setGrid<Vec2SGrid>(copyOfIn1);
}

template<>
void JitGrid::compMax<Vec3SGrid>(JitGrid& in1, JitGrid& in2) {
	Vec3SGrid::Ptr
		copyOfIn1 = in1.float3Grid->deepCopy(),
		copyOfIn2 = in2.float3Grid->deepCopy();

	openvdb::tools::compMax(*copyOfIn1, *copyOfIn2);
	setGrid<Vec3SGrid>(copyOfIn1);
}

void JitGrid::compMax(JitGrid& in1, JitGrid& in2) {
	if(!in1.equivalentFormats(in2)) {
		JitGrid in2Converted(in1.getGridType(), in1.getPlanecount(), in2.getVoxelSize());
		
		compMax(in1, in2Converted);
	}
	else {
		switch(in1.gridType) {
			case GridInt:
				switch(in1.planecount) {
					case 1: compMax<Int32Grid>(in1, in2); break;
					case 2: compMax<Vec2IGrid>(in1, in2); break;
					case 3: compMax<Vec3IGrid>(in1, in2); break;
				}
				break;
			
			case GridFloat:
				switch(in1.planecount) {
					case 1: compMax<FloatGrid>(in1, in2); break;
					case 2: compMax<Vec2SGrid>(in1, in2); break;
					case 3: compMax<Vec3SGrid>(in1, in2); break;
				}
				break;
		}
	}
}

template<>
void JitGrid::compMin<Int32Grid>(JitGrid& in1, JitGrid& in2) {
	Int32Grid::Ptr
		copyOfIn1 = in1.int1Grid->deepCopy(),
		copyOfIn2 = in2.int1Grid->deepCopy();

	openvdb::tools::compMin(*copyOfIn1, *copyOfIn2);
	setGrid<Int32Grid>(copyOfIn1);
}

template<>
void JitGrid::compMin<Vec2IGrid>(JitGrid& in1, JitGrid& in2) {
	Vec2IGrid::Ptr
		copyOfIn1 = in1.int2Grid->deepCopy(),
		copyOfIn2 = in2.int2Grid->deepCopy();

	openvdb::tools::compMin(*copyOfIn1, *copyOfIn2);
	setGrid<Vec2IGrid>(copyOfIn1);
}

template<>
void JitGrid::compMin<Vec3IGrid>(JitGrid& in1, JitGrid& in2) {
	Vec3IGrid::Ptr
		copyOfIn1 = in1.int3Grid->deepCopy(),
		copyOfIn2 = in2.int3Grid->deepCopy();

	openvdb::tools::compMin(*copyOfIn1, *copyOfIn2);
	setGrid<Vec3IGrid>(copyOfIn1);
}

template<>
void JitGrid::compMin<FloatGrid>(JitGrid& in1, JitGrid& in2) {
	FloatGrid::Ptr
		copyOfIn1 = in1.float1Grid->deepCopy(),
		copyOfIn2 = in2.float1Grid->deepCopy();

	openvdb::tools::compMin(*copyOfIn1, *copyOfIn2);
	setGrid<FloatGrid>(copyOfIn1);
}

template<>
void JitGrid::compMin<Vec2SGrid>(JitGrid& in1, JitGrid& in2) {
	Vec2SGrid::Ptr
		copyOfIn1 = in1.float2Grid->deepCopy(),
		copyOfIn2 = in2.float2Grid->deepCopy();

	openvdb::tools::compMin(*copyOfIn1, *copyOfIn2);
	setGrid<Vec2SGrid>(copyOfIn1);
}

template<>
void JitGrid::compMin<Vec3SGrid>(JitGrid& in1, JitGrid& in2) {
	Vec3SGrid::Ptr
		copyOfIn1 = in1.float3Grid->deepCopy(),
		copyOfIn2 = in2.float3Grid->deepCopy();

	openvdb::tools::compMin(*copyOfIn1, *copyOfIn2);
	setGrid<Vec3SGrid>(copyOfIn1);
}

void JitGrid::compMin(JitGrid& in1, JitGrid& in2) {
	if(!in1.equivalentFormats(in2)) {
		JitGrid in2Converted(in1.getGridType(), in1.getPlanecount(), in2.getVoxelSize());
		
		compMin(in1, in2Converted);
	}
	else {
		switch(in1.gridType) {
			case GridInt:
				switch(in1.planecount) {
					case 1: compMin<Int32Grid>(in1, in2); break;
					case 2: compMin<Vec2IGrid>(in1, in2); break;
					case 3: compMin<Vec3IGrid>(in1, in2); break;
				}
				break;
			
			case GridFloat:
				switch(in1.planecount) {
					case 1: compMin<FloatGrid>(in1, in2); break;
					case 2: compMin<Vec2SGrid>(in1, in2); break;
					case 3: compMin<Vec3SGrid>(in1, in2); break;
				}
				break;
		}
	}
}

template<>
void JitGrid::compSum<Int32Grid>(JitGrid& in1, JitGrid& in2) {
	Int32Grid::Ptr
		copyOfIn1 = in1.int1Grid->deepCopy(),
		copyOfIn2 = in2.int1Grid->deepCopy();

	openvdb::tools::compSum(*copyOfIn1, *copyOfIn2);
	setGrid<Int32Grid>(copyOfIn1);
}

template<>
void JitGrid::compSum<Vec2IGrid>(JitGrid& in1, JitGrid& in2) {
	Vec2IGrid::Ptr
		copyOfIn1 = in1.int2Grid->deepCopy(),
		copyOfIn2 = in2.int2Grid->deepCopy();

	openvdb::tools::compSum(*copyOfIn1, *copyOfIn2);
	setGrid<Vec2IGrid>(copyOfIn1);
}

template<>
void JitGrid::compSum<Vec3IGrid>(JitGrid& in1, JitGrid& in2) {
	Vec3IGrid::Ptr
		copyOfIn1 = in1.int3Grid->deepCopy(),
		copyOfIn2 = in2.int3Grid->deepCopy();

	openvdb::tools::compSum(*copyOfIn1, *copyOfIn2);
	setGrid<Vec3IGrid>(copyOfIn1);
}

template<>
void JitGrid::compSum<FloatGrid>(JitGrid& in1, JitGrid& in2) {
	FloatGrid::Ptr
		copyOfIn1 = in1.float1Grid->deepCopy(),
		copyOfIn2 = in2.float1Grid->deepCopy();

	openvdb::tools::compSum(*copyOfIn1, *copyOfIn2);
	setGrid<FloatGrid>(copyOfIn1);
}

template<>
void JitGrid::compSum<Vec2SGrid>(JitGrid& in1, JitGrid& in2) {
	Vec2SGrid::Ptr
		copyOfIn1 = in1.float2Grid->deepCopy(),
		copyOfIn2 = in2.float2Grid->deepCopy();

	openvdb::tools::compSum(*copyOfIn1, *copyOfIn2);
	setGrid<Vec2SGrid>(copyOfIn1);
}

template<>
void JitGrid::compSum<Vec3SGrid>(JitGrid& in1, JitGrid& in2) {
	Vec3SGrid::Ptr
		copyOfIn1 = in1.float3Grid->deepCopy(),
		copyOfIn2 = in2.float3Grid->deepCopy();

	openvdb::tools::compSum(*copyOfIn1, *copyOfIn2);
	setGrid<Vec3SGrid>(copyOfIn1);
}

void JitGrid::compSum(JitGrid& in1, JitGrid& in2) {
	if(!in1.equivalentFormats(in2)) {
		JitGrid in2Converted(in1.getGridType(), in1.getPlanecount(), in2.getVoxelSize());
		
		compSum(in1, in2Converted);
	}
	else {
		switch(in1.gridType) {
			case GridInt:
				switch(in1.planecount) {
					case 1: compSum<Int32Grid>(in1, in2); break;
					case 2: compSum<Vec2IGrid>(in1, in2); break;
					case 3: compSum<Vec3IGrid>(in1, in2); break;
				}
				break;
			
			case GridFloat:
				switch(in1.planecount) {
					case 1: compSum<FloatGrid>(in1, in2); break;
					case 2: compSum<Vec2SGrid>(in1, in2); break;
					case 3: compSum<Vec3SGrid>(in1, in2); break;
				}
				break;
		}
	}
}

template<>
void JitGrid::compMul<Int32Grid>(JitGrid& in1, JitGrid& in2) {
	Int32Grid::Ptr
		copyOfIn1 = in1.int1Grid->deepCopy(),
		copyOfIn2 = in2.int1Grid->deepCopy();

	openvdb::tools::compMul(*copyOfIn1, *copyOfIn2);
	setGrid<Int32Grid>(copyOfIn1);
}

template<>
void JitGrid::compMul<Vec2IGrid>(JitGrid& in1, JitGrid& in2) {
	Vec2IGrid::Ptr
		copyOfIn1 = in1.int2Grid->deepCopy(),
		copyOfIn2 = in2.int2Grid->deepCopy();

	openvdb::tools::compMul(*copyOfIn1, *copyOfIn2);
	setGrid<Vec2IGrid>(copyOfIn1);
}

template<>
void JitGrid::compMul<Vec3IGrid>(JitGrid& in1, JitGrid& in2) {
	Vec3IGrid::Ptr
		copyOfIn1 = in1.int3Grid->deepCopy(),
		copyOfIn2 = in2.int3Grid->deepCopy();

	openvdb::tools::compMul(*copyOfIn1, *copyOfIn2);
	setGrid<Vec3IGrid>(copyOfIn1);
}

template<>
void JitGrid::compMul<FloatGrid>(JitGrid& in1, JitGrid& in2) {
	FloatGrid::Ptr
		copyOfIn1 = in1.float1Grid->deepCopy(),
		copyOfIn2 = in2.float1Grid->deepCopy();

	openvdb::tools::compMul(*copyOfIn1, *copyOfIn2);
	setGrid<FloatGrid>(copyOfIn1);
}

template<>
void JitGrid::compMul<Vec2SGrid>(JitGrid& in1, JitGrid& in2) {
	Vec2SGrid::Ptr
		copyOfIn1 = in1.float2Grid->deepCopy(),
		copyOfIn2 = in2.float2Grid->deepCopy();

	openvdb::tools::compMul(*copyOfIn1, *copyOfIn2);
	setGrid<Vec2SGrid>(copyOfIn1);
}

template<>
void JitGrid::compMul<Vec3SGrid>(JitGrid& in1, JitGrid& in2) {
	Vec3SGrid::Ptr
		copyOfIn1 = in1.float3Grid->deepCopy(),
		copyOfIn2 = in2.float3Grid->deepCopy();

	openvdb::tools::compMul(*copyOfIn1, *copyOfIn2);
	setGrid<Vec3SGrid>(copyOfIn1);
}

void JitGrid::compMul(JitGrid& in1, JitGrid& in2) {
	if(!in1.equivalentFormats(in2)) {
		JitGrid in2Converted(in1.getGridType(), in1.getPlanecount(), in2.getVoxelSize());
		
		compMul(in1, in2Converted);
	}
	else {
		switch(in1.gridType) {
			case GridInt:
				switch(in1.planecount) {
					case 1: compMul<Int32Grid>(in1, in2); break;
					case 2: compMul<Vec2IGrid>(in1, in2); break;
					case 3: compMul<Vec3IGrid>(in1, in2); break;
				}
				break;
			
			case GridFloat:
				switch(in1.planecount) {
					case 1: compMul<FloatGrid>(in1, in2); break;
					case 2: compMul<Vec2SGrid>(in1, in2); break;
					case 3: compMul<Vec3SGrid>(in1, in2); break;
				}
				break;
		}
	}
}

template<>
void JitGrid::normalize<Int32Grid>(JitGrid& in1) {
	setGrid<Int32Grid>(in1.int1Grid);
}

template<>
void JitGrid::normalize<Vec2IGrid>(JitGrid& in1) {
	setGrid<Vec2IGrid>(openvdb::tools::normalize<Vec2IGrid>(*(in1.int2Grid)));
}

template<>
void JitGrid::normalize<Vec3IGrid>(JitGrid& in1) {
	setGrid<Vec3IGrid>(openvdb::tools::normalize<Vec3IGrid>(*(in1.int3Grid)));
}

template<>
void JitGrid::normalize<FloatGrid>(JitGrid& in1) {
	setGrid<FloatGrid>(in1.float1Grid);
}

template<>
void JitGrid::normalize<Vec2SGrid>(JitGrid& in1) {
	setGrid<Vec2SGrid>(openvdb::tools::normalize<Vec2SGrid>(*(in1.float2Grid)));
}

template<>
void JitGrid::normalize<Vec3SGrid>(JitGrid& in1) {
	setGrid<Vec3SGrid>(openvdb::tools::normalize<Vec3SGrid>(*(in1.float3Grid)));
}

void JitGrid::normalize(JitGrid& in1) {
	switch(in1.gridType) {
		case GridInt:
			switch(in1.planecount) {
				case 1: normalize<Int32Grid>(in1); break;
				case 2: normalize<Vec2IGrid>(in1); break;
				case 3: normalize<Vec3IGrid>(in1); break;
			}
			break;
		
		case GridFloat:
			switch(in1.planecount) {
				case 1: normalize<FloatGrid>(in1); break;
				case 2: normalize<Vec2SGrid>(in1); break;
				case 3: normalize<Vec3SGrid>(in1); break;
			}
			break;
	}
}

template<>
void JitGrid::magnitude<Int32Grid>(JitGrid& in1) {
	setGrid<Int32Grid>(in1.int1Grid);
}

template<>
void JitGrid::magnitude<Vec2IGrid>(JitGrid& in1) {
	setGrid<Int32Grid>(openvdb::tools::magnitude<Vec2IGrid>(*(in1.int2Grid)));
}

template<>
void JitGrid::magnitude<Vec3IGrid>(JitGrid& in1) {
	setGrid<Int32Grid>(openvdb::tools::magnitude<Vec3IGrid>(*(in1.int3Grid)));
}

template<>
void JitGrid::magnitude<FloatGrid>(JitGrid& in1) {
	setGrid<FloatGrid>(in1.float1Grid);
}

template<>
void JitGrid::magnitude<Vec2SGrid>(JitGrid& in1) {
	setGrid<FloatGrid>(openvdb::tools::magnitude<Vec2SGrid>(*(in1.float2Grid)));
}

template<>
void JitGrid::magnitude<Vec3SGrid>(JitGrid& in1) {
	setGrid<FloatGrid>(openvdb::tools::magnitude<Vec3SGrid>(*(in1.float3Grid)));
}

void JitGrid::magnitude(JitGrid& in1) {
	switch(in1.gridType) {
		case GridInt:
			switch(in1.planecount) {
				case 1: magnitude<Int32Grid>(in1); break;
				case 2: magnitude<Vec2IGrid>(in1); break;
				case 3: magnitude<Vec3IGrid>(in1); break;
			}
			break;
		
		case GridFloat:
			switch(in1.planecount) {
				case 1: magnitude<FloatGrid>(in1); break;
				case 2: magnitude<Vec2SGrid>(in1); break;
				case 3: magnitude<Vec3SGrid>(in1); break;
			}
			break;
	}
}


template<>
void JitGrid::curvature<FloatGrid>(JitGrid& in1) {
	setGrid<FloatGrid>(openvdb::tools::meanCurvature<FloatGrid>(*(in1.float1Grid)));
}

void JitGrid::curvature(JitGrid& in1) {
	switch(in1.gridType) {
		case GridFloat:
			switch(in1.planecount) {
				case 1: curvature<FloatGrid>(in1); break;
			}
			break;
			
		default: break;
	}
}

template<>
void JitGrid::laplacian<FloatGrid>(JitGrid& in1) {
	setGrid<FloatGrid>(openvdb::tools::laplacian<FloatGrid>(*(in1.float1Grid)));
}

void JitGrid::laplacian(JitGrid& in1) {
	switch(in1.gridType) {
		case GridFloat:
			switch(in1.planecount) {
				case 1: laplacian<FloatGrid>(in1); break;
			}
			break;
			
		default: break;
	}
}

template<>
void JitGrid::gradient<FloatGrid>(JitGrid& in1) {
	setGrid<Vec3SGrid>(openvdb::tools::gradient<FloatGrid>(*(in1.float1Grid)));
}

void JitGrid::gradient(JitGrid& in1) {
	switch(in1.gridType) {
		case GridFloat:
			switch(in1.planecount) {
				case 1: gradient<FloatGrid>(in1); break;
			}
			break;
			
		default: break;
	}
}

template<>
void JitGrid::divergence<Int32Grid>(JitGrid& in1) {
	setGrid<Int32Grid>(in1.int1Grid);
}

template<>
void JitGrid::divergence<Vec2IGrid>(JitGrid& in1) {
	setGrid<Int32Grid>(openvdb::tools::divergence<Vec2IGrid>(*(in1.int2Grid)));
}

template<>
void JitGrid::divergence<Vec3IGrid>(JitGrid& in1) {
	setGrid<Int32Grid>(openvdb::tools::divergence<Vec3IGrid>(*(in1.int3Grid)));
}

template<>
void JitGrid::divergence<FloatGrid>(JitGrid& in1) {
	setGrid<FloatGrid>(in1.float1Grid);
}

template<>
void JitGrid::divergence<Vec2SGrid>(JitGrid& in1) {
	setGrid<FloatGrid>(openvdb::tools::divergence<Vec2SGrid>(*(in1.float2Grid)));
}

template<>
void JitGrid::divergence<Vec3SGrid>(JitGrid& in1) {
	setGrid<FloatGrid>(openvdb::tools::divergence<Vec3SGrid>(*(in1.float3Grid)));
}

void JitGrid::divergence(JitGrid& in1) {
	switch(in1.gridType) {
		case GridInt:
			switch(in1.planecount) {
				case 1: divergence<Int32Grid>(in1); break;
				case 2: divergence<Vec2IGrid>(in1); break;
				case 3: divergence<Vec3IGrid>(in1); break;
			}
			break;
		
		case GridFloat:
			switch(in1.planecount) {
				case 1: divergence<FloatGrid>(in1); break;
				case 2: divergence<Vec2SGrid>(in1); break;
				case 3: divergence<Vec3SGrid>(in1); break;
			}
			break;
	}
}

template<>
void JitGrid::curl<Vec3IGrid>(JitGrid& in1) {
	setGrid<Vec3IGrid>(openvdb::tools::curl<Vec3IGrid>(*(in1.int3Grid)));
}

template<>
void JitGrid::curl<Vec3SGrid>(JitGrid& in1) {
	setGrid<Vec3SGrid>(openvdb::tools::curl<Vec3SGrid>(*(in1.float3Grid)));
}

void JitGrid::curl(JitGrid& in1) {
	switch(in1.gridType) {
		case GridInt:
			switch(in1.planecount) {
				case 3: curl<Vec3IGrid>(in1); break;
			}
			break;
		
		case GridFloat:
			switch(in1.planecount) {
				case 3: curl<Vec3SGrid>(in1); break;
			}
			break;
	}
}

void JitGrid::meanCurvatureFilter() {
	if(!isLevelset()) return;

	openvdb::tools::LevelSetFilter<FloatGrid> filter(*float1Grid);
	filter.meanCurvature();
}

void JitGrid::laplacianFilter(){
	if(!isLevelset()) return;
	
	openvdb::tools::LevelSetFilter<FloatGrid> filter(*float1Grid);
	filter.laplacian();
}

void JitGrid::gaussianFilter(float width){
	if(!isLevelset()) return;
	
	openvdb::tools::LevelSetFilter<FloatGrid> filter(*float1Grid);
	filter.gaussian(width);
}

void JitGrid::offsetFilter(float offset){
	if(!isLevelset()) return;
	
	openvdb::tools::LevelSetFilter<FloatGrid> filter(*float1Grid);
	filter.offset(offset);
}

void JitGrid::medianFilter(float width){
	if(!isLevelset()) return;
	
	openvdb::tools::LevelSetFilter<FloatGrid> filter(*float1Grid);
	filter.median(width);
}

void JitGrid::meanFilter(float width) {
	if(!isLevelset()) return;
	
	openvdb::tools::LevelSetFilter<FloatGrid> filter(*float1Grid);
	filter.mean(width);
}

Vec3s JitGrid::gradient(const Vec3s& xyz) const {
	if(isLevelset()) {
//		openvdb::math::BoxStencil<FloatGrid> stencil(*float1Grid);
		Coord ijk = worldToIndex(xyz);
		openvdb::math::SevenPointStencil<FloatGrid> stencil(*float1Grid);
		stencil.moveTo(ijk);
		
		openvdb::math::GenericMap map(*float1Grid);
        return openvdb::math::Gradient<openvdb::math::GenericMap, openvdb::math::CD_2ND>::result(map, stencil);
			
			
		
//		stencil.moveTo(ijk);
//		return stencil.gradient(Vec3s(ijk.x(), ijk.y(), ijk.z()));
	}
	return Vec3s();
}

Vec3s JitGrid::indexToWorld(const Coord& ijk) const {
	switch(gridType) {
		case GridInt:
			switch(planecount) {
				case 1: return int1Grid->transform().indexToWorld(ijk);
				case 2: return int2Grid->transform().indexToWorld(ijk);
				case 3: return int3Grid->transform().indexToWorld(ijk);
			}
			break;
		
		case GridFloat:
			switch(planecount) {
				case 1: return float1Grid->transform().indexToWorld(ijk);
				case 2: return float2Grid->transform().indexToWorld(ijk);
				case 3: return float3Grid->transform().indexToWorld(ijk);
			}
			break;
	}
	return Vec3s();
}

Coord JitGrid::worldToIndex(const Vec3s& xyz) const {
	switch(gridType) {
		case GridInt:
			switch(planecount) {
				case 1: return int1Grid->transform().worldToIndexNodeCentered(xyz);
				case 2: return int2Grid->transform().worldToIndexNodeCentered(xyz);
				case 3: return int3Grid->transform().worldToIndexNodeCentered(xyz);
			}
			break;
		
		case GridFloat:
			switch(planecount) {
				case 1: return float1Grid->transform().worldToIndexNodeCentered(xyz);
				case 2: return float2Grid->transform().worldToIndexNodeCentered(xyz);
				case 3: return float3Grid->transform().worldToIndexNodeCentered(xyz);
			}
			break;
	}
	return Coord();
}

void JitGrid::setIndex(const Coord& ijk, int *v) {
	switch(gridType) {
		case GridInt:
			switch(planecount) {
				case 1: int1Grid->getAccessor().setValueOn(ijk, *v); break;
				case 2: int2Grid->getAccessor().setValueOn(ijk, Vec2i(v)); break;
				case 3: int3Grid->getAccessor().setValueOn(ijk, Vec3i(v)); break;
			}
			break;
			
		default: break;
	}
}

void JitGrid::getIndex(const Coord& ijk, int *v) const {
	switch(gridType) {
		case GridInt:
			switch(planecount) {
				case 1: *v = int1Grid->getAccessor().getValue(ijk); break;
				case 2: {
					Vec2i val = int2Grid->getAccessor().getValue(ijk);
					v[0] = val[0];
					v[1] = val[1];
				} break;
				case 3: {
					Vec3i val = int3Grid->getAccessor().getValue(ijk);
					v[0] = val[0];
					v[1] = val[1];
					v[2] = val[2];
				} break;
			}
			break;
		default: break;
	}
}

void JitGrid::setPos(const Vec3s& xyz, int *v) {
	setIndex(worldToIndex(xyz), v);
}

void JitGrid::getPos(const Vec3s& xyz, int *v) const {
	getIndex(worldToIndex(xyz), v);
}

void JitGrid::setIndex(const Coord& ijk, float *v) {
	switch(gridType) {
		case GridFloat:
			switch(planecount) {
				case 1: float1Grid->getAccessor().setValueOn(ijk, *v); break;
				case 2: float2Grid->getAccessor().setValueOn(ijk, Vec2s(v)); break;
				case 3: float3Grid->getAccessor().setValueOn(ijk, Vec3s(v)); break;
			}
			break;
		default: break;
	}
}

void JitGrid::getIndex(const Coord& ijk, float *v) const {
	switch(gridType) {
		case GridFloat:
			switch(planecount) {
				case 1: *v = float1Grid->getAccessor().getValue(ijk); break;
				case 2: {
					Vec2s val = float2Grid->getAccessor().getValue(ijk);
					v[0] = val[0];
					v[1] = val[1];
				} break;
				case 3: {
					Vec3s val = float3Grid->getAccessor().getValue(ijk);
					v[0] = val[0];
					v[1] = val[1];
					v[2] = val[2];
				} break;
			}
			break;
		default: break;
	}
}

void JitGrid::setPos(const Vec3s& xyz, float *v) {
	setIndex(worldToIndex(xyz), v);
}

void JitGrid::getPos(const Vec3s& xyz, float *v) const {
	getIndex(worldToIndex(xyz), v);
}


void JitGrid::createGrid() {
	switch(gridType) {
		case GridInt:
			switch(planecount) {
				case 1:
					int1Grid = Int32Grid::create(0);
					int1Grid->setTransform(openvdb::math::Transform::createLinearTransform(voxelSize));
					break;
					
				case 2:
					int2Grid = Vec2IGrid::create(Vec2i(0, 0));
					int2Grid->setTransform(openvdb::math::Transform::createLinearTransform(voxelSize));
					break;
					
				case 3:
					int3Grid = Vec3IGrid::create(Vec3i(0, 0, 0));
					int3Grid->setTransform(openvdb::math::Transform::createLinearTransform(voxelSize));
					break;
			}
			break;
		
		case GridFloat:
			switch(planecount) {
				case 1:
					float1Grid = openvdb::createLevelSet<FloatGrid>(voxelSize, 4.);
					break;
					
				case 2:
					float2Grid = Vec2SGrid::create(Vec2s(0, 0));
					float2Grid->setTransform(openvdb::math::Transform::createLinearTransform(voxelSize));
					break;
					
				case 3:
					float3Grid = Vec3SGrid::create(Vec3s(0, 0, 0));
					float3Grid->setTransform(openvdb::math::Transform::createLinearTransform(voxelSize));
					break;
			}
			break;
	}
}

void JitGrid::destroyGrid() {
	switch(gridType) {
		case GridInt:
			switch(planecount) {
				case 1: int1Grid = Int32Grid::Ptr(); break;
				case 2: int2Grid = Vec2IGrid::Ptr(); break;
				case 3: int3Grid = Vec3IGrid::Ptr(); break;
			}
			break;
		
		case GridFloat:
			switch(planecount) {
				case 1: float1Grid = FloatGrid::Ptr(); break;
				case 2: float2Grid = Vec2SGrid::Ptr(); break;
				case 3: float3Grid = Vec3SGrid::Ptr(); break;
			}
			break;
	}
}

bool JitGrid::isLevelset() const {
	return gridType == GridFloat && planecount == 1;
}

bool JitGrid::equivalentFormats(const JitGrid& grid) const {
	return
		gridType == grid.getGridType() &&
		planecount == grid.getPlanecount();
}
