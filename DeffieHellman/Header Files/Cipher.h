#pragma once
#include <string>
#include "sha1.h"
#include "RC4.h"

namespace cipher
{
	class DeffieHellman
	{
	public:
		static std::string encrypt(std::string _inputStr, std::string pasword);
		static std::string decrypt(std::string _inputStr, std::string pasword);
		static std::string SHA1(const std::string &string)
		{
			return sha1(string);
		}
	};
}
