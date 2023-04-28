#ifndef _CRATE_H_
#define _CRATE_H_
#include "../Common/CoreUtil.h"
#include "../Common/CoreEngine.h"
#include "FrameResource.h"

class Crate :public CoreEngine {
private:
	int error;
	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;
public:
	virtual void OnUpdate();
	virtual void OnRender();
	void PopulateCommands();
	~Crate();
	Crate()=default;
	Crate(int among);
	virtual void OnInitialize();
	int GetError() { return error; }
};

#endif

