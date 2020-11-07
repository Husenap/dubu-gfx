#include "Application.h"

#include <filesystem>
#include <fstream>

constexpr uint32_t WIDTH                = 600;
constexpr uint32_t HEIGHT               = 600;
constexpr int      MAX_FRAMES_IN_FLIGHT = 2;

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

	Subscribe<dubu::window::EventResize>(
	    [&](const auto& e) { mIsMinimized = (e.width == 0 || e.height == 0); },
	    *mWindow);

	InitFramework();
	MainLoop();
}

void Application::MainLoop() {
	static double previousTime = glfwGetTime();
	static int    currentFrame = 0;

	while (!mWindow->ShouldClose()) {
		double currentTime = glfwGetTime();
		double deltaTime   = currentTime - previousTime;
		previousTime       = currentTime;

		mFPS = 0.995 * mFPS + 0.05 * (1.0 / deltaTime);

		if (++currentFrame % 10000 == 0) {
			glfwSetWindowTitle(mWindow->GetGLFWHandle(),
			                   std::to_string(mFPS).c_str());
		}

		mWindow->PollEvents();

		if (!mIsMinimized) {
			DrawFrame();
		}
	}

	mDevice->GetDevice().waitIdle();
}

void Application::InitFramework() {
	CreateInstance();
	CreateSurface();
	CreateDevice();
	CreateSwapchain();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateFramebuffer();
	CreateCommandPool();
	CreateCommandBuffer();
	CreateSyncObjects();
}

void Application::DrawFrame() {
	if (mDevice->GetDevice().waitForFences(
	        *mInFlightFences[mCurrentFrame],
	        VK_TRUE,
	        std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to wait for fences!");
	}

	auto nextImageResult = mDevice->GetDevice().acquireNextImageKHR(
	    mSwapchain->GetSwapchain(),
	    std::numeric_limits<uint64_t>::max(),
	    *mImageAvailableSemaphores[mCurrentFrame]);

	if (nextImageResult.result == vk::Result::eErrorOutOfDateKHR) {
		RecreateSwapchain();
		return;
	} else if (nextImageResult.result != vk::Result::eSuccess ||
	           nextImageResult.result == vk::Result::eSuboptimalKHR) {
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	uint32_t imageIndex = nextImageResult.value;

	if (mImagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		if (mDevice->GetDevice().waitForFences(
		        mImagesInFlight[imageIndex],
		        VK_TRUE,
		        std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess) {
			throw std::runtime_error("Failed to wait for fences!");
		}
	}

	mImagesInFlight[imageIndex] = *mInFlightFences[mCurrentFrame];

	vk::Semaphore waitSemaphores[] = {
	    *mImageAvailableSemaphores[mCurrentFrame]};
	vk::PipelineStageFlags waitStages[] = {
	    vk::PipelineStageFlagBits::eColorAttachmentOutput};
	vk::Semaphore signalSemaphores[] = {
	    *mRenderFinishedSemaphores[mCurrentFrame]};

	mDevice->GetDevice().resetFences(*mInFlightFences[mCurrentFrame]);

	mDevice->GetGraphicsQueue().submit(
	    vk::SubmitInfo{
	        .waitSemaphoreCount = 1,
	        .pWaitSemaphores    = waitSemaphores,
	        .pWaitDstStageMask  = waitStages,
	        .commandBufferCount = 1,
	        .pCommandBuffers    = &mCommandBuffer->GetCommandBuffer(
                static_cast<std::size_t>(imageIndex)),
	        .signalSemaphoreCount = 1,
	        .pSignalSemaphores    = signalSemaphores,
	    },
	    *mInFlightFences[mCurrentFrame]);

	try {
		auto presentResult =
		    mDevice->GetPresentQueue().presentKHR(vk::PresentInfoKHR{
		        .waitSemaphoreCount = 1,
		        .pWaitSemaphores    = signalSemaphores,
		        .swapchainCount     = 1,
		        .pSwapchains        = &mSwapchain->GetSwapchain(),
		        .pImageIndices      = &imageIndex,
		    });
		if (presentResult == vk::Result::eErrorOutOfDateKHR ||
		    presentResult == vk::Result::eSuboptimalKHR) {
			RecreateSwapchain();
		}
	} catch (const vk::OutOfDateKHRError&) {
		RecreateSwapchain();
	}

	mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Application::RecreateSwapchain() {
	if (mIsMinimized) {
		return;
	}

	mDevice->GetDevice().waitIdle();

	mCommandBuffer.reset();
	mFramebuffer.reset();
	mGraphicsPipeline.reset();
	mRenderPass.reset();
	mSwapchain.reset();

	CreateSwapchain();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateFramebuffer();
	CreateCommandBuffer();
}

void Application::CreateInstance() {
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
}

void Application::CreateSurface() {
	VkSurfaceKHR surface;
	if (glfwCreateWindowSurface(mInstance->GetInstance(),
	                            mWindow->GetGLFWHandle(),
	                            nullptr,
	                            &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to created Surface!");
	}

	mSurface = std::make_unique<dubu::gfx::Surface>(
	    vk::UniqueSurfaceKHR(surface, mInstance->GetInstance()));
}

void Application::CreateDevice() {
	mDevice = std::make_unique<dubu::gfx::Device>(mInstance->GetInstance(),
	                                              mSurface->GetSurface());
}

void Application::CreateSwapchain() {
	mSwapchain =
	    std::make_unique<dubu::gfx::Swapchain>(dubu::gfx::Swapchain::CreateInfo{
	        .device         = mDevice->GetDevice(),
	        .physicalDevice = mDevice->GetPhysicalDevice(),
	        .surface        = mSurface->GetSurface(),
	        .extent         = {.width = WIDTH, .height = HEIGHT},
	    });
}

void Application::CreateRenderPass() {
	mRenderPass = std::make_unique<dubu::gfx::RenderPass>(
	    dubu::gfx::RenderPass::CreateInfo{
	        .device      = mDevice->GetDevice(),
	        .attachments = {{
	            .format          = mSwapchain->GetImageFormat(),
	            .samples         = vk::SampleCountFlagBits::e1,
	            .loadOp          = vk::AttachmentLoadOp::eClear,
	            .storeOp         = vk::AttachmentStoreOp::eStore,
	            .stencilLoadOp   = vk::AttachmentLoadOp::eDontCare,
	            .stencilStoreOp  = vk::AttachmentStoreOp::eDontCare,
	            .initialLayout   = vk::ImageLayout::eUndefined,
	            .finalLayout     = vk::ImageLayout::ePresentSrcKHR,
	            .referenceLayout = vk::ImageLayout::eColorAttachmentOptimal,
	        }},
	        .subpasses   = {{vk::PipelineBindPoint::eGraphics, {0}}},
	    });
}

void Application::CreateGraphicsPipeline() {
	dubu::gfx::Shader shader({
	    .device = mDevice->GetDevice(),
	    .shaderModules =
	        {
	            {ReadFile("assets/shaders/shader.vert.spv"),
	             vk::ShaderStageFlagBits::eVertex},
	            {ReadFile("assets/shaders/shader.frag.spv"),
	             vk::ShaderStageFlagBits::eFragment},
	        },
	});

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
	    .vertexBindingDescriptionCount   = 0,
	    .vertexAttributeDescriptionCount = 0,
	};

	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
	    .topology               = vk::PrimitiveTopology::eTriangleList,
	    .primitiveRestartEnable = VK_FALSE,
	};

	mViewportState = std::make_unique<dubu::gfx::ViewportState>(
	    dubu::gfx::ViewportState::CreateInfo{
	        .viewports = {{
	            .x        = 0.f,
	            .y        = 0.f,
	            .width    = static_cast<float>(mSwapchain->GetExtent().width),
	            .height   = static_cast<float>(mSwapchain->GetExtent().height),
	            .minDepth = 0.f,
	            .maxDepth = 1.f,
	        }},
	        .scissors  = {{
                .offset = {.x = 0, .y = 0},
                .extent = mSwapchain->GetExtent(),
            }},
	    });

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

	dubu::gfx::DynamicState dynamicState(
	    {vk::DynamicState::eViewport, vk::DynamicState::eLineWidth});

	vk::UniquePipelineLayout pipelineLayout =
	    mDevice->GetDevice().createPipelineLayoutUnique({});

	mGraphicsPipeline = std::make_unique<dubu::gfx::GraphicsPipeline>(
	    dubu::gfx::GraphicsPipeline::CreateInfo{
	        .device             = mDevice->GetDevice(),
	        .shaderStages       = shader.GetShaderStages(),
	        .vertexInputState   = vertexInputInfo,
	        .inputAssemblyState = inputAssemblyInfo,
	        .viewportState      = mViewportState->GetViewportState(),
	        .rasterizationState = rasterizerInfo,
	        .multisampleState   = multisampleInfo,
	        .colorBlendState    = colorBlendInfo,
	        .dynamicState       = dynamicState.GetDynamicState(),
	        .pipelineLayout     = *pipelineLayout,
	        .renderPass         = mRenderPass->GetRenderPass(),
	        .subpass            = 0,
	    });
}

void Application::CreateFramebuffer() {
	mFramebuffer = std::make_unique<dubu::gfx::Framebuffer>(
	    dubu::gfx::Framebuffer::CreateInfo{
	        .device     = mDevice->GetDevice(),
	        .renderPass = mRenderPass->GetRenderPass(),
	        .imageViews = mSwapchain->GetImageViews(),
	        .extent     = mSwapchain->GetExtent(),
	    });
}

void Application::CreateCommandPool() {
	mCommandPool = std::make_unique<dubu::gfx::CommandPool>(
	    dubu::gfx::CommandPool::CreateInfo{
	        .device         = mDevice->GetDevice(),
	        .physicalDevice = mDevice->GetPhysicalDevice(),
	        .surface        = mSurface->GetSurface(),
	    });
}

void Application::CreateCommandBuffer() {
	mCommandBuffer = std::make_unique<dubu::gfx::CommandBuffer>(
	    dubu::gfx::CommandBuffer::CreateInfo{
	        .device      = mDevice->GetDevice(),
	        .commandPool = mCommandPool->GetCommandPool(),
	        .bufferCount = mFramebuffer->GetFramebufferCount(),
	    });

	mCommandBuffer->RecordCommands({
	    dubu::gfx::DrawingCommands::BeginRenderPass{
	        .renderPass   = mRenderPass->GetRenderPass(),
	        .framebuffers = mFramebuffer->GetFramebuffers(),
	        .renderArea = {.offset = {0, 0}, .extent = mSwapchain->GetExtent()},
	        .clearColor =
	            vk::ClearColorValue(std::array<float, 4>{0.f, 0.f, 0.f, 1.f}),
	    },

	    dubu::gfx::DrawingCommands::BindPipeline{
	        .pipeline  = mGraphicsPipeline->GetPipeline(),
	        .bindPoint = vk::PipelineBindPoint::eGraphics,
	    },

	    dubu::gfx::DrawingCommands::SetViewport{
	        .viewports = mViewportState->GetViewports(),
	    },

	    dubu::gfx::DrawingCommands::Draw{
	        .vertexCount   = 3,
	        .instanceCount = 1,
	    },

	    dubu::gfx::DrawingCommands::EndRenderPass{},
	});
}

void Application::CreateSyncObjects() {
	mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	mImagesInFlight.resize(mSwapchain->GetImageViews().size());
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		mRenderFinishedSemaphores[i] =
		    mDevice->GetDevice().createSemaphoreUnique({});
		mImageAvailableSemaphores[i] =
		    mDevice->GetDevice().createSemaphoreUnique({});
		mInFlightFences[i] = mDevice->GetDevice().createFenceUnique(
		    {.flags = vk::FenceCreateFlagBits::eSignaled});
	}
}
