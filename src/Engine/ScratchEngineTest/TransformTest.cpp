#include "CppUnitTest.h"
#include "DirectXMath.h"
#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace ScratchEngine;
using namespace ScratchEngine::Memory;

namespace ScratchEngineTest
{
	TEST_CLASS(GameObjectTest)
	{
#define tolerance (100 * g_XMEpsilon)

	public:
		TEST_METHOD(Constructor1)
		{
			GameObject* gameObject = new GameObject();


			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), gameObject->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), gameObject->GetLocalScale()));

			Assert::IsNull(gameObject->GetParent());

			Assert::AreEqual((size_t)0, gameObject->GetChildCount());
			Assert::IsNull(gameObject->GetChild(0));
		}

		TEST_METHOD(Constructor2)
		{
			GameObject* gameObject = new GameObject(XMVectorSet(10, 10, 10, 0), XMQuaternionRotationRollPitchYaw(0, 90, 0), XMVectorSet(2, 2, 2, 0));

			
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), gameObject->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(0, 90, 0), gameObject->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 2, 2, 0), gameObject->GetLocalScale()));

			Assert::IsNull(gameObject->GetParent());

			Assert::AreEqual((size_t)0, gameObject->GetChildCount());
			Assert::IsNull(gameObject->GetChild(0));
		}

		TEST_METHOD(GetPosition1)
		{
			GameObject* gameObject = new GameObject();


			gameObject->SetLocalPosition(10, 10, 10);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), gameObject->GetPosition()));
		}

		TEST_METHOD(GetPosition2)
		{
			GameObject* gameObject1 = new GameObject();
			GameObject* gameObject2 = new GameObject();


			gameObject1->SetLocalPosition(10, 10, 10);

			gameObject2->SetParent(gameObject1);
			gameObject2->SetLocalPosition(0, 0, 0);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), gameObject1->GetPosition()));

			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), gameObject2->GetPosition()), L"GetPosition() did not return the correct value when there is no local gameObjectation.");
		}

		TEST_METHOD(GetRotation1)
		{
			GameObject* gameObject = new GameObject();


			gameObject->SetLocalRotation(0, 90, 0);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), gameObject->GetRotation()));
		}

		TEST_METHOD(GetRotation2)
		{
			GameObject* gameObject1 = new GameObject();
			GameObject* gameObject2 = new GameObject();


			gameObject2->SetParent(gameObject1);

			gameObject1->SetLocalRotation(0, 90, 0);
			gameObject2->SetLocalRotation(90, 180, 270);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), gameObject1->GetRotation()));
			Assert::AreEqual(true, XMVector4NearEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetRotation(), tolerance));
		}

		TEST_METHOD(SetPosition1)
		{
			GameObject* gameObject = new GameObject();


			gameObject->SetPosition(10, 10, 10);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), gameObject->GetLocalPosition()));
		}

		TEST_METHOD(SetPosition2)
		{
			GameObject* gameObject1 = new GameObject();
			GameObject* gameObject2 = new GameObject();


			gameObject2->SetParent(gameObject1);

			gameObject2->SetPosition(10, 10, 10);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), gameObject2->GetLocalPosition()));
		}

		TEST_METHOD(SetPosition3)
		{
			GameObject* gameObject1 = new GameObject();
			GameObject* gameObject2 = new GameObject();


			gameObject2->SetParent(gameObject1);

			gameObject1->SetLocalPosition(10, 10, 10);
			gameObject2->SetPosition(20, 20, 20);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), gameObject2->GetLocalPosition()));
		}

		TEST_METHOD(SetPosition4)
		{
			GameObject* gameObject1 = new GameObject();
			GameObject* gameObject2 = new GameObject();


			gameObject2->SetParent(gameObject1);

			gameObject1->SetLocalRotation(0, 90, 0);
			gameObject2->SetPosition(20, 20, 20);


			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(-20, 20, 20, 0), gameObject2->GetLocalPosition(), tolerance));
		}

		TEST_METHOD(SetPosition5)
		{
			GameObject* gameObject1 = new GameObject();
			GameObject* gameObject2 = new GameObject();


			gameObject2->SetParent(gameObject1);

			gameObject1->SetLocalScale(2, 2, 2);
			gameObject2->SetPosition(20, 20, 20);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), gameObject2->GetLocalPosition()));
		}

		TEST_METHOD(SetRotation1)
		{
			GameObject* gameObject = new GameObject();


			gameObject->SetRotation(0, 90, 0);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), gameObject->GetLocalRotation()));
		}

		TEST_METHOD(SetRotation2)
		{
			GameObject* gameObject1 = new GameObject();
			GameObject* gameObject2 = new GameObject();


			gameObject2->SetParent(gameObject1);

			gameObject2->SetRotation(90, 180, 180);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionIdentity(), gameObject1->GetLocalRotation()));
			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetLocalRotation()));
		}

		TEST_METHOD(SetRotation3)
		{
			GameObject* gameObject1 = new GameObject();
			GameObject* gameObject2 = new GameObject();


			gameObject2->SetParent(gameObject1);

			gameObject1->SetLocalPosition(10, 10, 10);
			gameObject2->SetRotation(90, 180, 180);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionIdentity(), gameObject1->GetLocalRotation()));
			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetLocalRotation()));
		}

		TEST_METHOD(SetRotation4)
		{
			GameObject* gameObject1 = new GameObject();
			GameObject* gameObject2 = new GameObject();

			gameObject2->SetParent(gameObject1);

			gameObject1->SetLocalRotation(0, 90, 0);
			gameObject2->SetRotation(90, 180, 180);

			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), gameObject1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector4NearEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(270)), gameObject2->GetLocalRotation(), tolerance));
		}

		TEST_METHOD(SetRotation5)
		{
			GameObject* gameObject1 = new GameObject();
			GameObject* gameObject2 = new GameObject();


			gameObject2->SetParent(gameObject1);

			gameObject1->SetLocalScale(2, 2, 2);
			gameObject2->SetRotation(90, 180, 180);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionIdentity(), gameObject1->GetLocalRotation()));
			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetLocalRotation()));
		}

		//TEST_METHOD(SetScale1)
		//{
		//	GameObject* gameObject = new GameObject();


		//	gameObject->SetScale(2, 2, 2);

		//	
		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 2, 2, 0), gameObject->GetLocalScale()));
		//}

		//TEST_METHOD(SetScale2)
		//{
		//	GameObject* gameObject1 = new GameObject();
		//	GameObject* gameObject2 = new GameObject();


		//	gameObject2->SetParent(gameObject1);

		//	gameObject2->SetScale(2, 2, 2);


		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), gameObject1->GetLocalScale()));
		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 2, 2, 0), gameObject2->GetLocalScale()));
		//}

		//TEST_METHOD(SetScale3)
		//{
		//	GameObject* gameObject1 = new GameObject();
		//	GameObject* gameObject2 = new GameObject();


		//	gameObject2->SetParent(gameObject1);

		//	gameObject1->SetLocalPosition(10, 10, 10);
		//	gameObject2->SetScale(2, 2, 2);


		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), gameObject1->GetLocalScale()));
		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 2, 2, 0), gameObject2->GetLocalScale()));
		//}

		//TEST_METHOD(SetScale4)
		//{
		//	GameObject* gameObject1 = new GameObject();
		//	GameObject* gameObject2 = new GameObject();


		//	gameObject2->SetParent(gameObject1);

		//	gameObject1->SetLocalRotation(0, 90, 0);
		//	gameObject2->SetScale(2, 2, 2);


		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), gameObject1->GetLocalScale()));
		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 2, 2, 0), gameObject2->GetLocalScale()));
		//}

		//TEST_METHOD(SetScale5)
		//{
		//	GameObject* gameObject1 = new GameObject();
		//	GameObject* gameObject2 = new GameObject();


		//	gameObject2->SetParent(gameObject1);

		//	gameObject1->SetLocalScale(1, 0.5, 1);
		//	gameObject2->SetScale(2, 2, 2);


		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 0.5, 1, 0), gameObject1->GetLocalScale()));
		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 4, 2, 0), gameObject2->GetLocalScale()));
		//}

		TEST_METHOD(SetParent1)
		{
			GameObject* gameObject1 = new GameObject();
			GameObject* gameObject2 = new GameObject(XMVectorZero(), XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), XMVectorSet(3, 4, 5, 0));


			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), gameObject1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), gameObject1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), gameObject1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), gameObject1->GetScale()));
			Assert::IsNull(gameObject1->GetParent());
			Assert::AreEqual((size_t)0, gameObject1->GetChildCount());
			Assert::IsNull(gameObject1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), gameObject2->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), gameObject2->GetScale()));
			Assert::IsNull(gameObject2->GetParent());
			Assert::AreEqual((size_t)0, gameObject2->GetChildCount());
			Assert::IsNull(gameObject2->GetChild(0));


			gameObject2->SetParent(gameObject1);


			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), gameObject1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), gameObject1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), gameObject1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), gameObject1->GetScale()));
			Assert::IsNull(gameObject1->GetParent());
			Assert::AreEqual((size_t)1, gameObject1->GetChildCount());
			Assert::AreEqual((uptr)gameObject2, (uptr)gameObject1->GetChild(0));
			
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetRotation()));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(3, 4, 5, 0), gameObject2->GetLocalScale(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(3, 4, 5, 0), gameObject2->GetScale(), tolerance));
			Assert::AreEqual((uptr)gameObject1, (uptr)gameObject2->GetParent());
			Assert::AreEqual((size_t)0, gameObject2->GetChildCount());
			Assert::IsNull(gameObject2->GetChild(0));
		}

		TEST_METHOD(SetParent2)
		{
			GameObject* gameObject1 = new GameObject();
			GameObject* gameObject2 = new GameObject(XMVectorZero(), XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), XMVectorSet(3, 4, 5, 0));


			gameObject1->SetLocalPosition(10, 20, 30);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), gameObject1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), gameObject1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), gameObject1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), gameObject1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), gameObject1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), gameObject1->GetScale()));
			Assert::IsNull(gameObject1->GetParent());
			Assert::AreEqual((size_t)0, gameObject1->GetChildCount());
			Assert::IsNull(gameObject1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), gameObject2->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), gameObject2->GetScale()));
			Assert::IsNull(gameObject2->GetParent());
			Assert::AreEqual((size_t)0, gameObject2->GetChildCount());
			Assert::IsNull(gameObject2->GetChild(0));


			gameObject2->SetParent(gameObject1);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), gameObject1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), gameObject1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), gameObject1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), gameObject1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), gameObject1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), gameObject1->GetScale()));
			Assert::IsNull(gameObject1->GetParent());
			Assert::AreEqual((size_t)1, gameObject1->GetChildCount());
			Assert::AreEqual((uptr)gameObject2, (uptr)gameObject1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(-10, -20, -30, 0), gameObject2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetRotation()));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(3, 4, 5, 0), gameObject2->GetLocalScale(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(3, 4, 5, 0), gameObject2->GetScale(), tolerance));
			Assert::AreEqual((uptr)gameObject1, (uptr)gameObject2->GetParent());
			Assert::AreEqual((size_t)0, gameObject2->GetChildCount());
			Assert::IsNull(gameObject2->GetChild(0));
		}

		TEST_METHOD(SetParent3)
		{
			GameObject* gameObject1 = new GameObject();
			GameObject* gameObject2 = new GameObject(XMVectorZero(), XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), XMVectorSet(3, 4, 5, 0));


			gameObject1->SetLocalRotation(0, 90, 0);


			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), gameObject1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), gameObject1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), gameObject1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), gameObject1->GetScale()));
			Assert::IsNull(gameObject1->GetParent());
			Assert::AreEqual((size_t)0, gameObject1->GetChildCount());
			Assert::IsNull(gameObject1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), gameObject2->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), gameObject2->GetScale()));
			Assert::IsNull(gameObject2->GetParent());
			Assert::AreEqual((size_t)0, gameObject2->GetChildCount());
			Assert::IsNull(gameObject2->GetChild(0));


			gameObject2->SetParent(gameObject1);


			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), gameObject1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), gameObject1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), gameObject1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), gameObject1->GetScale()));
			Assert::IsNull(gameObject1->GetParent());
			Assert::AreEqual((size_t)1, gameObject1->GetChildCount());
			Assert::AreEqual((uptr)gameObject2, (uptr)gameObject1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetPosition()));
			Assert::AreEqual(true, XMVector4NearEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(270)), gameObject2->GetLocalRotation(), tolerance));
			Assert::AreEqual(true, XMVector4NearEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetRotation(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(3, 4, 5, 0), gameObject2->GetLocalScale(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(3, 4, 5, 0), gameObject2->GetScale(), tolerance));
			Assert::AreEqual((uptr)gameObject1, (uptr)gameObject2->GetParent());
			Assert::AreEqual((size_t)0, gameObject2->GetChildCount());
			Assert::IsNull(gameObject2->GetChild(0));
		}

		TEST_METHOD(SetParent4)
		{
			GameObject* gameObject1 = new GameObject();
			GameObject* gameObject2 = new GameObject(XMVectorZero(), XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), XMVectorSet(3, 4, 5, 0));


			gameObject1->SetLocalScale(0.5, 1, 2);


			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), gameObject1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), gameObject1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 1, 2, 0), gameObject1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 1, 2, 0), gameObject1->GetScale()));
			Assert::IsNull(gameObject1->GetParent());
			Assert::AreEqual((size_t)0, gameObject1->GetChildCount());
			Assert::IsNull(gameObject1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), gameObject2->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), gameObject2->GetScale()));
			Assert::IsNull(gameObject2->GetParent());
			Assert::AreEqual((size_t)0, gameObject2->GetChildCount());
			Assert::IsNull(gameObject2->GetChild(0));


			gameObject2->SetParent(gameObject1);


			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), gameObject1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), gameObject1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 1, 2, 0), gameObject1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 1, 2, 0), gameObject1->GetScale()));
			Assert::IsNull(gameObject1->GetParent());
			Assert::AreEqual((size_t)1, gameObject1->GetChildCount());
			Assert::AreEqual((uptr)gameObject2, (uptr)gameObject1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetRotation()));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(6, 2, 5, 0), gameObject2->GetLocalScale(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(3, 4, 5, 0), gameObject2->GetScale(), tolerance));
			Assert::AreEqual((uptr)gameObject1, (uptr)gameObject2->GetParent());
			Assert::AreEqual((size_t)0, gameObject2->GetChildCount());
			Assert::IsNull(gameObject2->GetChild(0));
		}

		TEST_METHOD(SetParent5)
		{
			GameObject* gameObject1 = new GameObject(XMVectorSet(10, 20, 30, 0), XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), XMVectorSet(0.5, 1, 2, 0));
			GameObject* gameObject2 = new GameObject(XMVectorZero(), XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), XMVectorSet(3, 4, 5, 0));


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), gameObject1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), gameObject1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), gameObject1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), gameObject1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 1, 2, 0), gameObject1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 1, 2, 0), gameObject1->GetScale()));
			Assert::IsNull(gameObject1->GetParent());
			Assert::AreEqual((size_t)0, gameObject1->GetChildCount());
			Assert::IsNull(gameObject1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), gameObject2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), gameObject2->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), gameObject2->GetScale()));
			Assert::IsNull(gameObject2->GetParent());
			Assert::AreEqual((size_t)0, gameObject2->GetChildCount());
			Assert::IsNull(gameObject2->GetChild(0));


			gameObject2->SetParent(gameObject1);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), gameObject1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), gameObject1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), gameObject1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), gameObject1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 1, 2, 0), gameObject1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 1, 2, 0), gameObject1->GetScale()));
			Assert::IsNull(gameObject1->GetParent());
			Assert::AreEqual((size_t)1, gameObject1->GetChildCount());
			Assert::AreEqual((uptr)gameObject2, (uptr)gameObject1->GetChild(0));

			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(60, -20, -5, 0), gameObject2->GetLocalPosition(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorZero(), gameObject2->GetPosition(), tolerance));
			Assert::AreEqual(true, XMVector4NearEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(270)), gameObject2->GetLocalRotation(), tolerance));
			Assert::AreEqual(true, XMVector4NearEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), gameObject2->GetRotation(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(1.5, 8, 5, 0), gameObject2->GetLocalScale(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(3, 4, 5, 0), gameObject2->GetScale(), tolerance));
			Assert::AreEqual((uptr)gameObject1, (uptr)gameObject2->GetParent());
			Assert::AreEqual((size_t)0, gameObject2->GetChildCount());
			Assert::IsNull(gameObject2->GetChild(0));
		}

		TEST_METHOD(Translate1)
		{
			GameObject* gameObject = new GameObject();


			gameObject->Translate(10, 20, 30, WORLD);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), gameObject->GetLocalPosition()));
		}

		TEST_METHOD(Translate2)
		{
			GameObject* gameObject1 = new GameObject();
			GameObject* gameObject2 = new GameObject();


			gameObject2->SetParent(gameObject1);

			gameObject1->SetLocalRotation(0, 90, 0);
			gameObject2->Translate(10, 20, 30, WORLD);


			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(-30, 20, 10, 0), gameObject2->GetLocalPosition(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(10, 20, 30, 0), gameObject2->GetPosition(), tolerance));
		}

		TEST_METHOD(Rotate1)
		{
			GameObject* gameObject = new GameObject();


			gameObject->Rotate(90, 180, 270, WORLD);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(270)), gameObject->GetLocalRotation()));
		}

		TEST_METHOD(Rotate2)
		{
			GameObject* gameObject1 = new GameObject();
			GameObject* gameObject2 = new GameObject();


			gameObject2->SetParent(gameObject1);

			gameObject1->SetLocalRotation(0, 90, 0);
			gameObject2->Rotate(90, 180, 270, WORLD);


			Assert::AreEqual(true, XMVector4NearEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(-90), XMConvertToRadians(90)), gameObject2->GetLocalRotation(), tolerance));
			Assert::AreEqual(true, XMVector4NearEqual(XMQuaternionMultiply(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(270))), gameObject2->GetRotation(), tolerance));
		}
	};
}
