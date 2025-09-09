#ifndef _CORE_PSO_H_
#define _CORE_PSO_H_

#include "..\CoreUtil.h"
using namespace std;
class RootSignature;
class CorePSO
{
public:

    CorePSO(const wchar_t* Name) : m_Name(Name), m_RootSignature(nullptr), m_PSO(nullptr) {}

    static void DestroyAll(void);

    void SetRootSignature(const Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature)
    {
        m_RootSignature = rootSignature;
    }

    const ID3D12RootSignature* GetRootSignature(void) const
    {
        return m_RootSignature.Get();
    }

    ID3D12PipelineState* GetPipelineStateObject(void) const { return m_PSO; }

protected:

    const wchar_t* m_Name;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;

    ID3D12PipelineState* m_PSO;
};

#endif
