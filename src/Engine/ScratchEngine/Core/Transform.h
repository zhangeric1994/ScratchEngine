#pragma once

#include <DirectXMath.h>
#include <vector>

#include "../Common/Typedefs.h"
#include "../Message/IMessageReceiver.h"
#include "../Message/Message.h"
#include "../Rendering/RenderingEngine.h"

#include "Space.h"

using namespace DirectX;
using namespace ScratchEngine::Rendering;


namespace ScratchEngine
{
	struct __declspec(dllexport) Transform : public IMessageReceiver
	{
		friend class GameObject;
		friend class RenderingEngine;
		friend class Scene;
		

	private: // 120 bytes
		XMMATRIX worldMatrix;

		XMVECTOR localPosition;
		XMVECTOR localRotation;
		XMVECTOR localScale;

		bool isDirty;

		size_t index;
		Transform* parent;
		std::vector<Transform*> children;


	public:
		Transform();
		Transform(float x, float y, float z);
		Transform(XMFLOAT3 position, XMFLOAT4 rotation, XMFLOAT3 scale);
		Transform(XMVECTOR position, XMVECTOR rotation, XMVECTOR scale);
		~Transform();

		__inline XMVECTOR GetLocalPosition();
		__inline XMVECTOR GetPosition();
		__inline XMVECTOR GetLocalRotation();
		__inline XMVECTOR GetRotation();
		__inline XMVECTOR GetLocalScale();
		__inline XMVECTOR GetScale();
		__inline Transform* GetParent();
		__inline Transform* GetChild(size_t index);
		__inline size_t GetChildCount();

		__inline void SetLocalPosition(f32 x, f32 y, f32 z);
		__inline void SetLocalPosition(XMFLOAT3 position);
		__inline void SetLocalPosition(XMVECTOR position);
		__inline void SetPosition(f32 x, f32 y, f32 z);
		__inline void SetPosition(XMFLOAT3 position);
		__inline void SetPosition(XMVECTOR position);
		__inline void SetLocalRotation(f32 x, f32 y, f32 z);
		__inline void SetLocalRotation(XMFLOAT4 rotation);
		__inline void SetLocalRotation(XMVECTOR rotation);
		__inline void SetRotation(f32 x, f32 y, f32 z);
		__inline void SetRotation(XMFLOAT4 rotation);
		__inline void SetRotation(XMVECTOR rotation);
		__inline void SetLocalScale(f32 x, f32 y, f32 z);
		__inline void SetLocalScale(XMFLOAT3 scale);
		__inline void SetLocalScale(XMVECTOR scale);
		//void SetScale(f32 x, f32 y, f32 z);
		//void SetScale(XMFLOAT3 scale);
		//void SetScale(XMVECTOR scale);

		void* operator new(size_t size);
		void operator delete(void* p);

		void Translate(float x, float y, float z, Space space = SELF);
		void Translate(XMFLOAT3 translation, Space space = SELF);
		void Translate(XMVECTOR translation, Space space = SELF);
		void Rotate(float x, float y, float z, Space space = SELF);
		void Rotate(XMFLOAT4 rotation, Space space = SELF);
		void Rotate(XMVECTOR rotation, Space space = SELF);
		//void Scale(XMFLOAT3 scale);
		//void Scale(XMVECTOR scale);

		virtual void SendMessage_(const Message& message);
		virtual void SendMessageUp(const Message& message, u32 level = UINT_MAX);
		virtual void SendMessageDown(const Message& message, u32 level = UINT_MAX);


	protected:
		void SetParent(Transform* other);


	private:
		XMMATRIX GetWorldMatrix();

		size_t AddChild(Transform* gameObject);
		void RemoveChild(Transform* gameObject);

		virtual void HandleMessage(const Message& message) { }

		__forceinline void _set_local_position(XMVECTOR v);
		__forceinline void _set_local_rotation(XMVECTOR q);
		__forceinline void _set_local_scale(XMVECTOR v);
		__forceinline void _set_dirty();

		__forceinline void _update_world_matrix();

		__forceinline void _translate(XMVECTOR v, Space space);
		__forceinline void _rotate(XMVECTOR q, Space space);
	};
}
