// MD5Calc.h: interface for the MD5Calc class.

#if !defined(MD5CALC_H_)
#define CMD5CALC_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _ReadBufSize 1000000

namespace dcpp {

class MD5Calc  
{
public:
	char* CalcMD5FromString(const char *s8_Input);
	char* CalcMD5FromFile(const wchar_t *s8_Path);

	void FreeBuffer();
	MD5Calc();
	virtual ~MD5Calc();

private:
	struct MD5Context {
		unsigned long buf[4];
		unsigned long bits[2];
		unsigned char in[64];
	};

	void MD5Init();
	void MD5Update(unsigned char *buf, unsigned len);
	void MD5Final (unsigned char digest[16]);

	void MD5Transform(unsigned long buf[4], unsigned long in[16]);
	char* MD5FinalToString();

	void byteReverse (unsigned char *buf, unsigned longs);

	char *mp_s8ReadBuffer;
	MD5Context ctx;
	char ms8_MD5[40]; // Output buffer
};

}

#endif // !defined(CMD5CALC_H_)