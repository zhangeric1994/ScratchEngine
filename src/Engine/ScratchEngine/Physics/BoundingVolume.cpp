#include "BoundingVolume.h"

ScratchEngine::Physics::AxisAlignedBoundingBox::AxisAlignedBoundingBox()
{
	min = XMVectorZero();
	max = XMVectorZero();
}

ScratchEngine::Physics::AxisAlignedBoundingBox::AxisAlignedBoundingBox(f32 minX, f32 minY, f32 minZ, f32 maxX, f32 maxY, f32 maxZ)
{
	min = XMVectorSet(minX, minY, minZ, 0);
	max = XMVectorSet(maxX, maxY, maxZ, 0);
}

ScratchEngine::Physics::AxisAlignedBoundingBox::AxisAlignedBoundingBox(XMVECTOR point1, XMVECTOR point2)
{
	min = point1;
	max = point2;
}

__inline ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::GetMinX() const
{
	return min.m128_f32[0];
}

__inline ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::GetMaxX() const
{
	return max.m128_f32[0];
}

__inline ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::GetMinY() const
{
	return min.m128_f32[1];
}

__inline ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::GetMaxY() const
{
	return max.m128_f32[1];
}

__inline ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::GetMinZ() const
{
	return min.m128_f32[2];
}

__inline ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::GetMaxZ() const
{
	return max.m128_f32[2];
}

__inline XMVECTOR ScratchEngine::Physics::AxisAlignedBoundingBox::GetCenter() const
{
	return XMVectorScale(XMVectorAdd(max, min), 0.5f);
}

__inline XMVECTOR ScratchEngine::Physics::AxisAlignedBoundingBox::GetHalfSize() const
{
	return XMVectorScale(XMVectorSubtract(max, min), 0.5f);
}

__inline ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::GetVolume() const
{
	XMVECTOR lwh = XMVectorSubtract(max, min);
	return lwh.m128_f32[0] * lwh.m128_f32[1] * lwh.m128_f32[2];
}

void ScratchEngine::Physics::AxisAlignedBoundingBox::SetCenter(f32 x, f32 y, f32 z)
{
	SetCenter({ x, y, z });
}

void ScratchEngine::Physics::AxisAlignedBoundingBox::SetCenter(XMVECTOR center)
{
	XMVECTOR halfSize = GetHalfSize();

	min = XMVectorSubtract(center, halfSize);
	max = XMVectorAdd(center, halfSize);
}

__inline bool ScratchEngine::Physics::AxisAlignedBoundingBox::DoesContain(const AxisAlignedBoundingBox& other) const
{
	return XMVector3GreaterOrEqual(max, other.max) && XMVector3LessOrEqual(min, other.min);
}

__inline bool ScratchEngine::Physics::AxisAlignedBoundingBox::DoesContain(AxisAlignedBoundingBox* other) const
{
	return XMVector3GreaterOrEqual(max, other->max) && XMVector3LessOrEqual(min, other->min);
}

__inline void* ScratchEngine::Physics::AxisAlignedBoundingBox::operator new(size_t size)
{
	return _aligned_malloc(size, 16);
}

__inline void ScratchEngine::Physics::AxisAlignedBoundingBox::operator delete(void* p)
{
	_aligned_free(p);
}

__inline void ScratchEngine::Physics::AxisAlignedBoundingBox::SetMinX(f32 value)
{
	min.m128_f32[0] = value;
}

__inline void ScratchEngine::Physics::AxisAlignedBoundingBox::SetMaxX(f32 value)
{
	max.m128_f32[0] = value;
}

__inline void ScratchEngine::Physics::AxisAlignedBoundingBox::SetMinY(f32 value)
{
	min.m128_f32[1] = value;
}

__inline void ScratchEngine::Physics::AxisAlignedBoundingBox::SetMaxY(f32 value)
{
	max.m128_f32[1] = value;
}

__inline void ScratchEngine::Physics::AxisAlignedBoundingBox::SetMinZ(f32 value)
{
	min.m128_f32[2] = value;
}

__inline void ScratchEngine::Physics::AxisAlignedBoundingBox::SetMaxZ(f32 value)
{
	max.m128_f32[2] = value;
}

__inline void ScratchEngine::Physics::AxisAlignedBoundingBox::Union(const AxisAlignedBoundingBox& other)
{
	min = XMVectorMin(min, other.min);
	max = XMVectorMax(max, other.max);
}

__inline ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::GetUnionVolume(const AxisAlignedBoundingBox & other) const
{
	XMVECTOR lwh = XMVectorSubtract(XMVectorMin(max, other.max), XMVectorMin(min, other.min));
	return lwh.m128_f32[0] * lwh.m128_f32[1] * lwh.m128_f32[2];
}


const int ScratchEngine::Physics::OrientedBoundingBox::iSurfaces[6][4] = { { 1, 0, 3, 2 },
																		   { 0, 4, 2, 6 },
																		   { 4, 0, 5, 1 },
																		   { 4, 5, 6, 7 },
																		   { 2, 6, 3, 7 },
																		   { 5, 1, 7, 3 } };

const int ScratchEngine::Physics::OrientedBoundingBox::iEdges[12][2] = { { 0, 1 },
																		 { 0, 2 },
																		 { 0, 4 },
																		 { 4, 6 },
																		 { 4, 5 },
																		 { 5, 7 },
																		 { 5, 1 },
																		 { 1, 3 },
																		 { 2, 6 },
																		 { 2, 3 },
																		 { 6, 7 },
																		 { 7, 3 } };

ScratchEngine::Physics::OrientedBoundingBox::OrientedBoundingBox(XMMATRIX worldMatrix, XMVECTOR size, XMVECTOR offset)
{
	SetData(worldMatrix, size, offset);
}

XMVECTOR ScratchEngine::Physics::OrientedBoundingBox::GetHalfDiagonalVector() const
{
	return XMVectorSubtract(H, center);
}

void ScratchEngine::Physics::OrientedBoundingBox::SetData(XMMATRIX worldMatrix, XMVECTOR size, XMVECTOR offset)
{
	center = XMVector3Transform(offset, worldMatrix);
	this->size = XMVectorMultiply({ XMVector3Length(worldMatrix.r[0]).m128_f32[0], XMVector3Length(worldMatrix.r[1]).m128_f32[0], XMVector3Length(worldMatrix.r[2]).m128_f32[0] }, size);

	XMVECTOR halfSize = XMVectorScale(size, 0.5f);

	f32 x = halfSize.m128_f32[0];
	f32 y = halfSize.m128_f32[1];
	f32 z = halfSize.m128_f32[2];

	A = XMVector3Transform({-x, -y, -z}, worldMatrix);
	B = XMVector3Transform({-x, -y, z}, worldMatrix);
	C = XMVector3Transform({-x, y, -z}, worldMatrix);
	D = XMVector3Transform({-x, y, z}, worldMatrix);
	E = XMVector3Transform({x, -y, -z}, worldMatrix);
	F = XMVector3Transform({-x, y, -z}, worldMatrix);
	G = XMVector3Transform({x, y, -z}, worldMatrix);
	H = XMVector3Transform({x, y, z}, worldMatrix);

	axisX = XMVector3Normalize(XMVectorSubtract(E, A));
	axisY = XMVector3Normalize(XMVectorSubtract(C, A));
	axisZ = XMVector3Normalize(XMVectorSubtract(B, A));

	negativeX = XMPlaneFromPoints(B, A, D);
	negativeZ = XMPlaneFromPoints(A, E, C);
	negativeY = XMPlaneFromPoints(E, A, F);
	positiveX = XMPlaneFromPoints(E, F, G);
	positiveY = XMPlaneFromPoints(C, G, D);
	positiveZ = XMPlaneFromPoints(F, B, H);
}


void ScratchEngine::Physics::BoundingSphere::SetData(XMVECTOR position, f32 radius)
{
	this->center = position;
	this->radius = radius;
}


void ScratchEngine::Physics::BoundingFrustum::SetData(XMMATRIX viewProjection)
{
	viewProjection = XMMatrixTranspose(viewProjection);

	leftPlane = XMVectorAdd(viewProjection.r[3], viewProjection.r[0]);
	rightPlane = XMVectorSubtract(viewProjection.r[3], viewProjection.r[0]);
	bottomPlane = XMVectorAdd(viewProjection.r[3], viewProjection.r[1]);
	topPlane = XMVectorSubtract(viewProjection.r[3], viewProjection.r[1]);
	nearPlane = XMVectorAdd(viewProjection.r[3], viewProjection.r[2]);
	farPlane = XMVectorSubtract(viewProjection.r[3], viewProjection.r[2]);
}
