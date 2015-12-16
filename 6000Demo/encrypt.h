#pragma once

int base64_encode(unsigned char* outputBuf, unsigned int* outputLen,
	unsigned char* inputBuf, unsigned int inputLen);

int base64_decode(unsigned char* outputBuf, unsigned int* outputLen,
	unsigned char* inputBuf, unsigned int inputLen);
int EncryptStrToMd5ToBase64(const char* _cSrcStr1, int _iStrLen1,
	const char * _cSrcStr2, int _iStrLen2, char *_cOutBuf, int _iOutLen);