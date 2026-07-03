#pragma once
#include <span>
#include <string>
#include <string_view>

#include "Avalon/Common.hpp"
#include "Avalon/Core/Math.hpp"

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
		constexpr static DeviceResultType PresentFailed = 0;
	}

	enum class LoadOp
	{
		Load,
		Clear,
		DontCare
	};

	enum class StoreOp
	{
		Store,
		DontCare
	};

	struct SwapchainDesc
	{
		VoidPtr nativeWindow;
		u32     width;
		u32     height;
		bool    vsync = true;
		bool    allowTearing = false;
	};

	struct ViewportInfo
	{
		f32 x{};
		f32 y{};
		f32 width{};
		f32 height{};
		f32 minDepth{};
		f32 maxDepth{1.0};
	};

	struct RenderPassDesc
	{
		//TODO
	};

	class IAdapter
	{
	public:
		virtual std::string Name() const = 0;

	protected:
		virtual ~IAdapter() = default;
	};

	class ITextureView
	{
	public:
	protected:
		virtual ~ITextureView() = default;
	};

	class ITexture
	{
	public:
		virtual ITextureView* GetView() = 0;
		virtual void          Destroy() = 0;
	protected:
		virtual ~ITexture() = default;
	};


	struct RenderPassColorInfo
	{
		ITextureView* view = nullptr;
		LoadOp        loadOp = LoadOp::DontCare;
		StoreOp       storeOp = StoreOp::DontCare;
		Vec4f         clearColor{};
	};

	struct BeginRenderPassInfo
	{
		Rect area = Rect{};
		std::span<RenderPassColorInfo> colors;
	};

	class ISwapChain
	{
	public:
		virtual u32       AcquireNextImage() = 0;
		virtual ITexture* GetBackBuffer(u32 index) = 0;

		virtual void Destroy() = 0;
	protected:
		virtual ~ISwapChain() = default;
	};

	class ICommandList
	{
	public:
		virtual void Reset() = 0;
		virtual void Close() = 0;


		virtual void BeginRenderPass(const BeginRenderPassInfo& info) = 0;
		virtual void EndRenderPass() = 0;

		virtual void SetViewport(const ViewportInfo& viewport) = 0;
		virtual void SetScissor(i32 x, i32 y, u32 width, u32 height) = 0;

		virtual void Destroy() = 0;

	protected:
		virtual ~ICommandList() = default;
	};


	class IDevice
	{
	public:
		virtual DeviceResultType Init(bool enableDebugLayers) = 0;
		virtual DeviceResultType CreateDevice(IAdapter* selectedAdapter) = 0;
		virtual DeviceResultType Destroy() = 0;
		virtual void             WaitIdle() = 0;

		virtual ICommandList*    BeginFrame() = 0;
		virtual DeviceResultType EndFrame(std::span<ISwapChain*> swapchains) = 0;

		virtual ISwapChain*   CreateSwapChain(const SwapchainDesc& swachainDesc) = 0;
		virtual ICommandList* CreateCommandList() = 0;

		virtual std::span<IAdapter*> Adapters() = 0;

	protected:
		virtual ~IDevice() = default;
	};

	DeviceResultType AV_API CreateDevice(u8 preferedDevice, IDevice** device);
}
