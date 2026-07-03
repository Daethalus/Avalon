#pragma once
#include <span>
#include <string>
#include <string_view>

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


	class IAdapter
	{
	public:
		virtual std::string Name() const = 0;
	protected:
		virtual ~IAdapter() = default;
	};


	class IDevice
	{
	public:
		virtual DeviceResultType Init() = 0;
		virtual DeviceResultType CreateDevice(IAdapter* selectedAdapter) = 0;
		virtual DeviceResultType Destroy() = 0;

		virtual std::span<IAdapter*> Adapters() = 0;

	protected:
		virtual ~IDevice() = default;
	};

	DeviceResultType AV_API CreateDevice(u8 preferedDevice, IDevice** device);
}
