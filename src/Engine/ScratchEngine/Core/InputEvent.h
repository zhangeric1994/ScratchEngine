#pragma once

namespace ScratchEngine 
{
	struct MousePoint
	{
		int x;
		int y;
	};

	class __declspec(dllexport) InputEvent
	{
	public:
		enum EventType
		{
			Down,
			Up,
			LPress,
			LRelease,
			RPress,
			RRelease,
			MPress,
			MRelease,
			MWheelUp,
			MWheelDown,
			MMove,
			RawMove,
			Invalid
		};

		InputEvent();
		InputEvent(const EventType type, const unsigned char key);
		InputEvent(const EventType type, const int x, const int y);

		bool IsDown() const;
		bool IsReleased() const;
		bool IsValid() const;
		unsigned char GetKeyCode() const;

		EventType GetType() const;
		MousePoint GetPos() const;
		int GetPosX() const;
		int GetPosY() const;

	private:
		EventType type;
		int x;
		int y;
		unsigned char key;
	};
}
