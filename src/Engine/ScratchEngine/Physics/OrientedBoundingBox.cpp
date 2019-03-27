#include "OrientedBoundingBox.h"

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

ScratchEngine::Physics::OrientedBoundingBox::OrientedBoundingBox(XMVECTOR size, XMVECTOR position, XMVECTOR rotation)
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

XMVECTOR ScratchEngine::Physics::OrientedBoundingBox::GetHalfSize() const
{
	return XMVectorSubtract(H, center);
}
