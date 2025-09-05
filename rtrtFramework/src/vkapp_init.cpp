

#include <array>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

#include <unordered_set>
#include <unordered_map>

#include "vkapp.h"
#include "app.h"
#include "extensions_vk.hpp"

#ifdef GUI
#include "backends/imgui_impl_glfw.h"
#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"
#endif


void VkApp::destroyAllVulkanResources()
{
    // @@  Uncomment next 3 lines when directed to do so at the end of postProcess().
    //vkWaitForFences(m_device, 1, &m_waitFence, VK_TRUE, UINT64_MAX);
    //vkResetFences(m_device, 1, &m_waitFence);
    //vkDeviceWaitIdle(m_device);
    
    #ifdef GUI
    vkDestroyDescriptorPool(m_device, m_imguiDescPool, nullptr);
    ImGui_ImplVulkan_Shutdown();
    #endif

    // Destroy all vulkan objects.
    // ...  All objects created on m_device must be destroyed before m_device.
    //vkDestroyDevice(m_device, nullptr);
    //vkDestroyInstance(m_instance, nullptr);
}

void VkApp::recreateSizedResources(VkExtent2D size)
{
    assert(false && "Not ready for resize events.");
    // Destroy everything related to the window size
    // (RE)Create them all at the new size
}
 
void VkApp::createInstance(bool doApiDump)
{
    uint32_t countGLFWextensions{0};
    const char** reqGLFWextensions = glfwGetRequiredInstanceExtensions(&countGLFWextensions);

    // @@ Append each GLFW required extension in reqGLFWextensions to reqInstanceExtensions
    // Print them out while you are at it
    printf("GLFW required extensions:\n");
    // ...

    // Suggestion: Parse a command line argument to set/unset doApiDump
    // If included, the api_dump layer should be first on reqInstanceLayers
    if (doApiDump)
        reqInstanceLayers.insert(reqInstanceLayers.begin(), "VK_LAYER_LUNARG_api_dump");
  
    uint32_t count;
    // @@ Understand the following three step process for requesting a
    // variable length list from Vulkan.  You will use this many more
    // times.
    //   Step 1: Ask for the count but not the data
    //   Step 2: Create a vector<...> of the correct size,
    //           or resize(...) an existing vector<...>.
    //   Step 3: Ask for the data to be filled into the vector<...>
    vkEnumerateInstanceLayerProperties(&count, nullptr);
    std::vector<VkLayerProperties> availableLayers(count);
    vkEnumerateInstanceLayerProperties(&count, availableLayers.data());

    // @@ Print out the availableLayers
    printf("InstanceLayer count: %d\n", count);
    // ...  use availableLayers[i].layerName

    // Another three step dance
    vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(count);
    vkEnumerateInstanceExtensionProperties(nullptr, &count, availableExtensions.data());

    // @@ Print out the availableExtensions
    printf("InstanceExtensions count: %d\n", count);
    // ...  use availableExtensions[i].extensionName

    VkApplicationInfo applicationInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    applicationInfo.pApplicationName = "rtrt";
    applicationInfo.pEngineName      = "no-engine";
    applicationInfo.apiVersion       = VK_MAKE_VERSION(1, 3, 0);

    VkInstanceCreateInfo instanceCreateInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instanceCreateInfo.pNext                   = nullptr;
    instanceCreateInfo.pApplicationInfo        = &applicationInfo;
    
    instanceCreateInfo.enabledExtensionCount   = reqInstanceExtensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = reqInstanceExtensions.data();
    
    instanceCreateInfo.enabledLayerCount       = reqInstanceLayers.size();
    instanceCreateInfo.ppEnabledLayerNames     = reqInstanceLayers.data();

    vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance);

    // @@ Verify success of vkCreateInstance like this:
    //    if (vkCreateInstance(...) != VK_SUCCESS)
    //        throw std::runtime_error("vkCreateInstance failed.");
    // @@ To destroy: vkDestroyInstance(m_instance, nullptr);
    // @@ Cut-and-paste the above three list printouts into your report.
}

void VkApp::createPhysicalDevice()
{
    // Get the GPU list;  Another three-step list retrieval process:
    uint physicalDevicesCount;
    vkEnumeratePhysicalDevices(m_instance, &physicalDevicesCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(physicalDevicesCount);
    vkEnumeratePhysicalDevices(m_instance, &physicalDevicesCount, physicalDevices.data());

    std::vector<uint32_t> compatibleDevices;
  
    printf("%d devices\n", physicalDevicesCount);
    int i = 0;

    // For each GPU:
    for (auto physicalDevice : physicalDevices) {

        // Get the GPU's properties
        VkPhysicalDeviceProperties GPUproperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &GPUproperties);

        // Get the GPU's extension list;  Another three-step list retrieval process:
        uint extCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, nullptr);
        std::vector<VkExtensionProperties> extensionProperties(extCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr,
                                             &extCount, extensionProperties.data());

        // @@ This code is in a loop iterating variable physicalDevice
        // through a list of all physicalDevices(GPUs).  The
        // physicalDevice's properties (GPUproperties) and a list of
        // its extension properties (extensionProperties) are retrieved
        // above, and here we judge if the physicalDevice
        // is compatible with our requirements. We consider a GPU to be
        // compatible if it satisfies both:
        
        //    GPUproperties.deviceType==VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        
        // and
        
        //    All reqDeviceExtensions can be found in the GPUs extensionProperties list
        //      That is: for all i, there exists a j such that:
        //                 reqDeviceExtensions[i] == extensionProperties[j].extensionName

        //  If a GPU is found to be compatible save it in m_physicalDevice.

        //  If none are found, declare failure and abort
        //     (And then find a better GPU for this class.)
        //  If several are found, tell me all about your system

        // Hint: Instead of a double nested pair of loops consider
        // making an std::unordered_set of all the device's
        // extensionNames and using its "find" method to search for
        // each required extension.
    }
    
    // @@ Document the GPU accepted, and any GPUs rejected.
    // Oddly, there is nothing to destroy here.
  
}

void VkApp::chooseQueueIndex()
{
    VkQueueFlags requiredQueueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT
                                      | VK_QUEUE_TRANSFER_BIT;

    // Retrieve the list of queue families. (3 step.)
    uint32_t mpCount;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &mpCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueProperties(mpCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &mpCount, queueProperties.data());

    // @@ How many queue families does your Vulkan offer?  Which of
    // the three flags does each offer?  Which of them, by index, has
    // the above three required flags?

    // @@ Search the list for (the index of) the first queue family
    // that has the required flags in queueProperties[i].queueFlags.  Record the index in
    // m_graphicsQueueIndex.

    // Nothing to destroy as m_graphicsQueueIndex is just an integer.
}


void VkApp::createDevice()
{
    // @@ Build a pNext chain of the following six "feature" structures:
    //   features2->features11->features12->features13->accelFeature->rtPipelineFeature->NULL

    // Hint: Keep it simple; add a second parameter (the pNext
    // pointer) to each structure's initializer pointing up to the
    // previous structure.
    
    // =============
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR rtPipelineFeature{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR};
    
    VkPhysicalDeviceAccelerationStructureFeaturesKHR accelFeature{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR};
    
    VkPhysicalDeviceVulkan13Features features13{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    
    VkPhysicalDeviceVulkan12Features features12{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
    
    VkPhysicalDeviceVulkan11Features features11{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
    
    VkPhysicalDeviceFeatures2 features2{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
    // =============
    
    // Ask Vulkan to fill in all structures on the pNext chain
    vkGetPhysicalDeviceFeatures2(m_physicalDevice, &features2);

    float priority = 1.0;
    VkDeviceQueueCreateInfo queueInfo{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    queueInfo.queueFamilyIndex = m_graphicsQueueIndex;
    queueInfo.queueCount       = 1;
    queueInfo.pQueuePriorities = &priority;
    
    VkDeviceCreateInfo deviceCreateInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    deviceCreateInfo.pNext            = &features2; // This is the whole pNext chain
  
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos    = &queueInfo;
    
    deviceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(reqDeviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = reqDeviceExtensions.data();

    vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device);

    // @@ Verify success of vkCreateDevice.
    // To destroy: vkDestroyDevice(m_device, nullptr);
}

void VkApp::getCommandQueue()
{
    vkGetDeviceQueue(m_device, m_graphicsQueueIndex, 0, &m_queue);
    // Returns void -- nothing to verify
    // Nothing to destroy -- the queue is owned by the device.
}

// Create a command pool, used to allocate command buffers, which in
// turn are use to gather and send commands to the GPU.  The flag
// makes it possible to reuse command buffers.  The queue index
// determines which queue the command buffers can be submitted to.
// Use the command pool to also create a command buffer.
void VkApp::createCommandPool()
{
    VkCommandPoolCreateInfo poolCreateInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolCreateInfo.queueFamilyIndex = m_graphicsQueueIndex;
    vkCreateCommandPool(m_device, &poolCreateInfo, nullptr, &m_cmdPool);
    // @@ Verify success of vkCreateCommandPool
    // @@ To destroy: vkDestroyCommandPool(m_device, m_cmdPool, nullptr);
    
    // Create a command buffer
    VkCommandBufferAllocateInfo allocateInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    allocateInfo.commandPool        = m_cmdPool;
    allocateInfo.commandBufferCount = 1;
    allocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    vkAllocateCommandBuffers(m_device, &allocateInfo, &m_commandBuffer);
    // @@ Verify success of vkAllocateCommandBuffers
    // @@ Nothing to destroy -- the pool owns the command buffer.
}
 
// Calling load_VK_EXTENSIONS from extensions_vk.cpp.  A Python script
// from NVIDIA created extensions_vk.cpp from the current Vulkan spec
// for the purpose of loading the symbols for all registered
// extension.  This be (indistinguishable from) magic.
void VkApp::loadExtensions()
{
    load_VK_EXTENSIONS(m_instance, vkGetInstanceProcAddr, m_device, vkGetDeviceProcAddr);
}

//  VkSurface is Vulkan's name for the screen.  Since GLFW creates and
//  manages the window, it creates the VkSurface at our request.
void VkApp::getSurface()
{
    VkBool32 isSupported;   // Supports drawing(presenting) on a screen

    glfwCreateWindowSurface(m_instance, app->GLFW_window, nullptr, &m_surface);
    vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, m_graphicsQueueIndex,
                                         m_surface, &isSupported);
    // @@ Verify success of glfwCreateWindowSurface.
    // @@ Verify success of vkGetPhysicalDeviceSurfaceSupportKHR.
    // @@ Verify isSupported==VK_TRUE, meaning that Vulkan supports presenting on this surface.
    // @@ To destroy: vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
}

// 
void VkApp::createSwapchain()
{
    VkSwapchainKHR oldSwapchain = m_swapchain;

    vkDeviceWaitIdle(m_device);  // Probably unnecessary

    // Get the surface's capabilities
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &capabilities);

    // @@  Roll your own three step process to retrieve a list of PRESENT MODEs into
    //    std::vector<VkPresentModeKHR> presentModes;
    // using vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &count, nullptr);
    // @@ Document your PRESENT MODEs. I especially want to know if
    // your system offers VK_PRESENT_MODE_MAILBOX_KHR mode.  My
    // high-end windows desktop does; My higher-end Linux laptop
    // doesn't.

    // Choose VK_PRESENT_MODE_FIFO_KHR as a default (this must be supported)
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR; // Support is required.
    // @@ But choose VK_PRESENT_MODE_MAILBOX_KHR if it can be found in
    // the retrieved presentModes. Several Vulkan tutorials opine that
    // MODE_MAILBOX is the premier mode, but this may not be best for
    // us.
  

    // Get the list of VkFormat's that are supported:
    // @@ Do the three step process to retrieve a list of surface formats into
    //   std::vector<VkSurfaceFormatKHR> formats;
    // using  vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &count, nullptr);
    // @@ Document the list you get.

    VkFormat surfaceFormat       = VK_FORMAT_UNDEFINED;               // Temporary value.
    VkColorSpaceKHR surfaceColor = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; // Temporary value
    // @@ Replace the above two temporary lines with the following two
    // to choose the first format and its color space as defaults:
    //  VkFormat surfaceFormat = formats[0].format;
    //  VkColorSpaceKHR surfaceColor  = formats[0].colorSpace;

    // @@ Then search the formats (from several lines up) to choose
    // format VK_FORMAT_B8G8R8A8_UNORM (and its color space) if such
    // exists.  Document your list of formats/color-spaces, and your
    // particular choice.
    
    // Get the swap chain extent
    VkExtent2D swapchainExtent = capabilities.currentExtent;
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        swapchainExtent = capabilities.currentExtent; }
    else {
        // Does this case ever happen?
        int width, height;
        glfwGetFramebufferSize(app->GLFW_window, &width, &height);

        swapchainExtent = VkExtent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        swapchainExtent.width = std::clamp(swapchainExtent.width,
                                           capabilities.minImageExtent.width,
                                           capabilities.maxImageExtent.width);
        swapchainExtent.height = std::clamp(swapchainExtent.height,
                                            capabilities.minImageExtent.height,
                                            capabilities.maxImageExtent.height); }

    // Test against valid size, typically hit when windows are minimized.
    // The app must prevent triggering this code in such a case
    assert(swapchainExtent.width && swapchainExtent.height);
    // If this assert fires, we have some work to do to better deal
    // with the situation.

    // Choose the number of swap chain images, within the bounds supported.
    uint imageCount = capabilities.minImageCount + 1; // Recommendation: minImageCount+1
    if (capabilities.maxImageCount > 0
        && imageCount > capabilities.maxImageCount) {
            imageCount = capabilities.maxImageCount; }
    
    assert (imageCount == 3);
    // If this triggers, disable the assert, BUT help me understand
    // the situation that caused it.  

    // Create the swap chain
    VkImageUsageFlags imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                                 | VK_IMAGE_USAGE_STORAGE_BIT
                                 | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    
    VkSwapchainCreateInfoKHR createInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    createInfo.surface                  = m_surface;
    createInfo.minImageCount            = imageCount;
    createInfo.imageFormat              = surfaceFormat;
    createInfo.imageColorSpace          = surfaceColor;
    createInfo.imageExtent              = swapchainExtent;
    createInfo.imageUsage               = imageUsage;
    createInfo.preTransform             = capabilities.currentTransform;
    createInfo.compositeAlpha           = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.imageArrayLayers         = 1;
    createInfo.imageSharingMode         = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount    = 1;
    createInfo.pQueueFamilyIndices      = &m_graphicsQueueIndex;
    createInfo.presentMode              = swapchainPresentMode;
    createInfo.oldSwapchain             = oldSwapchain;
    createInfo.clipped                  = true;

    vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain);
    
    // @@ Verify success of vkCreateSwapchainKHR
    
    // @@ Do the three step process to retrieve the list of swapchain images into
    //    std::vector<VkImage> m_swapchainImages;
    // using call vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_imageCount, nullptr);
    
    // @@ Verify success of vkGetSwapchainImagesKHR
    
    m_barriers.resize(m_imageCount);
    m_imageViews.resize(m_imageCount);

    // Create an VkImageView for each swap chain image.
    for (uint i=0;  i<m_imageCount;  i++) {
        VkImageViewCreateInfo createInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            createInfo.image = m_swapchainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = surfaceFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            vkCreateImageView(m_device, &createInfo, nullptr, &m_imageViews[i]); }

    // Create three VkImageMemoryBarrier structures (one for each swap
    // chain image) and specify the desired
    // layout (VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) for each.
    for (uint i=0;  i<m_imageCount;  i++) {
        VkImageSubresourceRange range = {0};
        range.aspectMask              = VK_IMAGE_ASPECT_COLOR_BIT;
        range.baseMipLevel            = 0;
        range.levelCount              = VK_REMAINING_MIP_LEVELS;
        range.baseArrayLayer          = 0;
        range.layerCount              = VK_REMAINING_ARRAY_LAYERS;
        
        VkImageMemoryBarrier memBarrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
        memBarrier.dstAccessMask        = 0;
        memBarrier.srcAccessMask        = 0;
        memBarrier.oldLayout            = VK_IMAGE_LAYOUT_UNDEFINED;
        memBarrier.newLayout            = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        memBarrier.image                = m_swapchainImages[i];
        memBarrier.subresourceRange     = range;
        m_barriers[i] = memBarrier;
    }

    // Create a temporary command buffer. submit the layout conversion
    // command, submit and destroy the command buffer.
    VkCommandBuffer cmd = createTempCmdBuffer();
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0,
                         nullptr, m_imageCount, m_barriers.data());
    submitTempCmdBuffer(cmd);

    // Create the three synchronization objects.  These are not
    // technically part of the swap chain, but they are used
    // exclusively for synchronizing the swap chain, so I include them
    // here.
    VkFenceCreateInfo fenceCreateInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_waitFence);
    NAME(m_waitFence, VK_OBJECT_TYPE_FENCE, "m_waitFence");
    
    VkSemaphoreCreateInfo semCreateInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    vkCreateSemaphore(m_device, &semCreateInfo, nullptr, &m_readSemaphore);
    vkCreateSemaphore(m_device, &semCreateInfo, nullptr, &m_writtenSemaphore);
    NAME(m_readSemaphore, VK_OBJECT_TYPE_SEMAPHORE, "m_readSemaphore");
    NAME(m_writtenSemaphore, VK_OBJECT_TYPE_SEMAPHORE, "m_writtenSemaphore");
        
    m_windowSize = swapchainExtent;
    
    // @@ Destroy m_imageViews (looping through all 3)
    //      vkDestroyImageView(m_device, m_imageViews[i], nullptr)
    // @@ Destroy the synchronization items: 
    //      vkDestroyFence(m_device, m_waitFence, nullptr);
    //      vkDestroySemaphore(m_device, m_readSemaphore, nullptr);
    //      vkDestroySemaphore(m_device, m_writtenSemaphore, nullptr);
    // @@ Destroy the actual swapchain with:
    //      vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
}

