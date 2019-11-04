#pragma once

#include "../header.h"

#include "../Common/Typedefs.h"
#include "../Message/IMessageReceiver.h"
#include "../Message/Message.h"
#include "../Rendering/RenderingEngine.h"

#include "Space.h"

using namespace ScratchEngine::Rendering;


namespace ScratchEngine
{
	struct __declspec(dllexport) Transform : public IMessageReceiver
	{
		friend class Animator;
		friend class GameObject;
		friend class RenderingEngine;
		friend class Scene;
		

	private: // 120 bytes
		XMMATRIX worldMatrix;

		XMVECTOR localPosition;
		XMVECTOR localRotation;
		XMVECTOR localScale;

		bool isDirty;

		Transform* parent;
		size_t index;
		vector<Transform*> children;


	public:
		Transform();
		Transform(float x, float y, float z);
		Transform(XMFLOAT3 position, XMFLOAT4 rotation, XMFLOAT3 scale);
		Transform(XMVECTOR position, XMVECTOR rotation, XMVECTOR scale);
		~Transform();

		XMVECTOR GetLocalPosition();
		XMVECTOR GetPosition();
		XMVECTOR GetLocalRotation();
		XMVECTOR GetRotation();
		XMVECTOR GetLocalScale();
		XMVECTOR GetScale();
		XMMATRIX GetWorldMatrix();
		XMVECTOR GetForwardVector();
		Transform* GetParent();
		Transform* GetChild(size_t index);
		size_t GetChildCount();

		void SetLocalPosition(f32 x, f32 y, f32 z);
		void SetLocalPosition(XMFLOAT3 position);
		void SetLocalPosition(XMVECTOR position);
		void SetPosition(f32 x, f32 y, f32 z);
		void SetPosition(XMFLOAT3 position);
		void SetPosition(XMVECTOR position);
		void SetLocalRotation(f32 x, f32 y, f32 z);
		void SetLocalRotation(XMFLOAT4 rotation);
		void SetLocalRotation(XMVECTOR rotation);
		void SetRotation(f32 x, f32 y, f32 z);
		void SetRotation(XMFLOAT4 rotation);
		void SetRotation(XMVECTOR rotation);
		void SetLocalScale(f32 size);
		void SetLocalScale(f32 x, f32 y, f32 z);
		void SetLocalScale(XMFLOAT3 scale);
		void SetLocalScale(XMVECTOR scale);
		//void SetScale(f32 x, f32 y, f32 z);
		//void SetScale(XMFLOAT3 scale);
		//void SetScale(XMVECTOR scale);

		void* operator new(size_t size);
		void operator delete(void* p);

		void Translate(float x, float y, float z, Space space = Space::SELF);
		void Translate(XMFLOAT3 translation, Space space = Space::SELF);
		void Translate(XMVECTOR translation, Space space = Space::SELF);
		void Rotate(float x, float y, float z, Space space = Space::SELF);
		void Rotate(XMFLOAT4 rotation, Space space = Space::SELF);
		void Rotate(XMVECTOR rotation, Space space = Space::SELF);
		//void Scale(XMFLOAT3 scale);
		//void Scale(XMVECTOR scale);

		virtual void SendMessage_(const Message& message);
		virtual void SendMessageUp(const Message& message, u32 level = UINT_MAX);
		virtual void SendMessageDown(const Message& message, u32 level = UINT_MAX);


	protected:
		void SetParent(Transform* other);


	private:
		virtual void HandleMessage(const Message& message) { }

		void __MarkDirty();
		
		size_t __AddChild(Transform* gameObject);
		void __RemoveChild(Transform* gameObject);

		void __UpdateWorldMatrix();
	};


	inline XMVECTOR Transform::GetForwardVector()
	{
		return XMVector3Normalize(XMVector3Rotate({ 0, 0, 1 }, GetRotation()));
	}
}


#include "Transform.inl"
