#pragma once
#include <queue>
#include <bitset>
#include <optional>


class KeyBoard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
			invalid,
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event() noexcept
			:
			type(Type::invalid),
			code(0u)
		{}
		Event(Type type, unsigned char code) noexcept
			:
			type(type),
			code(code)
		{}
		bool IsPress() const noexcept
		{
			return type == Type::Press;
		}
		bool IsRelease() const noexcept
		{
			return type == Type::Release;
		}
		bool IsValid() const noexcept
		{
			return type != Type::invalid;
		}
		unsigned char GetCode() const noexcept
		{
			return code;
		}
	};
	KeyBoard() = default;
	KeyBoard( const KeyBoard& ) = delete;
	KeyBoard& operator=( const KeyBoard& ) = delete;
	// Key inputs
	bool KeyIsPressed(unsigned char keycode) const noexcept;
	Event ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void FlushKey() noexcept;
	// Char inputs
	char ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;
	// auto reapeater enable/disable
	void EnableAutoRepeat() noexcept;
	void DisableAutoRepeat() noexcept;
	bool AutoReapeatIsEnabled() const noexcept;
private:
	void OnKeyPressed( unsigned char keycode ) noexcept;
	void OnKeyReleased( unsigned char keycode ) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;
private:
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int BufferSize = 16u;
	bool autorepeatenabled = false;
	std::bitset<nKeys> Keystates;
	std::queue<Event> Keybuffer;
	std::queue<char> Charbuffer;
};

