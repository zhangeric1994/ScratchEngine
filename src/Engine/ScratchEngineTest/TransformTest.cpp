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
			XMVECTOR v;


			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform1->SetLocalPosition(10, 10, 10);

			transform2->SetParent(transform1);
			transform2->SetLocalPosition(0, 0, 0);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), transform1->GetPosition()));

			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), transform2->GetPosition()), L"GetPosition() did not return the correct value when there is no local transformation.");
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
			transform2->SetPosition(10, 10, 10);


			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(-10, 10, 10, 0), transform2->GetLocalPosition(), 20 * g_XMEpsilon));
		}

		TEST_METHOD(SetPosition5)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform1->SetLocalScale(2, 2, 2);
			transform2->SetPosition(10, 10, 10);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(5, 5, 5, 0), transform2->GetLocalPosition()));
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

			transform2->SetRotation(0, 90, 0);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionIdentity(), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), transform2->GetLocalRotation()));
		}

		TEST_METHOD(SetRotation3)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform1->SetLocalPosition(10, 10, 10);
			transform2->SetRotation(0, 90, 0);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionIdentity(), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), transform2->GetLocalRotation()));
		}

		TEST_METHOD(SetRotation4)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform1->SetLocalRotation(0, 90, 0);
			transform2->SetRotation(90, 180, 90);

			XMVECTOR q = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(180), XMConvertToRadians(180));
			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMQuaternionEqual(q, transform2->GetLocalRotation()));
		}

		TEST_METHOD(SetRotation5)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform1->SetLocalScale(2, 2, 2);
			transform2->SetRotation(0, 90, 0);


			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionIdentity(), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMQuaternionEqual(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(90), XMConvertToRadians(0)), transform2->GetLocalRotation()));
		}

		TEST_METHOD(SetScale1)
		{
			Transform* transform = new Transform();


			transform->SetScale(2, 2, 2);

			
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 2, 2, 0), transform->GetLocalScale()));
		}

		TEST_METHOD(SetScale2)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform2->SetScale(2, 2, 2);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 2, 2, 0), transform2->GetLocalScale()));
		}

		TEST_METHOD(SetScale3)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform1->SetLocalPosition(10, 10, 10);
			transform2->SetScale(2, 2, 2);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 2, 2, 0), transform2->GetLocalScale()));
		}

		TEST_METHOD(SetScale4)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform1->SetLocalRotation(0, 90, 0);
			transform2->SetScale(2, 2, 2);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 2, 2, 0), transform2->GetLocalScale()));
		}

		TEST_METHOD(SetScale5)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);

			transform1->SetLocalScale(1, 0.5, 1);
			transform2->SetScale(2, 2, 2);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 0.5, 1, 0), transform1->GetLocalScale()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 4, 2, 0), transform2->GetLocalScale()));
		}

		TEST_METHOD(SetParent1)
		{
			Transform* transform1 = new Transform();
			Transform* transform2 = new Transform();


			transform2->SetParent(transform1);


			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform1->GetLocalScale()));

			Assert::IsNull(transform1->GetParent());

			Assert::AreEqual((size_t)1, transform1->GetChildCount());
			Assert::AreEqual((uptr)transform2, (uptr)transform1->GetChild(0));

			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform2->GetLocalScale()));

			Assert::AreEqual(true, XMVector3Equal(XMVectorZero(), transform2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform2->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(1, 1, 1, 0), transform2->GetScale()));

			Assert::AreEqual((uptr)transform1, (uptr)transform2->GetParent());

			Assert::AreEqual((size_t)0, transform2->GetChildCount());
			Assert::IsNull(transform2->GetChild(0));
		}

		TEST_METHOD(SetParent2)
		{
			Transform* transform1 = new Transform(XMVectorSet(-10, 0, 0, 0), XMQuaternionRotationRollPitchYaw(0, 90, 0), XMVectorSet(0.5, 0.5, 0.5, 0));
			Transform* transform2 = new Transform(XMVectorSet(10, 10, 10, 0), XMQuaternionRotationRollPitchYaw(0, 90, 0), XMVectorSet(2, 2, 2, 0));


			transform2->SetParent(transform1);


			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(-10, 0, 0, 0), transform1->GetLocalPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(0, 90, 0), transform1->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(0.5, 0.5, 0.5, 0), transform1->GetLocalScale()));

			Assert::IsNull(transform1->GetParent());

			Assert::AreEqual((size_t)1, transform1->GetChildCount());
			Assert::AreEqual((uptr)transform2, (uptr)transform1->GetChild(0));

			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(-20, 20, 40, 0), transform2->GetLocalPosition(), 20 * g_XMEpsilon), L"GetLocalPosition() did not return the correct value on child transform.");
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionIdentity(), transform2->GetLocalRotation()));
			Assert::AreEqual(true, XMVector3NearEqual(XMVectorSet(4, 4, 4, 0), transform2->GetLocalScale(), 20 * g_XMEpsilon));

			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(10, 10, 10, 0), transform2->GetPosition()));
			Assert::AreEqual(true, XMVector3Equal(XMQuaternionRotationRollPitchYaw(0, 90, 0), transform2->GetRotation()));
			Assert::AreEqual(true, XMVector3Equal(XMVectorSet(2, 2, 2, 0), transform2->GetScale()));

			Assert::AreEqual((uptr)transform1, (uptr)transform2->GetParent());

			Assert::AreEqual((size_t)0, transform2->GetChildCount());
			Assert::IsNull(transform2->GetChild(0));
		}
	};
}
