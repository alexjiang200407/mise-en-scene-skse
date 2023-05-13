#include "err.h"

MES::MESErr::MESErr(int line, const char* file)
	:
	line(line), file(file)
{
}

const char* MES::MESErr::what() const
{
	std::ostringstream oss;
	oss << GetType() << '\n'
		<< GetOrigin();

	logger::error("SHiet");

	return oss.str().c_str();
}

const char* MES::MESErr::GetType() const
{
	return "MES error!";
}

int MES::MESErr::GetLine() const
{
	return line;
}

const std::string& MES::MESErr::GetFile() const
{
	return file;
}

std::string MES::MESErr::GetOrigin() const
{
	std::ostringstream oss;
	oss << "File: " << GetFile() << '\n'
		<< "Line: " << GetLine() << '\n';


	return oss.str();
}
