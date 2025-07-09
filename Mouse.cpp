#include "Mouse.h"
#include "DeadWin.h"

std::pair<int, int> Mouse::GetPos() const noexcept
{
    return { x, y };
}

std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() noexcept
{
    if ( rawDeltaBuffer.empty() )
    {
        return std::nullopt;
    }
    const RawDelta d = rawDeltaBuffer.front();
    rawDeltaBuffer.pop();
    return d;
}

int Mouse::GetPosX() const noexcept
{
    return x;
}

int Mouse::GetPosY() const noexcept
{
    return y;
}

bool Mouse::IsInWindow() const noexcept
{
    return isInWindow;
}

bool Mouse::LeftIsPressed() const noexcept
{
    return IsLeftPressed;
}

bool Mouse::RightIsPressed() const noexcept
{
    return IsRightPressed;
}

Mouse::Event Mouse::Read() noexcept
{
    if (buffer.size() > 0u)
    {
        Mouse::Event e = buffer.front();
        buffer.pop();
        return e;
    }
    return Mouse::Event();
}

void Mouse::Flush() noexcept
{
    buffer = std::queue<Event>();
}

void Mouse::EnableRaw() noexcept
{
    rawEnabled = true;
}

void Mouse::DisableRaw() noexcept
{
    rawEnabled = false;
}

bool Mouse::RawEnabled() const noexcept
{
    return rawEnabled;
}

void Mouse::OnMouseMove(int newX, int newY) noexcept
{
    x = newX;
    y = newY;

    buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
    TrimBuffer();
}

void Mouse::OnLeftPressed(int x, int y) noexcept
{
    IsLeftPressed = true;

    buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
    TrimBuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
    IsRightPressed = true;

    buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
    TrimBuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
    IsLeftPressed = false;

    buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
    TrimBuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
    IsRightPressed = false;

    buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
    TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
    isInWindow = false;

    buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
    TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
    isInWindow = true;

    buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
    TrimBuffer();
}

void Mouse::OnRawDelta(int dx, int dy) noexcept
{
    rawDeltaBuffer.push( { dx, dy } );
    TrimBuffer();
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
    buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
    TrimBuffer();
}

void Mouse::OnWheelDown(int x, int y) noexcept
{
    buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
    TrimBuffer();
}

void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
    WheelDeltaCarry += delta;
    // ver WHEEL_DELTA is 120
    while (WheelDeltaCarry >= WHEEL_DELTA)
    {
        WheelDeltaCarry -= WHEEL_DELTA;
        OnWheelUp(x, y);
    }
    while (WheelDeltaCarry <= -WHEEL_DELTA)
    {
        WheelDeltaCarry += WHEEL_DELTA;
        OnWheelDown(x, y);
    }
}

void Mouse::TrimBuffer() noexcept
{
    while (buffer.size() > bufferSize)
    {
        buffer.pop();
    }
}

void Mouse::TrimRawInputBuffer() noexcept
{
    while (rawDeltaBuffer.size() > bufferSize)
    {
        rawDeltaBuffer.pop();
    }
}

