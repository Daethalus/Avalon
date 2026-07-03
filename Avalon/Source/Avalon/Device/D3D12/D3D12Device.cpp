#include "D3D12Device.hpp"

#include "D3D12Types.hpp"
#include "spdlog/spdlog.h"

namespace Avalon
{
	DeviceResultType D3D12Device::Init()
	{
		u32 flags = 0;
#if defined(_DEBUG)
		//flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		if (CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_factory)) != S_OK)
		{
			spdlog::error("[D3D12] - Error on create DXGIFactory2");
			return DeviceResult::UnknownError;
		}

		for (u32 i = 0;; ++i)
		{
			IDXGIAdapter4* adapter = nullptr;
			HRESULT hr = m_factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));

			if (hr == DXGI_ERROR_NOT_FOUND)
				break;

			m_adapters.emplace_back(new D3D12Adapter(adapter));
		}

		if (m_adapters.empty())
		{
			spdlog::error("[D3D12] - No adapters found");
			return DeviceResult::UnknownError;
		}

		return DeviceResult::Success;
	}

	DeviceResultType D3D12Device::CreateDevice(IAdapter* selectedAdapter)
	{
		if (selectedAdapter == nullptr)
		{
			selectedAdapter = m_adapters[0];
		}

		D3D12Adapter* adapter = static_cast<D3D12Adapter*>(selectedAdapter);
		if (D3D12CreateDevice(adapter->m_adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)) != S_OK)
		{
			spdlog::error("[D3D12] - Error on create D3D12Device");
		}

		//TODO

		return DeviceResult::Success;
	}

	DeviceResultType D3D12Device::Destroy()
	{
		for (IAdapter* adapter : m_adapters)
		{
			delete static_cast<D3D12Adapter*>(adapter);
		}

		if (m_device != nullptr) m_device->Release();
		if (m_factory != nullptr) m_factory->Release();

		//mimalloc
		delete this;
		return DeviceResult::Success;
	}

	std::span<IAdapter*> D3D12Device::Adapters()
	{
		return m_adapters;
	}
}
