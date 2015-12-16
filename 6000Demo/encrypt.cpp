#include "stdafx.h"

static char basis_64[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


static char index_64[128] =
{
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};



#define char64(c)  (((c) > 127) ? -1 : index_64[(c)])

int base64_decode(unsigned char* outputBuf, unsigned int* outputLen,
	unsigned char* inputBuf, unsigned int inputLen)
{
	unsigned char c1, c2, c3, c4;
	unsigned int count = 0;



	c1 = *inputBuf++;
	while (c1 != '\0')
	{
		c2 = *inputBuf++;
		c3 = *inputBuf++;
		c4 = *inputBuf++;

		c1 = char64(c1);
		c2 = char64(c2);

		outputBuf[count++] = ((c1 << 2) | ((c2 & 0x30) >> 4));

		if (c3 != '=')
		{
			c3 = char64(c3);
			outputBuf[count++] = (((c2 & 0XF) << 4) | ((c3 & 0x3C) >> 2));

			if (c4 != '=')
			{
				c4 = char64(c4);
				outputBuf[count++] = (((c3 & 0x03) << 6) | c4);
			}
		}

		c1 = *inputBuf++;
		if (c1 == '\r')
		{
			c1 = *inputBuf++;
			if (c1 == '\n')
				c1 = *inputBuf++;
		}
	}// while c1 != 0

	outputBuf[count] = '\0';//xumm add 
	*outputLen = count;



	return count;
}



int base64_encode(unsigned char* outputBuf, unsigned int* outputLen,
	unsigned char* inputBuf, unsigned int inputLen)
{
	unsigned char c1, c2, c3;
	unsigned char *pOutputBuf = outputBuf;

	unsigned int count = 0, charsPerLine = 0;


	c1 = 0x00;
	c2 = 0x00;
	c3 = 0x00;

	while (count < inputLen)
	{
		c1 = inputBuf[count++];
		if (count == inputLen)
		{
			c2 = 0x00;
			*pOutputBuf++ = basis_64[c1 >> 2];
			*pOutputBuf++ = basis_64[(((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4))];
			*pOutputBuf++ = '=';
			*pOutputBuf++ = '=';
		}
		else	// count < inputLen
		{
			c2 = inputBuf[count++];
			if (count == inputLen)
			{
				c3 = 0x00;
				*pOutputBuf++ = basis_64[c1 >> 2];
				*pOutputBuf++ = basis_64[(((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4))];
				*pOutputBuf++ = basis_64[(((c2 & 0x0F) << 2) | ((c3 & 0xC0) >> 6))];
				*pOutputBuf++ = '=';
			}
			else // count  < inputLen
			{
				c3 = inputBuf[count++];
				*pOutputBuf++ = basis_64[c1 >> 2];
				*pOutputBuf++ = basis_64[(((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4))];
				*pOutputBuf++ = basis_64[(((c2 & 0x0F) << 2) | ((c3 & 0xC0) >> 6))];
				*pOutputBuf++ = basis_64[(c3 & 0x3F)];
			}
		}

		charsPerLine += 4;

	} //end while

	*pOutputBuf++ = '\0';

	*outputLen = strlen((char*)outputBuf);

	return *outputLen;
}
typedef char ElemType;
//��ʼ�û���IP
static int IP_Table[64] = { 57,49,41,33,25,17,9,1,
59,51,43,35,27,19,11,3,
61,53,45,37,29,21,13,5,
63,55,47,39,31,23,15,7,
56,48,40,32,24,16,8,0,
58,50,42,34,26,18,10,2,
60,52,44,36,28,20,12,4,
62,54,46,38,30,22,14,6 };

//���ʼ�û���IP^-1
static int IP_1_Table[64] = {
	39,7,47,15,55,23,63,31,
	38,6,46,14,54,22,62,30,
	37,5,45,13,53,21,61,29,
	36,4,44,12,52,20,60,28,
	35,3,43,11,51,19,59,27,
	34,2,42,10,50,18,58,26,
	33,1,41,9,49,17,57,25,
	32,0,40,8,48,16,56,24
};


static int E_Table[48] = { 31, 0, 1, 2, 3, 4,
3,  4, 5, 6, 7, 8,
7,  8,9,10,11,12,
11,12,13,14,15,16,
15,16,17,18,19,20,
19,20,21,22,23,24,
23,24,25,26,27,28,
27,28,29,30,31, 0 };

//�û�����P
static int P_Table[32] = { 15,6,19,20,28,11,27,16,
0,14,22,25,4,17,30,9,
1,7,23,13,31,26,2,8,
18,12,29,5,21,10,3,24 };


static int PC_1[56] = { 56,48,40,32,24,16,8,
0,57,49,41,33,25,17,
9,1,58,50,42,34,26,
18,10,2,59,51,43,35,
62,54,46,38,30,22,14,
6,61,53,45,37,29,21,
13,5,60,52,44,36,28,
20,12,4,27,19,11,3 };

//�û�ѡ��2
static int PC_2[48] = { 13,16,10,23,0,4,2,27,
14,5,20,9,22,18,11,3,
25,7,15,6,26,19,12,1,
40,51,30,36,46,54,29,39,
50,44,32,47,43,48,38,55,
33,52,45,41,49,35,28,31 };

//S��
static int S[8][4][16] =//S1
{ { { 14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7 },
{ 0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8 },
{ 4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0 },
{ 15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13 } },
//S2
{ { 15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10 },
{ 3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5 },
{ 0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15 },
{ 13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9 } },
//S3
{ { 10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8 },
{ 13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1 },
{ 13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7 },
{ 1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12 } },
//S4
{ { 7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15 },
{ 13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9 },
{ 10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4 },
{ 3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14 } },
//S5
{ { 2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9 },
{ 14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6 },
{ 4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14 },
{ 11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3 } },
//S6
{ { 12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11 },
{ 10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8 },
{ 9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6 },
{ 4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13 } },
//S7
{ { 4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1 },
{ 13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6 },
{ 1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2 },
{ 6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12 } },
//S8
{ { 13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7 },
{ 1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2 },
{ 7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8 },
{ 2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11 } } };


//�ֽ�ת���ɶ�����
static int ByteToBit(ElemType ch, ElemType bit[8])
{
	int cnt;
	for (cnt = 0; cnt < 8; cnt++)
	{
		*(bit + cnt) = (ch >> cnt) & 1;
	}
	return 0;
}

static int Char8ToBit64(ElemType ch[8], ElemType bit[64])
{
	int cnt;
	for (cnt = 0; cnt < 8; cnt++)
	{
		ByteToBit(*(ch + cnt), bit + (cnt << 3));
	}
	return 0;
}


static int DES_PC1_Transform(ElemType key[64], ElemType tempbts[56]) {
	int cnt;
	for (cnt = 0; cnt < 56; cnt++) {
		tempbts[cnt] = key[PC_1[cnt]];
	}
	return 0;
}

//ѭ������
static int DES_ROL(ElemType data[56], int time) {
	ElemType temp[56];

	//���潫Ҫѭ���ƶ����ұߵ�λ
	memcpy(temp, data, time);
	memcpy(temp + time, data + 28, time);

	//ǰ28λ�ƶ�
	memcpy(data, data + time, 28 - time);
	memcpy(data + 28 - time, temp, time);

	//��28λ�ƶ�
	memcpy(data + 28, data + 28 + time, 28 - time);
	memcpy(data + 56 - time, temp + time, time);

	return 0;
}
static int MOVE_TIMES[16] = { 1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1 };
static int DES_PC2_Transform(ElemType key[56], ElemType tempbts[48]) {
	int cnt;
	for (cnt = 0; cnt < 48; cnt++) {
		tempbts[cnt] = key[PC_2[cnt]];
	}
	return 0;
}

//��������Կ
static int DES_MakeSubKeys(ElemType key[64], ElemType subKeys[16][48])
{
	ElemType temp[56];
	int cnt;
	DES_PC1_Transform(key, temp);//PC1�û�
	for (cnt = 0; cnt < 16; cnt++)
	{//16�ֵ���������16������Կ
		DES_ROL(temp, MOVE_TIMES[cnt]);//ѭ������
		DES_PC2_Transform(temp, subKeys[cnt]);//PC2�û�����������Կ
	}
	return 0;
}

static int DES_IP_Transform(ElemType data[64])
{
	int cnt;
	ElemType temp[64];
	for (cnt = 0; cnt < 64; cnt++)
	{
		temp[cnt] = data[IP_Table[cnt]];
	}
	memcpy(data, temp, 64);
	return 0;
}



static int DES_E_Transform(ElemType data[48]) {
	int cnt;
	ElemType temp[48];
	for (cnt = 0; cnt < 48; cnt++) {
		temp[cnt] = data[E_Table[cnt]];
	}
	memcpy(data, temp, 48);
	return 0;
}
static int DES_XOR(ElemType R[48], ElemType L[48], int count) {
	int cnt;
	for (cnt = 0; cnt < count; cnt++) {
		R[cnt] ^= L[cnt];
	}
	return 0;
}

//S���û�
static int DES_SBOX(ElemType data[48]) {
	int cnt;
	int line, row, output;
	int cur1, cur2;
	for (cnt = 0; cnt < 8; cnt++) {
		cur1 = cnt * 6;
		cur2 = cnt << 2;

		//������S���е�������
		line = (data[cur1] << 1) + data[cur1 + 5];
		row = (data[cur1 + 1] << 3) + (data[cur1 + 2] << 2)
			+ (data[cur1 + 3] << 1) + data[cur1 + 4];
		output = S[cnt][line][row];

		//��Ϊ2����
#if 1
		data[cur2] = (output & 0X08) >> 3;
		data[cur2 + 1] = (output & 0X04) >> 2;
		data[cur2 + 2] = (output & 0X02) >> 1;
		data[cur2 + 3] = output & 0x01;
#endif
#if 0
		data[cur2] = (output & 0X01);
		data[cur2 + 1] = (output & 0X02) >> 1;
		data[cur2 + 2] = (output & 0X04) >> 2;
		data[cur2 + 3] = (output & 0x08) >> 3;
#endif
	}
	return 0;
}
static int DES_P_Transform(ElemType data[32]) {
	int cnt;
	ElemType temp[32];
	for (cnt = 0; cnt < 32; cnt++) {
		temp[cnt] = data[P_Table[cnt]];
	}
	memcpy(data, temp, 32);
	return 0;
}
static int DES_Swap(ElemType left[32], ElemType right[32]) {
	ElemType temp[32];
	memcpy(temp, left, 32);
	memcpy(left, right, 32);
	memcpy(right, temp, 32);
	return 0;
}
static int DES_IP_1_Transform(ElemType data[64]) {
	int cnt;
	ElemType temp[64];
	for (cnt = 0; cnt < 64; cnt++) {
		temp[cnt] = data[IP_1_Table[cnt]];
	}
	memcpy(data, temp, 64);
	return 0;
}

static int BitToByte(ElemType bit[8], ElemType *ch)
{
	int cnt;
	for (cnt = 0; cnt < 8; cnt++)
	{
		*ch |= *(bit + cnt) << cnt;
	}
	return 0;
}



static int Bit64ToChar8(ElemType bit[64], ElemType ch[8])
{
	int cnt;
	memset(ch, 0, 8);
	for (cnt = 0; cnt < 8; cnt++) {
		BitToByte(bit + (cnt << 3), ch + cnt);
	}
	return 0;
}


//���ܵ�������
static int DES_EncryptBlock(ElemType plainBlock[8], ElemType subKeys[16][48], ElemType cipherBlock[8]) {
	ElemType plainBits[64];
	ElemType copyRight[48];
	int cnt;

	Char8ToBit64(plainBlock, plainBits);
	//��ʼ�û���IP�û���
	DES_IP_Transform(plainBits);
	///////////////////////////
	//16�ֵ���
	for (cnt = 0; cnt < 16; cnt++) {
		memcpy(copyRight, plainBits + 32, 32);
		//���Ұ벿�ֽ�����չ�û�����32λ��չ��48λ
		DES_E_Transform(copyRight);

		//���Ұ벿��������Կ����������
		DES_XOR(copyRight, subKeys[cnt], 48);
		//���������S�У����32λ���
		DES_SBOX(copyRight);
		//P�û�
		DES_P_Transform(copyRight);
		//��������벿�����Ұ벿�ֽ������
		DES_XOR(plainBits, copyRight, 32);
		if (cnt != 15) {
			//����������Ҳ��Ľ���
			DES_Swap(plainBits, plainBits + 32);
		}
	}
	/////////////////////////////////////
	//���ʼ�û���IP^1�û���
	DES_IP_1_Transform(plainBits);
	Bit64ToChar8(plainBits, cipherBlock);
	return 0;
}


int DES_Encrypt(char *_cOutputBuf, int _iOutputLen, char _cKey[8], const char *_cInputbuf, int _iInputLen)
{
	ElemType plainBlock[8], cipherBlock[8];
	ElemType bKey[64];
	ElemType subKeys[16][48];

	int i = 0;
	int iResidue = _iInputLen & 0x07;
	int iDivision = _iInputLen >> 3;
	int iRet = -1;
	int iOutBufLoc = 0;

	if ((NULL == _cInputbuf) || (NULL == _cOutputBuf) || (NULL == _cKey) ||
		(_iInputLen <= 0) ||
		_iOutputLen < ((iDivision + iResidue ? 1 : 0) << 3))
	{
		goto leave;
	}

	//����Կת��Ϊ��������
	Char8ToBit64((ElemType *)_cKey, bKey);
	//��������Կ
	DES_MakeSubKeys(bKey, subKeys);

	for (i = 0; i < iDivision; i++)
	{
		DES_EncryptBlock((ElemType *)_cInputbuf + iOutBufLoc, subKeys, (ElemType *)_cOutputBuf + iOutBufLoc);
		iOutBufLoc += 8;
	}

	if (iResidue > 0)
	{
		memcpy(plainBlock, _cInputbuf + iOutBufLoc, iResidue);
		memset(plainBlock + iResidue, '\0', 7 - iResidue);	//���
		plainBlock[7] = 8 - iResidue;//���һ���ַ�����������һ���ַ����ڵ��������ַ�����
		DES_EncryptBlock(plainBlock, subKeys, cipherBlock);
		memcpy(_cOutputBuf + iOutBufLoc, cipherBlock, 8);
		iOutBufLoc += 8;
	}
	iRet = iOutBufLoc;
leave:
	if ((_iOutputLen > 0) && (-1 == iRet))
	{
		_cOutputBuf[0] = 0;
	}
	return iRet;
}



int EncryptSrcToDesToBase64(char *_cOutputBuf, int _iOutputLen, char _cKey[8], const char *_cInputbuf, int _iInputLen)
{
	int iRet = -1;
	unsigned char ucTemp[256];
	int iTempLen = 256;
	unsigned char ucBaseOut[256];
	unsigned int u32BaseOutLen = 256;

	if ((iRet = DES_Encrypt((char *)ucTemp, iTempLen, _cKey, _cInputbuf, _iInputLen)) != -1)
	{
		if (iRet > 128)
		{
			goto leave;
		}
		base64_encode(ucBaseOut, &u32BaseOutLen, ucTemp, iRet);
		if (u32BaseOutLen + 1 > (unsigned int)_iOutputLen)
		{
			goto leave;
		}
		memcpy(_cOutputBuf, ucBaseOut, u32BaseOutLen + 1);
		iRet = u32BaseOutLen;
	}
leave:
	if ((_iOutputLen > 0) && (-1 == iRet))
	{
		_cOutputBuf[0] = 0;
	}
	return iRet;
}

typedef unsigned int UINT4;
typedef unsigned char *POINTER;
typedef unsigned short int UINT2;

/* MD5 context. */
typedef struct
{
	/* state (ABCD) */
	/*�ĸ�32bits�������ڴ�����ռ���õ�����ϢժҪ������Ϣ���ȡ�512bitsʱ��Ҳ���ڴ��ÿ��512bits���м���*/
	UINT4 state[4];

	/* number of bits, modulo 2^64 (lsb first) */
	/*�洢ԭʼ��Ϣ��bits������,����������bits���Ϊ 2^64 bits����Ϊ2^64��һ��64λ�������ֵ*/
	UINT4 count[2];

	/* input buffer */
	/*����������Ϣ�Ļ�������512bits*/
	unsigned char buffer[64];
} MD5_CTX;
static void MD5Init(MD5_CTX *context)
{
	/*����ǰ����Ч��Ϣ�ĳ������0,����ܼ�,��û����Ч��Ϣ,���ȵ�Ȼ��0��*/
	context->count[0] = context->count[1] = 0;

	/* Load magic initialization constants.*/
	/*��ʼ�����ӱ������㷨Ҫ�����������û��������*/
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
}

#define R_memset(x, y, z) memset(x, y, z)
#define R_memcpy(x, y, z) memcpy(x, y, z)
#define R_memcmp(x, y, z) memcmp(x, y, z)

static void Decode(UINT4 *output, unsigned char *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((UINT4)input[j]) | (((UINT4)input[j + 1]) << 8) |
		(((UINT4)input[j + 2]) << 16) | (((UINT4)input[j + 3]) << 24);
}

/* MD5 basic transformation. Transforms state based on block. */
/*
��512bits��Ϣ(��block������)����һ�δ���ÿ�δ����������
state[4]��md5�ṹ�е�state[4]�����ڱ����512bits��Ϣ���ܵ��м����������ս��
block[64]�������ܵ�512bits��Ϣ
*/
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
*/
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21
static void MD5Transform(UINT4 state[4], unsigned char block[64])
{
	UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

	Decode(x, block, 64);

	/* Round 1 */
	FF(a, b, c, d, x[0], S11, 0xd76aa478); /* 1 */
	FF(d, a, b, c, x[1], S12, 0xe8c7b756); /* 2 */
	FF(c, d, a, b, x[2], S13, 0x242070db); /* 3 */
	FF(b, c, d, a, x[3], S14, 0xc1bdceee); /* 4 */
	FF(a, b, c, d, x[4], S11, 0xf57c0faf); /* 5 */
	FF(d, a, b, c, x[5], S12, 0x4787c62a); /* 6 */
	FF(c, d, a, b, x[6], S13, 0xa8304613); /* 7 */
	FF(b, c, d, a, x[7], S14, 0xfd469501); /* 8 */
	FF(a, b, c, d, x[8], S11, 0x698098d8); /* 9 */
	FF(d, a, b, c, x[9], S12, 0x8b44f7af); /* 10 */
	FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

											/* Round 2 */
	GG(a, b, c, d, x[1], S21, 0xf61e2562); /* 17 */
	GG(d, a, b, c, x[6], S22, 0xc040b340); /* 18 */
	GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG(b, c, d, a, x[0], S24, 0xe9b6c7aa); /* 20 */
	GG(a, b, c, d, x[5], S21, 0xd62f105d); /* 21 */
	GG(d, a, b, c, x[10], S22, 0x2441453); /* 22 */
	GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG(b, c, d, a, x[4], S24, 0xe7d3fbc8); /* 24 */
	GG(a, b, c, d, x[9], S21, 0x21e1cde6); /* 25 */
	GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG(c, d, a, b, x[3], S23, 0xf4d50d87); /* 27 */
	GG(b, c, d, a, x[8], S24, 0x455a14ed); /* 28 */
	GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG(d, a, b, c, x[2], S22, 0xfcefa3f8); /* 30 */
	GG(c, d, a, b, x[7], S23, 0x676f02d9); /* 31 */
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

											/* Round 3 */
	HH(a, b, c, d, x[5], S31, 0xfffa3942); /* 33 */
	HH(d, a, b, c, x[8], S32, 0x8771f681); /* 34 */
	HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH(a, b, c, d, x[1], S31, 0xa4beea44); /* 37 */
	HH(d, a, b, c, x[4], S32, 0x4bdecfa9); /* 38 */
	HH(c, d, a, b, x[7], S33, 0xf6bb4b60); /* 39 */
	HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH(d, a, b, c, x[0], S32, 0xeaa127fa); /* 42 */
	HH(c, d, a, b, x[3], S33, 0xd4ef3085); /* 43 */
	HH(b, c, d, a, x[6], S34, 0x4881d05); /* 44 */
	HH(a, b, c, d, x[9], S31, 0xd9d4d039); /* 45 */
	HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH(b, c, d, a, x[2], S34, 0xc4ac5665); /* 48 */

										   /* Round 4 */
	II(a, b, c, d, x[0], S41, 0xf4292244); /* 49 */
	II(d, a, b, c, x[7], S42, 0x432aff97); /* 50 */
	II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II(b, c, d, a, x[5], S44, 0xfc93a039); /* 52 */
	II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II(d, a, b, c, x[3], S42, 0x8f0ccc92); /* 54 */
	II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II(b, c, d, a, x[1], S44, 0x85845dd1); /* 56 */
	II(a, b, c, d, x[8], S41, 0x6fa87e4f); /* 57 */
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II(c, d, a, b, x[6], S43, 0xa3014314); /* 59 */
	II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II(a, b, c, d, x[4], S41, 0xf7537e82); /* 61 */
	II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II(c, d, a, b, x[2], S43, 0x2ad7d2bb); /* 63 */
	II(b, c, d, a, x[9], S44, 0xeb86d391); /* 64 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	/* Zeroize sensitive information. */
	R_memset((POINTER)x, 0, sizeof(x));
}

static void MD5Update(MD5_CTX *context, unsigned char * input, unsigned int inputLen)
{
	unsigned int i, index, partLen;

	/* Compute number of bytes mod 64 */
	/*����������Ϣ��bits���ȵ��ֽ�����ģ64, 64bytes=512bits��
	�����ж�������Ϣ���ϵ�ǰ����������Ϣ���ܳ����ܲ��ܴﵽ512bits��
	����ܹ��ﵽ��Դչ���512bits����һ�δ���*/
	index = (unsigned int)((context->count[0] >> 3) & 0x3F);

	/* Update number of bits *//*����������Ϣ��bits����*/
	if ((context->count[0] += ((UINT4)inputLen << 3)) < ((UINT4)inputLen << 3))
		context->count[1]++;
	context->count[1] += ((UINT4)inputLen >> 29);

	/*�������е��ֽ������Ȼ�������ֽڿ��� �ճ�64��������*/
	partLen = 64 - index;

	/* Transform as many times as possible.
	*/
	/*�����ǰ������ֽ��� ���� �����ֽ������Ȳ���64�ֽ�������������ֽ���*/
	if (inputLen >= partLen)
	{
		/*�õ�ǰ��������ݰ�context->buffer�����ݲ���512bits*/
		R_memcpy((POINTER)&context->buffer[index], (POINTER)input, partLen);
		/*�û����������������512bits���Ѿ����浽context->buffer�У� ��һ��ת����ת��������浽context->state��*/
		MD5Transform(context->state, context->buffer);

		/*
		�Ե�ǰ�����ʣ���ֽ���ת�������ʣ����ֽ�<�������input��������>����512bits�Ļ� ����
		ת��������浽context->state��
		*/
		for (i = partLen; i + 63 < inputLen; i += 64)/*��i+63<inputlen��Ϊi+64<=inputlen���������*/
			MD5Transform(context->state, &input[i]);

		index = 0;
	}
	else
		i = 0;

	/* Buffer remaining input */
	/*�����뻺�����еĲ��������512bits��ʣ��������䵽context->buffer�У������Ժ���������*/
	R_memcpy((POINTER)&context->buffer[index], (POINTER)&input[i], inputLen - i);
}

static void Encode(unsigned char *output, UINT4 *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (unsigned char)(input[i] & 0xff);
		output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
		output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
		output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
	}
}
static unsigned char PADDING[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


static void MD5Final(unsigned char digest[16], MD5_CTX *context)
{
	unsigned char bits[8];
	unsigned int index, padLen;

	/* Save number of bits */
	/*��Ҫ��ת������Ϣ(���е�)��bits���ȿ�����bits��*/
	Encode(bits, context->count, 8);

	/* Pad out to 56 mod 64. */
	/* �������е�bits���ȵ��ֽ�����ģ64, 64bytes=512bits*/
	index = (unsigned int)((context->count[0] >> 3) & 0x3f);
	/*������Ҫ�����ֽ�����padLen��ȡֵ��Χ��1-64֮��*/
	padLen = (index < 56) ? (56 - index) : (120 - index);
	/*��һ�κ������þ��Բ����ٵ���MD5Transform�ı����ã���Ϊ��һ�β�������512bits*/
	MD5Update(context, PADDING, padLen);

	/* Append length (before padding) */
	/*����ԭʼ��Ϣ��bits���ȣ�bits���ȹ̶�����64bits��ʾ������һ���ܹ�ǡ�ɴչ�512bits�������Ҳ������*/
	MD5Update(context, bits, 8);

	/* Store state in digest */
	/*�����յĽ�����浽digest�С�ok�����ڴ󹦸����*/
	Encode(digest, context->state, 16);

	/* Zeroize sensitive information. */

	R_memset((POINTER)context, 0, sizeof(*context));
}

int MD5_Encrypt(char *_cOutputBuf, int _iOutputLen, const char *_cInputbuf, int _iInputLen)
{
	int iRet = -1;
	MD5_CTX context;
	unsigned char digest[16];
	if (NULL == _cInputbuf || NULL == _cOutputBuf || _iInputLen <= 0 || _iOutputLen < 16)
	{
		goto leave;
	}
	MD5Init(&context);
	MD5Update(&context, (unsigned char *)_cInputbuf, _iInputLen);
	MD5Final(digest, &context);
	memcpy(_cOutputBuf, digest, 16);
	iRet = 16;
leave:
	if ((_iOutputLen > 0) && (-1 == iRet))
	{
		_cOutputBuf[0] = 0;
	}
	return iRet;
}


int EncryptStrToMd5ToBase64(const char* _cSrcStr1, int _iStrLen1, const char * _cSrcStr2, int _iStrLen2, char *_cOutBuf, int _iOutLen)
{
	int iRet = -1;
	char cStr[128];
	char cTemp[16];
	int iTempLen = 16;
	unsigned char ucBaseOut[256];
	unsigned int u32BaseOutLen = 256;
	if (NULL == _cSrcStr1 || _iStrLen1 <= 0 || NULL == _cOutBuf)
	{
		goto leave;
	}
	_cOutBuf[0] = 0;
	if (_iStrLen2 <= 0 || NULL == _cSrcStr2)
	{
		_iStrLen2 = 0;
	}

	if (_iStrLen1 + _iStrLen2 > 128)
	{
		goto leave;
	}

	memcpy(cStr, _cSrcStr1, _iStrLen1);
	if (NULL != _cSrcStr2)
	{
		memcpy(cStr + _iStrLen1, _cSrcStr2, _iStrLen2);
	}

	cStr[_iStrLen1 + _iStrLen2] = 0;
	iTempLen = MD5_Encrypt(cTemp, 16, cStr, _iStrLen1 + _iStrLen2);
	base64_encode(ucBaseOut, &u32BaseOutLen, (unsigned char *)cTemp, iTempLen);
	if (u32BaseOutLen >= (unsigned int)_iOutLen)
	{
		goto leave;
	}
	memcpy(_cOutBuf, ucBaseOut, u32BaseOutLen + 1);
	iRet = u32BaseOutLen;
leave:
	if ((_iOutLen > 0) && -1 == iRet)
	{
		_cOutBuf[0] = 0;
	}
	return iRet;
}




