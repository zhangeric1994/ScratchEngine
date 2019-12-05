#include <cstdlib>

#include "Transform.h"


ScratchEngine::Transform::Transform()
{
	localPosition = XMVectorZero();
	localRotation = XMQuaternionIdentity();
	localScale = XMVectorSet(1, 1, 1, 0);

	parent = nullptr;

	isDirty = true;
	isFrameDirty = true;
}

ScratchEngine::Transform::Transform(float x, float y, float z)
{
	localPosition = XMVectorSet(x, y, z, 0);
	localRotation = XMQuaternionIdentity();
	localScale = XMVectorSet(1, 1, 1, 0);

	parent = nullptr;

	isDirty = true;
	isFrameDirty = true;
}

ScratchEngine::Transform::Transform(XMFLOAT3 position, XMFLOAT4 rotation, XMFLOAT3 scale)
{
	this->localPosition = XMLoadFloat3(&position);
	this->localRotation = XMLoadFloat4(&rotation);
	this->localScale = XMLoadFloat3(&scale);

	parent = nullptr;

	isDirty = true;
	isFrameDirty = true;
}

ScratchEngine::Transform::Transform(XMVECTOR position, XMVECTOR rotation, XMVECTOR scale)
{
	this->localPosition = position;
	this->localRotation = rotation;
	this->localScale = scale;

	parent = nullptr;

	isDirty = true;
	isFrameDirty = true;
}

ScratchEngine::Transform::~Transform()
{
}

//void ScratchEngine::Transform::SetScale(f32 x, f32 y, f32 z)
//{
//	_set_local_scale(parent ? XMVectorDivide(XMVectorSet(x, y, z, 0), parent->GetScale()) : XMVectorSet(x, y, z, 0));
//}
//
//void ScratchEngine::Transform::SetScale(XMFLOAT3 scale)
//{
//	_set_local_scale(parent ? XMVectorDivide(XMLoadFloat3(&scale), parent->GetScale()) : XMLoadFloat3(&scale));
//}
//
//void ScratchEngine::Transform::SetScale(XMVECTOR scale)
//{
//	_set_local_scale(parent ? XMVectorDivide(scale, parent->GetScale()) : scale);
//}

void ScratchEngine::Transform::SetParent(Transform* parent)
{
	XMVECTOR position = GetPosition();
	XMVECTOR rotation = GetRotation();
	XMVECTOR scale = GetScale();

	if (this->parent)
		this->parent->__RemoveChild(this);

	this->parent = parent;

	if (parent)
	{
		index = parent->__AddChild(this);

		SetLocalPosition(XMVector3Transform(position, XMMatrixInverse(nullptr, parent->GetWorldMatrix())));
		SetLocalRotation(XMQuaternionMultiply(rotation, XMQuaternionInverse(parent->GetRotation())));
		SetLocalScale(XMVectorDivide(scale, parent->GetScale()));
	}
	else
	{
		SetLocalPosition(position);
		SetLocalPosition(rotation);
		SetLocalPosition(scale);
	}
}

void ScratchEngine::Transform::UpdateFrameData()
{
	if (isFrameDirty)
	{
		frameData.worldMatrix = GetWorldMatrix();
		frameData.localPosition = localPosition;
		frameData.localRotation = localRotation;
		frameData.localScale = localScale;
		frameData.isDirty = true;

		isFrameDirty = false;
	}
	else
		frameData.isDirty = false;
}

void* ScratchEngine::Transform::operator new(size_t size)
{
	return _aligned_malloc(size, 16);
}

void ScratchEngine::Transform::operator delete(void* p)
{
	return _aligned_free(p);
}

//void ScratchEngine::Transform::Scale(XMFLOAT3 scale)
//{
//	XMVECTOR translation = XMVectorMax(XMLoadFloat3(&scale), XMVectorZero());
//
//	if (XMVector3NotEqual(translation, XMVectorSet(1, 1, 1, 0)))
//	{
//		__MarkDirty();
//
//		localScale = XMVectorMultiply(translation, localScale);
//	}
//}
//
//void ScratchEngine::Transform::Scale(XMVECTOR scale)
//{
//	scale = XMVectorMax(scale, XMVectorZero());
//
//	if (XMVector3NotEqual(scale, XMVectorSet(1, 1, 1, 0)))
//	{
//		__MarkDirty();
//
//		localScale = XMVectorMultiply(scale, localScale);
//	}
//}

void ScratchEngine::Transform::SendMessageUp(const Message& message, u32 level)
{
	HandleMessage(message);

	if (level-- > 0 && parent)
		parent->SendMessageUp(message, level);
}

void ScratchEngine::Transform::SendMessageDown(const Message& message, u32 level)
{
	HandleMessage(message);

	if (level-- > 0)
		for (auto it = children.begin(); it != children.end(); it++)
			(*it)->SendMessageDown(message, level);
}
