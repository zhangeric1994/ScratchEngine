#include <cstdlib>

#include "Transform.h"

ScratchEngine::Transform::Transform()
{
	localPosition = XMVectorZero();
	localRotation = XMQuaternionIdentity();
	localScale = XMVectorSet(1, 1, 1, 0);

	parent = nullptr;

	_set_dirty();
}

ScratchEngine::Transform::Transform(XMFLOAT3 position, XMFLOAT4 rotation, XMFLOAT3 scale)
{
	this->localPosition = XMLoadFloat3(&position);
	this->localRotation = XMLoadFloat4(&rotation);
	this->localScale = XMLoadFloat3(&scale);

	parent = nullptr;

	_set_dirty();
}

ScratchEngine::Transform::Transform(XMVECTOR position, XMVECTOR rotation, XMVECTOR scale)
{
	this->localPosition = position;
	this->localRotation = rotation;
	this->localScale = scale;

	parent = nullptr;

	_set_dirty();
}

ScratchEngine::Transform::~Transform()
{
}

XMVECTOR ScratchEngine::Transform::GetLocalPosition()
{
	return localPosition;
}

XMVECTOR ScratchEngine::Transform::GetPosition()
{
	if (parent)
		return XMVector3Transform(localPosition, parent->GetWorldMatrix());

	return localPosition;
}

XMVECTOR ScratchEngine::Transform::GetLocalRotation()
{
	return localRotation;
}

XMVECTOR ScratchEngine::Transform::GetRotation()
{
	if (parent)
		return XMQuaternionMultiply(parent->GetRotation(), localRotation);

	return localRotation;
}

XMVECTOR ScratchEngine::Transform::GetLocalScale()
{
	return localScale;
}

XMVECTOR ScratchEngine::Transform::GetScale()
{
	if (parent)
		return XMVectorMultiply(parent->GetScale(), localScale);

	return localScale;
}

ScratchEngine::Transform* ScratchEngine::Transform::GetParent()
{
	return parent;
}

ScratchEngine::Transform* ScratchEngine::Transform::GetChild(size_t index)
{
	if (index < children.size())
		return children[index];

	return nullptr;
}

size_t ScratchEngine::Transform::GetChildCount()
{
	return children.size();
}

void ScratchEngine::Transform::SetLocalPosition(f32 x, f32 y, f32 z)
{
	_set_local_position(XMVectorSet(x, y, z, 0));
}

void ScratchEngine::Transform::SetLocalPosition(XMFLOAT3 position)
{
	_set_local_position(XMLoadFloat3(&position));
}

void ScratchEngine::Transform::SetLocalPosition(XMVECTOR position)
{
	_set_local_position(position);
}

void ScratchEngine::Transform::SetPosition(f32 x, f32 y, f32 z)
{
	_set_local_position(parent ? XMVector3Transform(XMVectorSet(x, y, z, 0), XMMatrixInverse(nullptr, parent->GetWorldMatrix())) : XMVectorSet(x, y, z, 0));
}

void ScratchEngine::Transform::SetPosition(XMFLOAT3 position)
{
	_set_local_position(parent ? XMVector3Transform(XMLoadFloat3(&position), XMMatrixInverse(nullptr, parent->GetWorldMatrix())) : XMLoadFloat3(&position));
}

void ScratchEngine::Transform::SetPosition(XMVECTOR position)
{
	_set_local_position(parent ? XMVector3Transform(position, XMMatrixInverse(nullptr, parent->GetWorldMatrix())) : position);
}

void ScratchEngine::Transform::SetLocalRotation(f32 x, f32 y, f32 z)
{
	_set_local_rotation(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)));
}

void ScratchEngine::Transform::SetLocalRotation(XMFLOAT4 rotation)
{
	_set_local_rotation(XMLoadFloat4(&rotation));
}

void ScratchEngine::Transform::SetLocalRotation(XMVECTOR rotation)
{
	_set_local_rotation(rotation);
}

void ScratchEngine::Transform::SetRotation(f32 x, f32 y, f32 z)
{
	_set_local_rotation(parent ? XMQuaternionMultiply(XMQuaternionInverse(parent->GetRotation()), XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z))) : XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)));
}

void ScratchEngine::Transform::SetRotation(XMFLOAT4 rotation)
{
	_set_local_rotation(parent ? XMQuaternionMultiply(XMQuaternionInverse(parent->GetRotation()), XMLoadFloat4(&rotation)) : XMLoadFloat4(&rotation));
}

void ScratchEngine::Transform::SetRotation(XMVECTOR rotation)
{
	_set_local_rotation(parent ? XMQuaternionMultiply(XMQuaternionInverse(parent->GetRotation()), rotation) : rotation);
}

void ScratchEngine::Transform::SetLocalScale(f32 x, f32 y, f32 z)
{
	_set_local_scale(XMVectorSet(x, y, z, 0));
}

void ScratchEngine::Transform::SetLocalScale(XMFLOAT3 scale)
{
	_set_local_scale(XMLoadFloat3(&scale));
}

void ScratchEngine::Transform::SetLocalScale(XMVECTOR scale)
{
	_set_local_scale(scale);
}

void ScratchEngine::Transform::SetScale(f32 x, f32 y, f32 z)
{
	_set_local_scale(parent ? XMVectorDivide(XMVectorSet(x, y, z, 0), parent->GetScale()) : XMVectorSet(x, y, z, 0));
}

void ScratchEngine::Transform::SetScale(XMFLOAT3 scale)
{
	_set_local_scale(parent ? XMVectorDivide(XMLoadFloat3(&scale), parent->GetScale()) : XMLoadFloat3(&scale));
}

void ScratchEngine::Transform::SetScale(XMVECTOR scale)
{
	_set_local_scale(parent ? XMVectorDivide(scale, parent->GetScale()) : scale);
}

void ScratchEngine::Transform::SetParent(Transform* parent)
{
	XMVECTOR position = GetPosition();
	XMVECTOR rotation = GetRotation();
	XMVECTOR scale = GetScale();

	if (this->parent)
		this->parent->_remove_child(this);

	parent->_add_child(this);

	this->parent = parent;

	SetPosition(position);
	SetRotation(rotation);
	SetScale(scale);
}

void* ScratchEngine::Transform::operator new(size_t size)
{
	return _aligned_malloc(size, 16);
}

void ScratchEngine::Transform::operator delete(void* p)
{
	return _aligned_free(p);
}

void ScratchEngine::Transform::Translate(XMFLOAT3 translation, Space space)
{
	XMVECTOR v = XMLoadFloat3(&translation);

	if (XMVector3NotEqual(v, XMVectorZero()))
	{
		switch (space)
		{
		case WORLD:
			SetPosition(XMVectorAdd(GetPosition(), v));
			break;


		case SELF:
			_set_dirty();

			localPosition = XMVectorAdd(localPosition, v);
			break;
		}
	}
}

void ScratchEngine::Transform::Translate(XMVECTOR translation, Space space)
{
	if (XMVector3NotEqual(translation, XMVectorZero()))
	{
		switch (space)
		{
		case WORLD:
			SetPosition(XMVectorAdd(GetPosition(), translation));
			break;


		case SELF:
			_set_dirty();

			localPosition = XMVectorAdd(localPosition, translation);
			break;
		}
	}
}

void ScratchEngine::Transform::Rotate(XMFLOAT4 rotation)
{
	XMVECTOR q = XMLoadFloat4(&rotation);

	if (XMQuaternionNotEqual(q, XMQuaternionIdentity()))
	{
		_set_dirty();

		localRotation = XMQuaternionMultiply(q, localRotation);
	}
}

void ScratchEngine::Transform::Rotate(XMVECTOR rotation)
{
	if (XMVector3NotEqual(rotation, XMQuaternionIdentity()))
	{
		_set_dirty();

		localRotation = XMQuaternionMultiply(rotation, localRotation);
	}
}

void ScratchEngine::Transform::Scale(XMFLOAT3 scale)
{
	XMVECTOR v = XMVectorMax(XMLoadFloat3(&scale), XMVectorZero());

	if (XMVector3NotEqual(v, XMVectorSet(1, 1, 1, 0)))
	{
		_set_dirty();

		localScale = XMVectorMultiply(v, localScale);
	}
}

void ScratchEngine::Transform::Scale(XMVECTOR scale)
{
	scale = XMVectorMax(scale, XMVectorZero());

	if (XMVector3NotEqual(scale, XMVectorSet(1, 1, 1, 0)))
	{
		_set_dirty();

		localScale = XMVectorMultiply(scale, localScale);
	}
}

void ScratchEngine::Transform::SendMessage(const Message& message)
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

XMMATRIX ScratchEngine::Transform::GetWorldMatrix()
{
	if (isDirty)
		_update_world_matrix();

	return worldMatrix;
}

void ScratchEngine::Transform::_update_world_matrix()
{
	if (parent)
		worldMatrix = parent->GetWorldMatrix() * XMMatrixScalingFromVector(localScale) * XMMatrixRotationQuaternion(localRotation) * XMMatrixTranslationFromVector(localPosition);
	else
		worldMatrix = XMMatrixScalingFromVector(localScale) * XMMatrixRotationQuaternion(localRotation) * XMMatrixTranslationFromVector(localPosition);

	isDirty = false;
}

void ScratchEngine::Transform::_set_local_position(XMVECTOR v)
{
	if (XMVector3NotEqual(v, localPosition))
	{
		_set_dirty();

		localPosition = v;
	}
}

void ScratchEngine::Transform::_set_local_rotation(XMVECTOR q)
{
	if (XMQuaternionNotEqual(q, localRotation))
	{
		_set_dirty();

		localRotation = q;
	}
}

void ScratchEngine::Transform::_set_local_scale(XMVECTOR v)
{
	v = XMVectorMax(v, XMVectorZero());

	if (XMVector3NotEqual(v, localScale))
	{
		_set_dirty();

		localScale = v;
	}
}

void ScratchEngine::Transform::_set_dirty()
{
	if (!isDirty)
	{
		isDirty = true;

		for (auto it = children.begin(); it != children.end(); it++)
			(*it)->_set_dirty();
	}
}

void ScratchEngine::Transform::_add_child(Transform * gameObject)
{
	children.push_back(gameObject);
}

void ScratchEngine::Transform::_remove_child(Transform * gameObject)
{
	for (auto it = children.begin(); it != children.end(); it++)
		if (*it == gameObject)
		{
			children.erase(it);
			break;
		}
}
