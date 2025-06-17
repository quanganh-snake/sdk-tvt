
#ifndef __BASE_64_H__
#define __BASE_64_H__

class CBase64
{
public:
	CBase64(void);
	~CBase64(void);

	void SetLineMaxLen(int lineMaxLen);

	int Encode(const unsigned char *pSrc, int srcLen, char *pDest, int destLen);
	int Decode(const char *pSrc, int srcLen, unsigned char *pDest, int destLen);

private:
	int			m_lineMaxLen;
};

#endif
