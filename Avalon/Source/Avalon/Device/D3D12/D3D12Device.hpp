#pragma once
#include "Avalon/Device/IDevice.hpp"

namespace Avalon
{
	class D3D12Device : public IDevice
	{
	public:
		DeviceResultType Init() override;
	};
}
