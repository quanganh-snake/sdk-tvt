// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// SDKDEMO.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"



int GetBitPos( unsigned long num)
{
	int test = 0;
	int i = -1;
	if(num == 0)
		return i;

	
	while (!test)
	{
		i++;
		test = (num >> i) & 0x1;
	}
	return i;
}
