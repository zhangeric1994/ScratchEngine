#include <cstdlib>

#include "Transform.h"

ScratchEngine::Transform::Transform()
{
	localPosition = XMVectorZero();
	localRotation = XMQuaternionIdentity();
	localScale = XMVectorSet(1, 1, 1, 0);

	parent = nullptr;

	__MarkDirty();
}

ScratchEngine::Transform::Transform(float x, float y, float z)
{
	localPosition = XMVectorSet(x, y, z, 0);
	localRotation = XMQuaternionIdentity();
	localScale = XMVectorSet(1, 1, 1, 0);

	parent = nullptr;

	__MarkDirty();
}

ScratchEngine::Transform::Transform(XMFLOAT3 position, XMFLOAT4 rotation, XMFLOAT3 scale)
{
	this->localPosition = XMLoadFloat3(&position);
	this->localRotation = XMLoadFloat4(&rotation);
	this->localScale = XMLoadFloat3(&scale);

	parent = nullptr;

	__MarkDirty();
}

ScratchEngine::Transform::Transform(XMVECTOR position, XMVECTOR rotation, XMVECTOR scale)
{
	this->localPosition = position;
	this->localRotation = rotation;
	this->localScale = scale;

	parent = nullptr;

	__MarkDirty();
}

ScratchEngine::Transform::~Transform()
{
}

__inline XMVECTOR ScratchEngine::Transform::GetLocalPosition()
{
	return localPosition;
}

__inline XMVECTOR ScratchEngine::Transform::GetPosition()
{
	return parent ? XMVector3Transform(localPosition, parent->GetWorldMatrix()) : localPosition;
}

__inline XMVECTOR ScratchEngine::Transform::GetLocalRotation()
{
	return localRotation;
}

__inline XMVECTOR ScratchEngine::Transform::GetRotation()
{
	return parent ? XMQuaternionMultiply(localRotation, parent->GetRotation()) : localRotation;
}

__inline XMVECTOR ScratchEngine::Transform::GetLocalScale()
{
	return localScale;
}

__inline XMVECTOR ScratchEngine::Transform::GetScale()
{
	return parent ? XMVectorMultiply(localScale, parent->GetScale()) : localScale;
}

__inline XMMATRIX ScratchEngine::Transform::GetWorldMatrix()
{
	if (isDirty)
		__UpdateWorldMatrix();

	return worldMatrix;
}

__inline ScratchEngine::Transform* ScratchEngine::Transform::GetParent()
{
	return parent;
}

__inline ScratchEngine::Transform* ScratchEngine::Transform::GetChild(size_t index)
{
	return index < children.size() ? children[index] : nullptr;
}

__inline size_t ScratchEngine::Transform::GetChildCount()
{
	return children.size();
}

__inline void ScratchEngine::Transform::SetLocalPosition(f32 x, f32 y, f32 z)
{
	SetLocalPosition(XMVectorSet(x, y, z, 0));
}

__inline void ScratchEngine::Transform::SetLocalPosition(XMFLOAT3 position)
{
	SetLocalPosition(XMLoadFloat3(&position));
}

__inline void ScratchEngine::Transform::SetLocalPosition(XMVECTOR position)
{
	if (XMVector3NotEqual(position, localPosition))
	{
		localPosition = position;

		__MarkDirty();
	}
}

__inline void ScratchEngine::Transform::SetPosition(f32 x, f32 y, f32 z)
{
	SetLocalPosition(parent ? XMVector3Transform(XMVectorSet(x, y, z, 0), XMMatrixInverse(nullptr, parent->GetWorldMatrix())) : XMVectorSet(x, y, z, 0));
}

__inline void ScratchEngine::Transform::SetPosition(XMFLOAT3 position)
{
	SetLocalPosition(parent ? XMVector3Transform(XMLoadFloat3(&position), XMMatrixInverse(nullptr, parent->GetWorldMatrix())) : XMLoadFloat3(&position));
}

__inline void ScratchEngine::Transform::SetPosition(XMVECTOR position)
{
	SetLocalPosition(parent ? XMVector3Transform(position, XMMatrixInverse(nullptr, parent->GetWorldMatrix())) : position);
}

__inline void ScratchEngine::Transform::SetLocalRotation(f32 x, f32 y, f32 z)
{
	SetLocalRotation(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)));
}

__inline void ScratchEngine::Transform::SetLocalRotation(XMFLOAT4 rotation)
{
	SetLocalRotation(XMLoadFloat4(&rotation));
}

__inline void ScratchEngine::Transform::SetLocalRotation(XMVECTOR rotation)
{
	if (XMQuaternionNotEqual(rotation, localRotation))
	{
		localRotation = rotation;

		__MarkDirty();
	}
}

__inline void ScratchEngine::Transform::SetRotation(f32 x, f32 y, f32 z)
{
	SetLocalRotation(parent ? XMQuaternionMultiply(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)), XMQuaternionInverse(parent->GetRotation())) : XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)));
}

__inline void ScratchEngine::Transform::SetRotation(XMFLOAT4 rotation)
{
	SetLocalRotation(parent ? XMQuaternionMultiply(XMLoadFloat4(&rotation), XMQuaternionInverse(parent->GetRotation())) : XMLoadFloat4(&rotation));
}

__inline void ScratchEngine::Transform::SetRotation(XMVECTOR rotation)
{
	SetLocalRotation(parent ? XMQuaternionMultiply(rotation, XMQuaternionInverse(parent->GetRotation())) : rotation);
}

__inline void ScratchEngine::Transform::SetLocalScale(f32 x, f32 y, f32 z)
{
	SetLocalScale(XMVectorSet(x, y, z, 0));
}

__inline void ScratchEngine::Transform::SetLocalScale(XMFLOAT3 scale)
{
	SetLocalScale(XMLoadFloat3(&scale));
}

__inline void ScratchEngine::Transform::SetLocalScale(XMVECTOR scale)
{
	scale = XMVectorMax(scale, XMVectorZero());

	if (XMVector3NotEqual(scale, localScale))
	{
		localScale = scale;

		__MarkDirty();
	}
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

void* ScratchEngine::Transform::operator new(size_t size)
{
	return _aligned_malloc(size, 16);
}

void ScratchEngine::Transform::operator delete(void* p)
{
	return _aligned_free(p);
}

__inline void ScratchEngine::Transform::Translate(float x, float y, float z, Space space)
{
	Translate(XMVectorSet(x, y, z, 0), space);
}

__inline void ScratchEngine::Transform::Translate(XMFLOAT3 translation, Space space)
{
	Translate(XMLoadFloat3(&translation), space);
}

__inline void ScratchEngine::Transform::Translate(XMVECTOR translation, Space space)
{
	if (XMVector3NotEqual(translation, XMVectorZero()))
	{
		switch (space)
		{
		case Space::SELF:
			localPosition = XMVectorAdd(localPosition, XMVector3Rotate(translation, localRotation));
			break;


		case Space::PARENT:
			localPosition = XMVectorAdd(localPosition, translation);
			break;


		case Space::WORLD:
			localPosition = XMVectorAdd(localPosition, parent ? XMVector3Transform(translation, XMMatrixInverse(nullptr, GetWorldMatrix())) : translation);
			break;
		}

		__MarkDirty();
	}
}

__inline void ScratchEngine::Transform::Rotate(float x, float y, float z, Space space)
{
	Rotate(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)), space);
}

__inline void ScratchEngine::Transform::Rotate(XMFLOAT4 rotation, Space space)
{
	Rotate(XMLoadFloat4(&rotation), space);
}

__inline void ScratchEngine::Transform::Rotate(XMVECTOR rotation, Space space)
{
	if (XMVector3NotEqual(rotation, XMQuaternionIdentity()))
	{
		switch (space)
		{
		case Space::SELF:
			localRotation = XMQuaternionMultiply(rotation, localRotation);
			break;


		case Space::PARENT:
			localRotation = XMQuaternionMultiply(localRotation, rotation);
			break;


		case Space::WORLD:
			if (parent)
			{
				XMVECTOR parentRotation = parent->GetRotation();
				localRotation = XMQuaternionMultiply(localRotation, XMQuaternionMultiply(parentRotation, XMQuaternionMultiply(rotation, XMQuaternionInverse(parentRotation))));
			}
			else
				localRotation = XMQuaternionMultiply(localRotation, rotation);
			break;



		}

		__MarkDirty();
	}
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

__inline void ScratchEngine::Transform::SendMessage_(const Message& message)
{
	HandleMessage(message);
}

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

__forceinline void ScratchEngine::Transform::__MarkDirty()
{
	if (!isDirty)
	{
		isDirty = true;

		for (auto it = children.begin(); it != children.end(); it++)
			(*it)->__MarkDirty();
	}
}

__forceinline size_t ScratchEngine::Transform::__AddChild(Transform* other)
{
	children.push_back(other);
	return children.size() - 1;
}

__forceinline void ScratchEngine::Transform::__RemoveChild(Transform* transform)
{
	children.erase(children.begin() + transform->index);
}

__forceinline void ScratchEngine::Transform::__UpdateWorldMatrix()
{
	if (parent)
		worldMatrix = (XMMatrixScalingFromVector(localScale) * XMMatrixRotationQuaternion(localRotation) * XMMatrixTranslationFromVector(localPosition)) * parent->GetWorldMatrix();
	else
		worldMatrix = XMMatrixScalingFromVector(localScale) * XMMatrixRotationQuaternion(localRotation) * XMMatrixTranslationFromVector(localPosition);

	isDirty = false;
}
