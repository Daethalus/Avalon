#include "IDevice.hpp"

#include "D3D12/D3D12Device.hpp"
#include "spdlog/spdlog.h"
#include "Vulkan/VulkanDevice.hpp"


namespace Avalon
{
	DeviceResultType CreateDevice(u8 deviceType, IDevice** device)
	{
		switch (deviceType)
		{
			case DeviceType::Vulkan:
				*device = new VulkanDevice;
				return DeviceResult::Success;
			case DeviceType::D3D12:
				*device = new D3D12Device;
				return DeviceResult::Success;
			default:
				break;
		}

		spdlog::error("selected device {} not available ", deviceType);
		return DeviceResult::UnknownDeviceType;
	}
}
