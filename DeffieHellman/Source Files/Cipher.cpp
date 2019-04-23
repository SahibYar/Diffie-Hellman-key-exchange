#include "cipher.h"
#include <ctype.h>

namespace cipher
{
	std::string DeffieHellman::encrypt(std::string _inputStr, std::string password) {
		RC4 rc4;
		_inputStr.shrink_to_fit();
		password.shrink_to_fit();
		return rc4.doRC4(_inputStr, password);
	}

	std::string DeffieHellman::decrypt(std::string _inputStr, std::string password) {
		RC4 rc4;
		_inputStr.shrink_to_fit();
		password.shrink_to_fit();
		return rc4.doRC4(_inputStr, password);
	}
}
