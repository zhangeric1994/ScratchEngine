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

ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::GetMinX() const
{
	return min.m128_f32[0];
}

ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::GetMaxX() const
{
	return max.m128_f32[0];
}

ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::GetMinY() const
{
	return min.m128_f32[1];
}

ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::GetMaxY() const
{
	return max.m128_f32[1];
}

ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::GetMinZ() const
{
	return min.m128_f32[2];
}

ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::GetMaxZ() const
{
	return max.m128_f32[2];
}

ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::GetVolume() const
{
	return _get_box_volume(XMVectorSubtract(max, min));
}

void* ScratchEngine::Physics::AxisAlignedBoundingBox::operator new(size_t size)
{
	return _aligned_malloc(size, 16);
}

void ScratchEngine::Physics::AxisAlignedBoundingBox::operator delete(void* p)
{
	_aligned_free(p);
}

void ScratchEngine::Physics::AxisAlignedBoundingBox::SetMinX(f32 value)
{
	min.m128_f32[0] = value;
}

void ScratchEngine::Physics::AxisAlignedBoundingBox::SetMaxX(f32 value)
{
	max.m128_f32[0] = value;
}

void ScratchEngine::Physics::AxisAlignedBoundingBox::SetMinY(f32 value)
{
	min.m128_f32[1] = value;
}

void ScratchEngine::Physics::AxisAlignedBoundingBox::SetMaxY(f32 value)
{
	max.m128_f32[1] = value;
}

void ScratchEngine::Physics::AxisAlignedBoundingBox::SetMinZ(f32 value)
{
	min.m128_f32[2] = value;
}

void ScratchEngine::Physics::AxisAlignedBoundingBox::SetMaxZ(f32 value)
{
	max.m128_f32[2] = value;
}

void ScratchEngine::Physics::AxisAlignedBoundingBox::Union(const AxisAlignedBoundingBox& other)
{
	min = XMVectorMin(min, other.min);
	max = XMVectorMin(max, other.max);
}

ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::GetUnionVolume(const AxisAlignedBoundingBox & other) const
{
	return _get_box_volume(XMVectorSubtract(XMVectorMin(max, other.max), XMVectorMin(min, other.min)));
}

ScratchEngine::f32 ScratchEngine::Physics::AxisAlignedBoundingBox::_get_box_volume(XMVECTOR lwh) const
{
	return XMVectorMultiply(lwh, XMVectorMultiply(XMVectorSwizzle(lwh, 1, 2, 0, 3), XMVectorSwizzle(lwh, 2, 0, 1, 3))).m128_f32[0];
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

ScratchEngine::Physics::OrientedBoundingBox::OrientedBoundingBox(XMVECTOR position, XMVECTOR rotation, XMVECTOR size)
{
	SetData(position, rotation, size);
}

XMVECTOR ScratchEngine::Physics::OrientedBoundingBox::GetHalfSize() const
{
	return XMVectorSubtract(H, center);
}

__inline void ScratchEngine::Physics::OrientedBoundingBox::SetData(XMVECTOR position, XMVECTOR rotation, XMVECTOR size)
{
	center = position;

	XMVECTOR halfSize = XMVectorScale(size, 0.5f);

	XMVECTOR min = XMVector3Rotate(XMVectorSubtract(position, halfSize), rotation);
	XMVECTOR max = XMVector3Rotate(XMVectorAdd(position, halfSize), rotation);

	axisX = XMVector3Rotate({ 1, 0, 0 }, rotation);
	axisY = XMVector3Rotate({ 0, 1, 0 }, rotation);
	axisZ = XMVector3Rotate({ 0, 0, 1 }, rotation);

	f32 minX = min.m128_f32[0];
	f32 maxX = max.m128_f32[0];
	f32 minY = min.m128_f32[1];
	f32 maxY = max.m128_f32[1];
	f32 minZ = min.m128_f32[2];
	f32 maxZ = max.m128_f32[2];

	A = { minX, minY, minZ };
	B = { minX, minY, maxZ };
	C = { minX, maxY, minZ };
	D = { minX, maxY, maxZ };
	E = { maxX, minY, minZ };
	F = { maxX, minY, maxZ };
	G = { maxX, maxY, minZ };
	H = { maxX, maxY, maxZ };

	negativeX = XMPlaneFromPoints(B, A, D);
	negativeZ = XMPlaneFromPoints(A, E, C);
	negativeY = XMPlaneFromPoints(E, A, F);
	positiveX = XMPlaneFromPoints(E, F, G);
	positiveY = XMPlaneFromPoints(C, G, D);
	positiveZ = XMPlaneFromPoints(F, B, H);
}
