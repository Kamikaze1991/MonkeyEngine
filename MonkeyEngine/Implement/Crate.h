#ifndef _CRATE_H_
#define _CRATE_H_
#include "../Common/CoreUtil.h"
#include "../Common/CoreEngine.h"
#include "FrameResource.h"

class Crate :public CoreEngine {
private:
	std::vector<std::unique_ptr<FrameResource>> FrameResources;
	FrameResource* CurrFrameResource = nullptr;
	ComPtr<ID3D12DescriptorHeap> ShadowResourceViewDescriptorHeap = nullptr;
	int CurrentFrameResourceIndex = 0;
public:
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnInitialize();
	virtual void OnInitializeUi();
	
	void PopulateCommands();
	~Crate();
	Crate()=default;
	Crate(int among);
	
private:
	//specific funcions
	void BuildFrameResurces();
	void BuildLocalDescriptorHeap();
	void BuilduserInterface();
};

#endif

