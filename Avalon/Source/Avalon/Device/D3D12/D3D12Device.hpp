#pragma once
#include <vector>
#include "Avalon/Device/Device.hpp"

#include <d3d12.h>
#include <dxgi1_6.h>

#include "D3D12Types.hpp"

namespace Avalon
{
	struct D3D12DescriptorAllocation;

	class D3D12Device : public IDevice
	{
	public:
		DeviceResultType     Init(bool enableDebugLayers) override;
		DeviceResultType     CreateDevice(IAdapter* selectedAdapter) override;
		DeviceResultType     Destroy() override;
		std::span<IAdapter*> Adapters() override;
		void                 WaitIdle() override;

		ICommandList*    BeginFrame() override;
		DeviceResultType EndFrame(std::span<ISwapChain*> swapchains) override;

		ISwapChain*   CreateSwapChain(const SwapchainDesc& swachainDesc) override;
		ICommandList* CreateCommandList() override;

		ID3D12Device10* GetDevice() const
		{
			return m_device;
		}

		D3D12DescriptorHeapAllocator& GetSRVHeap()
		{
			return m_srvHeap;
		}

		D3D12DescriptorHeapAllocator& GetRTVHeap()
		{
			return m_rtvHeap;
		}

	private:
		std::vector<IAdapter*> m_adapters;

		ID3D12Debug1*       m_debug = nullptr;
		IDXGIFactory7*      m_factory = nullptr;
		ID3D12Device10*     m_device = nullptr;
		ID3D12CommandQueue* m_queue = nullptr;


		ID3D12Fence*  m_fence = nullptr;
		HANDLE        m_fenceEvent = nullptr;
		ICommandList* m_commands[AV_FRAMES_IN_FLIGHT] = {};
		u64           m_fenceValue[AV_FRAMES_IN_FLIGHT] = {};
		u64						m_nextFenceValue = {};
		u32           m_frameIndex = 0;

		D3D12DescriptorHeapAllocator m_srvHeap{};
		D3D12DescriptorHeapAllocator m_rtvHeap{};
	};
}
