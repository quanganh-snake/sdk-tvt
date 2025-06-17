#pragma once
class CMsgBase
{
public:
	CMsgBase(void);
	virtual ~CMsgBase(void);

	virtual void UpdateMsg(LONG ID, LONG lCommand, void *data, int length)  {return;};
	

};
