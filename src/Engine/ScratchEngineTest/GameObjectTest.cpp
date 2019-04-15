#include "CppUnitTest.h"
#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace ScratchEngine;
using namespace ScratchEngine::Memory;

namespace ScratchEngineTest
{
	//template<class T> __declspec(dllimport) T* AddComponent();
	//template<class T> __declspec(dllimport) T* GetComponent();

	struct TestComponent1 : public GameComponent
	{
		int data;
		bool isUpdated;
		int numMessagesReceived;

		TestComponent1()
		{
			data = 0;
			isUpdated = false;
			numMessagesReceived = 0;
		}

		TestComponent1(int data)
		{
			this->data = data;
			isUpdated = false;
			numMessagesReceived = 0;
		}

		void Update()
		{
			isUpdated = true;
		}

		void HandleMessage(const Message& message)
		{
			++numMessagesReceived;
		}
	};

	struct TestComponent2 : public GameComponent
	{
		int data;
		bool isUpdated;
		int numMessagesReceived;

		TestComponent2()
		{
			data = 0;
			isUpdated = false;
			numMessagesReceived = 0;
		}

		TestComponent2(int data)
		{
			this->data = data;
			isUpdated = false;
			numMessagesReceived = 0;
		}

		void Update()
		{
			isUpdated = true;
		}

		void HandleMessage(const Message& message)
		{
			++numMessagesReceived;
		}
	};

	TEST_CLASS(GameObjectTest)
	{
	public:
		TEST_METHOD(SetParent1)
		{
			GameObject* testObject1 = new GameObject();
			GameObject* testObject2 = new GameObject();
			GameObject* testObject3 = new GameObject();


			testObject2->SetParent(testObject1);


			Assert::AreEqual((uptr)testObject1, (uptr)testObject2->GetParent());

			Assert::AreEqual((size_t)1, testObject1->GetChildCount());
			Assert::AreEqual((uptr)testObject2, (uptr)testObject1->GetChild(0));
		}

		TEST_METHOD(SetParent2)
		{
			GameObject* testObject1 = new GameObject();
			GameObject* testObject2 = new GameObject();
			GameObject* testObject3 = new GameObject();


			testObject2->SetParent(testObject3);
			testObject2->SetParent(testObject1);


			Assert::AreEqual((uptr)testObject1, (uptr)testObject2->GetParent());

			Assert::AreEqual((size_t)1, testObject1->GetChildCount());
			Assert::AreEqual((uptr)testObject2, (uptr)testObject1->GetChild(0));

			Assert::AreEqual((size_t)0, testObject3->GetChildCount());
			Assert::IsNull(testObject3->GetChild(0));
		}

		TEST_METHOD(AddComponent1)
		{
			GameObject* gameObject = new GameObject();


			TestComponent1* testComponent1 = gameObject->AddComponent<TestComponent1>();
			TestComponent2* testComponent2 = gameObject->AddComponent<TestComponent2>();


			Assert::AreNotEqual((uptr)testComponent1, (uptr)testComponent2);

			Assert::AreEqual((uptr)testComponent1, (uptr)gameObject->GetComponent<TestComponent1>());
			Assert::AreEqual((uptr)testComponent2, (uptr)gameObject->GetComponent<TestComponent2>());

			Assert::AreEqual((uptr)gameObject, (uptr)testComponent1->GetGameObject());
			Assert::AreEqual(0, testComponent1->data);
			Assert::AreEqual(false, testComponent1->isUpdated);
			Assert::AreEqual(0, testComponent1->numMessagesReceived);

			Assert::AreEqual((uptr)gameObject, (uptr)testComponent2->GetGameObject());
			Assert::AreEqual(0, testComponent2->data);
			Assert::AreEqual(false, testComponent2->isUpdated);
			Assert::AreEqual(0, testComponent2->numMessagesReceived);
		}

		TEST_METHOD(AddComponent2)
		{
			GameObject* gameObject = new GameObject();


			TestComponent1* testComponent1 = gameObject->AddComponent<TestComponent1>(1);
			TestComponent2* testComponent2 = gameObject->AddComponent<TestComponent2>(2);


			Assert::AreNotEqual((uptr)testComponent1, (uptr)testComponent2);

			Assert::AreEqual((uptr)testComponent1, (uptr)gameObject->GetComponent<TestComponent1>());
			Assert::AreEqual((uptr)testComponent2, (uptr)gameObject->GetComponent<TestComponent2>());

			Assert::AreEqual((uptr)gameObject, (uptr)testComponent1->GetGameObject());
			Assert::AreEqual(1, testComponent1->data);
			Assert::AreEqual(false, testComponent1->isUpdated);
			Assert::AreEqual(0, testComponent1->numMessagesReceived);

			Assert::AreEqual((uptr)gameObject, (uptr)testComponent2->GetGameObject());
			Assert::AreEqual(2, testComponent2->data);
			Assert::AreEqual(false, testComponent2->isUpdated);
			Assert::AreEqual(0, testComponent2->numMessagesReceived);
		}

		TEST_METHOD(RemoveComponent)
		{
			GameObject* gameObject = new GameObject();


			TestComponent1* testComponent1 = gameObject->AddComponent<TestComponent1>();
			TestComponent2* testComponent2 = gameObject->AddComponent<TestComponent2>();

			gameObject->RemoveComponent<TestComponent1>();


			Assert::IsNull(gameObject->GetComponent<TestComponent1>());
			Assert::AreEqual((uptr)testComponent2, (uptr)gameObject->GetComponent<TestComponent2>());

			Assert::AreEqual((uptr)gameObject, (uptr)testComponent2->GetGameObject());
		}

		TEST_METHOD(SendMessage1)
		{
			GameObject* gameObject = new GameObject();


			Message message;
			gameObject->SendMessage_(message);
		}

		TEST_METHOD(SendMessage2)
		{
			GameObject* gameObject = new GameObject();


			TestComponent1* testComponent1 = gameObject->AddComponent<TestComponent1>();
			TestComponent2* testComponent2 = gameObject->AddComponent<TestComponent2>();

			Message message;
			gameObject->SendMessage_(message);


			Assert::AreEqual(1, testComponent1->numMessagesReceived);
			Assert::AreEqual(1, testComponent2->numMessagesReceived);
		}

		TEST_METHOD(SendMessage3)
		{
			GameObject* testObject1 = new GameObject();
			GameObject* testObject2 = new GameObject();


			TestComponent1* testComponent11 = testObject1->AddComponent<TestComponent1>();
			TestComponent2* testComponent12 = testObject1->AddComponent<TestComponent2>();
			
			TestComponent1* testComponent21 = testObject2->AddComponent<TestComponent1>();

			testObject2->SetParent(testObject1);

			Message message;
			testObject1->SendMessage_(message);


			Assert::AreEqual(1, testComponent11->numMessagesReceived);
			Assert::AreEqual(1, testComponent12->numMessagesReceived);

			Assert::AreEqual(0, testComponent21->numMessagesReceived);
		}

		TEST_METHOD(SendMessageDown1)
		{
			GameObject* gameObject = new GameObject();


			Message message;
			gameObject->SendMessageDown(message);
		}

		TEST_METHOD(SendMessageDown2)
		{
			GameObject* gameObject = new GameObject();


			TestComponent1* testComponent1 = gameObject->AddComponent<TestComponent1>();
			TestComponent2* testComponent2 = gameObject->AddComponent<TestComponent2>();

			Message message;
			gameObject->SendMessageDown(message);

			Assert::AreEqual(1, testComponent1->numMessagesReceived);
			Assert::AreEqual(1, testComponent2->numMessagesReceived);
		}

		TEST_METHOD(SendMessageDown3)
		{
			GameObject* testObject1 = new GameObject();
			GameObject* testObject2 = new GameObject();
			GameObject* testObject3 = new GameObject();


			TestComponent1* testComponent11 = testObject1->AddComponent<TestComponent1>();
			TestComponent2* testComponent12 = testObject1->AddComponent<TestComponent2>();
			TestComponent1* testComponent21 = testObject2->AddComponent<TestComponent1>();
			TestComponent1* testComponent31 = testObject3->AddComponent<TestComponent1>();

			testObject2->SetParent(testObject1);
			testObject3->SetParent(testObject2);

			Message message;
			testObject1->SendMessageDown(message);


			Assert::AreEqual(1, testComponent11->numMessagesReceived);
			Assert::AreEqual(1, testComponent12->numMessagesReceived);

			Assert::AreEqual(1, testComponent21->numMessagesReceived);

			Assert::AreEqual(1, testComponent31->numMessagesReceived);
		}

		TEST_METHOD(SendMessageDown4)
		{
			GameObject* testObject1 = new GameObject();
			GameObject* testObject2 = new GameObject();
			GameObject* testObject3 = new GameObject();


			TestComponent1* testComponent11 = testObject1->AddComponent<TestComponent1>();
			TestComponent2* testComponent12 = testObject1->AddComponent<TestComponent2>();
			TestComponent1* testComponent21 = testObject2->AddComponent<TestComponent1>();
			TestComponent1* testComponent31 = testObject3->AddComponent<TestComponent1>();

			testObject2->SetParent(testObject1);
			testObject3->SetParent(testObject2);

			Message message;
			testObject1->SendMessageDown(message, 1);


			Assert::AreEqual(1, testComponent11->numMessagesReceived);
			Assert::AreEqual(1, testComponent12->numMessagesReceived);

			Assert::AreEqual(1, testComponent21->numMessagesReceived);
			
			Assert::AreEqual(0, testComponent31->numMessagesReceived);
		}

		TEST_METHOD(SendMessageUp1)
		{
			GameObject* gameObject = new GameObject();


			Message message;
			gameObject->SendMessageUp(message);


		}

		TEST_METHOD(SendMessageUp2)
		{
			GameObject* gameObject = new GameObject();


			TestComponent1* testComponent1 = gameObject->AddComponent<TestComponent1>();
			TestComponent2* testComponent2 = gameObject->AddComponent<TestComponent2>();

			Message message;
			gameObject->SendMessageUp(message);


			Assert::AreEqual(1, testComponent1->numMessagesReceived);
			Assert::AreEqual(1, testComponent2->numMessagesReceived);
		}

		TEST_METHOD(SendMessageUp3)
		{
			GameObject* testObject1 = new GameObject();
			GameObject* testObject2 = new GameObject();
			GameObject* testObject3 = new GameObject();


			TestComponent1* testComponent11 = testObject1->AddComponent<TestComponent1>();
			TestComponent2* testComponent12 = testObject1->AddComponent<TestComponent2>();
			TestComponent1* testComponent21 = testObject2->AddComponent<TestComponent1>();
			TestComponent1* testComponent31 = testObject3->AddComponent<TestComponent1>();

			testObject2->SetParent(testObject1);
			testObject3->SetParent(testObject2);

			Message message;
			testObject3->SendMessageUp(message);


			Assert::AreEqual(1, testComponent21->numMessagesReceived);

			Assert::AreEqual(1, testComponent11->numMessagesReceived);
			Assert::AreEqual(1, testComponent12->numMessagesReceived);
		}

		TEST_METHOD(SendMessageUp4)
		{
			GameObject* testObject1 = new GameObject();
			GameObject* testObject2 = new GameObject();
			GameObject* testObject3 = new GameObject();


			TestComponent1* testComponent11 = testObject1->AddComponent<TestComponent1>();
			TestComponent2* testComponent12 = testObject1->AddComponent<TestComponent2>();
			TestComponent1* testComponent21 = testObject2->AddComponent<TestComponent1>();
			TestComponent1* testComponent31 = testObject3->AddComponent<TestComponent1>();

			testObject2->SetParent(testObject1);
			testObject3->SetParent(testObject2);

			Message message;
			testObject3->SendMessageUp(message, 1);


			Assert::AreEqual(1, testComponent21->numMessagesReceived);

			Assert::AreEqual(0, testComponent11->numMessagesReceived);
			Assert::AreEqual(0, testComponent12->numMessagesReceived);
		}
	};
}
