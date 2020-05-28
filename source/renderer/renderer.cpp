/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "diracsea.h"
#include "renderer.h"

#include <SDL.h>
#include <SDL_vulkan.h>
#include <Vulkan.h>

#include "platform/platform.h"


#define VK_FUNCTION_PTR_DECLARATION(fun) PFN_##fun fun = nullptr;

#define PLATFORM_SPECIFIC_INSTANCE_LEVEL_FUNCTIONS(x)

#if defined(VK_USE_PLATFORM_WIN32_KHR)
	#define PLATFORM_SPECIFIC_INSTANCE_LEVEL_FUNCTIONS(x) x(vkCreateWin32SurfaceKHR)
#elif defined(VK_USE_PLATFORM_XCB_KHR)
	#define PLATFORM_SPECIFIC_INSTANCE_LEVEL_FUNCTIONS(x) x(vkCreateXcbSurfaceKHR)
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
	#define PLATFORM_SPECIFIC_INSTANCE_LEVEL_FUNCTIONS(x) x(vkCreateXlibSurfaceKHR)
#endif

#define INSTANCE_LEVEL_FUNCTIONS(x)\
	x(vkEnumerateDeviceExtensionProperties)\
	x(vkGetPhysicalDeviceSurfaceSupportKHR)\
	x(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)\
	x(vkGetPhysicalDeviceSurfaceFormatsKHR)\
	x(vkGetPhysicalDeviceSurfacePresentModesKHR)\
	x(vkDestroySurfaceKHR)\
	PLATFORM_SPECIFIC_INSTANCE_LEVEL_FUNCTIONS(x)

#define DEVICE_LEVEL_FUNCTIONS(x)\
	x(vkGetDeviceQueue)\
	x(vkDeviceWaitIdle)\
	x(vkDestroyDevice)\
	x(vkCreateSemaphore)\
	x(vkCreateCommandPool)\
	x(vkAllocateCommandBuffers)\
	x(vkBeginCommandBuffer)\
	x(vkCmdPipelineBarrier)\
	x(vkCmdClearColorImage)\
	x(vkEndCommandBuffer)\
	x(vkQueueSubmit)\
	x(vkFreeCommandBuffers)\
	x(vkDestroyCommandPool)\
	x(vkDestroySemaphore)\
	x(vkCreateSwapchainKHR)\
	x(vkDestroySwapchainKHR)\
	x(vkGetSwapchainImagesKHR)\
	x(vkAcquireNextImageKHR)\
	x(vkQueuePresentKHR)\
	x(vkCreateImageView)\
	x(vkCreateRenderPass)\
	x(vkCreateFramebuffer)\
	x(vkCreateShaderModule)\
	x(vkCreatePipelineLayout)\
	x(vkCreateGraphicsPipelines)\
	x(vkCmdBeginRenderPass)\
	x(vkCmdBindPipeline)\
	x(vkCmdDraw)\
	x(vkCmdEndRenderPass)\
	x(vkDestroyShaderModule)\
	x(vkDestroyPipelineLayout)\
	x(vkDestroyPipeline)\
	x(vkDestroyRenderPass)\
	x(vkDestroyFramebuffer)\
	x(vkDestroyImageView)


///////////////////////////
// Shader Bank

// Takes a macro list and creates an enum and an array of vertex and fragment shader file names
// provides a namespace for loading/unloading the files and querying by enum for O(1) access

// usage:
// #define MY_SHADERS_LIST(x)\
// 	x(shader1)
// 	x(shader2)
//
// SHADER_BANK(MyShaders, MY_SHADERS_LIST)
// MyShaders::LoadFiles();
// MyShaders::GetVertexShader(EMyShaders::Enum::shader1);
// MyShaders::GetFragmentShader(EMyShaders::Enum::shader1);
// MyShaders::UnloadFiles();

// TODO: sort out working directory and data directory via cmake
#define SHADER_BANK_MEMBER_VERT_FILENAME(member) "../data/shaders/"#member".vert.spv",
#define SHADER_BANK_MEMBER_FRAG_FILENAME(member) "../data/shaders/"#member".frag.spv",

#define SHADER_BANK(name, list)\
	SCOPED_AUTO_ENUM(name, list, uint16_t)\
	namespace name {\
		const char* vertexShaderFilePaths[E##name::count] = {\
			list(SHADER_BANK_MEMBER_VERT_FILENAME)\
		};\
		const char* fragmentShaderFilePaths[E##name::count] = {\
			list(SHADER_BANK_MEMBER_FRAG_FILENAME)\
		};\
		platform::SFile vertexShaders[E##name::count];\
		platform::SFile fragmentShaders[E##name::count];\
		void UnloadFiles() {\
			for (size_t i = 0; i < E##name::count; ++i){\
				vertexShaders[i] = platform::SFile();\
				fragmentShaders[i] = platform::SFile();\
			}\
		}\
		bool LoadFiles() {\
			bool bVertexFilesLoaded = platform::LoadFiles(vertexShaderFilePaths, E##name::count, vertexShaders);\
			if (bVertexFilesLoaded)\
			{\
				bool bFragFilesLoaded = platform::LoadFiles(fragmentShaderFilePaths, E##name::count, fragmentShaders);\
				if (bFragFilesLoaded) return true;\
			}\
			UnloadFiles();\
			return false;\
		}\
		inline const platform::SFile& GetVertexShader(E##name::Enum shaderEnum){\
			assert((size_t) shaderEnum < E##name::count);\
			return vertexShaders[(size_t) shaderEnum];\
		}\
		inline const platform::SFile& GetFragmentShader(E##name::Enum shaderEnum){\
			assert((size_t) shaderEnum < E##name::count);\
			return fragmentShaders[(size_t) shaderEnum];\
		}\
	}

///////////////////////////
// Default Shaders
#define DEFAULT_SHADERS(x)\
	x(test)

SHADER_BANK(DefaultShaders, DEFAULT_SHADERS)

namespace vulkan
{
/////////////////////////////////////////////////////////
// Constants
static constexpr uint32_t INVALID_QUEUE_FAMILY_PROPERTIES_INDEX = UINT32_MAX;
static constexpr size_t MAX_IMAGE_COUNT = 4;
static constexpr size_t MAX_COMMAND_BUFFER_COUNT = MAX_IMAGE_COUNT;

/////////////////////////////////////////////////////////
// State

///////////////////////////
// SDevice
struct SDevice
{
	enum class EState : uint8_t
	{
		Uninitialized,
		Initialized,
		Garbage,
	};

	DEVICE_LEVEL_FUNCTIONS(VK_FUNCTION_PTR_DECLARATION)

	VkDevice handle = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;
	uint32_t graphicsQueueFamilyIndex = INVALID_QUEUE_FAMILY_PROPERTIES_INDEX;
	uint32_t presentQueueFamilyIndex = INVALID_QUEUE_FAMILY_PROPERTIES_INDEX;
	EState state = EState::Uninitialized;
};

///////////////////////////
// SInstance
struct SInstance
{
	enum class EState : uint8_t
	{
		Uninitialized,
		Initialized,
		Garbage
	};

	INSTANCE_LEVEL_FUNCTIONS(VK_FUNCTION_PTR_DECLARATION)

	VkInstance handle = VK_NULL_HANDLE;
	EState state = EState::Uninitialized;
};

///////////////////////////
// SImage
struct SImage
{
	VkImage handle = VK_NULL_HANDLE;
	VkImageView view = VK_NULL_HANDLE;
	VkSampler sampler = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
};

///////////////////////////
// SSwapChain
struct SSwapChain
{
	enum EState
	{
		Uninitialized,
		Initialized,
		Garbage
	};

	VkSwapchainKHR handle = VK_NULL_HANDLE;
	VkSurfaceFormatKHR surfaceFormat = { VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_MAX_ENUM_KHR };
	SImage images[MAX_IMAGE_COUNT] = { SImage() };
	uint32_t imageCount = 0;
	VkExtent2D extent = { 0, 0 };
	EState state = EState::Uninitialized;
};

///////////////////////////
// SRecordCommandBuffer
struct SRecordCommandBuffer // state used during command buffer recording
{
	SRecordCommandBuffer()
	{
		memset(this, 0, sizeof(SRecordCommandBuffer));

		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageSubresourceRange.baseMipLevel = 0;
		imageSubresourceRange.levelCount = 1;
		imageSubresourceRange.baseArrayLayer = 0;
		imageSubresourceRange.layerCount = 1;

		barrierPresentToClear.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrierPresentToClear.pNext = nullptr;
		barrierPresentToClear.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		barrierPresentToClear.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
		barrierPresentToClear.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrierPresentToClear.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrierPresentToClear.srcQueueFamilyIndex = UINT32_MAX;
		barrierPresentToClear.dstQueueFamilyIndex = UINT32_MAX;
		barrierPresentToClear.image = VK_NULL_HANDLE;
		barrierPresentToClear.subresourceRange = imageSubresourceRange;

		barrierClearToPresent.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrierClearToPresent.pNext = nullptr;
		barrierClearToPresent.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
		barrierClearToPresent.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		barrierClearToPresent.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrierClearToPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		barrierClearToPresent.srcQueueFamilyIndex = UINT32_MAX;
		barrierClearToPresent.dstQueueFamilyIndex = UINT32_MAX;
		barrierClearToPresent.image = VK_NULL_HANDLE;
		barrierClearToPresent.subresourceRange = imageSubresourceRange;

		clearColor = { 1.0f, 0.8f, 0.4f, 0.0f };
	}

	VkCommandBufferBeginInfo commandBufferBeginInfo;
	VkImageSubresourceRange imageSubresourceRange;
	VkImageMemoryBarrier barrierPresentToClear;
	VkImageMemoryBarrier barrierClearToPresent;
	VkClearColorValue clearColor;
};

// TODO: Add allocation callbacks for debugging
static const VkAllocationCallbacks* g_pAllocationCallbacks = nullptr;

static SDevice g_device;
static SInstance g_instance;

static VkSurfaceKHR g_presentationSurface = VK_NULL_HANDLE;
static VkSemaphore g_imageAvailableSemaphore = VK_NULL_HANDLE;
static VkSemaphore g_renderingFinishedSemaphore = VK_NULL_HANDLE;
static SSwapChain g_swapChain;

static VkCommandBuffer g_presentCommandBuffers[MAX_COMMAND_BUFFER_COUNT] = { VK_NULL_HANDLE };
static uint32_t g_presentCommandBufferCount = 0;
static VkCommandPool g_presentCommandPool = VK_NULL_HANDLE;
static SRecordCommandBuffer g_recordBufferState;

static VkRenderPass g_renderPass = VK_NULL_HANDLE;
static VkFramebuffer g_frameBuffers[MAX_IMAGE_COUNT] = { VK_NULL_HANDLE };

/////////////////////////////////////////////////////////
// Functions

ERunResult SetDevice(
	VkDevice _device,
	VkPhysicalDevice _physicalDevice,
	uint32_t _graphicsQueueFamilyIndex,
	uint32_t _presentQueueFamilyIndex)
{
	assert(g_device.state == SDevice::EState::Uninitialized);
	assert(_device != VK_NULL_HANDLE);
	assert(_physicalDevice != VK_NULL_HANDLE);
	assert(_graphicsQueueFamilyIndex != INVALID_QUEUE_FAMILY_PROPERTIES_INDEX);
	assert(_presentQueueFamilyIndex != INVALID_QUEUE_FAMILY_PROPERTIES_INDEX);

	g_device.handle = _device;
	g_device.physicalDevice = _physicalDevice;
	g_device.graphicsQueueFamilyIndex = _graphicsQueueFamilyIndex;
	g_device.presentQueueFamilyIndex = _presentQueueFamilyIndex;

#define SET_DEVICE_LEVEL_FUNCTION(fun)                                                                            \
		g_device.fun = (PFN_##fun)vkGetDeviceProcAddr(g_device.handle, #fun);                                         \
		if (g_device.fun == nullptr) { printf("Vulkan failed to load device function %s\n", #fun); return eRR_Error; }

	DEVICE_LEVEL_FUNCTIONS(SET_DEVICE_LEVEL_FUNCTION)
#undef SET_DEVICE_LEVEL_FUNCTION

	g_device.vkGetDeviceQueue(g_device.handle, g_device.graphicsQueueFamilyIndex, 0, &g_device.graphicsQueue);
	assert(g_device.graphicsQueue != nullptr);

	g_device.vkGetDeviceQueue(g_device.handle, g_device.presentQueueFamilyIndex, 0, &g_device.presentQueue);
	assert(g_device.presentQueue != nullptr);

	// Update the record buffer state to reference the correct present queue family index
	g_recordBufferState.barrierPresentToClear.srcQueueFamilyIndex = g_device.presentQueueFamilyIndex;
	g_recordBufferState.barrierPresentToClear.dstQueueFamilyIndex = g_device.presentQueueFamilyIndex;
	g_recordBufferState.barrierClearToPresent.srcQueueFamilyIndex = g_device.presentQueueFamilyIndex;
	g_recordBufferState.barrierClearToPresent.dstQueueFamilyIndex = g_device.presentQueueFamilyIndex;

	g_device.state = SDevice::EState::Initialized;
	return eRR_Success;
}

ERunResult SetInstance(VkInstance _instance)
{
	assert(g_instance.state == SInstance::EState::Uninitialized);
	g_instance.handle = _instance;

#define SET_INSTANCE_LEVEL_FUNCTION(fun)                                                                               \
		g_instance.fun = (PFN_##fun)vkGetInstanceProcAddr(g_instance.handle, #fun);                                        \
		if (g_instance.fun == nullptr) { printf("Vulkan failed to load instance function %s\n", #fun); return eRR_Error; } 

	INSTANCE_LEVEL_FUNCTIONS(SET_INSTANCE_LEVEL_FUNCTION)
#undef SET_DEVICE_LEVEL_FUNCTION

	g_instance.state = SInstance::EState::Initialized;
	return eRR_Success;
}

bool CreateSwapChainImageViews();
bool SetSwapChain(VkSwapchainKHR _swapChain, VkSurfaceFormatKHR _surfaceFormat, uint32_t _imageCount, VkImage* _swapChainImages, const VkExtent2D& _extent)
{
	assert(g_swapChain.state == SSwapChain::EState::Uninitialized);
	assert(_swapChain != VK_NULL_HANDLE);
	assert(_surfaceFormat.format != VK_FORMAT_UNDEFINED);

	g_swapChain.handle = _swapChain;
	g_swapChain.surfaceFormat = _surfaceFormat;
	g_swapChain.imageCount = _imageCount;
	g_swapChain.extent = _extent;

	for (uint32_t i = 0; i < _imageCount; ++i)
	{
		g_swapChain.images[i].handle = _swapChainImages[i];
	}

	if (!CreateSwapChainImageViews())
		return false;

	g_swapChain.state = SSwapChain::EState::Initialized;
	return true;
}

void SetPresentationSurface(VkSurfaceKHR _surface)
{
	assert(g_presentationSurface == VK_NULL_HANDLE);
	g_presentationSurface = _surface;
}

bool CheckExtensionAvailability(const char* extensionName, const VkExtensionProperties* extensions, uint32_t extensionsCount)
{
	for (size_t i = 0; i < extensionsCount; ++i)
	{
		if (strcmp(extensions[i].extensionName, extensionName) == 0)
			return true;
	}

	return false;
}

bool CheckPhysicalDeviceProperties(
	VkPhysicalDevice physicalDevice, 
	uint32_t* pGraphicsQueueFamiliesIndex,
	uint32_t* pPresentQueueFamiliesIndex)
{
	assert(pGraphicsQueueFamiliesIndex != nullptr);
	*pGraphicsQueueFamiliesIndex = INVALID_QUEUE_FAMILY_PROPERTIES_INDEX;
	*pPresentQueueFamiliesIndex = INVALID_QUEUE_FAMILY_PROPERTIES_INDEX;
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;

	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
	vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

	const uint32_t majorVersion = VK_VERSION_MAJOR(deviceProperties.apiVersion);
	const uint32_t minorVersion = VK_VERSION_MINOR(deviceProperties.apiVersion);
	const uint32_t patchVersion = VK_VERSION_PATCH(deviceProperties.apiVersion);

	// TODO: Add beter properties and feature testing, as well as enabling tested features on logical device
	if (majorVersion < 1 || deviceProperties.limits.maxImageDimension2D < 4096)
	{
		return false;
	}

	{ // device extensions
		uint32_t extensionsCount = 0;
		if ((g_instance.vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, nullptr) != VK_SUCCESS) || extensionsCount == 0)
		{
			puts("Vulkan failed to get device extensions count!");
			return false;
		}

		VkExtensionProperties* const availableExtensions = (VkExtensionProperties*)alloca(sizeof(VkExtensionProperties) * extensionsCount);
		if ((g_instance.vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, availableExtensions) != VK_SUCCESS) || extensionsCount == 0)
		{
			puts("Vulkan failed to enumerate device extension properties!");
			return false;
		}

		if (!CheckExtensionAvailability(VK_KHR_SWAPCHAIN_EXTENSION_NAME, availableExtensions, extensionsCount))
		{
			return false;
		}
	}

	{ // queue family properties
		uint32_t queueFamiliesCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
		if (queueFamiliesCount == 0)
		{
			puts("Vulkan failed to get queue family properties count!");
			return false;
		}

		VkQueueFamilyProperties* pQueueFamilyProperties = (VkQueueFamilyProperties*)alloca(sizeof(VkQueueFamilyProperties) * queueFamiliesCount);
		VkBool32* pQueuePresentSupport = (VkBool32*)alloca(sizeof(VkBool32) * queueFamiliesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, pQueueFamilyProperties);
		for (uint32_t i = 0; i < queueFamiliesCount; ++i)
		{
			g_instance.vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, g_presentationSurface, pQueuePresentSupport + i);
			// TODO: Add better queue family properties testing?
			if (pQueueFamilyProperties[i].queueCount > 0 && pQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				if (*pGraphicsQueueFamiliesIndex == INVALID_QUEUE_FAMILY_PROPERTIES_INDEX)
				{
					*pGraphicsQueueFamiliesIndex = i;
				}

				if (pQueuePresentSupport[i])
				{
					*pGraphicsQueueFamiliesIndex = i;
					*pPresentQueueFamiliesIndex = i;
					return true;
				}
			}
		}

		if (*pGraphicsQueueFamiliesIndex == INVALID_QUEUE_FAMILY_PROPERTIES_INDEX)
		{
			return false;
		}

		// We don't have a queue that supports both graphics and present so we have to use separate queues
		for (uint32_t i = 0; i < queueFamiliesCount; ++i)
		{
			if (pQueuePresentSupport[i])
			{
				*pPresentQueueFamiliesIndex = i;
				return true;
			}
		}
	}

	return false;
}

bool CreateSwapChainImageViews()
{
	assert(g_device.state == SDevice::EState::Initialized);

	VkImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = nullptr;
	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.image = nullptr; // defined in loop below
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = g_swapChain.surfaceFormat.format;

	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	for (uint32_t i = 0; i < g_swapChain.imageCount; ++i)
	{
		imageViewCreateInfo.image = g_swapChain.images[i].handle;
		if (g_device.vkCreateImageView(
			g_device.handle,
			&imageViewCreateInfo,
			g_pAllocationCallbacks,
			&g_swapChain.images[i].view) != VK_SUCCESS)
		{
			puts("Vulkan failed to create image view!");
			return false;
		}
	}

	return true;
}

bool RecordCommandBuffers()
{
	assert(g_device.state == SDevice::EState::Initialized);
	assert(g_swapChain.state == SSwapChain::EState::Initialized);

	for (uint32_t i = 0; i < g_swapChain.imageCount; ++i)
	{
		g_recordBufferState.barrierPresentToClear.image = g_swapChain.images[i].handle;
		g_recordBufferState.barrierClearToPresent.image = g_swapChain.images[i].handle;

		g_device.vkBeginCommandBuffer(g_presentCommandBuffers[i], &g_recordBufferState.commandBufferBeginInfo);
		g_device.vkCmdPipelineBarrier(
			g_presentCommandBuffers[i],
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&g_recordBufferState.barrierPresentToClear);

		g_device.vkCmdClearColorImage(
			g_presentCommandBuffers[i],
			g_swapChain.images[i].handle,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			&g_recordBufferState.clearColor,
			1,
			&g_recordBufferState.imageSubresourceRange);

		g_device.vkCmdPipelineBarrier(
			g_presentCommandBuffers[i],
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&g_recordBufferState.barrierClearToPresent);

		if (g_device.vkEndCommandBuffer(g_presentCommandBuffers[i]) != VK_SUCCESS)
		{
			puts("Vulkan could not record command buffers!");
			return false;
		}
	}

	return true;
}

ERunResult DestroyState()
{
	ERunResult destroyResult = g_device.state == SDevice::EState::Initialized ? eRR_Success : eRR_Error;
	if (g_device.handle != VK_NULL_HANDLE)
	{
		g_device.vkDeviceWaitIdle(g_device.handle);

		if (g_presentCommandBufferCount > 0)
		{
			g_device.vkFreeCommandBuffers(g_device.handle, g_presentCommandPool, g_presentCommandBufferCount, g_presentCommandBuffers);
			g_presentCommandBufferCount = 0;
		}
		else
		{
			destroyResult = eRR_Error;
		}

		if (g_presentCommandPool != VK_NULL_HANDLE)
		{
			g_device.vkDestroyCommandPool(g_device.handle, g_presentCommandPool, g_pAllocationCallbacks);
			g_presentCommandPool = VK_NULL_HANDLE;
		}
		else
		{
			destroyResult = eRR_Error;
		}

		if (g_renderingFinishedSemaphore != VK_NULL_HANDLE)
		{
			g_device.vkDestroySemaphore(g_device.handle, g_renderingFinishedSemaphore, g_pAllocationCallbacks);
			g_renderingFinishedSemaphore = VK_NULL_HANDLE;
		}
		else
		{
			destroyResult = eRR_Error;
		}

		if (g_imageAvailableSemaphore != VK_NULL_HANDLE)
		{
			g_device.vkDestroySemaphore(g_device.handle, g_imageAvailableSemaphore, g_pAllocationCallbacks);
			g_imageAvailableSemaphore = VK_NULL_HANDLE;
		}
		else
		{
			destroyResult = eRR_Error;
		}

		if (g_swapChain.state == SSwapChain::EState::Initialized && g_swapChain.handle != VK_NULL_HANDLE)
		{
			g_device.vkDestroySwapchainKHR(g_device.handle, g_swapChain.handle, g_pAllocationCallbacks);
		}

		g_swapChain.handle = VK_NULL_HANDLE;
		g_swapChain.state = SSwapChain::EState::Garbage;

		if (g_device.handle != VK_NULL_HANDLE)
		{
			g_device.vkDestroyDevice(g_device.handle, g_pAllocationCallbacks);
		}

		g_device.handle = VK_NULL_HANDLE;
		g_device.state = SDevice::EState::Garbage;
	}
	else
	{
		destroyResult = eRR_Error;
	}

	if (g_instance.state != SInstance::EState::Initialized)
	{
		destroyResult = eRR_Error;
	}

	if (g_instance.handle != VK_NULL_HANDLE)
	{
		if (g_presentationSurface != VK_NULL_HANDLE)
		{
			g_instance.vkDestroySurfaceKHR(g_instance.handle, g_presentationSurface, g_pAllocationCallbacks);
			g_presentationSurface = VK_NULL_HANDLE;
		}
		vkDestroyInstance(g_instance.handle, g_pAllocationCallbacks);
	}

	g_instance.handle = VK_NULL_HANDLE;
	g_instance.state = SInstance::EState::Garbage;

	SDL_Vulkan_UnloadLibrary();
	DefaultShaders::UnloadFiles();
	return destroyResult;
}

} // vulkan namespace


namespace renderer
{

ERunResult Initialize()
{
	SDL_Window* pWindow = platform::GetWindow();
	assert(pWindow != nullptr);

	if (!DefaultShaders::LoadFiles())
	{
		puts("Failed to load default shader files!");
		return eRR_Error;
	}

	{ // Vulkan instance creation
		uint32_t extensionCount = 0;
		SDL_Vulkan_GetInstanceExtensions(pWindow, &extensionCount, nullptr);
		const char** const extensionNames = (const char**)alloca(sizeof(const char*) * extensionCount);
		SDL_Vulkan_GetInstanceExtensions(pWindow, &extensionCount, extensionNames);

		VkApplicationInfo appInfo;
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = nullptr;
		appInfo.pApplicationName = "Dirac Sea Engine demo";
		appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
		appInfo.pEngineName = "Dirac Sea Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
		appInfo.apiVersion = VK_API_VERSION_1_2;

		// TODO: Add define enabled layers for debugging
		const uint32_t layerCount = 0;
		const char** const layerNames = nullptr;

		VkInstanceCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.pApplicationInfo = &appInfo;
		info.enabledLayerCount = layerCount;
		info.ppEnabledLayerNames = layerNames;
		info.enabledExtensionCount = extensionCount;
		info.ppEnabledExtensionNames = extensionNames;

		VkInstance instance = VK_NULL_HANDLE;
		VkResult res = vkCreateInstance(&info, vulkan::g_pAllocationCallbacks, &instance);
		if (res != VK_SUCCESS)
		{
			printf("Vulkcan could not create instance!\n");
			return eRR_Error;
		}

		if (vulkan::SetInstance(instance) != eRR_Success)
		{
			return eRR_Error;
		}
	} // ~Vulkan instance creation

	{ // Vulkan surface creation
		VkSurfaceKHR presentationSurface;
		if (SDL_Vulkan_CreateSurface(pWindow, vulkan::g_instance.handle, &presentationSurface) == SDL_FALSE)
		{
			printf("SDL could not create window surface! SDL_Error: %s\n", SDL_GetError());
			return eRR_Error;
		}

		vulkan::SetPresentationSurface(presentationSurface);
	} // ~Vulkan surface creation

	{ // Vulkan logical device creation
		VkDevice vkDevice;
		uint32_t graphicsQueueFamilyIndex = vulkan::INVALID_QUEUE_FAMILY_PROPERTIES_INDEX;
		uint32_t presentQueueFamilyIndex = vulkan::INVALID_QUEUE_FAMILY_PROPERTIES_INDEX;
		uint32_t numDevices = 0;
		if (vkEnumeratePhysicalDevices(vulkan::g_instance.handle, &numDevices, nullptr) != VK_SUCCESS)
		{
			puts("Vulkan failed to find number of physical devices!");
			return eRR_Error;
		}
		else if (numDevices == 0)
		{
			puts("Vulkan found no physical devices!");
			return eRR_Error;
		}

		VkPhysicalDevice* const physicalDevices = (VkPhysicalDevice*)alloca(sizeof(VkPhysicalDevice) * numDevices);
		if (vkEnumeratePhysicalDevices(vulkan::g_instance.handle, &numDevices, physicalDevices) != VK_SUCCESS)
		{
			puts("Vulkan failed to enumerate physical devices!");
			return eRR_Error;
		}

		// TODO: Select best fit physical device based on features/type
		VkPhysicalDevice selectedPhysicalDevice = VK_NULL_HANDLE;
		for (uint32_t i = 0; i < numDevices; ++i)
		{
			if (vulkan::CheckPhysicalDeviceProperties(physicalDevices[i], &graphicsQueueFamilyIndex, &presentQueueFamilyIndex))
			{
				selectedPhysicalDevice = physicalDevices[i];
			}
		}

		if (selectedPhysicalDevice == VK_NULL_HANDLE)
		{
			puts("Failed to find a compatible vulkan physical device!");
			return eRR_Error;
		}

		if (graphicsQueueFamilyIndex == vulkan::INVALID_QUEUE_FAMILY_PROPERTIES_INDEX)
		{
			puts("Vulkan unable to find appropriate graphics queue family properties");
			return eRR_Error;
		}

		if (presentQueueFamilyIndex == vulkan::INVALID_QUEUE_FAMILY_PROPERTIES_INDEX)
		{
			puts("Vulan unable to find appropriate present queue family properties");
			return eRR_Error;
		}

		static constexpr uint32_t queueCount = 1;
		static constexpr float queuePriorities[queueCount] = { 1.0f };

		const uint32_t createInfoCount = graphicsQueueFamilyIndex == presentQueueFamilyIndex ? 1 : 2;
		VkDeviceQueueCreateInfo createInfoArray[2];

		{
			VkDeviceQueueCreateInfo& queueCreateInfo = createInfoArray[0];
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.pNext = nullptr;
			queueCreateInfo.flags = 0;
			queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
			queueCreateInfo.queueCount = queueCount;
			queueCreateInfo.pQueuePriorities = queuePriorities;
		}

		if (createInfoCount > 1)
		{
			VkDeviceQueueCreateInfo& queueCreateInfo = createInfoArray[1];
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.pNext = nullptr;
			queueCreateInfo.flags = 0;
			queueCreateInfo.queueFamilyIndex = presentQueueFamilyIndex;
			queueCreateInfo.queueCount = queueCount;
			queueCreateInfo.pQueuePriorities = queuePriorities;
		}

		static const uint32_t extensionsCount = 1;
		const char* extensions[extensionsCount] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		VkDeviceCreateInfo deviceCreateInfo;
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = nullptr;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.queueCreateInfoCount = createInfoCount;
		deviceCreateInfo.pQueueCreateInfos = createInfoArray;
		deviceCreateInfo.enabledLayerCount = 0;
		deviceCreateInfo.ppEnabledLayerNames = nullptr;
		deviceCreateInfo.enabledExtensionCount = extensionsCount;
		deviceCreateInfo.ppEnabledExtensionNames = extensions;
		deviceCreateInfo.pEnabledFeatures = nullptr;

		const VkResult deviceCreationResult = vkCreateDevice(
			selectedPhysicalDevice,
			&deviceCreateInfo,
			vulkan::g_pAllocationCallbacks,
			&vkDevice);

		if (deviceCreationResult != VK_SUCCESS)
		{
			puts("Vulkan failed to create logical device!");
			return eRR_Error;
		}

		if (vulkan::SetDevice(vkDevice, selectedPhysicalDevice, graphicsQueueFamilyIndex, presentQueueFamilyIndex))
		{
			return eRR_Error;
		}

	} // ~Vulkan logical device creation

	{ // Vulkan semaphore creation
		VkSemaphoreCreateInfo semaphoreCreateInfo;
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = 0;

		if (vulkan::g_device.vkCreateSemaphore(vulkan::g_device.handle, &semaphoreCreateInfo, nullptr, &vulkan::g_imageAvailableSemaphore) != VK_SUCCESS)
		{
			puts("Vulkan unable to create image available semaphore!");
			return eRR_Error;
		}

		if (vulkan::g_device.vkCreateSemaphore(vulkan::g_device.handle, &semaphoreCreateInfo, nullptr, &vulkan::g_renderingFinishedSemaphore) != VK_SUCCESS)
		{
			puts("Vulkan unable to create image available semaphore!");
			return eRR_Error;
		}
	} // ~Vulkan semaphore creation

	{ // Vulkan swap chain creation
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		if (vulkan::g_instance.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			vulkan::g_device.physicalDevice,
			vulkan::g_presentationSurface,
			&surfaceCapabilities) != VK_SUCCESS)
		{
			puts("Vulkan unable to query presentation surface capabilities!");
			return eRR_Error;
		}

		uint32_t formatsCount;
		if (vulkan::g_instance.vkGetPhysicalDeviceSurfaceFormatsKHR(
			vulkan::g_device.physicalDevice,
			vulkan::g_presentationSurface,
			&formatsCount,
			nullptr) != VK_SUCCESS)
		{
			puts("Vulkan unable to get physical device surface formats count!");
			return eRR_Error;
		}

		if (formatsCount == 0)
		{
			puts("Vulkan found 0 physical device surface formats!");
			return eRR_Error;
		}

		VkSurfaceFormatKHR* const pSurfaceFormats = (VkSurfaceFormatKHR*)alloca(sizeof(VkSurfaceFormatKHR) * formatsCount);
		if (vulkan::g_instance.vkGetPhysicalDeviceSurfaceFormatsKHR(
			vulkan::g_device.physicalDevice,
			vulkan::g_presentationSurface,
			&formatsCount,
			pSurfaceFormats) != VK_SUCCESS)
		{
			puts("Vulkan unable to get physical device surface formats!");
			return eRR_Error;
		}

		uint32_t presentModesCount;
		if (vulkan::g_instance.vkGetPhysicalDeviceSurfacePresentModesKHR(
			vulkan::g_device.physicalDevice,
			vulkan::g_presentationSurface,
			&presentModesCount,
			nullptr) != VK_SUCCESS)
		{
			puts("Vulkan unable to find physical device presentation modes count!");
			return eRR_Error;
		}

		if (presentModesCount == 0)
		{
			puts("Vulkan found 0 physical device presentation modes!");
			return eRR_Error;
		}

		VkPresentModeKHR* const presentModes = (VkPresentModeKHR*)alloca(sizeof(VkPresentModeKHR) * presentModesCount);
		if (vulkan::g_instance.vkGetPhysicalDeviceSurfacePresentModesKHR(
			vulkan::g_device.physicalDevice,
			vulkan::g_presentationSurface,
			&presentModesCount,
			presentModes) != VK_SUCCESS)
		{
			puts("Vulkan unable to find physical device presentation modes!");
			return eRR_Error;
		}

		// Set of images defined in a swap chain may not always be available for application to render to:
		// One may be displayed and one may wait in a queue to be presented
		// If application wants to use more images at the same time it must ask for more images
		uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
		if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
		{
			imageCount = surfaceCapabilities.maxImageCount;
		}

		if (imageCount > vulkan::MAX_IMAGE_COUNT)
		{
			// need to resize max or consider dynamic allocation if we ever hit this
			puts("Image count is greater than MAX_IMAGE_COUNT!");
			return eRR_Error;
		}

		VkSurfaceFormatKHR surfaceFormat;
		surfaceFormat.format = VK_FORMAT_UNDEFINED;
		surfaceFormat.colorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR;

		// If only one entry and it is undefined, then no format is preferred, use the one we want
		if (formatsCount == 1 && pSurfaceFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			surfaceFormat = { VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
		}
		else
		{
			// If multiple present, prefer the one we want
			for (uint32_t i = 0; i < formatsCount; ++i)
			{
				if (pSurfaceFormats[i].format == VK_FORMAT_R8G8B8A8_UNORM)
				{
					surfaceFormat = pSurfaceFormats[i];
					break;
				}
			}

			// We didn't find the format we want, just take the first
			if (surfaceFormat.format == VK_FORMAT_UNDEFINED)
			{
				surfaceFormat = pSurfaceFormats[0];
			}
		}

		// select size of swap chain images
		VkExtent2D swapChainExtent = surfaceCapabilities.currentExtent;
		if (surfaceCapabilities.currentExtent.width == -1)
		{
			swapChainExtent = { 640, 480 };
			if (swapChainExtent.width < surfaceCapabilities.minImageExtent.width)
			{
				swapChainExtent.width = surfaceCapabilities.minImageExtent.width;
			}

			if (swapChainExtent.height < surfaceCapabilities.minImageExtent.height)
			{
				swapChainExtent.height = surfaceCapabilities.minImageExtent.height;
			}

			if (swapChainExtent.width > surfaceCapabilities.maxImageExtent.width)
			{
				swapChainExtent.width = surfaceCapabilities.maxImageExtent.width;
			}

			if (swapChainExtent.height > surfaceCapabilities.maxImageExtent.height)
			{
				swapChainExtent.height = surfaceCapabilities.maxImageExtent.height;
			}
		}

		// define swap chain usage flags - color attachment flag must always be supported
		// we can define other usage flags but we always need to check if they are supported
		VkImageUsageFlags swapChainUsageFlags = 0;
		if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		{
			swapChainUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}
		else
		{
			puts("Vulkan VK_IMAGE_USAGE_TRANSFER_DST not supported by physical device surface capabilities!");
			return eRR_Error;
		}

		// Here we can consider various surface transformations (for example for tablets with multiple orientations), 
		// but I don't care about that right now, so won't...
		// So use identity if supported, otherwise current transform
		VkSurfaceTransformFlagBitsKHR surfaceTransformFlags = surfaceCapabilities.currentTransform;
		if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		{
			surfaceTransformFlags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}

		// Setup presentation mode
		// FIFO present mode is always available, but less ideal for input-lag
		// MAILBOX is lowest latency V-Sync enabled mode (similar to triple-buffering), so prefer it
		VkPresentModeKHR presentMode = VkPresentModeKHR(-1);
		{
			for (uint32_t i = 0; i < presentModesCount; ++i)
			{
				if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					presentMode = presentModes[i];
					break;
				}
			}

			if (presentMode == VkPresentModeKHR(-1))
			{
				for (uint32_t i = 0; i < presentModesCount; ++i)
				{
					if (presentModes[i] == VK_PRESENT_MODE_FIFO_KHR)
					{
						presentMode = presentModes[i];
						break;
					}
				}
			}

			// FIFO should always be available!
			if (presentMode == VkPresentModeKHR(-1))
			{
				puts("Vulkan unable to find FIFO present mode! This is expected to be alway supported!");
				return eRR_Error;
			}
		}

		VkSwapchainKHR oldSwapChain = vulkan::g_swapChain.handle;
		VkSwapchainKHR newSwapChain = VK_NULL_HANDLE;

		// Create the actual swap chain now that we have all the parameters
		VkSwapchainCreateInfoKHR swapChainCreateInfo;
		swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainCreateInfo.pNext = nullptr;
		swapChainCreateInfo.flags = 0;
		swapChainCreateInfo.surface = vulkan::g_presentationSurface;
		swapChainCreateInfo.minImageCount = imageCount;
		swapChainCreateInfo.imageFormat = surfaceFormat.format;
		swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapChainCreateInfo.imageExtent = swapChainExtent;
		swapChainCreateInfo.imageArrayLayers = 1;
		swapChainCreateInfo.imageUsage = swapChainUsageFlags;
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCreateInfo.queueFamilyIndexCount = 0;
		swapChainCreateInfo.pQueueFamilyIndices = nullptr;
		swapChainCreateInfo.preTransform = surfaceTransformFlags;
		swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapChainCreateInfo.presentMode = presentMode;
		swapChainCreateInfo.clipped = VK_TRUE;
		swapChainCreateInfo.oldSwapchain = oldSwapChain;

		if (vulkan::g_device.vkCreateSwapchainKHR(vulkan::g_device.handle, &swapChainCreateInfo, vulkan::g_pAllocationCallbacks, &newSwapChain) != VK_SUCCESS)
		{
			puts("Vulkan failed to create swap chain!");
			return eRR_Error;
		}

		if (oldSwapChain != VK_NULL_HANDLE)
		{
			vulkan::g_device.vkDestroySwapchainKHR(vulkan::g_device.handle, oldSwapChain, vulkan::g_pAllocationCallbacks);
		}

		VkImage* swapChainImages = (VkImage*)alloca(sizeof(VkImage) * imageCount);
		if (vulkan::g_device.vkGetSwapchainImagesKHR(
			vulkan::g_device.handle,
			newSwapChain,
			&imageCount,
			swapChainImages) != VK_SUCCESS)
		{
			puts("Vulkan could not get swap chain images!");
			return eRR_Error;
		}

		if (!vulkan::SetSwapChain(newSwapChain, surfaceFormat, imageCount, swapChainImages, swapChainExtent))
		{
			puts("Failed to set initialize swap chain!");
			return eRR_Error;
		}

		{ // Create render pass
			VkAttachmentDescription attachmentDescriptions[] =
			{
				{
					0, // flags
					vulkan::g_swapChain.surfaceFormat.format, // format
					VK_SAMPLE_COUNT_1_BIT, // samples
					VK_ATTACHMENT_LOAD_OP_CLEAR, // loadOp
					VK_ATTACHMENT_STORE_OP_STORE, // storeOp
					VK_ATTACHMENT_LOAD_OP_DONT_CARE, // stencilLoadOp
					VK_ATTACHMENT_STORE_OP_DONT_CARE, // stencilStoreOp
					VK_IMAGE_LAYOUT_UNDEFINED, // initialLayout
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, // finalLyout
				}
			};

			VkAttachmentReference colorAttachmentReferences[] =
			{
				{
					0, // attachment
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL // layout
				}
			};

			VkSubpassDescription subpassDescriptions[] =
			{
				{
					0, // flags
					VK_PIPELINE_BIND_POINT_GRAPHICS, // pipelineBindPoint
					0, // inputAttachmentCount
					nullptr, // pInputAttachments
					1, // colorAttachmentCount
					colorAttachmentReferences, // pColorAttachments
					nullptr, // pResolveAttachments
					nullptr, // pDepthStencilAttachments
					0, // presetveAttachmentCount
					nullptr, // pPreserveAttachments
				}
			};

			VkRenderPassCreateInfo renderPassCreateInfo;
			renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassCreateInfo.pNext = nullptr;
			renderPassCreateInfo.flags = 0;
			renderPassCreateInfo.attachmentCount = 1;
			renderPassCreateInfo.pAttachments = attachmentDescriptions;
			renderPassCreateInfo.subpassCount = 1;
			renderPassCreateInfo.pSubpasses = subpassDescriptions;
			renderPassCreateInfo.dependencyCount = 0;
			renderPassCreateInfo.pDependencies = nullptr;

			if (vulkan::g_device.vkCreateRenderPass(
						vulkan::g_device.handle,
						&renderPassCreateInfo,
						vulkan::g_pAllocationCallbacks,
						&vulkan::g_renderPass) != VK_SUCCESS)
			{
				puts("Vulkan failed to create render pass!");
				return eRR_Error;
			}
		} // ~create render pass

		{ // create frame buffers
			VkFramebufferCreateInfo frameBufferCreateInfo;
			frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferCreateInfo.pNext = nullptr;
			frameBufferCreateInfo.flags = 0;
			frameBufferCreateInfo.renderPass = vulkan::g_renderPass;
			frameBufferCreateInfo.attachmentCount = 1;
			frameBufferCreateInfo.pAttachments = nullptr; // defined in loop below
			frameBufferCreateInfo.width = 300;
			frameBufferCreateInfo.height = 300;
			frameBufferCreateInfo.layers = 1;

			for (uint32_t i = 0; i < vulkan::g_swapChain.imageCount; ++i)
			{
				frameBufferCreateInfo.pAttachments = &vulkan::g_swapChain.images[i].view;
				if (vulkan::g_device.vkCreateFramebuffer(
					vulkan::g_device.handle,
					&frameBufferCreateInfo,
					vulkan::g_pAllocationCallbacks,
					vulkan::g_frameBuffers) != VK_SUCCESS)
				{
					puts("Vulkan failed to create frame buffer!");
					return eRR_Error;
				}
			}
		} // ~create frame buffers

		{ // Create command buffers
			VkCommandPoolCreateInfo commandPoolCreateInfo;
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.pNext = nullptr;
			commandPoolCreateInfo.flags = 0;
			commandPoolCreateInfo.queueFamilyIndex = vulkan::g_device.presentQueueFamilyIndex;

			if (vkCreateCommandPool(
				vulkan::g_device.handle,
				&commandPoolCreateInfo,
				vulkan::g_pAllocationCallbacks,
				&vulkan::g_presentCommandPool) != VK_SUCCESS)
			{
				puts("Vulkan could not create a command pool!");
				return eRR_Error;
			}

			assert(vulkan::g_presentCommandBufferCount == 0); // TODO - dynamic free/realloc of command buffers on swapchain recreation
			memset(vulkan::g_presentCommandBuffers, 0, sizeof(VkCommandBuffer) * vulkan::MAX_COMMAND_BUFFER_COUNT);
			vulkan::g_presentCommandBufferCount = imageCount;

			VkCommandBufferAllocateInfo commandBufferAllocateInfo;
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.pNext = nullptr;
			commandBufferAllocateInfo.commandPool = vulkan::g_presentCommandPool;
			commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocateInfo.commandBufferCount = vulkan::g_presentCommandBufferCount;

			if (vkAllocateCommandBuffers(vulkan::g_device.handle, &commandBufferAllocateInfo, vulkan::g_presentCommandBuffers) != VK_SUCCESS)
			{
				puts("Vulkan failed to allocate command buffers!");
				return eRR_Error;
			}

			if (!vulkan::RecordCommandBuffers())
			{
				puts("Vulkan failed to record command buffers!");
				return eRR_Error;
			}
		}
	} // ~Vulkan swap chain creation


  return eRR_Success;
}

ERunResult Render()
{
	// Acquire next image state
	uint32_t imageIndex = UINT32_MAX;
	VkResult acquireNextImageResult = VK_RESULT_MAX_ENUM;
	const uint64_t timeout = UINT64_MAX;
	const VkFence fence = VK_NULL_HANDLE;

	// Queue submit state
	VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &vulkan::g_imageAvailableSemaphore;
	submitInfo.pWaitDstStageMask = &waitDstStageMask;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = vulkan::g_presentCommandBuffers;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &vulkan::g_renderingFinishedSemaphore;

	// Presentation state
	VkResult presentResult = VK_RESULT_MAX_ENUM;
	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &vulkan::g_renderingFinishedSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &vulkan::g_swapChain.handle;
	presentInfo.pImageIndices = nullptr;
	presentInfo.pResults = nullptr;

	acquireNextImageResult = vulkan::g_device.vkAcquireNextImageKHR(
		vulkan::g_device.handle,
		vulkan::g_swapChain.handle,
		timeout,
		vulkan::g_imageAvailableSemaphore,
		fence,
		&imageIndex);

	switch (acquireNextImageResult)
	{
	case VK_SUCCESS:
	case VK_SUBOPTIMAL_KHR:
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		break; // TODO: HANDLE WINDOW RESIZE
	default:
		puts("A problem occured during swap chain image acquisition!");
		return eRR_Error;
	}

	submitInfo.pCommandBuffers = &vulkan::g_presentCommandBuffers[imageIndex];
	if (vulkan::g_device.vkQueueSubmit(vulkan::g_device.presentQueue, 1, &submitInfo, fence) != VK_SUCCESS)
	{
		puts("Vulkan failed to submit to presentation queue!");
		return eRR_Error;
	}

	presentInfo.pImageIndices = &imageIndex;
	presentResult = vulkan::g_device.vkQueuePresentKHR(vulkan::g_device.presentQueue, &presentInfo);
	switch (presentResult)
	{
	case VK_SUCCESS:
	case VK_SUBOPTIMAL_KHR:
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		break; // TODO: HANDLE WINDOW RESIZE
	default:
		puts("A problem occured during image presentation!");
		return eRR_Error;
	}

  return eRR_Success;
}

ERunResult Shutdown()
{
	return vulkan::DestroyState();
}

} // renderer namespace
