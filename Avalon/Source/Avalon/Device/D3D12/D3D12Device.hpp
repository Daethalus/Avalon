#pragma once
#include <vector>
#include "Avalon/Device/IDevice.hpp"

#include <d3d12.h>
#include <dxgi1_6.h>

namespace Avalon
{
	class D3D12Device : public IDevice
	{
	public:
		DeviceResultType     Init(bool enableDebugLayers) override;
		DeviceResultType     CreateDevice(IAdapter* selectedAdapter) override;
		DeviceResultType     Destroy() override;
		std::span<IAdapter*> Adapters() override;

		ISwapChain*   CreateSwapChain(const SwapchainDesc& swachainDesc) override;
		IRenderPass*  CreateRenderPass(const RenderPassDesc& renderPassDesc) override;
		ICommandList* CreateCommandList() override;

		DeviceResultType SubmitAndPresent(ICommandList* commandList, ISwapChain* swapChain) override;

	private:
		std::vector<IAdapter*> m_adapters;

		ID3D12Debug1*       m_debug = nullptr;
		IDXGIFactory7*      m_factory = nullptr;
		ID3D12Device10*     m_device = nullptr;
		ID3D12CommandQueue* m_queue = nullptr;
	};
}
