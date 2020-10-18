#include "Application.h"

#include <filesystem>
#include <fstream>

constexpr uint32_t WIDTH  = 600;
constexpr uint32_t HEIGHT = 600;

dubu::gfx::blob ReadFile(std::filesystem::path filepath) {
	std::ifstream file(filepath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		return {};
	}

	dubu::gfx::blob data(static_cast<std::size_t>(file.tellg()));

	file.seekg(0);
	file.read(data.data(), data.size());

	return data;
}

void Application::Run() {
	mWindow =
	    std::make_unique<dubu::window::GLFWWindow>(WIDTH, HEIGHT, "dubu-gfx");

	InitFramework();
	MainLoop();
}

void Application::InitFramework() {
	uint32_t     extensionCount = 0;
	const char** extensionNames =
	    glfwGetRequiredInstanceExtensions(&extensionCount);
	std::vector<const char*> requiredExtensions;
	for (uint32_t i = 0; i < extensionCount; ++i) {
		requiredExtensions.push_back(extensionNames[i]);
	}

	mInstance =
	    std::make_unique<dubu::gfx::Instance>(dubu::gfx::Instance::CreateInfo{
	        .applicationName    = "sample app",
	        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
	        .engineName         = "dubu-vk",
	        .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
	        .requiredExtensions = requiredExtensions,
	    });

	VkSurfaceKHR surface;
	if (glfwCreateWindowSurface(mInstance->GetInstance(),
	                            mWindow->GetGLFWHandle(),
	                            nullptr,
	                            &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to created Surface!");
	}

	mSurface = std::make_unique<dubu::gfx::Surface>(
	    vk::UniqueSurfaceKHR(surface, mInstance->GetInstance()));

	mDevice = std::make_unique<dubu::gfx::Device>(mInstance->GetInstance(),
	                                              mSurface->GetSurface());

	mSwapchain = std::make_unique<dubu::gfx::Swapchain>(
	    mDevice->GetDevice(),
	    mDevice->GetPhysicalDevice(),
	    mSurface->GetSurface(),
	    vk::Extent2D{.width = WIDTH, .height = HEIGHT});

	dubu::gfx::ShaderModule vertexShaderModule(
	    mDevice->GetDevice(),
	    ReadFile("assets/shaders/shader.vert.spv"),
	    vk::ShaderStageFlagBits::eVertex);
	dubu::gfx::ShaderModule fragmentShaderModule(
	    mDevice->GetDevice(),
	    ReadFile("assets/shaders/shader.frag.spv"),
	    vk::ShaderStageFlagBits::eFragment);

	vk::PipelineShaderStageCreateInfo shaderStages[] = {
	    vertexShaderModule.GetPipelineShaderStageCreateInfo(),
	    fragmentShaderModule.GetPipelineShaderStageCreateInfo(),
	};

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
	    .vertexBindingDescriptionCount   = 0,
	    .vertexAttributeDescriptionCount = 0,
	};

	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
	    .topology               = vk::PrimitiveTopology::eTriangleList,
	    .primitiveRestartEnable = VK_FALSE,
	};

	vk::Viewport viewport{
	    .x        = 0.f,
	    .y        = 0.f,
	    .width    = static_cast<float>(mSwapchain->GetExtent().width),
	    .height   = static_cast<float>(mSwapchain->GetExtent().height),
	    .minDepth = 0.f,
	    .maxDepth = 1.f,
	};

	vk::Rect2D scissor{
	    .offset = {.x = 0, .y = 0},
	    .extent = mSwapchain->GetExtent(),
	};

	vk::PipelineViewportStateCreateInfo viewportStateInfo{
	    .viewportCount = 1,
	    .pViewports    = &viewport,
	    .scissorCount  = 1,
	    .pScissors     = &scissor,
	};

	vk::PipelineRasterizationStateCreateInfo rasterizerInfo{
	    .depthClampEnable        = VK_FALSE,
	    .rasterizerDiscardEnable = VK_FALSE,
	    .polygonMode             = vk::PolygonMode::eFill,
	    .cullMode                = vk::CullModeFlagBits::eBack,
	    .frontFace               = vk::FrontFace::eClockwise,
	    .depthBiasEnable         = VK_FALSE,
	    .lineWidth               = 1.f,
	};

	vk::PipelineMultisampleStateCreateInfo multisampleInfo{
	    .rasterizationSamples = vk::SampleCountFlagBits::e1,
	    .sampleShadingEnable  = VK_FALSE,
	};

	vk::PipelineColorBlendAttachmentState colorBlendAttachement{
	    .blendEnable = VK_FALSE,
	    .colorWriteMask =
	        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
	        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
	};

	vk::PipelineColorBlendStateCreateInfo colorBlendInfo{
	    .logicOpEnable   = VK_FALSE,
	    .attachmentCount = 1,
	    .pAttachments    = &colorBlendAttachement,
	};

	vk::DynamicState dynamicStates[] = {vk::DynamicState::eViewport,
	                                    vk::DynamicState::eLineWidth};

	vk::PipelineDynamicStateCreateInfo dynamicStateInfo{
	    .dynamicStateCount = 2,
	    .pDynamicStates    = dynamicStates,
	};

	vk::UniquePipelineLayout pipelineLayout =
	    mDevice->GetDevice().createPipelineLayoutUnique({});

	vk::AttachmentDescription colorAttachment{
	    .format         = mSwapchain->GetImageFormat(),
	    .samples        = vk::SampleCountFlagBits::e1,
	    .loadOp         = vk::AttachmentLoadOp::eClear,
	    .storeOp        = vk::AttachmentStoreOp::eStore,
	    .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
	    .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
	    .initialLayout  = vk::ImageLayout::eUndefined,
	    .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
	};

	vk::AttachmentReference colorAttachmentReference{
	    .attachment = 0,
	    .layout     = vk::ImageLayout::eColorAttachmentOptimal,
	};

	vk::SubpassDescription subpassDescription{
	    .pipelineBindPoint    = vk::PipelineBindPoint::eGraphics,
	    .colorAttachmentCount = 1,
	    .pColorAttachments    = &colorAttachmentReference,
	};

	vk::UniqueRenderPass renderPass =
	    mDevice->GetDevice().createRenderPassUnique(vk::RenderPassCreateInfo{
	        .attachmentCount = 1,
	        .pAttachments    = &colorAttachment,
	        .subpassCount    = 1,
	        .pSubpasses      = &subpassDescription,
	    });
}

void Application::MainLoop() {
	while (!mWindow->ShouldClose()) {
		mWindow->PollEvents();
	}
}
