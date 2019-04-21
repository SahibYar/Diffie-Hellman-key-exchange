#pragma once
#include <string>

namespace cipher
{
	class DeffieHellman
	{
	public:
		static std::string encrypt(std::string _inputStr, int _offset);
		static std::string decrypt(std::string _inputStr, int _offset);
		static char bound(char _charToCheck, char _left, char _right);
	};
}
