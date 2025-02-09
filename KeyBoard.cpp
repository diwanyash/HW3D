#include "KeyBoard.h"

bool KeyBoard::KeyIsPressed(unsigned char keycode) const noexcept
{
    return Keystates[keycode];
}

KeyBoard::Event KeyBoard::ReadKey() noexcept
{
    if (Keybuffer.size() > 0u)
    {
        KeyBoard::Event e = Keybuffer.front();
        Keybuffer.pop();
        return e;
    }
    else
    {
        return KeyBoard::Event();
    }
}

bool KeyBoard::KeyIsEmpty() const noexcept
{
    return Keybuffer.empty();
}

void KeyBoard::FlushKey() noexcept
{
    Keybuffer = std::queue<Event>();
}

char KeyBoard::ReadChar() noexcept
{
    if (Charbuffer.size() > 0u)
    {
        unsigned char charcode = Charbuffer.front();
        Charbuffer.pop();
        return charcode;
    }
    else
    {
        return 0;
    }
}

bool KeyBoard::CharIsEmpty() const noexcept
{
    return Charbuffer.empty();
}

void KeyBoard::FlushChar() noexcept
{
    Charbuffer = std::queue<char>();
}

void KeyBoard::Flush() noexcept
{
    FlushKey();
    FlushChar();
}

void KeyBoard::EnableAutoRepeat() noexcept
{
    autorepeatenabled = true;
}

void KeyBoard::DisableAutoRepeat() noexcept
{
    autorepeatenabled = false;
}

bool KeyBoard::AutoReapeatIsEnabled() const noexcept
{
    return autorepeatenabled;
}

void KeyBoard::OnKeyPressed(unsigned char keycode) noexcept
{
    Keystates[keycode] = true;
    Keybuffer.push(KeyBoard::Event(KeyBoard::Event::Type::Press, keycode));
    TrimBuffer(Keybuffer);
}

void KeyBoard::OnKeyReleased(unsigned char keycode) noexcept
{
    Keystates[keycode] = false;
    Keybuffer.push(KeyBoard::Event(KeyBoard::Event::Type::Release, keycode));
    TrimBuffer(Keybuffer);
}

void KeyBoard::OnChar(char character) noexcept
{
    Charbuffer.push(character);
    TrimBuffer(Charbuffer);
}

void KeyBoard::ClearState() noexcept
{
    Keystates.reset();
}

template<typename T>
inline void KeyBoard::TrimBuffer(std::queue<T>& buffer) noexcept
{
    while (buffer.size() > BufferSize)
    {
        buffer.pop();
    }
}
