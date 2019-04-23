//
//  rc4.h
//  rc4
//
//  Created by Duye Chen on 4/8/15.
//

#include <iostream>

#ifndef rc4_rc4_h
#define rc4_rc4_h

class RC4
{
public:
	void ksa(std::string key, int keylength);
	std::string prga(std::string in, int len);
	std::string doRC4(std::string in, std::string key);
private:
	int S[256];
	char k[256];
};

#endif
