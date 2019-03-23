#include "CppUnitTest.h"
#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace ScratchEngine;
using namespace ScratchEngine::Memory;

namespace ScratchEngineTest
{
	//template<class T> __declspec(dllimport) T* AddComponent();
	//template<class T> __declspec(dllimport) T* GetComponent();

	struct TestObject : GameObject
	{
		bool isUpdated;
		int numMessagesReceived;

		void Update()
		{
			isUpdated = true;
		}

		void HandleMessage(const Message& message)
		{
			++numMessagesReceived;
		}
	};

	struct TestComponent1 : GameComponent
	{
		bool isUpdated;
		int numMessagesReceived;

		void Update()
		{
			isUpdated = true;
		}

		void HandleMessage(const Message& message)
		{
			++numMessagesReceived;
		}
	};

	struct TestComponent2 : GameComponent
	{
		bool isUpdated;
		int numMessagesReceived;

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
			TestObject* testObject1 = new TestObject();
			TestObject* testObject2 = new TestObject();
			TestObject* testObject3 = new TestObject();


			testObject2->SetParent(testObject1);


			Assert::AreEqual((uptr)testObject1, (uptr)testObject2->GetParent());

			Assert::AreEqual((size_t)1, testObject1->GetChildCount());
			Assert::AreEqual((uptr)testObject2, (uptr)testObject1->GetChild(0));
		}

		TEST_METHOD(SetParent2)
		{
			TestObject* testObject1 = new TestObject();
			TestObject* testObject2 = new TestObject();
			TestObject* testObject3 = new TestObject();


			testObject2->SetParent(testObject3);
			testObject2->SetParent(testObject1);


			Assert::AreEqual((uptr)testObject1, (uptr)testObject2->GetParent());

			Assert::AreEqual((size_t)1, testObject1->GetChildCount());
			Assert::AreEqual((uptr)testObject2, (uptr)testObject1->GetChild(0));

			Assert::AreEqual((size_t)0, testObject3->GetChildCount());
			Assert::IsNull(testObject3->GetChild(0));
		}

		TEST_METHOD(AddComponent)
		{
			TestObject* testObject = new TestObject();


			TestComponent1* testComponent1 = testObject->AddComponent<TestComponent1>();
			TestComponent2* testComponent2 = testObject->AddComponent<TestComponent2>();


			Assert::AreNotEqual((uptr)testComponent1, (uptr)testComponent2);

			Assert::AreEqual((uptr)testComponent1, (uptr)testObject->GetComponent<TestComponent1>());
			Assert::AreEqual((uptr)testComponent2, (uptr)testObject->GetComponent<TestComponent2>());
		}

		TEST_METHOD(RemoveComponent)
		{
			TestObject* testObject = new TestObject();


			TestComponent1* testComponent1 = testObject->AddComponent<TestComponent1>();
			TestComponent2* testComponent2 = testObject->AddComponent<TestComponent2>();

			testObject->RemoveComponent<TestComponent1>();


			Assert::IsNull(testObject->GetComponent<TestComponent1>());
			Assert::AreEqual((uptr)testComponent2, (uptr)testObject->GetComponent<TestComponent2>());
		}

		TEST_METHOD(SendMessage1)
		{
			TestObject* testObject = new TestObject();


			Message message;
			testObject->SendMessage(message);


			Assert::AreEqual(1, testObject->numMessagesReceived);
		}

		TEST_METHOD(SendMessage2)
		{
			TestObject* testObject = new TestObject();


			TestComponent1* testComponent1 = testObject->AddComponent<TestComponent1>();
			TestComponent2* testComponent2 = testObject->AddComponent<TestComponent2>();

			Message message;
			testObject->SendMessage(message);


			Assert::AreEqual(1, testObject->numMessagesReceived);
			Assert::AreEqual(1, testComponent1->numMessagesReceived);
			Assert::AreEqual(1, testComponent2->numMessagesReceived);
		}

		TEST_METHOD(SendMessage3)
		{
			TestObject* testObject1 = new TestObject();
			TestObject* testObject2 = new TestObject();


			TestComponent1* testComponent11 = testObject1->AddComponent<TestComponent1>();
			TestComponent2* testComponent12 = testObject1->AddComponent<TestComponent2>();
			
			TestComponent1* testComponent21 = testObject2->AddComponent<TestComponent1>();

			testObject2->SetParent(testObject1);

			Message message;
			testObject1->SendMessage(message);


			Assert::AreEqual(1, testObject1->numMessagesReceived);
			Assert::AreEqual(1, testComponent11->numMessagesReceived);
			Assert::AreEqual(1, testComponent12->numMessagesReceived);

			Assert::AreEqual(0, testObject2->numMessagesReceived);
			Assert::AreEqual(0, testComponent21->numMessagesReceived);
		}

		TEST_METHOD(SendMessageDown1)
		{
			TestObject* testObject = new TestObject();


			Message message;
			testObject->SendMessageDown(message);

			Assert::AreEqual(1, testObject->numMessagesReceived);
		}

		TEST_METHOD(SendMessageDown2)
		{
			TestObject* testObject = new TestObject();


			TestComponent1* testComponent1 = testObject->AddComponent<TestComponent1>();
			TestComponent2* testComponent2 = testObject->AddComponent<TestComponent2>();

			Message message;
			testObject->SendMessageDown(message);

			Assert::AreEqual(1, testObject->numMessagesReceived);
			Assert::AreEqual(1, testComponent1->numMessagesReceived);
			Assert::AreEqual(1, testComponent2->numMessagesReceived);
		}

		TEST_METHOD(SendMessageDown3)
		{
			TestObject* testObject1 = new TestObject();
			TestObject* testObject2 = new TestObject();
			TestObject* testObject3 = new TestObject();


			TestComponent1* testComponent11 = testObject1->AddComponent<TestComponent1>();
			TestComponent2* testComponent12 = testObject1->AddComponent<TestComponent2>();
			TestComponent1* testComponent21 = testObject2->AddComponent<TestComponent1>();
			TestComponent1* testComponent31 = testObject3->AddComponent<TestComponent1>();

			testObject2->SetParent(testObject1);
			testObject3->SetParent(testObject2);

			Message message;
			testObject1->SendMessageDown(message);


			Assert::AreEqual(1, testObject1->numMessagesReceived);
			Assert::AreEqual(1, testComponent11->numMessagesReceived);
			Assert::AreEqual(1, testComponent12->numMessagesReceived);

			Assert::AreEqual(1, testObject2->numMessagesReceived);
			Assert::AreEqual(1, testComponent21->numMessagesReceived);

			Assert::AreEqual(1, testObject3->numMessagesReceived);
			Assert::AreEqual(1, testComponent31->numMessagesReceived);
		}

		TEST_METHOD(SendMessageDown4)
		{
			TestObject* testObject1 = new TestObject();
			TestObject* testObject2 = new TestObject();
			TestObject* testObject3 = new TestObject();


			TestComponent1* testComponent11 = testObject1->AddComponent<TestComponent1>();
			TestComponent2* testComponent12 = testObject1->AddComponent<TestComponent2>();
			TestComponent1* testComponent21 = testObject2->AddComponent<TestComponent1>();
			TestComponent1* testComponent31 = testObject3->AddComponent<TestComponent1>();

			testObject2->SetParent(testObject1);
			testObject3->SetParent(testObject2);

			Message message;
			testObject1->SendMessageDown(message, 1);


			Assert::AreEqual(1, testObject1->numMessagesReceived);
			Assert::AreEqual(1, testComponent11->numMessagesReceived);
			Assert::AreEqual(1, testComponent12->numMessagesReceived);

			Assert::AreEqual(1, testObject2->numMessagesReceived);
			Assert::AreEqual(1, testComponent21->numMessagesReceived);
			
			Assert::AreEqual(0, testObject3->numMessagesReceived);
			Assert::AreEqual(0, testComponent31->numMessagesReceived);
		}

		TEST_METHOD(SendMessageUp1)
		{
			TestObject* testObject = new TestObject();


			Message message;
			testObject->SendMessageUp(message);


			Assert::AreEqual(1, testObject->numMessagesReceived);
		}

		TEST_METHOD(SendMessageUp2)
		{
			TestObject* testObject = new TestObject();


			TestComponent1* testComponent1 = testObject->AddComponent<TestComponent1>();
			TestComponent2* testComponent2 = testObject->AddComponent<TestComponent2>();

			Message message;
			testObject->SendMessageUp(message);


			Assert::AreEqual(1, testObject->numMessagesReceived);
			Assert::AreEqual(1, testComponent1->numMessagesReceived);
			Assert::AreEqual(1, testComponent2->numMessagesReceived);
		}

		TEST_METHOD(SendMessageUp3)
		{
			TestObject* testObject1 = new TestObject();
			TestObject* testObject2 = new TestObject();
			TestObject* testObject3 = new TestObject();


			TestComponent1* testComponent11 = testObject1->AddComponent<TestComponent1>();
			TestComponent2* testComponent12 = testObject1->AddComponent<TestComponent2>();
			TestComponent1* testComponent21 = testObject2->AddComponent<TestComponent1>();
			TestComponent1* testComponent31 = testObject3->AddComponent<TestComponent1>();

			testObject2->SetParent(testObject1);
			testObject3->SetParent(testObject2);

			Message message;
			testObject3->SendMessageUp(message);


			Assert::AreEqual(1, testObject2->numMessagesReceived);
			Assert::AreEqual(1, testComponent21->numMessagesReceived);

			Assert::AreEqual(1, testObject1->numMessagesReceived);
			Assert::AreEqual(1, testComponent11->numMessagesReceived);
			Assert::AreEqual(1, testComponent12->numMessagesReceived);
		}

		TEST_METHOD(SendMessageUp4)
		{
			TestObject* testObject1 = new TestObject();
			TestObject* testObject2 = new TestObject();
			TestObject* testObject3 = new TestObject();


			TestComponent1* testComponent11 = testObject1->AddComponent<TestComponent1>();
			TestComponent2* testComponent12 = testObject1->AddComponent<TestComponent2>();
			TestComponent1* testComponent21 = testObject2->AddComponent<TestComponent1>();
			TestComponent1* testComponent31 = testObject3->AddComponent<TestComponent1>();

			testObject2->SetParent(testObject1);
			testObject3->SetParent(testObject2);

			Message message;
			testObject3->SendMessageUp(message, 1);


			Assert::AreEqual(1, testObject2->numMessagesReceived);
			Assert::AreEqual(1, testComponent21->numMessagesReceived);

			Assert::AreEqual(0, testObject1->numMessagesReceived);
			Assert::AreEqual(0, testComponent11->numMessagesReceived);
			Assert::AreEqual(0, testComponent12->numMessagesReceived);
		}
	};
}
