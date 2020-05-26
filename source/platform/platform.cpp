/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <SDL.h>
#include <SDL_vulkan.h>
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
	EState state = EState::Uninitialized;
};

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

// TODO: Add allocation callbacks for debugging
static const VkAllocationCallbacks* g_pAllocationCallbacks = nullptr;
static SDevice g_device;
static SInstance g_instance;
static VkSurfaceKHR g_presentationSurface = VK_NULL_HANDLE;
static VkSemaphore g_imageAvailableSemaphore = VK_NULL_HANDLE;
static VkSemaphore g_renderingFinishedSemaphore = VK_NULL_HANDLE;

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

void DestroyState()
{
	assert(g_device.state == SDevice::EState::Initialized);
	assert(g_device.device != VK_NULL_HANDLE);
	assert(g_instance.state == SInstance::EState::Initialized);
	assert(g_instance.instance != VK_NULL_HANDLE);
	assert(g_presentationSurface != VK_NULL_HANDLE);

	g_device.vkDeviceWaitIdle(g_device.device);
	g_device.vkDestroyDevice(g_device.device, g_pAllocationCallbacks);

	vkDestroyInstance(g_instance.instance, g_pAllocationCallbacks);
	SDL_Vulkan_UnloadLibrary();
	g_device.state = SDevice::EState::Garbage;
	g_instance.state = SInstance::EState::Garbage;
	g_presentationSurface = VK_NULL_HANDLE;
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
			VkDeviceQueueCreateInfo& queueCreateInfo = createInfoArray[0];
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

		if (vkCreateSemaphore(vulkan::g_device.device, &semaphoreCreateInfo, nullptr, &vulkan::g_imageAvailableSemaphore) != VK_SUCCESS)
		{
			puts("Vulkan unable to create image available semaphore!");
			return eRR_Error;
		}

		if (vkCreateSemaphore(vulkan::g_device.device, &semaphoreCreateInfo, nullptr, &vulkan::g_renderingFinishedSemaphore) != VK_SUCCESS)
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
	} // ~Vulkan swap chain creation

	/////////////////////////////////////////////////////////
	// Runtime

	/////////////////////////////////////////////////////////
	// Deconstruction
	vulkan::DestroyState();
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
	return eRR_Success;
}

} // platform namespace

#pragma warning(pop)
