#include "Platform.hpp"

#ifdef AV_WIN

#include "Avalon/Device/Device.hpp"

namespace Avalon
{
	u8 Platform::GetDeviceType(u8 preferredDeviceType)
	{
		if (preferredDeviceType == 0)
		{
			return DeviceType::D3D12;
		}
		return preferredDeviceType;
	}
}

#endif
