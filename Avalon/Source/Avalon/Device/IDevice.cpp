#include "IDevice.hpp"

#include "Avalon/Core/Allocator.hpp"
#include "Avalon/Platform/Platform.hpp"
#include "D3D12/D3D12Device.hpp"
#include "spdlog/spdlog.h"
#include "Vulkan/VulkanDevice.hpp"


namespace Avalon
{
	DeviceResultType CreateDevice(u8 preferedDevice, IDevice** device)
	{
		u8 deviceType = Platform::GetDeviceType(preferedDevice);
		switch (deviceType)
		{
			// case DeviceType::Vulkan:
			// 	*device = Alloc<VulkanDevice>();
			// 	return DeviceResult::Success;
			case DeviceType::D3D12:
				*device = Alloc<D3D12Device>();
				return DeviceResult::Success;
			default:
				break;
		}

		spdlog::error("selected device {} not available ", deviceType);
		return DeviceResult::UnknownDeviceType;
	}
}
