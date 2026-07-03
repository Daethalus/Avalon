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

		DeviceResultType Init() override;
		DeviceResultType CreateDevice(IAdapter* selectedAdapter) override;
		DeviceResultType Destroy() override;

		std::span<IAdapter*> Adapters() override;
	private:
		std::vector<IAdapter*> m_adapters;

		IDXGIFactory7* m_factory = nullptr;
		ID3D12Device10* m_device = nullptr;

	};
}
