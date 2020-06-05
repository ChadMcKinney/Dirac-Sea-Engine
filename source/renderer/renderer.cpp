/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "diracsea.h"
#include "renderer.h"

#include <array>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <Vulkan.h>

#include "math/matrix44.h"
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
	x(vkDestroyImageView)\
	x(vkCreateFence)\
	x(vkCreateBuffer)\
	x(vkGetBufferMemoryRequirements)\
	x(vkAllocateMemory)\
	x(vkBindBufferMemory)\
	x(vkMapMemory)\
	x(vkFlushMappedMemoryRanges)\
	x(vkUnmapMemory)\
	x(vkCmdSetViewport)\
	x(vkCmdSetScissor)\
	x(vkCmdBindVertexBuffers)\
	x(vkWaitForFences)\
	x(vkResetFences)\
	x(vkFreeMemory)\
	x(vkDestroyBuffer)\
	x(vkDestroyFence)\
	x(vkCmdCopyBuffer)\
	x(vkCreateImage)\
	x(vkGetImageMemoryRequirements)\
	x(vkBindImageMemory)\
	x(vkCreateSampler)\
	x(vkCmdCopyBufferToImage)\
	x(vkCreateDescriptorSetLayout)\
	x(vkCreateDescriptorPool)\
	x(vkAllocateDescriptorSets)\
	x(vkUpdateDescriptorSets)\
	x(vkCmdBindDescriptorSets)\
	x(vkDestroyDescriptorPool)\
	x(vkDestroyDescriptorSetLayout)\
	x(vkDestroySampler)\
	x(vkDestroyImage)

namespace vulkan
{
/////////////////////////////////////////////////////////
// Constants
static constexpr uint32_t INVALID_QUEUE_FAMILY_PROPERTIES_INDEX = UINT32_MAX;
static constexpr size_t MAX_IMAGE_COUNT = 4;
static constexpr size_t MAX_COMMAND_BUFFER_COUNT = MAX_IMAGE_COUNT;
static constexpr size_t RENDER_RESOURCES_COUNT = 3;
static_assert(RENDER_RESOURCES_COUNT <= MAX_COMMAND_BUFFER_COUNT);

static constexpr VkDeviceSize kMatrix44Alignment = 4;
static constexpr VkDeviceSize kMatrix43Alignment = 4;
static constexpr VkDeviceSize kMatrix33Alignment = 4;
static constexpr VkDeviceSize kMatrix22Alignment = 2;

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
	VkDeviceSize memoryAlignment = { 0 };
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
// SPrepareFrame
struct SPrepareFrame // state used during frame preparation
{
	SPrepareFrame()
	{
		memset(this, 0, sizeof(SPrepareFrame));

		// frameBufferCreateInfo 
		frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferCreateInfo.pNext = nullptr;
		frameBufferCreateInfo.flags = 0;
		frameBufferCreateInfo.renderPass = VK_NULL_HANDLE;
		frameBufferCreateInfo.attachmentCount = 1;
		frameBufferCreateInfo.pAttachments = nullptr;
		frameBufferCreateInfo.width = 0;
		frameBufferCreateInfo.height = 0;
		frameBufferCreateInfo.layers = 1;

		// commandBufferBeginInfo 
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		// imageSubresourceRange
		imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageSubresourceRange.baseMipLevel = 0;
		imageSubresourceRange.levelCount = 1;
		imageSubresourceRange.baseArrayLayer = 0;
		imageSubresourceRange.layerCount = 1;

		// barrierPresentToDraw
		barrierPresentToDraw.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrierPresentToDraw.pNext = nullptr;
		barrierPresentToDraw.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		barrierPresentToDraw.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrierPresentToDraw.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrierPresentToDraw.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		barrierPresentToDraw.srcQueueFamilyIndex = UINT32_MAX;
		barrierPresentToDraw.dstQueueFamilyIndex = UINT32_MAX;
		barrierPresentToDraw.image = VK_NULL_HANDLE;
		barrierPresentToDraw.subresourceRange = imageSubresourceRange;

		// clearColor 
		clearColor = { 0.1f, 0.4f, 0.4f, 0.0f };

		// renderPassBeginInfo 
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = VK_NULL_HANDLE;
		renderPassBeginInfo.framebuffer = VK_NULL_HANDLE;
		renderPassBeginInfo.renderArea.extent = { 0, 0 };
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		// viewport 
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = 0.0f;
		viewport.height = 0.0f;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		// scissor 
		scissor.offset = { 0, 0 };
		scissor.extent = { 0, 0 };

		// barrierDrawToPresent 
		barrierDrawToPresent.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrierDrawToPresent.pNext = nullptr;
		barrierDrawToPresent.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrierDrawToPresent.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		barrierDrawToPresent.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		barrierDrawToPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		barrierDrawToPresent.srcQueueFamilyIndex = UINT32_MAX;
		barrierDrawToPresent.dstQueueFamilyIndex = UINT32_MAX;
		barrierDrawToPresent.image = VK_NULL_HANDLE;
		barrierDrawToPresent.subresourceRange = imageSubresourceRange;
	}

	VkFramebufferCreateInfo  frameBufferCreateInfo;
	VkCommandBufferBeginInfo commandBufferBeginInfo;
	VkImageSubresourceRange imageSubresourceRange;
	VkClearValue clearColor;
	VkImageMemoryBarrier barrierPresentToDraw;
	VkRenderPassBeginInfo renderPassBeginInfo;
	VkViewport viewport;
	VkRect2D scissor;
	VkImageMemoryBarrier barrierDrawToPresent;
};

///////////////////////////
// SVertexData
struct SVertexData
{
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 0;
	float u = 0;
	float v = 0;
};

///////////////////////////
// SBuffer
struct SBuffer
{
	VkBuffer handle = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkDeviceSize size = 0;
};

///////////////////////////
// SRenderResources
struct SRenderResources
{
	VkFramebuffer frameBuffer = VK_NULL_HANDLE;
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
	VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
	VkSemaphore renderingFinishedSemaphore = VK_NULL_HANDLE;
	VkFence fence = VK_NULL_HANDLE;
};

///////////////////////////
// SDescriptorSet
struct SDescriptorSet
{
	VkDescriptorSet handle = VK_NULL_HANDLE;
	VkDescriptorPool pool = VK_NULL_HANDLE;
	VkDescriptorSetLayout layout = VK_NULL_HANDLE;
};

// TODO: Add allocation callbacks for debugging
static const VkAllocationCallbacks* g_pAllocationCallbacks = nullptr;

static SDevice g_device;
static SInstance g_instance;

static VkSurfaceKHR g_presentationSurface = VK_NULL_HANDLE;
static SSwapChain g_swapChain;

static VkCommandPool g_graphicsCommandPool = VK_NULL_HANDLE;
static SPrepareFrame g_prepareFrameState;

static SRenderResources g_renderResources[RENDER_RESOURCES_COUNT];

static VkRenderPass g_renderPass = VK_NULL_HANDLE;
static VkPipelineLayout g_pipelineLayout = VK_NULL_HANDLE;
static VkPipeline g_graphicsPipeline = VK_NULL_HANDLE;
static SBuffer g_vertexBuffer;
static SBuffer g_uniformBuffer;
static SBuffer g_stagingBuffer;
static SImage g_image;
static SDescriptorSet g_descriptorSet;

///////////////////////////
// Shader Bank

// Takes a macro list and creates an enum and an array of vertex and fragment shader file names
// provides a static class instance for loading/unloading the shader files, 
// creating/destroying shader modules and querying by enum for O(1) access

// usage:
// #define MY_SHADERS_LIST(x)\
// 	x(shader1)
// 	x(shader2)
//
// SHADER_BANK(MyShaders, MY_SHADERS_LIST)
//
// MyShaders.LoadFiles();
// MyShaders.GetVertexShader(EMyShaders::Enum::shader1);
// MyShaders.GetFragmentShader(EMyShaders::Enum::shader1);
// MyShaders.CreateShaderModules();
// MyShaders.DestroyShaderModules();
// MyShaders.UnloadFiles();

#ifndef _RELEASE
#define PRINT_SHADERS_ON_LOAD 0
#endif

VkShaderModule CreateShaderModule(const char* fileName, const platform::SFile& shaderFile);

template <typename Enum, size_t EnumCount>
class SShaderBankState
{
public:
	SShaderBankState(const char** _vertexShaderFilePaths, const char** _fragmentShaderFilePaths)
		: m_vertexShaderFilePaths(_vertexShaderFilePaths)
		, m_fragmentShaderFilePaths(_fragmentShaderFilePaths)
	{
		assert(m_vertexShaderFilePaths);
		assert(m_fragmentShaderFilePaths);
	}

	~SShaderBankState()
	{
		assert(m_bShaderModulesCreated == false);
		assert(m_bShadersLoaded == false);
	}

	bool UnloadFiles()
	{
		bool bSuccess = m_bShadersLoaded;
		for (size_t i = 0; i < EnumCount; ++i)
		{
			m_vertexShaders[i] = platform::SFile();
			m_fragmentShaders[i] = platform::SFile();
		}
		m_bShadersLoaded = false;
		return bSuccess;
	}

	bool LoadFiles()
	{
		assert(m_bShadersLoaded == false);
		m_bShadersLoaded = true;
		bool bVertexFilesLoaded = platform::LoadFiles(m_vertexShaderFilePaths, EnumCount, platform::EFileType::Binary, m_vertexShaders.data());
		if (bVertexFilesLoaded)
		{
			bool bFragFilesLoaded = platform::LoadFiles(m_fragmentShaderFilePaths, EnumCount, platform::EFileType::Binary, m_fragmentShaders.data());
#if PRINT_SHADERS_ON_LOAD
			if (bFragFilesLoaded)
			{
				for (size_t i = 0; i < EnumCount; ++i)
				{
					puts("=====================================================");
					printf("Shader: %s\n", m_vertexShaderFilePaths[i]);
					const size_t numBytes = m_vertexShaders[i].numBytes;
					const char* pData = m_vertexShaders[i].pData.get();
					for (size_t j = 0; j < numBytes; ++j)
					{
						printf("%c", pData[j]);
					}
					puts("\n=====================================================");
				}
			}
#endif
			if (bFragFilesLoaded)
				return true;
		}
		UnloadFiles();
		m_bShadersLoaded = false;
		return false;
	}

	bool DestroyShaderModules()
	{
		assert(m_bShadersLoaded == true);
		bool bSuccess = m_bShaderModulesCreated;
		for (size_t i = 0; i < EnumCount; ++i)
		{
			g_device.vkDestroyShaderModule(g_device.handle, m_vertexShaderModules[i], g_pAllocationCallbacks);
			m_vertexShaderModules[i] = VK_NULL_HANDLE;
			g_device.vkDestroyShaderModule(g_device.handle, m_fragmentShaderModules[i], g_pAllocationCallbacks);
			m_fragmentShaderModules[i] = VK_NULL_HANDLE;
		}
		m_bShaderModulesCreated = false;
		return bSuccess;
	}

	bool CreateShaderModules()
	{
		assert(m_bShadersLoaded == true);
		assert(m_bShaderModulesCreated == false);
		bool bSuccess = true;
		for (size_t i = 0; i < EnumCount && bSuccess; ++i)
		{
			m_vertexShaderModules[i] = CreateShaderModule(m_vertexShaderFilePaths[i], m_vertexShaders[i]);
			m_fragmentShaderModules[i] = CreateShaderModule(m_fragmentShaderFilePaths[i], m_fragmentShaders[i]);
			bSuccess &= m_vertexShaderModules[i] != VK_NULL_HANDLE && m_fragmentShaderModules[i] != VK_NULL_HANDLE;
		}

		m_bShaderModulesCreated = true;
		if (bSuccess == false)
		{
			DestroyShaderModules();
			return false;
		}

		return true;
	}

	inline const char* GetVertexShaderFilePath(Enum shaderEnum)
	{
		assert((size_t)shaderEnum < EnumCount);
		assert(m_bShadersLoaded);
		return m_vertexShaderFilePaths[(size_t)shaderEnum];
	}

	inline const char* GetFragmentShaderFilePath(Enum shaderEnum)
	{
		assert((size_t)shaderEnum < EnumCount);
		assert(m_bShadersLoaded);
		return m_fragmentShaderFilePaths[(size_t)shaderEnum];
	}

	inline const platform::SFile& GetVertexShader(Enum shaderEnum)
	{
		assert((size_t)shaderEnum < EnumCount);
		assert(m_bShadersLoaded);
		return m_vertexShaders[(size_t)shaderEnum];
	}

	inline const platform::SFile& GetFragmentShader(Enum shaderEnum)
	{
		assert((size_t)shaderEnum < EnumCount);
		assert(m_bShadersLoaded);
		return m_fragmentShaders[(size_t)shaderEnum];
	}

	inline VkShaderModule GetVertexShaderModule(Enum shaderEnum)
	{
		assert((size_t)shaderEnum < m_vertexShaderModules.size());
		assert(m_bShaderModulesCreated);
		assert(m_vertexShaderModules[(size_t)shaderEnum] != VK_NULL_HANDLE);
		return m_vertexShaderModules[(size_t)shaderEnum];
	}

	inline VkShaderModule GetFragmentShaderModule(Enum shaderEnum)
	{
		assert((size_t)shaderEnum < m_fragmentShaderModules.size());
		assert(m_bShaderModulesCreated);
		assert(m_fragmentShaderModules[(size_t)shaderEnum] != VK_NULL_HANDLE);
		return m_fragmentShaderModules[(size_t)shaderEnum];
	}

private:
	const char** m_vertexShaderFilePaths;
	const char** m_fragmentShaderFilePaths;
	std::array<platform::SFile, EnumCount> m_vertexShaders = { platform::SFile() };
	std::array<platform::SFile, EnumCount> m_fragmentShaders = { platform::SFile() };
	std::array<VkShaderModule, EnumCount> m_vertexShaderModules = { VK_NULL_HANDLE };
	std::array<VkShaderModule, EnumCount> m_fragmentShaderModules = { VK_NULL_HANDLE };
	bool m_bShadersLoaded = false;
	bool m_bShaderModulesCreated = false;
};

#define SHADER_BANK_MEMBER_VERT_FILENAME(member) "data/shaders/"#member".vert.spv",
#define SHADER_BANK_MEMBER_FRAG_FILENAME(member) "data/shaders/"#member".frag.spv",

#define SHADER_BANK(name, list)\
	SCOPED_AUTO_ENUM(name, list, uint16_t)\
	namespace __##name {\
		const char* vertexShaderFilePaths[E##name::count] = {\
			list(SHADER_BANK_MEMBER_VERT_FILENAME)\
		};\
		const char* fragmentShaderFilePaths[E##name::count] = {\
			list(SHADER_BANK_MEMBER_FRAG_FILENAME)\
		};\
	}\
	static SShaderBankState<E##name::Enum, E##name::count> name(__##name::vertexShaderFilePaths, __##name::fragmentShaderFilePaths);\

///////////////////////////
// Default Shaders
#define DEFAULT_SHADERS(x)\
	x(test)\
	x(sdf)

SHADER_BANK(DefaultShaders, DEFAULT_SHADERS)

/////////////////////////////////////////////////////////
// Functions

ERunResult SetDevice(
	VkDevice _device,
	VkPhysicalDevice _physicalDevice,
	uint32_t _graphicsQueueFamilyIndex,
	uint32_t _presentQueueFamilyIndex,
	VkDeviceSize _memoryAlignment)
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
	g_device.memoryAlignment = _memoryAlignment;

#define SET_DEVICE_LEVEL_FUNCTION(fun)                                                                            \
		g_device.fun = (PFN_##fun)vkGetDeviceProcAddr(g_device.handle, #fun);                                         \
		if (g_device.fun == nullptr) { printf("Vulkan failed to load device function %s\n", #fun); return eRR_Error; }

	DEVICE_LEVEL_FUNCTIONS(SET_DEVICE_LEVEL_FUNCTION)
#undef SET_DEVICE_LEVEL_FUNCTION

	g_device.vkGetDeviceQueue(g_device.handle, g_device.graphicsQueueFamilyIndex, 0, &g_device.graphicsQueue);
	assert(g_device.graphicsQueue != nullptr);

	g_device.vkGetDeviceQueue(g_device.handle, g_device.presentQueueFamilyIndex, 0, &g_device.presentQueue);
	assert(g_device.presentQueue != nullptr);

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

	g_prepareFrameState.viewport.width = (float) g_swapChain.extent.width;
	g_prepareFrameState.viewport.height = (float) g_swapChain.extent.height;
	g_prepareFrameState.scissor.extent = g_swapChain.extent;
	g_prepareFrameState.renderPassBeginInfo.renderArea.extent = g_swapChain.extent;

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
	uint32_t* pPresentQueueFamiliesIndex,
	VkDeviceSize* memoryAlignment)
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
					*memoryAlignment = deviceProperties.limits.nonCoherentAtomSize;
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
				*memoryAlignment = deviceProperties.limits.nonCoherentAtomSize;
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

bool PrepareFrame(VkCommandBuffer commandBuffer, const SImage& image, VkFramebuffer& outFrameBuffer)
{
	assert(g_device.state == SDevice::EState::Initialized);
	assert(g_swapChain.state == SSwapChain::EState::Initialized);
	assert(g_renderPass != VK_NULL_HANDLE);
	assert(commandBuffer != VK_NULL_HANDLE);
	assert(image.handle != VK_NULL_HANDLE);
	assert(image.view != VK_NULL_HANDLE);

	/////////////////////////////////////////////////////
	// create frame buffer
	{
		if (outFrameBuffer != VK_NULL_HANDLE)
		{
			g_device.vkDestroyFramebuffer(g_device.handle, outFrameBuffer, g_pAllocationCallbacks);
		}

		g_prepareFrameState.frameBufferCreateInfo.renderPass = g_renderPass;
		g_prepareFrameState.frameBufferCreateInfo.pAttachments = &image.view;
		g_prepareFrameState.frameBufferCreateInfo.width = g_swapChain.extent.width;
		g_prepareFrameState.frameBufferCreateInfo.height = g_swapChain.extent.height;
		
		if (g_device.vkCreateFramebuffer(
			g_device.handle,
			&g_prepareFrameState.frameBufferCreateInfo,
			g_pAllocationCallbacks,
			&outFrameBuffer) != VK_NULL_HANDLE)
		{
			puts("Vulkan failed to create frame buffer!");
			return false;
		}
	} // ~create frame buffer
	/////////////////////////////////////////////////////

	/////////////////////////////////////////////////////
	// Record commands
	assert(outFrameBuffer != VK_NULL_HANDLE);

	g_device.vkBeginCommandBuffer(commandBuffer, &g_prepareFrameState.commandBufferBeginInfo);

	uint32_t presentQueueFamilyIndex = vulkan::g_device.presentQueueFamilyIndex;
	uint32_t graphicsQueueFamilyIndex = vulkan::g_device.graphicsQueueFamilyIndex;

	if (presentQueueFamilyIndex == graphicsQueueFamilyIndex)
	{
		presentQueueFamilyIndex = graphicsQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	}

	g_prepareFrameState.barrierPresentToDraw.image = image.handle;
	g_prepareFrameState.barrierPresentToDraw.srcQueueFamilyIndex = presentQueueFamilyIndex;
	g_prepareFrameState.barrierPresentToDraw.dstQueueFamilyIndex = graphicsQueueFamilyIndex;

	g_device.vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		0, // dependencyFlags
		0, // memoryBarrierCount
		nullptr, // pMemoryBarries
		0, // bufferMemoryCount
		nullptr, // pBufferMemoryBarriers
		1, // imageMemoryarrierCount
		&g_prepareFrameState.barrierPresentToDraw);

	g_prepareFrameState.renderPassBeginInfo.renderPass = g_renderPass;
	g_prepareFrameState.renderPassBeginInfo.framebuffer = outFrameBuffer;

	g_device.vkCmdBeginRenderPass(commandBuffer, &g_prepareFrameState.renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	g_device.vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_graphicsPipeline);

	g_device.vkCmdSetViewport(commandBuffer, 0, 1, &g_prepareFrameState.viewport);
	g_device.vkCmdSetScissor(commandBuffer, 0, 1, &g_prepareFrameState.scissor);

	VkDeviceSize offset = 0;
	g_device.vkCmdBindVertexBuffers(
		commandBuffer,
		0, // firstBinding
		1, // bindingCount
		&g_vertexBuffer.handle,
		&offset);

	assert(vulkan::g_pipelineLayout != VK_NULL_HANDLE);
	assert(vulkan::g_descriptorSet.handle != VK_NULL_HANDLE);
	g_device.vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		vulkan::g_pipelineLayout,
		0, // first set
		1, // descriptor set count
		&g_descriptorSet.handle,
		0, // dynamic offset count
		nullptr /* dynamic offsets */);

	g_device.vkCmdDraw(commandBuffer, 4, 1, 0, 0);
	g_device.vkCmdEndRenderPass(commandBuffer);

	g_prepareFrameState.barrierDrawToPresent.image = image.handle;
	g_prepareFrameState.barrierDrawToPresent.srcQueueFamilyIndex = graphicsQueueFamilyIndex;
	g_prepareFrameState.barrierDrawToPresent.dstQueueFamilyIndex = presentQueueFamilyIndex;

	g_device.vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0, // dependencyFlags
		0, // memoryBarrierCount
		nullptr, // pMemoryBarries
		0, // bufferMemoryCount
		nullptr, // pBufferMemoryBarriers
		1, // imageMemoryarrierCount
		&g_prepareFrameState.barrierDrawToPresent);

	if (g_device.vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		puts("Vulkan could not record command buffers!");
		return false;
	}

	return true;
}

VkShaderModule CreateShaderModule(const char* fileName, const platform::SFile& shaderFile)
{
	assert(shaderFile.numBytes > 0);
	assert(shaderFile.pData != nullptr);
	assert(fileName != nullptr);
	VkShaderModuleCreateInfo  shaderModuleCreateInfo;
	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfo.pNext = nullptr;
	shaderModuleCreateInfo.flags = 0;
	shaderModuleCreateInfo.codeSize = shaderFile.numBytes;
	shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderFile.pData.get());

	VkShaderModule shaderModule;
	if (g_device.vkCreateShaderModule(
		g_device.handle,
		&shaderModuleCreateInfo,
		g_pAllocationCallbacks,
		&shaderModule) != VK_SUCCESS)
	{
		printf("Failed to create shader module for shader file: %s\n", fileName);
		return VK_NULL_HANDLE;
	}

	return shaderModule;
}

ERunResult DestroyState()
{
	ERunResult destroyResult = g_device.state == SDevice::EState::Initialized ? eRR_Success : eRR_Error;
	if (g_device.handle != VK_NULL_HANDLE)
	{
		g_device.vkDeviceWaitIdle(g_device.handle);

		for (size_t i = 0; i < RENDER_RESOURCES_COUNT; ++i)
		{
			SRenderResources& resource = g_renderResources[i];
			if (resource.frameBuffer != VK_NULL_HANDLE)
			{
				g_device.vkDestroyFramebuffer(g_device.handle, resource.frameBuffer, g_pAllocationCallbacks);
			}

			if (resource.commandBuffer != VK_NULL_HANDLE)
			{
				g_device.vkFreeCommandBuffers(g_device.handle, g_graphicsCommandPool, 1, &resource.commandBuffer);
				resource.commandBuffer = VK_NULL_HANDLE;
			}
			else
			{
				printf("[%s] commandBuffer is unexpectedly null!\n", __FUNCTION__);
				destroyResult = eRR_Error;
			}

			if (resource.imageAvailableSemaphore != VK_NULL_HANDLE)
			{
				g_device.vkDestroySemaphore(g_device.handle, resource.imageAvailableSemaphore, g_pAllocationCallbacks);
				resource.imageAvailableSemaphore = VK_NULL_HANDLE;
			}
			else
			{
				printf("[%s] imageAvailableSemaphore is unexpectedly null!\n", __FUNCTION__);
				destroyResult = eRR_Error;
			}

			if (resource.renderingFinishedSemaphore != VK_NULL_HANDLE)
			{
				g_device.vkDestroySemaphore(g_device.handle, resource.renderingFinishedSemaphore, g_pAllocationCallbacks);
				resource.renderingFinishedSemaphore = VK_NULL_HANDLE;
			}
			else
			{
				printf("[%s] rendingFinishedSemaphore is unexpectedly null!\n", __FUNCTION__);
				destroyResult = eRR_Error;
			}

			if (resource.fence != VK_NULL_HANDLE)
			{
				g_device.vkDestroyFence(g_device.handle, resource.fence, g_pAllocationCallbacks);
				resource.fence = VK_NULL_HANDLE;
			}
			else
			{
				printf("[%s] fence is unexpectedly null!\n", __FUNCTION__);
				destroyResult = eRR_Error;
			}

			resource = SRenderResources();
		}

		if (g_graphicsCommandPool != VK_NULL_HANDLE)
		{
			g_device.vkDestroyCommandPool(g_device.handle, g_graphicsCommandPool, g_pAllocationCallbacks);
			g_graphicsCommandPool = VK_NULL_HANDLE;
		}
		else
		{
			printf("[%s] g_graphicsCommandPool is unexpectedly null!\n", __FUNCTION__);
			destroyResult = eRR_Error;
		}

		if (g_vertexBuffer.handle != VK_NULL_HANDLE)
		{
			g_device.vkDestroyBuffer(g_device.handle, g_vertexBuffer.handle, g_pAllocationCallbacks);
			g_vertexBuffer.handle = VK_NULL_HANDLE;
		}
		else
		{
			printf("[%s] g_verteBuffer.handle is unexpectedly null!\n", __FUNCTION__);
			destroyResult = eRR_Error;
		}

		if (g_vertexBuffer.memory != VK_NULL_HANDLE)
		{
			g_device.vkFreeMemory(g_device.handle, g_vertexBuffer.memory, g_pAllocationCallbacks);
			g_vertexBuffer.memory = VK_NULL_HANDLE;
		}
		else
		{
			printf("[%s] g_verteBuffer.memory is unexpectedly null!\n", __FUNCTION__);
			destroyResult = eRR_Error;
		}

		if (g_stagingBuffer.handle != VK_NULL_HANDLE)
		{
			g_device.vkDestroyBuffer(g_device.handle, g_stagingBuffer.handle, g_pAllocationCallbacks);
			g_stagingBuffer.handle = VK_NULL_HANDLE;
		}
		else
		{
			printf("[%s] g_stagingBuffer.handle is unexpectedly null!\n", __FUNCTION__);
			destroyResult = eRR_Error;
		}

		if (g_stagingBuffer.memory != VK_NULL_HANDLE)
		{
			g_device.vkFreeMemory(g_device.handle, g_stagingBuffer.memory, g_pAllocationCallbacks);
			g_stagingBuffer.memory = VK_NULL_HANDLE;
		}
		else
		{
			printf("[%s] g_stagingBuffer.handle is unexpectedly null!\n", __FUNCTION__);
			destroyResult = eRR_Error;
		}

		if (g_pipelineLayout != VK_NULL_HANDLE)
		{
			g_device.vkDestroyPipelineLayout(g_device.handle, g_pipelineLayout, g_pAllocationCallbacks);
			g_pipelineLayout = VK_NULL_HANDLE;
		}
		else
		{
			printf("[%s] g_pipelineLayout is unexpectedly null!\n", __FUNCTION__);
		}

		if (g_graphicsPipeline != VK_NULL_HANDLE)
		{
			g_device.vkDestroyPipeline(g_device.handle, g_graphicsPipeline, g_pAllocationCallbacks);
			g_graphicsPipeline = VK_NULL_HANDLE;
		}
		else
		{
			printf("[%s] g_graphicsPipeline is unexpectedly null!\n", __FUNCTION__);
			destroyResult = eRR_Error;
		}

		if (DefaultShaders.DestroyShaderModules() == false)
		{
			puts("[Renderer] Unloading destroying default shader modules found unexpected behavior!");
			destroyResult = eRR_Error;
		}

		if (g_renderPass != VK_NULL_HANDLE)
		{
			g_device.vkDestroyRenderPass(g_device.handle, g_renderPass, g_pAllocationCallbacks);
			g_renderPass = VK_NULL_HANDLE;
		}
		else
		{
			printf("[%s] g_renderPass is unexpectedly null!\n", __FUNCTION__);
			destroyResult = eRR_Error;
		}

		if (g_descriptorSet.pool != VK_NULL_HANDLE)
		{
			g_device.vkDestroyDescriptorPool(g_device.handle, g_descriptorSet.pool, g_pAllocationCallbacks);
			g_descriptorSet.pool = VK_NULL_HANDLE;
		}
		else
		{
			printf("[%s] g_descriptorSet.pool is unexpectedly null!\n", __FUNCTION__);
			destroyResult = eRR_Error;
		}

		if (g_descriptorSet.layout != VK_NULL_HANDLE)
		{
			g_device.vkDestroyDescriptorSetLayout(g_device.handle, g_descriptorSet.layout, g_pAllocationCallbacks);
			g_descriptorSet.layout = VK_NULL_HANDLE;
		}
		else
		{
			printf("[%s] g_descriptorSet.layout is unexpectedly null!\n", __FUNCTION__);
			destroyResult = eRR_Error;
		}

		if (g_uniformBuffer.handle != VK_NULL_HANDLE)
		{
			g_device.vkDestroyBuffer(g_device.handle, g_uniformBuffer.handle, g_pAllocationCallbacks);
			g_uniformBuffer.handle = VK_NULL_HANDLE;
		}
		else
		{
			printf("[%s] g_uniformBuffer.handle is unexpectedly null!\n", __FUNCTION__);
			destroyResult = eRR_Error;
		}

		if (g_uniformBuffer.memory != VK_NULL_HANDLE)
		{
			g_device.vkFreeMemory(g_device.handle, g_uniformBuffer.memory, g_pAllocationCallbacks);
			g_stagingBuffer.memory = VK_NULL_HANDLE;
		}
		else
		{
			printf("[%s] g_uniformBuffer.memory is unexpectedly null!\n", __FUNCTION__);
			destroyResult = eRR_Error;
		}

		if (g_image.sampler != VK_NULL_HANDLE)
		{
			g_device.vkDestroySampler(g_device.handle, g_image.sampler, g_pAllocationCallbacks);
			g_image.sampler = VK_NULL_HANDLE;
		}
		else
		{
			printf("[%s] g_image.sampler is unexpectedly null!\n", __FUNCTION__);
			destroyResult = eRR_Error;
		}

		if (g_image.view != VK_NULL_HANDLE)
		{
			g_device.vkDestroyImageView(g_device.handle, g_image.view, g_pAllocationCallbacks);
			g_image.view = VK_NULL_HANDLE;
		}
		else
		{
			printf("[%s] g_image.view is unexpectedly null!\n", __FUNCTION__);
			destroyResult = eRR_Error;
		}

		if (g_image.handle != VK_NULL_HANDLE)
		{
			g_device.vkDestroyImage(g_device.handle, g_image.handle, g_pAllocationCallbacks);
			g_image.handle = VK_NULL_HANDLE;
		}
		else
		{
			printf("[%s] g_image.handle is unexpectedly null!\n", __FUNCTION__);
			destroyResult = eRR_Error;
		}

		if (g_image.memory != VK_NULL_HANDLE)
		{
			g_device.vkFreeMemory(g_device.handle, g_image.memory, g_pAllocationCallbacks);
			g_image.memory = VK_NULL_HANDLE;
		}
		else
		{
			printf("[%s] g_image.memory is unexpectedly null!\n", __FUNCTION__);
			destroyResult = eRR_Error;
		}

		for (uint32_t i = 0; i < g_swapChain.imageCount; ++i)
		{
			SImage& image = g_swapChain.images[i];
			if (g_swapChain.images[i].view != VK_NULL_HANDLE)
			{
				g_device.vkDestroyImageView(g_device.handle, image.view, g_pAllocationCallbacks);
			}

			g_swapChain.images[i] = SImage();
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
		printf("[%s] g_device is unexpectedly null!\n", __FUNCTION__);
		destroyResult = eRR_Error;
	}

	if (g_instance.state != SInstance::EState::Initialized)
	{
		printf("[%s] g_instance is uninitialized!\n", __FUNCTION__);
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
	else
	{
		printf("[%s] g_instance is unexpectedly null!\n", __FUNCTION__);
		destroyResult = eRR_Error;
	}

	g_instance.handle = VK_NULL_HANDLE;
	g_instance.state = SInstance::EState::Garbage;

	SDL_Vulkan_UnloadLibrary();

	if (DefaultShaders.UnloadFiles() == false)
	{
		puts("[Renderer] Unloading default shader files found unexpected behavior!");
		destroyResult = eRR_Error;
	}

	return destroyResult;
}

bool CreateBuffer(
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlagBits memoryProperty,
	VkDeviceSize size,
	SBuffer& outBuffer)
{
	assert(g_device.handle != VK_NULL_HANDLE);
	assert(size > 0);
	assert(outBuffer.handle == VK_NULL_HANDLE);
	assert(memoryProperty != 0);

	VkBufferCreateInfo bufferCreateInfo;
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = nullptr;

	VkBuffer bufferHandle;
	if (vulkan::g_device.vkCreateBuffer(
		vulkan::g_device.handle,
		&bufferCreateInfo,
		vulkan::g_pAllocationCallbacks,
		&bufferHandle) != VK_SUCCESS)
	{
		puts("Vulkan failed to created vertex buffer!");
		return eRR_Error;
	}

	assert(bufferHandle);
	outBuffer.handle = bufferHandle;
	outBuffer.size = size;

	////////////////////////////
	{ // allocate buffer memory
		bool bMemoryAllocated = false;
		VkMemoryRequirements bufferMemoryRequirements;
		vulkan::g_device.vkGetBufferMemoryRequirements(vulkan::g_device.handle, bufferHandle, &bufferMemoryRequirements);

		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(vulkan::g_device.physicalDevice, &memoryProperties);

		VkMemoryAllocateInfo memoryAllocateInfo;
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = bufferMemoryRequirements.size;
		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
		{
			if ((bufferMemoryRequirements.memoryTypeBits & BIT(i)) &&
				(memoryProperties.memoryTypes[i].propertyFlags & memoryProperty))
			{
				memoryAllocateInfo.memoryTypeIndex = i;
				if (vulkan::g_device.vkAllocateMemory(
					vulkan::g_device.handle,
					&memoryAllocateInfo,
					vulkan::g_pAllocationCallbacks,
					&outBuffer.memory) == VK_SUCCESS)
				{
					bMemoryAllocated = true;
					break;
				}
			}
		}

		if (!bMemoryAllocated)
		{
			puts("Vulkan failed to allocate memory for vertex buffer!");
			return false;
		}
	} // ~allocate buffer memory
	////////////////////////////

	assert(outBuffer.memory != VK_NULL_HANDLE);
	if (vulkan::g_device.vkBindBufferMemory(
		vulkan::g_device.handle,
		outBuffer.handle,
		outBuffer.memory,
		0) != VK_SUCCESS)
	{
		puts("Vulkan failed to bind vertex buffer memory!");
		return false;
	}

	return true;
}

bool CreateTexture()
{
	assert(g_device.handle != VK_NULL_HANDLE);
	assert(g_device.state == SDevice::EState::Initialized);
	platform::ImageSurfacePtr pImage = platform::LoadImage("data/images/tentacle.bmp");
	if (!pImage)
	{
		printf("[%s] Failed to load texture!\n", __FUNCTION__);
		return false;
	}

	static const uint32_t kRGBASize = sizeof(uint32_t);
	const uint32_t formatPixelSize = pImage->format->BytesPerPixel;
	const VkFormat imageFormat = VK_FORMAT_R8G8B8A8_UNORM;

	assert(pImage->w > 0);
	assert(pImage->h > 0);

	/////////////////////////////////
	{ // create image
		VkImageCreateInfo imageCreateInfo;
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = nullptr;
		imageCreateInfo.flags = 0;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = imageFormat;
		imageCreateInfo.extent = { (uint32_t)pImage->w, (uint32_t)pImage->h, 1 };
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount = 0;
		imageCreateInfo.pQueueFamilyIndices = nullptr;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		if (g_device.vkCreateImage(
			g_device.handle,
			&imageCreateInfo,
			g_pAllocationCallbacks,
			&g_image.handle) != VK_SUCCESS)
		{
			printf("[%s] Vulkan failed to create image!\n", __FUNCTION__);
			return false;
		}
	} // ~create image
	/////////////////////////////////

	/////////////////////////////////
	{ // allocate image memory
		assert(g_image.handle != VK_NULL_HANDLE);

		const VkMemoryPropertyFlagBits memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		VkMemoryRequirements imageMemoryRequirements;
		vkGetImageMemoryRequirements(g_device.handle, g_image.handle, &imageMemoryRequirements);

		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(g_device.physicalDevice, &memoryProperties);

		VkMemoryAllocateInfo memoryAllocateInfo;
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = nullptr;
		memoryAllocateInfo.allocationSize = imageMemoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = 0; // filled out in loop below

		bool bAllocatedMemory = false;
		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
		{
			if ((imageMemoryRequirements.memoryTypeBits & BIT(i)) &&
				(memoryProperties.memoryTypes[i].propertyFlags & memoryProperty))
			{
				memoryAllocateInfo.memoryTypeIndex = i;
				if (g_device.vkAllocateMemory(
					g_device.handle,
					&memoryAllocateInfo,
					g_pAllocationCallbacks,
					&g_image.memory) == VK_SUCCESS)
				{
					bAllocatedMemory = true;
					break;
				}
			}
		}

		if (bAllocatedMemory == false)
		{
			printf("[%s] Failed to allocate memory for image!\n", __FUNCTION__);
			return false;
		}
	} // ~allocate image memory
	/////////////////////////////////

	/////////////////////////////////
	{ // bind image memory
		assert(g_image.handle != VK_NULL_HANDLE);
		assert(g_image.memory != VK_NULL_HANDLE);
		if (g_device.vkBindImageMemory(
			g_device.handle,
			g_image.handle,
			g_image.memory,
			0 /* memory offset */) != VK_SUCCESS)
		{
			printf("[%s] Vulkan failed to bind image memory!\n", __FUNCTION__);
			return false;
		}
	} // ~bind image memory
	/////////////////////////////////

	/////////////////////////////////
	{ // create image view
		VkImageViewCreateInfo imageViewCreateInfo;
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = g_image.handle;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = imageFormat;

		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		if (g_device.vkCreateImageView(
			g_device.handle,
			&imageViewCreateInfo,
			g_pAllocationCallbacks,
			&g_image.view) != VK_SUCCESS)
		{
			printf("[%s] Failed to create image view!\n", __FUNCTION__);
			return false;
		}
	} // ~create image view
	/////////////////////////////////

	/////////////////////////////////
	{ // create sampler
		VkSamplerCreateInfo samplerCreateInfo;
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.pNext = nullptr;
		samplerCreateInfo.flags = 0;
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.anisotropyEnable = VK_FALSE;
		samplerCreateInfo.maxAnisotropy = 1.0f;
		samplerCreateInfo.compareEnable = VK_FALSE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = 0.0f;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

		if (g_device.vkCreateSampler(
			g_device.handle,
			&samplerCreateInfo,
			g_pAllocationCallbacks,
			&g_image.sampler) != VK_SUCCESS)
		{
			printf("[%s] failed to create sampler!", __FUNCTION__);
			return eRR_Error;
		}
	} // ~create sampler
	/////////////////////////////////

	/////////////////////////////////
	{ // copy texture data
		assert(g_stagingBuffer.handle != VK_NULL_HANDLE);
		assert(g_stagingBuffer.memory != VK_NULL_HANDLE);
		assert(g_stagingBuffer.size > 0);

		///////////////////////////////////////////////////////
		// 1. Copy texture data from image to staging buffer

		const uint32_t dataSize = pImage->pitch * (uint32_t) pImage->h;
		assert(g_stagingBuffer.size >= dataSize);

		void* pStagingBufferMemory = nullptr;
		if (g_device.vkMapMemory(
			g_device.handle,
			g_stagingBuffer.memory,
			0, // offset
			dataSize,
			0, // memory map flags
			&pStagingBufferMemory) != VK_SUCCESS)
		{
			printf("[%s] failed to map staging buffer memory!\n", __FUNCTION__);
			return false;
		}

		// TODO: Load images in a better/consistent format to allow for memcpy
		// instead of SDL_GetRGBA (and support across platforms/endianness)
		
		const size_t width = (size_t)pImage->w;
		const size_t height = (size_t)pImage->h;
		const SDL_PixelFormat* srcFormat = pImage->format;
		const uint32_t* srcPixels = (uint32_t*) pImage->pixels;
		uint32_t* dstPixels = (uint32_t*) pStagingBufferMemory;

		if (formatPixelSize == kRGBASize && !kIsBigEndian && srcFormat->format == SDL_PIXELFORMAT_RGBA32)
		{
			memcpy(dstPixels, srcPixels, dataSize);
		}
		else
		{
			struct SRGBA
			{
				uint8_t r, g, b, a;
			} rgba;
			static_assert(sizeof(SRGBA) == kRGBASize);

			for (size_t h = 0; h < height; ++h)
			{
				for (size_t w = 0; w < width; ++w)
				{
					const size_t pos = w + (h * width);
					SDL_GetRGBA(
						srcPixels[pos],
						srcFormat,
						&rgba.r, &rgba.g, &rgba.b, &rgba.a);
					memcpy(dstPixels + pos, &rgba, kRGBASize);
				}
			}
		}

		const VkDeviceSize flushSize = (dataSize + g_device.memoryAlignment - 1) / g_device.memoryAlignment;

		VkMappedMemoryRange flushRange;
		flushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		flushRange.pNext = nullptr;
		flushRange.memory = g_stagingBuffer.memory;
		flushRange.offset = 0;
		flushRange.size = flushSize * g_device.memoryAlignment;

		g_device.vkFlushMappedMemoryRanges(g_device.handle, 1, &flushRange);
		g_device.vkUnmapMemory(g_device.handle, g_stagingBuffer.memory);

		/////////////////////////////////////////////////////////////////////////////
		// 2. Create command buffers to transfer from staging buffer to vulkan image 

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = 0;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		VkCommandBuffer commandBuffer = g_renderResources[0].commandBuffer;
		assert(commandBuffer != VK_NULL_HANDLE);

		g_device.vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

		VkImageMemoryBarrier imageMemoryBarrierUndefinedToTransferDST;
		imageMemoryBarrierUndefinedToTransferDST.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrierUndefinedToTransferDST.pNext = nullptr;
		imageMemoryBarrierUndefinedToTransferDST.srcAccessMask = 0;
		imageMemoryBarrierUndefinedToTransferDST.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrierUndefinedToTransferDST.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageMemoryBarrierUndefinedToTransferDST.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageMemoryBarrierUndefinedToTransferDST.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrierUndefinedToTransferDST.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrierUndefinedToTransferDST.image = g_image.handle;

		imageMemoryBarrierUndefinedToTransferDST.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageMemoryBarrierUndefinedToTransferDST.subresourceRange.baseMipLevel = 0;
		imageMemoryBarrierUndefinedToTransferDST.subresourceRange.levelCount = 1;
		imageMemoryBarrierUndefinedToTransferDST.subresourceRange.baseArrayLayer = 0;
		imageMemoryBarrierUndefinedToTransferDST.subresourceRange.layerCount = 1;

		g_device.vkCmdPipelineBarrier(
			commandBuffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, // dependency flags
			0, // memory barrier count
			nullptr, // memory barriers
			0, // buffer memory barrier count
			nullptr, // buffer memory barriers
			1, // image memory barrier count
			&imageMemoryBarrierUndefinedToTransferDST);

		VkBufferImageCopy bufferImageCopy;
		bufferImageCopy.bufferOffset = 0;
		bufferImageCopy.bufferRowLength = 0;
		bufferImageCopy.bufferImageHeight = 0;
		bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferImageCopy.imageSubresource.mipLevel = 0;
		bufferImageCopy.imageSubresource.baseArrayLayer = 0;
		bufferImageCopy.imageSubresource.layerCount = 1;
		bufferImageCopy.imageOffset = { 0, 0, 0 };
		bufferImageCopy.imageExtent = { (uint32_t) width, (uint32_t) height, 1 };

		g_device.vkCmdCopyBufferToImage(
			commandBuffer,
			g_stagingBuffer.handle,
			g_image.handle,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, // region count
			&bufferImageCopy);

		VkImageMemoryBarrier imageMemoryBarrierTransferToShaderRead;
		imageMemoryBarrierTransferToShaderRead.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrierTransferToShaderRead.pNext = nullptr;
		imageMemoryBarrierTransferToShaderRead.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrierTransferToShaderRead.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		imageMemoryBarrierTransferToShaderRead.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageMemoryBarrierTransferToShaderRead.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageMemoryBarrierTransferToShaderRead.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrierTransferToShaderRead.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrierTransferToShaderRead.image = g_image.handle;
		imageMemoryBarrierTransferToShaderRead.subresourceRange = imageMemoryBarrierUndefinedToTransferDST.subresourceRange;

		g_device.vkCmdPipelineBarrier(
			commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0, // dependency flags
			0, // memory barrier count
			nullptr, // memory barriers
			0, // buffer memory barrier count
			nullptr, // buffer memory barriers
			1, // image barrier count
			&imageMemoryBarrierTransferToShaderRead);

		g_device.vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		if (g_device.vkQueueSubmit(
			g_device.graphicsQueue,
			1, // submit count
			&submitInfo,
			VK_NULL_HANDLE /* fence */) != VK_SUCCESS)
		{
			printf("[%s] failed to queue command buffer submission!", __FUNCTION__);
			return false;
		}

		// TODO: consider synchronization later more fully with architecture,
		// Use semaphores to coordinate instead of just waiting on the device
		g_device.vkDeviceWaitIdle(g_device.handle);
	} // ~copy texture data
	/////////////////////////////////

	return true;
}

} // vulkan namespace


namespace renderer
{

ERunResult Initialize()
{
	SDL_Window* pWindow = platform::GetWindow();
	assert(pWindow != nullptr);

	if (!vulkan::DefaultShaders.LoadFiles())
	{
		puts("Failed to load default shader files!");
		return eRR_Error;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
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
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
	{ // Vulkan surface creation
		VkSurfaceKHR presentationSurface;
		if (SDL_Vulkan_CreateSurface(pWindow, vulkan::g_instance.handle, &presentationSurface) == SDL_FALSE)
		{
			printf("SDL could not create window surface! SDL_Error: %s\n", SDL_GetError());
			return eRR_Error;
		}

		vulkan::SetPresentationSurface(presentationSurface);
	} // ~Vulkan surface creation
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
	{ // Vulkan logical device creation
		VkDevice vkDevice;
		uint32_t graphicsQueueFamilyIndex = vulkan::INVALID_QUEUE_FAMILY_PROPERTIES_INDEX;
		uint32_t presentQueueFamilyIndex = vulkan::INVALID_QUEUE_FAMILY_PROPERTIES_INDEX;
		VkDeviceSize memoryAlignment = { 8llu };
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
			if (vulkan::CheckPhysicalDeviceProperties(
				physicalDevices[i], 
				&graphicsQueueFamilyIndex, 
				&presentQueueFamilyIndex,
				&memoryAlignment))
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

		if (vulkan::SetDevice(
			vkDevice,
			selectedPhysicalDevice,
			graphicsQueueFamilyIndex,
			presentQueueFamilyIndex,
			memoryAlignment))
		{
			return eRR_Error;
		}

	} // ~Vulkan logical device creation
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
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

	} // ~Vulkan swap chain creation
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
	{ // Create command buffers
		VkCommandPoolCreateInfo commandPoolCreateInfo;
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.pNext = nullptr;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		commandPoolCreateInfo.queueFamilyIndex = vulkan::g_device.graphicsQueueFamilyIndex;

		if (vulkan::g_device.vkCreateCommandPool(
			vulkan::g_device.handle,
			&commandPoolCreateInfo,
			vulkan::g_pAllocationCallbacks,
			&vulkan::g_graphicsCommandPool) != VK_SUCCESS)
		{
			puts("Vulkan could not create a command pool!");
			return eRR_Error;
		}

		VkCommandBufferAllocateInfo commandBufferAllocateInfo;
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = vulkan::g_graphicsCommandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;

		for (size_t i = 0; i < vulkan::RENDER_RESOURCES_COUNT; ++i)
		{
			if (vulkan::g_device.vkAllocateCommandBuffers(
				vulkan::g_device.handle, 
				&commandBufferAllocateInfo,
				&vulkan::g_renderResources[i].commandBuffer) != VK_SUCCESS)
			{
				puts("Vulkan failed to allocate command buffers!");
				return eRR_Error;
			}
		}
	} // ~create command buffers
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
	{ // Vulkan semaphore creation
		VkSemaphoreCreateInfo semaphoreCreateInfo;
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = 0;

		for (size_t i = 0; i < vulkan::RENDER_RESOURCES_COUNT; ++i)
		{
			if (vulkan::g_device.vkCreateSemaphore(
				vulkan::g_device.handle,
				&semaphoreCreateInfo,
				nullptr,
				&vulkan::g_renderResources[i].imageAvailableSemaphore) != VK_SUCCESS)
			{
				puts("Vulkan unable to create image available semaphore!");
				return eRR_Error;
			}

			if (vulkan::g_device.vkCreateSemaphore(
				vulkan::g_device.handle,
				&semaphoreCreateInfo,
				nullptr,
				&vulkan::g_renderResources[i].renderingFinishedSemaphore) != VK_SUCCESS)
			{
				puts("Vulkan unable to create rendering finished semaphore!");
				return eRR_Error;
			}
		}
	} // ~Vulkan semaphore creation
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
	{ // fence creation
		VkFenceCreateInfo fenceCreateInfo;
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < vulkan::RENDER_RESOURCES_COUNT; ++i)
		{
			if (vulkan::g_device.vkCreateFence(
				vulkan::g_device.handle,
				&fenceCreateInfo,
				vulkan::g_pAllocationCallbacks,
				&vulkan::g_renderResources[i].fence) != VK_SUCCESS)
			{
				puts("Vulkan fence creation failed!");
				return eRR_Error;
			}
		}
	} // ~fence creation
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
	{ // create staging buffer
		const uint32_t stagingBufferSize = 1 << 18;
		const VkBufferUsageFlags stagingBufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		const VkMemoryPropertyFlagBits stagingBufferMemoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

		if (vulkan::CreateBuffer(
			stagingBufferUsage,
			stagingBufferMemoryProperty,
			stagingBufferSize,
			vulkan::g_stagingBuffer) == false)
		{
			puts("Failed to create staging buffer!");
			return eRR_Error;
		}
	} // ~create staging buffer
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
	{ // Create uniform buffer
		const VkBufferUsageFlags uniformBufferUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		const VkMemoryPropertyFlagBits uniformBufferMemoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		if (vulkan::CreateBuffer(
			uniformBufferUsage,
			uniformBufferMemoryProperty,
			sizeof(Matrix44l),
			vulkan::g_uniformBuffer) == false)
		{
			puts("Vulkan failed to create uniform buffer!");
			return eRR_Error;
		}
	} // ~create uniform buffer
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
	{ // create texture
	if (!vulkan::CreateTexture())
	{
		puts("Failed to create texture!");
		return eRR_Error;
	}
	} // create texture
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
	{ // create descriptor set
		assert(vulkan::g_image.handle != VK_NULL_HANDLE);
		const uint32_t numDescriptors = 2;

		{ // create descriptor set layout
			VkDescriptorSetLayoutBinding layoutBindings[numDescriptors]
			{
				{
					0, // binding
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, // descriptor type
					1, // descriptor count
					VK_SHADER_STAGE_FRAGMENT_BIT, // stage flags
					nullptr // immutable samplers
				},
				{
					1, // binding
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // descriptor type
					1, // descriptor count
					VK_SHADER_STAGE_FRAGMENT_BIT, // stage flags
					nullptr // immutable samplers
				}
			};

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pNext = nullptr;
			descriptorSetLayoutCreateInfo.flags = 0;
			descriptorSetLayoutCreateInfo.bindingCount = numDescriptors;
			descriptorSetLayoutCreateInfo.pBindings = layoutBindings;

			if (vulkan::g_device.vkCreateDescriptorSetLayout(
				vulkan::g_device.handle,
				&descriptorSetLayoutCreateInfo,
				vulkan::g_pAllocationCallbacks,
				&vulkan::g_descriptorSet.layout) != VK_SUCCESS)
			{
				printf("[%s] failed to create descriptor set layout!", __FUNCTION__);
				return eRR_Error;
			}
		} // ~create descriptor set layout

		{ // create descriptor pool
			const VkDescriptorPoolSize poolSizes[numDescriptors] =
			{
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 }
			};

			VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
			descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolCreateInfo.pNext = nullptr;
			descriptorPoolCreateInfo.flags = 0;
			descriptorPoolCreateInfo.maxSets = 1;
			descriptorPoolCreateInfo.poolSizeCount = numDescriptors;
			descriptorPoolCreateInfo.pPoolSizes = poolSizes;

			if (vulkan::g_device.vkCreateDescriptorPool(
				vulkan::g_device.handle,
				&descriptorPoolCreateInfo,
				vulkan::g_pAllocationCallbacks,
				&vulkan::g_descriptorSet.pool) != VK_NULL_HANDLE)
			{
				printf("[%s] failed to create descriptor pool!", __FUNCTION__);
				return eRR_Error;
			}
		} // ~create descriptor pool

		{ // allocate descriptor set
			assert(vulkan::g_descriptorSet.pool != VK_NULL_HANDLE);
			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.pNext = nullptr;
			descriptorSetAllocateInfo.descriptorPool = vulkan::g_descriptorSet.pool;
			descriptorSetAllocateInfo.descriptorSetCount = 1;
			descriptorSetAllocateInfo.pSetLayouts = &vulkan::g_descriptorSet.layout;

			if (vulkan::g_device.vkAllocateDescriptorSets(
				vulkan::g_device.handle,
				&descriptorSetAllocateInfo,
				&vulkan::g_descriptorSet.handle) != VK_SUCCESS)
			{
				printf("[%s] failed to allocate descriptor sets!", __FUNCTION__);
				return eRR_Error;
			}
		} // ~allocate descriptor set

		{ // update descriptor set
			assert(vulkan::g_image.handle != VK_NULL_HANDLE);
			assert(vulkan::g_image.view != VK_NULL_HANDLE);
			assert(vulkan::g_image.sampler != VK_NULL_HANDLE);
			assert(vulkan::g_descriptorSet.handle != VK_NULL_HANDLE);

			VkDescriptorImageInfo imageInfo;
			imageInfo.sampler = vulkan::g_image.sampler;
			imageInfo.imageView = vulkan::g_image.view;
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkDescriptorBufferInfo bufferInfo;
			bufferInfo.buffer = vulkan::g_uniformBuffer.handle;
			bufferInfo.offset = 0;
			bufferInfo.range = vulkan::g_uniformBuffer.size;

			VkWriteDescriptorSet descriptorWrites[numDescriptors] =
			{
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, // sType
					nullptr, // pNext
					vulkan::g_descriptorSet.handle, // dstSet
					0, // dstBinding
					0, // dstArrayElement
					1, // descriptor count
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, // descriptorType
					&imageInfo, // pImageInfo
					nullptr, // pBufferInfo
					nullptr // pTexelBufferView
				},
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, // sType
					nullptr, // pNext
					vulkan::g_descriptorSet.handle, // dstSet
					1, // dstBinding
					0, // dstArrayElement
					1, // descriptor count
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // descriptorType
					nullptr, // pImageInfo
					&bufferInfo, // pBufferInfo
					nullptr // pTexelBufferView
				}
			};

			vulkan::g_device.vkUpdateDescriptorSets(
				vulkan::g_device.handle,
				2, // descriptor write count
				descriptorWrites,
				0, // descriptor copy count
				nullptr /* descriptor copies */);
		} // ~update descriptor set

	} // ~create descriptor set
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
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
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // initialLayout
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // finalLyout
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
				0, // preserveAttachmentCount
				nullptr, // pPreserveAttachments
			}
		};

#if 0
		static const uint32_t numDependencies = 2;
		VkSubpassDependency dependencies[numDependencies] =
		{
			{
				VK_SUBPASS_EXTERNAL, // srcSubpass
				0, // dstSubPass
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, // srcStageMask
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
				VK_ACCESS_MEMORY_READ_BIT, // srcAccessMask
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, // dstAccessMask
				VK_DEPENDENCY_BY_REGION_BIT // dependencyFlags
			},
			{
				0, // srcSubpass
				VK_SUBPASS_EXTERNAL, // dstSubPass
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // srcStageMask
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, // dstStageMask
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, // srcAccessMask
				VK_ACCESS_MEMORY_READ_BIT, // dstAccessMask
				VK_DEPENDENCY_BY_REGION_BIT // dependencyFlags
			}
		};
#endif

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
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
	{ // create rendering pipeline
		if (!vulkan::DefaultShaders.CreateShaderModules())
		{
			puts("Vulkan failed to create graphics pipeline!");
			return eRR_Error;
		}

		static const uint32_t numShaderStageCreateInfos = 2;
		VkPipelineShaderStageCreateInfo shaderStageCreateInfos[numShaderStageCreateInfos] =
		{
			// Vertex shader
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, // sType
				nullptr, // pNext
				0, // flags
				VK_SHADER_STAGE_VERTEX_BIT, // stage
				vulkan::DefaultShaders.GetVertexShaderModule(vulkan::EDefaultShaders::Enum::sdf), // module
				"main", // name
				nullptr, // pSpecializationInfo
			},
			// Fragment shader
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, // sType
				nullptr, // pNext
				0, // flags
				VK_SHADER_STAGE_FRAGMENT_BIT, // stage
				vulkan::DefaultShaders.GetFragmentShaderModule(vulkan::EDefaultShaders::Enum::sdf), // module
				"main", // name
				nullptr, // pSpecializationInfo
			},
		};

		static const uint32_t numVertexBindingDescriptions = 1;
		VkVertexInputBindingDescription vertexBindingDescriptions[numVertexBindingDescriptions] =
		{
			{
				0, // binding
				sizeof(vulkan::SVertexData), // stride
				VK_VERTEX_INPUT_RATE_VERTEX // inputRate
			}
		};

		assert(numVertexBindingDescriptions > 0);
		static const uint32_t numVertexAttributDescriptions = 2;
		VkVertexInputAttributeDescription vertexAttributeDescriptions[numVertexAttributDescriptions] =
		{
			{
				0, // location
				vertexBindingDescriptions[0].binding, //binding
				VK_FORMAT_R32G32B32A32_SFLOAT, // format
				0
			},
			{
				1, // location
				vertexBindingDescriptions[0].binding, // binding
				VK_FORMAT_R32G32_SFLOAT, //format
				4 * sizeof(float)
			}
		};

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.pNext = nullptr;
		vertexInputStateCreateInfo.flags = 0;
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = numVertexBindingDescriptions;
		vertexInputStateCreateInfo.pVertexBindingDescriptions = vertexBindingDescriptions;
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = numVertexAttributDescriptions;
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions;

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyeStateCreateInfo;
		inputAssemblyeStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyeStateCreateInfo.pNext = nullptr;
		inputAssemblyeStateCreateInfo.flags = 0;
		inputAssemblyeStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		inputAssemblyeStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
		viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCreateInfo.pNext = nullptr;
		viewportStateCreateInfo.flags = 0;
		viewportStateCreateInfo.viewportCount = 1;
		viewportStateCreateInfo.pViewports = nullptr; // dynamically set
		viewportStateCreateInfo.scissorCount = 1;
		viewportStateCreateInfo.pScissors = nullptr; // dynamically set

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
		rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationStateCreateInfo.pNext = nullptr;
		rasterizationStateCreateInfo.flags = 0;
		rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
		rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
		rasterizationStateCreateInfo.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
		multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateCreateInfo.pNext = nullptr;
		multisampleStateCreateInfo.flags = 0;
		multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		multisampleStateCreateInfo.minSampleShading = 1.0f;
		multisampleStateCreateInfo.pSampleMask = nullptr;
		multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
		multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
		colorBlendAttachmentState.blendEnable = VK_FALSE;
		colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentState.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
		colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCreateInfo.pNext = nullptr;
		colorBlendStateCreateInfo.flags = 0;
		colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendStateCreateInfo.attachmentCount = 1;
		colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
		memset(colorBlendStateCreateInfo.blendConstants, 0, sizeof(float) * 4);

		{ // create pipeline layout
			assert(vulkan::g_descriptorSet.handle != VK_NULL_HANDLE);
			assert(vulkan::g_descriptorSet.layout != VK_NULL_HANDLE);
			VkPipelineLayoutCreateInfo layoutCreateInfo;
			layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			layoutCreateInfo.pNext = nullptr;
			layoutCreateInfo.flags = 0;
			layoutCreateInfo.setLayoutCount = 1;
			layoutCreateInfo.pSetLayouts = &vulkan::g_descriptorSet.layout;
			layoutCreateInfo.pushConstantRangeCount = 0;
			layoutCreateInfo.pPushConstantRanges = nullptr;

			if (vulkan::g_device.vkCreatePipelineLayout(
				vulkan::g_device.handle,
				&layoutCreateInfo,
				vulkan::g_pAllocationCallbacks,
				&vulkan::g_pipelineLayout) != VK_SUCCESS)
			{
				puts("Vulkan failed to create pipeline layout!");
				return eRR_Error;
			}

		} // ~create pipeline layout

		assert(vulkan::g_pipelineLayout != VK_NULL_HANDLE);

		static const uint32_t numDynamicStates = 2;
		VkDynamicState dynamicStates[numDynamicStates] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.pNext = nullptr;
		dynamicStateCreateInfo.flags = 0;
		dynamicStateCreateInfo.dynamicStateCount = numDynamicStates;
		dynamicStateCreateInfo.pDynamicStates = dynamicStates;

		VkGraphicsPipelineCreateInfo pipelineCreateInfo;
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.pNext = nullptr;
		pipelineCreateInfo.flags = 0;
		pipelineCreateInfo.stageCount = numShaderStageCreateInfos;
		pipelineCreateInfo.pStages = shaderStageCreateInfos;
		pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyeStateCreateInfo;
		pipelineCreateInfo.pTessellationState = nullptr;
		pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
		pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
		pipelineCreateInfo.pDepthStencilState = nullptr;
		pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
		pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineCreateInfo.layout = vulkan::g_pipelineLayout;
		pipelineCreateInfo.renderPass = vulkan::g_renderPass;
		pipelineCreateInfo.subpass = 0;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCreateInfo.basePipelineIndex = -1;

		if (vulkan::g_device.vkCreateGraphicsPipelines(
			vulkan::g_device.handle,
			VK_NULL_HANDLE,
			1,
			&pipelineCreateInfo,
			vulkan::g_pAllocationCallbacks,
			&vulkan::g_graphicsPipeline) != VK_SUCCESS)
		{
			puts("Vulkan failed to create graphics pipeline!");
			return eRR_Error;
		}
	} // ~create rendering pipeline
	///////////////////////////////////////////////////////////////////////////////////////////////////

	const vulkan::SVertexData vertexData[] =
	{
		{
			-1.f, -1.f, 0.0f, 1.0f,
      -0.1f, -0.1f,
		},
		{
			-1.f, 1.f, 0.0f, 1.0f,
      -0.1f, 1.1f,
		},
		{
			1.f, -1.f, 0.0f, 1.0f,
      1.1f, -0.1f,
		},
		{
			1.f, 1.f, 0.0f, 1.0f,
      1.1f, 1.1f,
		}
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	{ // create vertex buffer
		const uint32_t vertexBufferSize = sizeof(vertexData);
		const VkBufferUsageFlags vertexBufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		const VkMemoryPropertyFlagBits vertexBufferMemoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		if (vulkan::CreateBuffer(
			vertexBufferUsage,
			vertexBufferMemoryProperty,
			vertexBufferSize,
			vulkan::g_vertexBuffer) == false)
		{
			puts("Failed to create vertex buffer!");
			return eRR_Error;
		}
	} // ~create vertex buffer
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
	{ // copy vertex data from staging buffer -> vertex buffer
		assert(vulkan::g_vertexBuffer.handle != VK_NULL_HANDLE);
		assert(vulkan::g_vertexBuffer.memory != VK_NULL_HANDLE);
		assert(vulkan::g_vertexBuffer.size > 0);
		assert(vulkan::g_stagingBuffer.handle != VK_NULL_HANDLE);
		assert(vulkan::g_stagingBuffer.memory != VK_NULL_HANDLE);
		assert(vulkan::g_stagingBuffer.size >= vulkan::g_vertexBuffer.size);

		void* pStagingBufferMemory = nullptr;
		if (vulkan::g_device.vkMapMemory(
			vulkan::g_device.handle,
			vulkan::g_stagingBuffer.memory,
			0, // offset
			vulkan::g_stagingBuffer.size,
			0, // flags
			&pStagingBufferMemory) != VK_SUCCESS)
		{
			puts("Vulkan failed to map vertex buffer memory!");
			return eRR_Error;
		}

		assert(pStagingBufferMemory != nullptr);
		memcpy(pStagingBufferMemory, vertexData, vulkan::g_vertexBuffer.size);

		const VkDeviceSize flushSize = (vulkan::g_vertexBuffer.size + vulkan::g_device.memoryAlignment - 1) / vulkan::g_device.memoryAlignment;

		VkMappedMemoryRange flushRange;
		flushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		flushRange.pNext = nullptr;
		flushRange.memory = vulkan::g_stagingBuffer.memory;
		flushRange.offset = 0;
		flushRange.size = flushSize * vulkan::g_device.memoryAlignment;

		vulkan::g_device.vkFlushMappedMemoryRanges(vulkan::g_device.handle, 1, &flushRange);
		vulkan::g_device.vkUnmapMemory(vulkan::g_device.handle, vulkan::g_stagingBuffer.memory);

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		VkCommandBuffer commandBuffer = vulkan::g_renderResources[0].commandBuffer;
		assert(commandBuffer != VK_NULL_HANDLE);

		vulkan::g_device.vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

		VkBufferCopy bufferCopyInfo;
		bufferCopyInfo.srcOffset = 0;
		bufferCopyInfo.dstOffset = 0;
		bufferCopyInfo.size = vulkan::g_vertexBuffer.size;

		vulkan::g_device.vkCmdCopyBuffer(
			commandBuffer,
			vulkan::g_stagingBuffer.handle,
			vulkan::g_vertexBuffer.handle,
			1, // regionCount
			&bufferCopyInfo);

		VkBufferMemoryBarrier bufferMemoryBarrier;
		bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		bufferMemoryBarrier.pNext = nullptr;
		bufferMemoryBarrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
		bufferMemoryBarrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
		bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferMemoryBarrier.buffer = vulkan::g_vertexBuffer.handle;
		bufferMemoryBarrier.offset = 0;
		bufferMemoryBarrier.size = VK_WHOLE_SIZE;

		vulkan::g_device.vkCmdPipelineBarrier(
			commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			0,
			0,
			nullptr,
			1,
			&bufferMemoryBarrier,
			0,
			nullptr);

		vulkan::g_device.vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		if (vulkan::g_device.vkQueueSubmit(
			vulkan::g_device.graphicsQueue,
			1,
			&submitInfo,
			VK_NULL_HANDLE) != VK_SUCCESS)
		{
			puts("Vulkan failed to submit queue to copy staging buffer to vertex buffer");
			return eRR_Error;
		}

		vulkan::g_device.vkDeviceWaitIdle(vulkan::g_device.handle); // TODO: use semaphores instead of simply waiting on the device
	} // ~copy vertex data from staging buffer -> vertex buffer
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////
	{ // Copy matrix to staging buffer, then copy to host memory uniform buffer
		static const Matrix44l matrix = Matrix44l::CreateRotationAndTranslation(
			Matrix33l::CreateRotationZ(kFLocalQuarterPi),
			Vec3l(-0.5f, -0.5f, 0.0f)
		);
		static const VkDeviceSize matrixSize = sizeof(matrix);

		{ // Map staging buffer memory and copy projection matrix to it
			void* pStagingBufferMemoryPoint;
			if (vulkan::g_device.vkMapMemory(
				vulkan::g_device.handle,
				vulkan::g_stagingBuffer.memory,
				0, // offset
				matrixSize,
				0, // flags
				&pStagingBufferMemoryPoint) != VK_SUCCESS)
			{
				puts("Vulkan failed to map staging buffer for uniform data!");
				return eRR_Error;
			}

			memcpy(pStagingBufferMemoryPoint, &matrix, matrixSize);

			VkMappedMemoryRange flushRange;
			flushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			flushRange.pNext = nullptr;
			flushRange.memory = vulkan::g_stagingBuffer.memory;
			flushRange.offset = 0;
			flushRange.size = matrixSize;

			vulkan::g_device.vkFlushMappedMemoryRanges(vulkan::g_device.handle, 1, &flushRange);
			vulkan::g_device.vkUnmapMemory(vulkan::g_device.handle, vulkan::g_stagingBuffer.memory);
		} // ~Map staging buffer memory and copy projection matrix to it

		{ // copy from the staging buffer into the uniform buffer's device local memory
			VkCommandBuffer commandBuffer = vulkan::g_renderResources[0].commandBuffer;
			VkCommandBufferBeginInfo commandBufferBeginInfo;
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.pNext = nullptr;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			commandBufferBeginInfo.pInheritanceInfo = nullptr;

			vulkan::g_device.vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

			VkBufferCopy bufferCopyInfo;
			bufferCopyInfo.srcOffset = 0;
			bufferCopyInfo.dstOffset = 0;
			bufferCopyInfo.size = matrixSize;

			vulkan::g_device.vkCmdCopyBuffer(
				commandBuffer,
				vulkan::g_stagingBuffer.handle,
				vulkan::g_uniformBuffer.handle,
				1,
				&bufferCopyInfo);

			VkBufferMemoryBarrier bufferMemoryBarrier;
			bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			bufferMemoryBarrier.pNext = nullptr;
			bufferMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			bufferMemoryBarrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
			bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bufferMemoryBarrier.buffer = vulkan::g_uniformBuffer.handle;
			bufferMemoryBarrier.offset = 0;
			bufferMemoryBarrier.size = matrixSize;

			vulkan::g_device.vkCmdPipelineBarrier(
				commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
				0, // dependency flags
				0, // memory barrier count
				nullptr, // memory barriers
				1, // buffer memory barrier count
				&bufferMemoryBarrier, // buffer memory barriers
				0, // image memory barrier count
				nullptr /* image memory barriers */);

			vulkan::g_device.vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo;
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pNext = nullptr;
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.pWaitDstStageMask = nullptr;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;

			if (vulkan::g_device.vkQueueSubmit(
				vulkan::g_device.graphicsQueue,
				1,
				&submitInfo,
				VK_NULL_HANDLE) != VK_SUCCESS)
			{
				puts("Vulkan failed to submit graphics queue for the uniform buffer!");
				return eRR_Error;
			}

			vulkan::g_device.vkDeviceWaitIdle(vulkan::g_device.handle); // TODO: use semaphores instead of simply waiting on the device
		} // Copy uniform buffer to staging buffer, then copy to host memory

	} // ~Copy matrix to staging buffer, then copy to host memory uniform buffer
	///////////////////////////////////////////////////////////////////////////////////////////////////

	return eRR_Success;
}

ERunResult Render()
{
	/////////////////////////
	// Rendering setup
	static size_t resourceIndex = 0;
	vulkan::SRenderResources& currentRenderingResource = vulkan::g_renderResources[resourceIndex];
	resourceIndex = (resourceIndex + 1) % vulkan::RENDER_RESOURCES_COUNT;

	/////////////////////////
	// Fence handling
	if (vulkan::g_device.vkWaitForFences(
		vulkan::g_device.handle,
		1, // fenceCount
		&currentRenderingResource.fence,
		VK_FALSE, // waitAll
		1000000000 /*timeout*/) != VK_SUCCESS)
	{
		puts("Vulkan waiting for fence is taking too long!");
		return eRR_Error;
	}

	vulkan::g_device.vkResetFences(
		vulkan::g_device.handle,
		1,
		&currentRenderingResource.fence);

	/////////////////////////
	// Acquire next image 
	uint32_t imageIndex = UINT32_MAX;
	const uint64_t timeout = UINT64_MAX;
	VkResult acquireNextImageResult = vulkan::g_device.vkAcquireNextImageKHR(
		vulkan::g_device.handle,
		vulkan::g_swapChain.handle,
		timeout,
		currentRenderingResource.imageAvailableSemaphore,
		VK_NULL_HANDLE, // fence
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

	/////////////////////////
	// Prepare frame
	if (!vulkan::PrepareFrame(
		currentRenderingResource.commandBuffer,
		vulkan::g_swapChain.images[imageIndex],
		currentRenderingResource.frameBuffer))
	{
		puts("renderer failed to prepare frame!");
		return eRR_Error;
	}

	/////////////////////////
	// Queue submit 
	VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &currentRenderingResource.imageAvailableSemaphore;
	submitInfo.pWaitDstStageMask = &waitDstStageMask;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &currentRenderingResource.commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &currentRenderingResource.renderingFinishedSemaphore;

	if (vulkan::g_device.vkQueueSubmit(
		vulkan::g_device.graphicsQueue,
		1,
		&submitInfo,
		currentRenderingResource.fence) != VK_SUCCESS)
	{
		puts("Vulkan failed to submit to presentation queue!");
		return eRR_Error;
	}

	/////////////////////////
	// Presentation 
	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &currentRenderingResource.renderingFinishedSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &vulkan::g_swapChain.handle;
	presentInfo.pImageIndices = nullptr;
	presentInfo.pResults = nullptr;
	presentInfo.pImageIndices = &imageIndex;

	VkResult presentResult = vulkan::g_device.vkQueuePresentKHR(vulkan::g_device.presentQueue, &presentInfo);
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
