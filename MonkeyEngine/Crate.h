#ifndef _CRATE_H_
#define _CRATE_H_
#include "Common/CoreUtil.h"
#include "Common/CoreGraphics.h"

class Crate :public CoreGraphics {
private:
	int error;
public:
	Crate()=default;
	Crate(int among);
	virtual void OnInitialize();
	int GetError() { return error; }
};

#endif
