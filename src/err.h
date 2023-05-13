#pragma once
#include <exception>
#include <string>
#include <sstream>
#include "logger.h"

namespace MES
{
	class MESErr
		:
		public std::exception
	{
	public:
		MESErr(int line, const char* file);
	public:
		const char* what() const override;

		virtual const char* GetType() const;

		int GetLine() const;

		const std::string& GetFile() const;

		std::string GetOrigin() const;
	
	private:
		int line;
		std::string file;
	};
}