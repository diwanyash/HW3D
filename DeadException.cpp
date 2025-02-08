#include "DeadException.h"
#include <sstream>

Deadexception::Deadexception(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{
}

const char* Deadexception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Deadexception::GetType() const noexcept
{
	return "Dead Exception";
}

int Deadexception::GetLine() const noexcept
{
	return line;
}

const std::string& Deadexception::GetFile() const noexcept
{
	return file;
}

std::string Deadexception::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}
