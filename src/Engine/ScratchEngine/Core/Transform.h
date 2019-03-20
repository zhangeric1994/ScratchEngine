#pragma once

#include <DirectXMath.h>
#include <vector>

#include "../Message/IMessageReceiver.h"
#include "../Message/Message.h"
#include "../Utility/Typedefs.h"

#include "Space.h"

using namespace DirectX;

namespace ScratchEngine
{
	class __declspec(dllexport) Transform : public IMessageReceiver
	{
		friend class GameObject;

	private: // 120 bytes
		XMMATRIX worldMatrix;

		XMVECTOR localPosition;
		XMVECTOR localRotation;
		XMVECTOR localScale;

		bool isDirty;

		Transform* parent;
		std::vector<Transform*> children;


	public:
		Transform();
		Transform(XMFLOAT3 position, XMFLOAT4 rotation, XMFLOAT3 scale);
		Transform(XMVECTOR position, XMVECTOR rotation, XMVECTOR scale);
		~Transform();

		XMVECTOR GetLocalPosition();
		XMVECTOR GetPosition();
		XMVECTOR GetLocalRotation();
		XMVECTOR GetRotation();
		XMVECTOR GetLocalScale();
		XMVECTOR GetScale();
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
		void SetLocalScale(f32 x, f32 y, f32 z);
		void SetLocalScale(XMFLOAT3 scale);
		void SetLocalScale(XMVECTOR scale);
		void SetScale(f32 x, f32 y, f32 z);
		void SetScale(XMFLOAT3 scale);
		void SetScale(XMVECTOR scale);
		void SetParent(Transform* parent);

		void* operator new(size_t size);
		void operator delete(void* p);

		void Translate(XMFLOAT3 translation, Space space = SELF);
		void Translate(XMVECTOR translation, Space space = SELF);
		void Rotate(XMFLOAT4 rotation);
		void Rotate(XMVECTOR rotation);
		void Scale(XMFLOAT3 scale);
		void Scale(XMVECTOR scale);

		virtual void SendMessage(const Message& message);
		virtual void SendMessageUp(const Message& message, u32 level = UINT_MAX);
		virtual void SendMessageDown(const Message& message, u32 level = UINT_MAX);


	private:
		XMMATRIX GetWorldMatrix();

		virtual void HandleMessage(const Message& message) { }

		__forceinline void _update_world_matrix();

		__forceinline void _set_local_position(XMVECTOR v);
		__forceinline void _set_local_rotation(XMVECTOR q);
		__forceinline void _set_local_scale(XMVECTOR v);
		__forceinline void _set_dirty();

		__forceinline void _add_child(Transform* gameObject);
		__forceinline void _remove_child(Transform* gameObject);
	};
}
