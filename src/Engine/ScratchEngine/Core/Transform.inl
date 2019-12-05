inline XMVECTOR ScratchEngine::Transform::GetLocalPosition()
{
	return localPosition;
}

inline XMVECTOR ScratchEngine::Transform::GetPosition()
{
	return parent ? XMVector3Transform(localPosition, parent->GetWorldMatrix()) : localPosition;
}

inline XMVECTOR ScratchEngine::Transform::GetLocalRotation()
{
	return localRotation;
}

inline XMVECTOR ScratchEngine::Transform::GetRotation()
{
	return parent ? XMQuaternionMultiply(localRotation, parent->GetRotation()) : localRotation;
}

inline XMVECTOR ScratchEngine::Transform::GetLocalScale()
{
	return localScale;
}

inline XMVECTOR ScratchEngine::Transform::GetScale()
{
	return parent ? XMVectorMultiply(localScale, parent->GetScale()) : localScale;
}

inline XMMATRIX ScratchEngine::Transform::GetWorldMatrix()
{
	if (isDirty)
		__UpdateWorldMatrix();

	return worldMatrix;
}

inline ScratchEngine::Transform* ScratchEngine::Transform::GetParent()
{
	return parent;
}

inline ScratchEngine::Transform* ScratchEngine::Transform::GetChild(size_t index)
{
	return index < children.size() ? children[index] : nullptr;
}

inline size_t ScratchEngine::Transform::GetChildCount()
{
	return children.size();
}

inline void ScratchEngine::Transform::SetLocalPosition(f32 x, f32 y, f32 z)
{
	SetLocalPosition(XMVectorSet(x, y, z, 0));
}

inline void ScratchEngine::Transform::SetLocalPosition(XMFLOAT3 position)
{
	SetLocalPosition(XMLoadFloat3(&position));
}

inline void ScratchEngine::Transform::SetLocalPosition(XMVECTOR position)
{
	if (XMVector3NotEqual(position, localPosition))
	{
		localPosition = position;

		__MarkDirty();
	}
}

inline void ScratchEngine::Transform::SetPosition(f32 x, f32 y, f32 z)
{
	SetLocalPosition(parent ? XMVector3Transform(XMVectorSet(x, y, z, 0), XMMatrixInverse(nullptr, parent->GetWorldMatrix())) : XMVectorSet(x, y, z, 0));
}

inline void ScratchEngine::Transform::SetPosition(XMFLOAT3 position)
{
	SetLocalPosition(parent ? XMVector3Transform(XMLoadFloat3(&position), XMMatrixInverse(nullptr, parent->GetWorldMatrix())) : XMLoadFloat3(&position));
}

inline void ScratchEngine::Transform::SetPosition(XMVECTOR position)
{
	SetLocalPosition(parent ? XMVector3Transform(position, XMMatrixInverse(nullptr, parent->GetWorldMatrix())) : position);
}

inline void ScratchEngine::Transform::SetLocalRotation(f32 x, f32 y, f32 z)
{
	SetLocalRotation(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)));
}

inline void ScratchEngine::Transform::SetLocalRotation(XMFLOAT4 rotation)
{
	SetLocalRotation(XMLoadFloat4(&rotation));
}

inline void ScratchEngine::Transform::SetLocalRotation(XMVECTOR rotation)
{
	if (XMQuaternionNotEqual(rotation, localRotation))
	{
		localRotation = rotation;

		__MarkDirty();
	}
}

inline void ScratchEngine::Transform::SetRotation(f32 x, f32 y, f32 z)
{
	SetLocalRotation(parent ? XMQuaternionMultiply(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)), XMQuaternionInverse(parent->GetRotation())) : XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)));
}

inline void ScratchEngine::Transform::SetRotation(XMFLOAT4 rotation)
{
	SetLocalRotation(parent ? XMQuaternionMultiply(XMLoadFloat4(&rotation), XMQuaternionInverse(parent->GetRotation())) : XMLoadFloat4(&rotation));
}

inline void ScratchEngine::Transform::SetRotation(XMVECTOR rotation)
{
	SetLocalRotation(parent ? XMQuaternionMultiply(rotation, XMQuaternionInverse(parent->GetRotation())) : rotation);
}

inline void ScratchEngine::Transform::SetLocalScale(f32 size)
{
	SetLocalScale(XMVectorSet(size, size, size, 0));
}

inline void ScratchEngine::Transform::SetLocalScale(f32 x, f32 y, f32 z)
{
	SetLocalScale(XMVectorSet(x, y, z, 0));
}

inline void ScratchEngine::Transform::SetLocalScale(XMFLOAT3 scale)
{
	SetLocalScale(XMLoadFloat3(&scale));
}

inline void ScratchEngine::Transform::SetLocalScale(XMVECTOR scale)
{
	scale = XMVectorMax(scale, XMVectorZero());

	if (XMVector3NotEqual(scale, localScale))
	{
		localScale = scale;

		__MarkDirty();
	}
}

inline void ScratchEngine::Transform::Translate(float x, float y, float z, Space space)
{
	Translate(XMVectorSet(x, y, z, 0), space);
}

inline void ScratchEngine::Transform::Translate(XMFLOAT3 translation, Space space)
{
	Translate(XMLoadFloat3(&translation), space);
}

inline void ScratchEngine::Transform::Translate(XMVECTOR translation, Space space)
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

inline void ScratchEngine::Transform::Rotate(float x, float y, float z, Space space)
{
	Rotate(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)), space);
}

inline void ScratchEngine::Transform::Rotate(XMFLOAT4 rotation, Space space)
{
	Rotate(XMLoadFloat4(&rotation), space);
}

inline void ScratchEngine::Transform::Rotate(XMVECTOR rotation, Space space)
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

inline void ScratchEngine::Transform::SendMessage_(const Message& message)
{
	HandleMessage(message);
}

inline void ScratchEngine::Transform::__MarkDirty()
{
	if (!isDirty)
	{
		isDirty = true;
		isFrameDirty = true;

		for (auto it = children.begin(); it != children.end(); it++)
			(*it)->__MarkDirty();
	}
}

inline size_t ScratchEngine::Transform::__AddChild(Transform* other)
{
	children.push_back(other);
	return children.size() - 1;
}

inline void ScratchEngine::Transform::__RemoveChild(Transform* transform)
{
	children.erase(children.begin() + transform->index);
}

inline void ScratchEngine::Transform::__UpdateWorldMatrix()
{
	if (parent)
		worldMatrix = (XMMatrixScalingFromVector(localScale) * XMMatrixRotationQuaternion(localRotation) * XMMatrixTranslationFromVector(localPosition)) * parent->GetWorldMatrix();
	else
		worldMatrix = XMMatrixScalingFromVector(localScale) * XMMatrixRotationQuaternion(localRotation) * XMMatrixTranslationFromVector(localPosition);

	isDirty = false;
}