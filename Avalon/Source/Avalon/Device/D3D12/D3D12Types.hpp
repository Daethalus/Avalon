#pragma once
#include <dxgi1_6.h>

#include "Avalon/Device/IDevice.hpp"

namespace Avalon
{
	class D3D12Adapter : public IAdapter
	{
	public:
		D3D12Adapter(IDXGIAdapter4* adapter) : m_adapter(adapter) {}

		std::string Name() const override;

		friend class D3D12Device;

	protected:
		~D3D12Adapter() override = default;

	private:
		IDXGIAdapter4* m_adapter = nullptr;
	};
}
