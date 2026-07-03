#pragma once
#include "Avalon/Common.hpp"

namespace Avalon
{
	namespace DeviceType
	{
		constexpr static u8 None = 0;
		constexpr static u8 Vulkan = 1;
		constexpr static u8 D3D12 = 2;
	}

	using DeviceResultType = u16;

	namespace DeviceResult
	{
		constexpr static DeviceResultType Success = 0;
		constexpr static DeviceResultType UnknownDeviceType = 1;
		constexpr static DeviceResultType UnknownError = 2;
	}

	class IDevice
	{
	public:
		virtual ~IDevice() = default;
		virtual DeviceResultType Init() = 0;
	};

	DeviceResultType AV_API CreateDevice(u8 deviceType, IDevice** device);
}
