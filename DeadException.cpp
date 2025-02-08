#include "DeadException.h"
#include <sstream>

Deadexceptiom::Deadexceptiom(int line, const char* file) noexcept
	:
	line(GetLine()),
	file(GetFile())
{
}

const char* Deadexceptiom::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Deadexceptiom::GetType() const noexcept
{
	return "Dead Exception";
}

int Deadexceptiom::GetLine() const noexcept
{
	return line;
}

const std::string& Deadexceptiom::GetFile() const noexcept
{
	return file;
}

std::string Deadexceptiom::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}
