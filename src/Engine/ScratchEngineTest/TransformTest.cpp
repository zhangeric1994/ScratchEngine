#include "CppUnitTest.h"
#include "DirectXMath.h"
#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace ScratchEngine;
using namespace ScratchEngine::Memory;

namespace ScratchEngineTest
{
	TEST_CLASS(TransformTest)
	{
#define tolerance (100 * g_XMEpsilon)

	public:
		TEST_METHOD(Constructor1)
		{
			Transform* transform = new Transform();


			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform->GetLocalScale()));

			Assert::IsNull(transform->GetParent());

			Assert::AreEqual((size_t)0, transform->GetChildCount());
			Assert::IsNull(transform->GetChild(0));
		}

		TEST_METHOD(Constructor2)
		{
			Transform* transform = new Transform(XMVectorSet(10, 10, 10, 0), XMQuaternionRotationRollPitchYaw(0, 90, 0), XMVectorSet(2, 2, 2, 0));

			
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), transform->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(0, 90, 0), transform->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 2, 2, 0), transform->GetLocalScale()));

			Assert::IsNull(transform->GetParent());

			Assert::AreEqual((size_t)0, transform->GetChildCount());
			Assert::IsNull(transform->GetChild(0));
		}

		TEST_METHOD(GetPosition1)
		{
			Transform* transform = new Transform();


			transform->SetLocalPosition(10, 10, 10);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), transform->GetPosition()));
		}

		TEST_METHOD(GetPosition2)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform1->SetLocalPosition(10, 10, 10);

			transform2->SetParent(transform1);
			transform2->SetLocalPosition(0, 0, 0);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), transform1->GetPosition()));

			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), transform2->GetPosition()), L"GetPosition() did not return the correct value when there is no local transformation.");
		}

		TEST_METHOD(GetRotation1)
		{
			Transform* transform = new Transform();


			transform->SetLocalRotation(0, 90, 0);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), transform->GetRotation()));
		}

		TEST_METHOD(GetRotation2)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform1->SetLocalRotation(0, 90, 0);
			transform2->SetLocalRotation(90, 180, 270);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), transform1->GetRotation()));
			Assert::AreEqual(true, XMVector4NearEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetRotation(), tolerance));
		}

		TEST_METHOD(SetPosition1)
		{
			Transform* transform = new Transform();


			transform->SetPosition(10, 10, 10);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), transform->GetLocalPosition()));
		}

		TEST_METHOD(SetPosition2)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform2->SetPosition(10, 10, 10);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), transform2->GetLocalPosition()));
		}

		TEST_METHOD(SetPosition3)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform1->SetLocalPosition(10, 10, 10);
			transform2->SetPosition(20, 20, 20);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), transform2->GetLocalPosition()));
		}

		TEST_METHOD(SetPosition4)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform1->SetLocalRotation(0, 90, 0);
			transform2->SetPosition(20, 20, 20);


			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(-20, 20, 20, 0), transform2->GetLocalPosition(), tolerance));
		}

		TEST_METHOD(SetPosition5)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform1->SetLocalScale(2, 2, 2);
			transform2->SetPosition(20, 20, 20);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), transform2->GetLocalPosition()));
		}

		TEST_METHOD(SetRotation1)
		{
			Transform* transform = new Transform();


			transform->SetRotation(0, 90, 0);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), transform->GetLocalRotation()));
		}

		TEST_METHOD(SetRotation2)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform2->SetRotation(90, 180, 180);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionIdentity(), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetLocalRotation()));
		}

		TEST_METHOD(SetRotation3)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform1->SetLocalPosition(10, 10, 10);
			transform2->SetRotation(90, 180, 180);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionIdentity(), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetLocalRotation()));
		}

		TEST_METHOD(SetRotation4)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();

			transform2->SetParent(transform1);

			transform1->SetLocalRotation(0, 90, 0);
			transform2->SetRotation(90, 180, 180);

			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector4NearEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(270)), transform2->GetLocalRotation(), tolerance));
		}

		TEST_METHOD(SetRotation5)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform1->SetLocalScale(2, 2, 2);
			transform2->SetRotation(90, 180, 180);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionIdentity(), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetLocalRotation()));
		}

		//TEST_METHOD(SetScale1)
		//{
		//	Transform* transform = new Transform();


		//	transform->SetScale(2, 2, 2);

		//	
		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 2, 2, 0), transform->GetLocalScale()));
		//}

		//TEST_METHOD(SetScale2)
		//{
		//	Transform* transform1 = new Transform();
		//	Transform* transform2 = new Transform();


		//	transform2->SetParent(transform1);

		//	transform2->SetScale(2, 2, 2);


		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetLocalScale()));
		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 2, 2, 0), transform2->GetLocalScale()));
		//}

		//TEST_METHOD(SetScale3)
		//{
		//	Transform* transform1 = new Transform();
		//	Transform* transform2 = new Transform();


		//	transform2->SetParent(transform1);

		//	transform1->SetLocalPosition(10, 10, 10);
		//	transform2->SetScale(2, 2, 2);


		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetLocalScale()));
		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 2, 2, 0), transform2->GetLocalScale()));
		//}

		//TEST_METHOD(SetScale4)
		//{
		//	Transform* transform1 = new Transform();
		//	Transform* transform2 = new Transform();


		//	transform2->SetParent(transform1);

		//	transform1->SetLocalRotation(0, 90, 0);
		//	transform2->SetScale(2, 2, 2);


		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetLocalScale()));
		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 2, 2, 0), transform2->GetLocalScale()));
		//}

		//TEST_METHOD(SetScale5)
		//{
		//	Transform* transform1 = new Transform();
		//	Transform* transform2 = new Transform();


		//	transform2->SetParent(transform1);

		//	transform1->SetLocalScale(1, 0.5, 1);
		//	transform2->SetScale(2, 2, 2);


		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 0.5, 1, 0), transform1->GetLocalScale()));
		//	Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 4, 2, 0), transform2->GetLocalScale()));
		//}

		TEST_METHOD(SetParent1)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform(XMVectorZero(), XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), XMVectorSet(3, 4, 5, 0));


			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetScale()));
			Assert::IsNull(transform1->GetParent());
			Assert::AreEqual((size_t)0, transform1->GetChildCount());
			Assert::IsNull(transform1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), transform2->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), transform2->GetScale()));
			Assert::IsNull(transform2->GetParent());
			Assert::AreEqual((size_t)0, transform2->GetChildCount());
			Assert::IsNull(transform2->GetChild(0));


			transform2->SetParent(transform1);


			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetScale()));
			Assert::IsNull(transform1->GetParent());
			Assert::AreEqual((size_t)1, transform1->GetChildCount());
			Assert::AreEqual((uptr)transform2, (uptr)transform1->GetChild(0));
			
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetRotation()));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(3, 4, 5, 0), transform2->GetLocalScale(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(3, 4, 5, 0), transform2->GetScale(), tolerance));
			Assert::AreEqual((uptr)transform1, (uptr)transform2->GetParent());
			Assert::AreEqual((size_t)0, transform2->GetChildCount());
			Assert::IsNull(transform2->GetChild(0));
		}

		TEST_METHOD(SetParent2)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform(XMVectorZero(), XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), XMVectorSet(3, 4, 5, 0));


			transform1->SetLocalPosition(10, 20, 30);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), transform1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), transform1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetScale()));
			Assert::IsNull(transform1->GetParent());
			Assert::AreEqual((size_t)0, transform1->GetChildCount());
			Assert::IsNull(transform1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), transform2->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), transform2->GetScale()));
			Assert::IsNull(transform2->GetParent());
			Assert::AreEqual((size_t)0, transform2->GetChildCount());
			Assert::IsNull(transform2->GetChild(0));


			transform2->SetParent(transform1);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), transform1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), transform1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetScale()));
			Assert::IsNull(transform1->GetParent());
			Assert::AreEqual((size_t)1, transform1->GetChildCount());
			Assert::AreEqual((uptr)transform2, (uptr)transform1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(-10, -20, -30, 0), transform2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetRotation()));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(3, 4, 5, 0), transform2->GetLocalScale(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(3, 4, 5, 0), transform2->GetScale(), tolerance));
			Assert::AreEqual((uptr)transform1, (uptr)transform2->GetParent());
			Assert::AreEqual((size_t)0, transform2->GetChildCount());
			Assert::IsNull(transform2->GetChild(0));
		}

		TEST_METHOD(SetParent3)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform(XMVectorZero(), XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), XMVectorSet(3, 4, 5, 0));


			transform1->SetLocalRotation(0, 90, 0);


			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), transform1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetScale()));
			Assert::IsNull(transform1->GetParent());
			Assert::AreEqual((size_t)0, transform1->GetChildCount());
			Assert::IsNull(transform1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), transform2->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), transform2->GetScale()));
			Assert::IsNull(transform2->GetParent());
			Assert::AreEqual((size_t)0, transform2->GetChildCount());
			Assert::IsNull(transform2->GetChild(0));


			transform2->SetParent(transform1);


			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), transform1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetScale()));
			Assert::IsNull(transform1->GetParent());
			Assert::AreEqual((size_t)1, transform1->GetChildCount());
			Assert::AreEqual((uptr)transform2, (uptr)transform1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetPosition()));
			Assert::AreEqual(true, XMVector4NearEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(270)), transform2->GetLocalRotation(), tolerance));
			Assert::AreEqual(true, XMVector4NearEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetRotation(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(3, 4, 5, 0), transform2->GetLocalScale(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(3, 4, 5, 0), transform2->GetScale(), tolerance));
			Assert::AreEqual((uptr)transform1, (uptr)transform2->GetParent());
			Assert::AreEqual((size_t)0, transform2->GetChildCount());
			Assert::IsNull(transform2->GetChild(0));
		}

		TEST_METHOD(SetParent4)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform(XMVectorZero(), XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), XMVectorSet(3, 4, 5, 0));


			transform1->SetLocalScale(0.5, 1, 2);


			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 1, 2, 0), transform1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 1, 2, 0), transform1->GetScale()));
			Assert::IsNull(transform1->GetParent());
			Assert::AreEqual((size_t)0, transform1->GetChildCount());
			Assert::IsNull(transform1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), transform2->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), transform2->GetScale()));
			Assert::IsNull(transform2->GetParent());
			Assert::AreEqual((size_t)0, transform2->GetChildCount());
			Assert::IsNull(transform2->GetChild(0));


			transform2->SetParent(transform1);


			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 1, 2, 0), transform1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 1, 2, 0), transform1->GetScale()));
			Assert::IsNull(transform1->GetParent());
			Assert::AreEqual((size_t)1, transform1->GetChildCount());
			Assert::AreEqual((uptr)transform2, (uptr)transform1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetRotation()));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(6, 2, 5, 0), transform2->GetLocalScale(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(3, 4, 5, 0), transform2->GetScale(), tolerance));
			Assert::AreEqual((uptr)transform1, (uptr)transform2->GetParent());
			Assert::AreEqual((size_t)0, transform2->GetChildCount());
			Assert::IsNull(transform2->GetChild(0));
		}

		TEST_METHOD(SetParent5)
		{
			Transform* transform1 = new Transform(XMVectorSet(10, 20, 30, 0), XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), XMVectorSet(0.5, 1, 2, 0));
			Transform* transform2 = new Transform(XMVectorZero(), XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), XMVectorSet(3, 4, 5, 0));


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), transform1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), transform1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), transform1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 1, 2, 0), transform1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 1, 2, 0), transform1->GetScale()));
			Assert::IsNull(transform1->GetParent());
			Assert::AreEqual((size_t)0, transform1->GetChildCount());
			Assert::IsNull(transform1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), transform2->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(3, 4, 5, 0), transform2->GetScale()));
			Assert::IsNull(transform2->GetParent());
			Assert::AreEqual((size_t)0, transform2->GetChildCount());
			Assert::IsNull(transform2->GetChild(0));


			transform2->SetParent(transform1);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), transform1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), transform1->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), transform1->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 1, 2, 0), transform1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 1, 2, 0), transform1->GetScale()));
			Assert::IsNull(transform1->GetParent());
			Assert::AreEqual((size_t)1, transform1->GetChildCount());
			Assert::AreEqual((uptr)transform2, (uptr)transform1->GetChild(0));

			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(60, -20, -5, 0), transform2->GetLocalPosition(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorZero(), transform2->GetPosition(), tolerance));
			Assert::AreEqual(true, XMVector4NearEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(270)), transform2->GetLocalRotation(), tolerance));
			Assert::AreEqual(true, XMVector4NearEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180)), transform2->GetRotation(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(1.5, 8, 5, 0), transform2->GetLocalScale(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(3, 4, 5, 0), transform2->GetScale(), tolerance));
			Assert::AreEqual((uptr)transform1, (uptr)transform2->GetParent());
			Assert::AreEqual((size_t)0, transform2->GetChildCount());
			Assert::IsNull(transform2->GetChild(0));
		}

		TEST_METHOD(Translate1)
		{
			Transform* transform = new Transform();


			transform->Translate(10, 20, 30, WORLD);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 20, 30, 0), transform->GetLocalPosition()));
		}

		TEST_METHOD(Translate2)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform1->SetLocalRotation(0, 90, 0);
			transform2->Translate(10, 20, 30, WORLD);


			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(-30, 20, 10, 0), transform2->GetLocalPosition(), tolerance));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(10, 20, 30, 0), transform2->GetPosition(), tolerance));
		}

		TEST_METHOD(Rotate1)
		{
			Transform* transform = new Transform();


			transform->Rotate(90, 180, 270, WORLD);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(270)), transform->GetLocalRotation()));
		}

		TEST_METHOD(Rotate2)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform1->SetLocalRotation(0, 90, 0);
			transform2->Rotate(90, 180, 270, WORLD);


			Assert::AreEqual(true, XMVector4NearEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(-90), XMConvertToRadians(90)), transform2->GetLocalRotation(), tolerance));
			Assert::AreEqual(true, XMVector4NearEqual(XMQuaternionMultiply(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(270))), transform2->GetRotation(), tolerance));
		}
	};
}
