#pragma once
#include <utility>
#include <queue>

class Mouse {
	friend class Window;
public:
	class Event {
	public:
		enum class Type
		{
			LPress,
			RPress,
			RRelease,
			LRelease,
			Move,
			WheelUp,
			WheelDown,
			Enter,
			Leave,
			Invalid
		};
	private:
		Type type;
		int x;
		int y;
		bool IsLeftPressed;
		bool IsRightPressed;
	public:
		Event() noexcept
			:
			type(Type::Invalid),
			x(0),
			y(0),
			IsLeftPressed(false),
			IsRightPressed(false)
		{}
		Event(Type type, const Mouse& parent) noexcept
			:
			type(type),
			x(parent.x),
			y(parent.y),
			IsLeftPressed(parent.IsLeftPressed),
			IsRightPressed(parent.IsRightPressed)
		{}
		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}
		Type GetType() const noexcept
		{
			return type;
		}
		std::pair<int, int> GetPos() const noexcept
		{
			return {x,y};
		}
		int GetPosX() const noexcept
		{
			return x;
		}
		int GetPosY() const noexcept
		{
			return y;
		}
		bool LeftIsPressed() const noexcept
		{
			return IsLeftPressed;
		}
		bool RightIsPressed() const noexcept
		{
			return IsRightPressed;
		}
	};
public:
	Mouse () = default;
	Mouse (const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept; 
	int GetPosY() const noexcept;
	bool IsEmpty() const noexcept
	{
		return buffer.empty();
	}
	bool IsInWindow() const noexcept;
	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	Mouse::Event Read() noexcept;
	void Flush() noexcept;
private:
	void OnMouseMove(int x, int y) noexcept;
	void OnLeftPressed(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void TrimBuffer() noexcept;
private:
	static constexpr unsigned char bufferSize = 16u;
	int x;
	int y;
	bool IsLeftPressed = false;
	bool IsRightPressed = false;
	bool isInWindow = false;
	std::queue<Event> buffer;
};