/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <thread>
#include <Vulkan.h>

#pragma warning(push)
#pragma warning(disable: 6255) // disable alloca overflow warnings
#pragma warning(disable: 6011) // disable null pointer dereference, we are assert pointer validity prior to access
#pragma warning(disable: 26812) // Vulkan uses unscoped enums, shutup msvc

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
	x(vkQueuePresentKHR)
	
enum ERunResult : int
{
	eRR_Success = 0,
	eRR_Error = 1
};

static constexpr uint32_t INVALID_QUEUE_FAMILY_PROPERTIES_INDEX = UINT32_MAX;

namespace vulkan
{
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

	VkDevice device = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;
	uint32_t graphicsQueueFamilyIndex = INVALID_QUEUE_FAMILY_PROPERTIES_INDEX;
	uint32_t presentQueueFamilyIndex = INVALID_QUEUE_FAMILY_PROPERTIES_INDEX;
	uint32_t imageCount = 0;
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

	SInstance() {}
	SInstance(VkInstance _instance)
		: instance(_instance)
	{
	}

	INSTANCE_LEVEL_FUNCTIONS(VK_FUNCTION_PTR_DECLARATION)

	VkInstance instance = VK_NULL_HANDLE;
	EState state = EState::Uninitialized;
};

static constexpr size_t MAX_IMAGE_COUNT = 4;
static constexpr size_t MAX_COMMAND_BUFFER_COUNT = MAX_IMAGE_COUNT;

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
		barrierPresentToClear.image = swapChainImages[0];
		barrierPresentToClear.subresourceRange = imageSubresourceRange;

		barrierClearToPresent.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrierClearToPresent.pNext = nullptr;
		barrierClearToPresent.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
		barrierClearToPresent.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		barrierClearToPresent.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrierClearToPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		barrierClearToPresent.srcQueueFamilyIndex = UINT32_MAX;
		barrierClearToPresent.dstQueueFamilyIndex = UINT32_MAX;
		barrierClearToPresent.image = swapChainImages[0];
		barrierClearToPresent.subresourceRange = imageSubresourceRange;

		clearColor = { 1.0f, 0.8f, 0.4f, 0.0f };
	}

	VkImage swapChainImages[MAX_IMAGE_COUNT];
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
static VkSwapchainKHR g_swapChain = VK_NULL_HANDLE;

static VkCommandBuffer g_presentCommandBuffers[MAX_COMMAND_BUFFER_COUNT] = { VK_NULL_HANDLE };
static uint32_t g_presentCommandBufferCount = 0;
static VkCommandPool g_presentCommandPool = VK_NULL_HANDLE;
static SRecordCommandBuffer g_recordBufferState;

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

	g_device.device = _device;
	g_device.physicalDevice = _physicalDevice;
	g_device.graphicsQueueFamilyIndex = _graphicsQueueFamilyIndex;
	g_device.presentQueueFamilyIndex = _presentQueueFamilyIndex;

#define SET_DEVICE_LEVEL_FUNCTION(fun)                                                                            \
		g_device.fun = (PFN_##fun)vkGetDeviceProcAddr(g_device.device, #fun);                                         \
		if (g_device.fun == nullptr) { printf("Vulkan failed to load device function %s\n", #fun); return eRR_Error; }

	DEVICE_LEVEL_FUNCTIONS(SET_DEVICE_LEVEL_FUNCTION)
#undef SET_DEVICE_LEVEL_FUNCTION

	g_device.vkGetDeviceQueue(g_device.device, g_device.graphicsQueueFamilyIndex, 0, &g_device.graphicsQueue);
	assert(g_device.graphicsQueue != nullptr);

	g_device.vkGetDeviceQueue(g_device.device, g_device.presentQueueFamilyIndex, 0, &g_device.presentQueue);
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
	g_instance.instance = _instance;

#define SET_INSTANCE_LEVEL_FUNCTION(fun)                                                                               \
		g_instance.fun = (PFN_##fun)vkGetInstanceProcAddr(g_instance.instance, #fun);                                        \
		if (g_instance.fun == nullptr) { printf("Vulkan failed to load instance function %s\n", #fun); return eRR_Error; } 

	INSTANCE_LEVEL_FUNCTIONS(SET_INSTANCE_LEVEL_FUNCTION)
#undef SET_DEVICE_LEVEL_FUNCTION

	g_instance.state = SInstance::EState::Initialized;
	return eRR_Success;
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

bool RecordCommandBuffers()
{
	if (g_device.vkGetSwapchainImagesKHR(
		g_device.device,
		g_swapChain,
		&g_device.imageCount,
		g_recordBufferState.swapChainImages) != VK_SUCCESS)
	{
		puts("Vulkan could not get swap chain images!");
		return false;
	}

	for (uint32_t i = 0; i < g_device.imageCount; ++i)
	{
		g_recordBufferState.barrierPresentToClear.image = g_recordBufferState.swapChainImages[i];
		g_recordBufferState.barrierClearToPresent.image = g_recordBufferState.swapChainImages[i];

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
			g_recordBufferState.swapChainImages[i],
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

void DestroyState()
{
	assert(g_device.state == SDevice::EState::Initialized);
	assert(g_device.device != VK_NULL_HANDLE);
	assert(g_instance.state == SInstance::EState::Initialized);
	assert(g_instance.instance != VK_NULL_HANDLE);
	assert(g_presentationSurface != VK_NULL_HANDLE);
	assert(g_presentCommandBufferCount > 0);
	assert(g_presentCommandPool != VK_NULL_HANDLE);


	g_device.vkDeviceWaitIdle(g_device.device);

	g_device.vkFreeCommandBuffers(g_device.device, g_presentCommandPool, g_presentCommandBufferCount, g_presentCommandBuffers);
	g_presentCommandBufferCount = 0;

	g_device.vkDestroyCommandPool(g_device.device, g_presentCommandPool, g_pAllocationCallbacks);
	g_presentCommandPool = VK_NULL_HANDLE;

	g_device.vkDestroySwapchainKHR(g_device.device, g_swapChain, g_pAllocationCallbacks);

	g_device.vkDestroySemaphore(g_device.device, g_renderingFinishedSemaphore, g_pAllocationCallbacks);
	g_device.vkDestroySemaphore(g_device.device, g_imageAvailableSemaphore, g_pAllocationCallbacks);

	g_device.vkDestroyDevice(g_device.device, g_pAllocationCallbacks);

	g_instance.vkDestroySurfaceKHR(g_instance.instance, g_presentationSurface, g_pAllocationCallbacks);
	g_presentationSurface = VK_NULL_HANDLE;

	vkDestroyInstance(g_instance.instance, g_pAllocationCallbacks);
	SDL_Vulkan_UnloadLibrary();

	g_device.state = SDevice::EState::Garbage;
	g_instance.state = SInstance::EState::Garbage;
}

} // vulkan namespace

namespace platform
{

int RunPlatform()
{
	/////////////////////////////////////////////////////////
	// Initialization
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return eRR_Error;
	}

	static const int kScreenWidth = 1024;
	static const int kScreenHeight = 768;

	SDL_Window* pWindow = SDL_CreateWindow(
		"Dirac Sea Engine",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		kScreenWidth,
		kScreenHeight,
		SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

	if (pWindow == nullptr)
	{
		printf("SDL could not create window! SDL_Error: %s\n", SDL_GetError());
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

		VkInstance instance;
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
		if (SDL_Vulkan_CreateSurface(pWindow, vulkan::g_instance.instance, &presentationSurface) == SDL_FALSE)
		{
			printf("SDL could not create window surface! SDL_Error: %s\n", SDL_GetError());
			return eRR_Error;
		}

		vulkan::SetPresentationSurface(presentationSurface);
	} // ~Vulkan surface creation

	{ // Vulkan logical device creation
		VkDevice vkDevice;
		uint32_t graphicsQueueFamilyIndex = INVALID_QUEUE_FAMILY_PROPERTIES_INDEX;
		uint32_t presentQueueFamilyIndex = INVALID_QUEUE_FAMILY_PROPERTIES_INDEX;
		uint32_t numDevices = 0;
		if (vkEnumeratePhysicalDevices(vulkan::g_instance.instance, &numDevices, nullptr) != VK_SUCCESS)
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
		if (vkEnumeratePhysicalDevices(vulkan::g_instance.instance, &numDevices, physicalDevices) != VK_SUCCESS)
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

		if (graphicsQueueFamilyIndex == INVALID_QUEUE_FAMILY_PROPERTIES_INDEX)
		{
			puts("Vulkan unable to find appropriate graphics queue family properties");
			return eRR_Error;
		}

		if (presentQueueFamilyIndex == INVALID_QUEUE_FAMILY_PROPERTIES_INDEX)
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

		if (vulkan::g_device.vkCreateSemaphore(vulkan::g_device.device, &semaphoreCreateInfo, nullptr, &vulkan::g_imageAvailableSemaphore) != VK_SUCCESS)
		{
			puts("Vulkan unable to create image available semaphore!");
			return eRR_Error;
		}

		if (vulkan::g_device.vkCreateSemaphore(vulkan::g_device.device, &semaphoreCreateInfo, nullptr, &vulkan::g_renderingFinishedSemaphore) != VK_SUCCESS)
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

		VkSwapchainKHR oldSwapChain = vulkan::g_swapChain;

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

		if (vulkan::g_device.vkCreateSwapchainKHR(vulkan::g_device.device, &swapChainCreateInfo, vulkan::g_pAllocationCallbacks, &vulkan::g_swapChain) != VK_SUCCESS)
		{
			puts("Vulkan failed to create swap chain!");
			return eRR_Error;
		}

		vulkan::g_device.imageCount = imageCount;

		{ // Create command buffers
			VkCommandPoolCreateInfo commandPoolCreateInfo;
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.pNext = nullptr;
			commandPoolCreateInfo.flags = 0;
			commandPoolCreateInfo.queueFamilyIndex = vulkan::g_device.presentQueueFamilyIndex;

			if (vkCreateCommandPool(
				vulkan::g_device.device,
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

			if (vkAllocateCommandBuffers(vulkan::g_device.device, &commandBufferAllocateInfo, vulkan::g_presentCommandBuffers) != VK_SUCCESS)
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

		if (oldSwapChain != VK_NULL_HANDLE)
		{
			vulkan::g_device.vkDestroySwapchainKHR(vulkan::g_device.device, oldSwapChain, vulkan::g_pAllocationCallbacks);
		}
	} // ~Vulkan swap chain creation

	/////////////////////////////////////////////////////////
	// Runtime

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
	presentInfo.pSwapchains = &vulkan::g_swapChain;
	presentInfo.pImageIndices = nullptr;
	presentInfo.pResults = nullptr;

	for (size_t i = 0; i < 4096; ++i)
	{
		acquireNextImageResult = vulkan::g_device.vkAcquireNextImageKHR(
			vulkan::g_device.device,
			vulkan::g_swapChain,
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
	}

	/////////////////////////////////////////////////////////
	// Deconstruction
	vulkan::DestroyState();
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
	return eRR_Success;
}

} // platform namespace

#pragma warning(pop)
