#pragma once

class Color
{
public:
	constexpr Color() : value() {}
	constexpr Color(const Color& col) : value(col.value)
	{}
	constexpr Color(unsigned char r, unsigned char g, unsigned char b)
		:
		value((r << 16u) | (g << 8u) | b)
	{}
	Color& operator =(Color color)
	{
		value = color.value;
		return *this;
	}
	bool operator==(Color rhs)
	{
		return value == rhs.value;
	}
	bool operator!=(Color rhs)
	{
		return value != rhs.value;
	}
	constexpr unsigned char GetX() const
	{
		return value >> 24u;
	}
	constexpr unsigned char GetR() const
	{
		return (value >> 16u) & 0xFFu;
	}
	constexpr unsigned char GetG() const
	{
		return (value >> 8u) & 0xFFu;
	}
	constexpr unsigned char GetB() const
	{
		return value & 0xFFu;
	}
	void SetR(unsigned char r)
	{
		value = (value & 0xFF00FFFFu) | (r << 16u);
	}
	void SetG(unsigned char g)
	{
		value = (value & 0xFFFF00FFu) | (g << 8u);
	}
	void SetB(unsigned char b)
	{
		value = (value & 0xFFFFFF00u) | b;
	}
private:
	unsigned int value;
};

namespace Colors
{
	static constexpr Color Red =		{	255u,	0u,		0u};
	static constexpr Color Green =		{	0u,		255u,	0u};
	static constexpr Color Blue =		{	0u,		0u,		255u};
	static constexpr Color White =		{	255u,	255u,	255u};
	static constexpr Color Black =		{	0u,		0u,		0u};
										
	static constexpr Color Yellow =		{	255u,	255u,	0u};
	static constexpr Color Orange =		{	255u,	160u,	16u};
	static constexpr Color Gold =		{	255u,	215u,	0u};
	static constexpr Color Cyne =		{	0u,		255u,	255u};
	static constexpr Color Pink =		{	255u,	96u,	208u};
	static constexpr Color PalePink =	{	255u,	208u,	160u};
	static constexpr Color Magenta =	{	255u,	0u,		255u};
	static constexpr Color LightBlue =	{	80u,	208u,	255u};
}