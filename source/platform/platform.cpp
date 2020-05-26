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

	SDevice() {}

	SDevice(
		VkDevice _device,
		PFN_vkGetDeviceQueue _vkGetDeviceQueue,
		PFN_vkDestroyDevice _vkDestroyDevice,
		PFN_vkDeviceWaitIdle _vkDeviceWaitIdle,
		uint32_t _queueFamilyIndex)
		: device(_device)
		, vkGetDeviceQueue(_vkGetDeviceQueue)
		, vkDestroyDevice(_vkDestroyDevice)
		, vkDeviceWaitIdle(_vkDeviceWaitIdle)
		, queueFamilyIndex(_queueFamilyIndex)
	{
		assert(vkGetDeviceQueue != nullptr);
		assert(vkDestroyDevice != nullptr);
		assert(vkDeviceWaitIdle != nullptr);
	}

	VkDevice device = VK_NULL_HANDLE;
	PFN_vkGetDeviceQueue vkGetDeviceQueue = nullptr;
	PFN_vkDestroyDevice vkDestroyDevice = nullptr;
	PFN_vkDeviceWaitIdle vkDeviceWaitIdle = nullptr;
	VkQueue queue = VK_NULL_HANDLE;
	uint32_t queueFamilyIndex = INVALID_QUEUE_FAMILY_PROPERTIES_INDEX;
	EState state = EState::Uninitialized;
};

// TODO: Add allocation callbacks for debugging
static const VkAllocationCallbacks* g_pAllocationCallbacks = nullptr;
static SDevice g_device;
static VkInstance g_instance = VK_NULL_HANDLE;

/////////////////////////////////////////////////////////
// Functions

void SetDevice(const SDevice& device)
{
	assert(g_device.state == SDevice::EState::Uninitialized);
	assert(device.state == SDevice::EState::Uninitialized);
	g_device = device;
	// TODO: consider if we need more queues than just the one
	g_device.vkGetDeviceQueue(g_device.device, g_device.queueFamilyIndex, 0, &g_device.queue);
	assert(g_device.queue != nullptr);
	g_device.state = SDevice::EState::Initialized;
}

bool CheckPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamiliesIndex)
{
	assert(pQueueFamiliesIndex != nullptr);
	*pQueueFamiliesIndex = INVALID_QUEUE_FAMILY_PROPERTIES_INDEX;
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;

	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
	vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

	const uint32_t majorVersion = VK_VERSION_MAJOR(deviceProperties.apiVersion);
	const uint32_t minorVersion = VK_VERSION_MINOR(deviceProperties.apiVersion);
	const uint32_t patchVersion = VK_VERSION_PATCH(deviceProperties.apiVersion);

	// TODO: Add beter properties and feature testing, as well as enabling tested features on logical device
	if (majorVersion < 1 && deviceProperties.limits.maxImageDimension2D < 4096)
	{
		return false;
	}

	uint32_t queueFamiliesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
	if (queueFamiliesCount == 0)
	{
		puts("Vulkan failed to get queue family properties count!");
		return false;
	}

	VkQueueFamilyProperties* pQueueFamilyProperties = (VkQueueFamilyProperties*)alloca(sizeof(VkQueueFamilyProperties) * queueFamiliesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, pQueueFamilyProperties);
	for (uint32_t i = 0; i < queueFamiliesCount; ++i)
	{
		// TODO: Add better queue family properties testing?
		if (pQueueFamilyProperties[i].queueCount > 0 && pQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			*pQueueFamiliesIndex = i;
			return true;
		}
	}

	return false;
}

void DestroyState()
{
	assert(g_device.state == SDevice::EState::Initialized);
	assert(g_device.device != VK_NULL_HANDLE);

	g_device.vkDeviceWaitIdle(g_device.device);
	g_device.vkDestroyDevice(g_device.device, g_pAllocationCallbacks);

	assert(g_instance != VK_NULL_HANDLE);
	vkDestroyInstance(g_instance, g_pAllocationCallbacks);
	SDL_Vulkan_UnloadLibrary();
	g_device.state = SDevice::EState::Garbage;
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


	{ // Vulkan initialization
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

		VkResult res = vkCreateInstance(&info, vulkan::g_pAllocationCallbacks, &vulkan::g_instance);
		if (res != VK_SUCCESS)
		{
			printf("Vulkcan could not create instance!\n");
			return eRR_Error;
		}
	} // ~Vulkan Initialization

	VkSurfaceKHR surface;
	if (SDL_Vulkan_CreateSurface(pWindow, vulkan::g_instance, &surface) == SDL_FALSE)
	{
		printf("SDL could not create window surface! SDL_Error: %s\n", SDL_GetError());
		return eRR_Error;
	}


	{ // Create logical devices
		VkDevice vkDevice;
		uint32_t queueFamilyIndex = INVALID_QUEUE_FAMILY_PROPERTIES_INDEX;
		uint32_t numDevices = 0;
		if (vkEnumeratePhysicalDevices(vulkan::g_instance, &numDevices, nullptr) != VK_SUCCESS)
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
		if (vkEnumeratePhysicalDevices(vulkan::g_instance, &numDevices, physicalDevices) != VK_SUCCESS)
		{
			puts("Vulkan failed to enumerate physical devices!");
			return eRR_Error;
		}

		// TODO: Select best fit physical device based on features/type
		VkPhysicalDevice selectedPhysicalDevice = VK_NULL_HANDLE;
		for (uint32_t i = 0; i < numDevices; ++i)
		{
			if (vulkan::CheckPhysicalDeviceProperties(physicalDevices[i], &queueFamilyIndex))
			{
				selectedPhysicalDevice = physicalDevices[i];
			}
		}

		if (selectedPhysicalDevice == VK_NULL_HANDLE)
		{
			puts("Failed to find a compatible vulkan physical device!");
			return eRR_Error;
		}

		if (queueFamilyIndex == INVALID_QUEUE_FAMILY_PROPERTIES_INDEX)
		{
			puts("Unable to find appropriate queue family properties");
			return eRR_Error;
		}

		// TODO: Manage multiple queue families/priorities if necessary
		static constexpr uint32_t queueCount = 1;
		const float queuePriorities[queueCount] = { 1.0f };

		VkDeviceQueueCreateInfo queueCreateInfo;
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.pNext = nullptr;
		queueCreateInfo.flags = 0;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
		queueCreateInfo.queueCount = queueCount;
		queueCreateInfo.pQueuePriorities = queuePriorities;

		VkDeviceCreateInfo deviceCreateInfo;
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = nullptr;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
		deviceCreateInfo.enabledLayerCount = 0;
		deviceCreateInfo.ppEnabledLayerNames = nullptr;
		deviceCreateInfo.enabledExtensionCount = 0;
		deviceCreateInfo.ppEnabledExtensionNames = nullptr;
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

		const PFN_vkGetDeviceQueue vkGetDeviceQueue = (PFN_vkGetDeviceQueue)vkGetDeviceProcAddr(vkDevice, "vkGetDeviceQueue");
		const PFN_vkDestroyDevice vkDestroyDevice = (PFN_vkDestroyDevice)vkGetDeviceProcAddr(vkDevice, "vkDestroyDevice");
		const PFN_vkDeviceWaitIdle vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle)vkGetDeviceProcAddr(vkDevice, "vkDeviceWaitIdle");

		vulkan::SetDevice(vulkan::SDevice(vkDevice, vkGetDeviceQueue, vkDestroyDevice, vkDeviceWaitIdle, queueFamilyIndex));
	} // ~logical device creation

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
