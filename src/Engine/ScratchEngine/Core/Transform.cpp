#include <cstdlib>

#include "Transform.h"

ScratchEngine::Transform::Transform()
{
	localPosition = XMVectorZero();
	localRotation = XMVectorZero();
	localScale = { 1, 1, 1, 0 };

	set_dirty();
}

ScratchEngine::Transform::Transform(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale)
{
	this->localPosition = XMLoadFloat3(&position);
	this->localRotation = XMLoadFloat3(&rotation);
	this->localScale = XMLoadFloat3(&scale);

	set_dirty();
}

ScratchEngine::Transform::Transform(XMVECTOR position, XMVECTOR rotation, XMVECTOR scale)
{
	this->localPosition = position;
	this->localRotation = rotation;
	this->localScale = scale;

	set_dirty();
}

ScratchEngine::Transform::~Transform()
{
}

XMVECTOR ScratchEngine::Transform::GetLocalPosition()
{
	return localPosition;
}

XMVECTOR ScratchEngine::Transform::GetLocalRotation()
{
	return localRotation;
}

XMVECTOR ScratchEngine::Transform::GetLocalScale()
{
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
	if (x != localPosition.m128_f32[0] || y != localPosition.m128_f32[1] || z != localPosition.m128_f32[2])
	{
		set_dirty();

		localPosition = { x, y, z, 0 };
	}
}

void ScratchEngine::Transform::SetLocalPosition(XMFLOAT3 position)
{
	if (position.x != this->localPosition.m128_f32[0] || position.y != this->localPosition.m128_f32[1] || position.z != this->localPosition.m128_f32[2])
	{
		set_dirty();

		this->localPosition = XMLoadFloat3(&position);
	}
}

void ScratchEngine::Transform::SetLocalPosition(XMVECTOR position)
{
	if (XMVector3NotEqualInt(position, this->localPosition))
	{
		set_dirty();

		this->localPosition = position;
	}
}

void ScratchEngine::Transform::SetLocalRotation(f32 x, f32 y, f32 z)
{
	if (x != localRotation.m128_f32[0] || y != localRotation.m128_f32[1] || z != localRotation.m128_f32[2])
	{
		set_dirty();

		localRotation = { x, y, z, 0 };
	}
}

void ScratchEngine::Transform::SetLocalRotation(XMFLOAT3 rotation)
{
	if (rotation.x != this->localRotation.m128_f32[0] || rotation.y != this->localRotation.m128_f32[1] || rotation.z != this->localRotation.m128_f32[2])
	{
		set_dirty();

		this->localRotation = XMLoadFloat3(&rotation);
	}
}

void ScratchEngine::Transform::SetLocalRotation(XMVECTOR rotation)
{
	if (XMVector3NotEqualInt(rotation, this->localRotation))
	{
		set_dirty();

		this->localRotation = rotation;
	}
}

void ScratchEngine::Transform::SetLocalScale(f32 x, f32 y, f32 z)
{
	x = __max(0, x);
	y = __max(0, y);
	z = __max(0, z);

	if (x != localScale.m128_f32[0] || y != localScale.m128_f32[1] || z != localScale.m128_f32[2])
	{
		set_dirty();

		localScale = { x, y, z, 0 };
	}
}

void ScratchEngine::Transform::SetLocalScale(XMFLOAT3 scale)
{
	scale.x = __max(0, scale.x);
	scale.y = __max(0, scale.y);
	scale.z = __max(0, scale.z);

	if (scale.x != this->localScale.m128_f32[0] || scale.y != this->localScale.m128_f32[1] || scale.z != this->localScale.m128_f32[2])
	{
		set_dirty();

		this->localScale = XMLoadFloat3(&scale);
	}
}

void ScratchEngine::Transform::SetLocalScale(XMVECTOR scale)
{
	scale = XMVectorMax(scale, XMVectorZero());

	if (XMVector3NotEqualInt(scale, this->localScale))
	{
		set_dirty();

		this->localScale = scale;
	}
}

void ScratchEngine::Transform::SetParent(Transform* parent)
{
	if (this->parent)
		this->parent->remove_child(this);

	parent->add_child(this);

	this->parent = parent;

	set_dirty();
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
	if (translation.x != 0 || translation.y != 0 || translation.z != 0)
	{
		set_dirty();

		XMVECTOR v = XMLoadFloat3(&translation);

		switch (space)
		{
		case WORLD:
			localPosition = XMVectorAdd(localPosition, v);
			break;

		case SELF:
			localPosition = XMVectorAdd(localPosition, XMVector3Rotate(v, XMQuaternionRotationRollPitchYawFromVector(localRotation)));
			break;
		}
	}
}

void ScratchEngine::Transform::Translate(XMVECTOR translation, Space space)
{
	if (XMVector3NotEqualInt(translation, XMVectorZero()))
	{
		set_dirty();

		switch (space)
		{
		case WORLD:
			localPosition = XMVectorAdd(localPosition, translation);
			break;

		case SELF:
			localPosition = XMVectorAdd(localPosition, XMVector3Rotate(translation, XMQuaternionRotationRollPitchYawFromVector(localRotation)));
			break;
		}
	}
}

void ScratchEngine::Transform::Rotate(XMFLOAT3 eulerAngles)
{
	if (eulerAngles.x != 0 || eulerAngles.y != 0 || eulerAngles.z != 0)
	{
		set_dirty();

		XMVECTOR v = XMLoadFloat3(&eulerAngles);
		localRotation = XMVectorAdd(localRotation, v);
	}
}

void ScratchEngine::Transform::Rotate(XMVECTOR eulerAngles)
{
	if (XMVector3NotEqualInt(eulerAngles, XMVectorZero()))
	{
		set_dirty();

		localRotation = XMVectorAdd(localRotation, eulerAngles);
	}
}

void ScratchEngine::Transform::Scale(XMFLOAT3 scale)
{
	if (scale.x != 1 || scale.y != 1 || scale.z != 1)
	{
		set_dirty();

		XMVECTOR v = XMLoadFloat3(&scale);
		this->localScale = XMVectorMultiply(this->localScale, v);
	}
}

void ScratchEngine::Transform::Scale(XMVECTOR scale)
{
	if (XMVector3Greater(scale, XMVectorZero()) && XMVector3NotEqualInt(scale, { 1, 1, 1, 0 }))
	{
		set_dirty();

		this->localScale = XMVectorMultiply(this->localScale, scale);
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
		update_world_matrix();

	return worldMatrix;
}

void ScratchEngine::Transform::update_world_matrix()
{
	worldMatrix = XMMatrixTranspose(XMMatrixScalingFromVector(localScale) * XMMatrixRotationRollPitchYawFromVector(localRotation) * XMMatrixTranslationFromVector(localPosition));

	if (parent)
		worldMatrix = parent->GetWorldMatrix() * worldMatrix;

	isDirty = false;
}

void ScratchEngine::Transform::set_dirty()
{
	if (!isDirty)
	{
		isDirty = true;

		for (auto it = children.begin(); it != children.end(); it++)
			(*it)->set_dirty();
	}
}

void ScratchEngine::Transform::add_child(Transform * gameObject)
{
	children.push_back(gameObject);
}

void ScratchEngine::Transform::remove_child(Transform * gameObject)
{
	for (auto it = children.begin(); it != children.end(); it++)
		if (*it == gameObject)
		{
			children.erase(it);
			break;
		}
}
