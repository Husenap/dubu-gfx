#include "Application.h"

#include <filesystem>
#include <fstream>

#include <dubu_gfx/vulkan/QueueFamilyIndices.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_vulkan.h>
#include <imgui/imgui.h>

constexpr uint32_t WIDTH                = 1600;
constexpr uint32_t HEIGHT               = 900;
constexpr int      MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<Vertex> VERTICES = {
    {{-1.0f, -1.0f}, {0.988f, 0.812f, 0.651f}},
    {{+1.0f, -1.0f}, {0.996f, 0.592f, 0.643f}},
    {{+1.0f, +1.0f}, {1.000f, 0.373f, 0.635f}},
    {{-1.0f, +1.0f}, {0.996f, 0.592f, 0.643f}},
};
const std::vector<uint16_t> INDICES = {0, 1, 2, 0, 2, 3};

dubu::gfx::blob ReadFile(std::filesystem::path filepath) {
	std::ifstream file(filepath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		return {};
	}

	dubu::gfx::blob data(static_cast<std::size_t>(file.tellg()));

	file.seekg(0);
	file.read(data.data(), static_cast<std::streamsize>(data.size()));

	return data;
}

void Application::Run() {
	mWindow =
	    std::make_unique<dubu::window::GLFWWindow>(WIDTH, HEIGHT, "dubu-gfx");

	Subscribe<dubu::window::EventResize>(
	    [&](const auto& e) { mIsMinimized = (e.width == 0 || e.height == 0); },
	    *mWindow);
	Subscribe<dubu::window::EventKeyPress>(
	    [&](const auto& e) {
		    switch (e.key) {
		    case dubu::window::Key::KeyGraveAccent:
			    mShowDemoWindow = !mShowDemoWindow;
			    break;
		    case dubu::window::Key::KeyEscape:
			    glfwSetWindowShouldClose(mWindow->GetGLFWHandle(), GLFW_TRUE);
			    break;
		    default:
			    break;
		    }
		    if (e.key == dubu::window::Key::KeyGraveAccent) {
		    }
	    },
	    *mWindow);

	double time0 = glfwGetTime();
	InitFramework();
	InitImGui();
	double time1 = glfwGetTime();
	DUBU_LOG_INFO("Initialization took {:.4f}ms", (time1 - time0) * 1000.0);
	MainLoop();
}

void Application::MainLoop() {
	while (!mWindow->ShouldClose()) {
		mWindow->PollEvents();

		if (!mIsMinimized) {
			DrawFrame();
		} else {
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			Update();
			ImGui::Render();
			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}
	}

	mDevice->GetDevice().waitIdle();

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Application::Update() {
	if (mShowDemoWindow) {
		ImGui::ShowDemoWindow(&mShowDemoWindow);
	}
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
	CreateVertexBuffer();
	CreateCommandBuffer();
	CreateSyncObjects();
}

static void CheckVkResult(VkResult err) {
	if (err == 0) {
		return;
	}
	DUBU_LOG_ERROR("[vulkan] Error: VkResult = {}", err);
	if (err < 0) {
		abort();
	}
}

void Application::InitImGui() {
	const uint32_t commonPoolSize = 1000;

	std::vector<vk::DescriptorPoolSize> poolSizes = {
	    {vk::DescriptorType::eSampler, commonPoolSize},
	    {vk::DescriptorType::eCombinedImageSampler, commonPoolSize},
	    {vk::DescriptorType::eSampledImage, commonPoolSize},
	    {vk::DescriptorType::eStorageImage, commonPoolSize},
	    {vk::DescriptorType::eUniformTexelBuffer, commonPoolSize},
	    {vk::DescriptorType::eStorageTexelBuffer, commonPoolSize},
	    {vk::DescriptorType::eUniformBuffer, commonPoolSize},
	    {vk::DescriptorType::eStorageBuffer, commonPoolSize},
	    {vk::DescriptorType::eUniformBufferDynamic, commonPoolSize},
	    {vk::DescriptorType::eStorageBufferDynamic, commonPoolSize},
	    {vk::DescriptorType::eInputAttachment, commonPoolSize},
	};
	mImGuiDescriptorPool = std::make_unique<dubu::gfx::DescriptorPool>(
	    dubu::gfx::DescriptorPool::CreateInfo{
	        .device    = mDevice->GetDevice(),
	        .poolSizes = poolSizes,
	        .maxSets = static_cast<uint32_t>(poolSizes.size()) * commonPoolSize,
	    });

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	io.ConfigViewportsNoAutoMerge  = true;
	io.ConfigViewportsNoDecoration = false;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding              = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplGlfw_InitForVulkan(mWindow->GetGLFWHandle(), true);
	ImGui_ImplVulkan_InitInfo initInfo{
	    .Instance        = mInstance->GetInstance(),
	    .PhysicalDevice  = mDevice->GetPhysicalDevice(),
	    .Device          = mDevice->GetDevice(),
	    .QueueFamily     = *mDevice->GetQueueFamilies().graphicsFamily,
	    .Queue           = mDevice->GetGraphicsQueue(),
	    .PipelineCache   = VK_NULL_HANDLE,
	    .DescriptorPool  = mImGuiDescriptorPool->GetDescriptorPool(),
	    .MinImageCount   = mFramebuffer->GetFramebufferCount(),
	    .ImageCount      = mFramebuffer->GetFramebufferCount(),
	    .CheckVkResultFn = CheckVkResult,
	};
	ImGui_ImplVulkan_Init(&initInfo, mRenderPass->GetRenderPass());

	{
		dubu::gfx::CommandPool commandPool(dubu::gfx::CommandPool::CreateInfo{
		    .device        = mDevice->GetDevice(),
		    .queueFamilies = mDevice->GetQueueFamilies(),
		});

		dubu::gfx::CommandBuffer commandBuffer(
		    dubu::gfx::CommandBuffer::CreateInfo{
		        .device      = mDevice->GetDevice(),
		        .commandPool = commandPool.GetCommandPool(),
		        .bufferCount = 1,
		    });

		commandBuffer.GetCommandBuffer(0).begin(vk::CommandBufferBeginInfo{
		    .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
		});

		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer.GetCommandBuffer(0));

		commandBuffer.GetCommandBuffer(0).end();

		mDevice->GetGraphicsQueue().submit(vk::SubmitInfo{
		    .commandBufferCount = 1,
		    .pCommandBuffers    = &commandBuffer.GetCommandBuffer(0),
		});

		mDevice->GetGraphicsQueue().waitIdle();

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
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

	ImGui_ImplVulkan_SetMinImageCount(mFramebuffer->GetFramebufferCount());
}

void Application::RecordCommands(uint32_t imageIndex) {
	mCommandBuffer->RecordCommands(
	    static_cast<std::size_t>(imageIndex),
	    {
	        dubu::gfx::DrawingCommands::BeginRenderPass{
	            .renderPass   = mRenderPass->GetRenderPass(),
	            .framebuffers = mFramebuffer->GetFramebuffers(),
	            .renderArea   = {.offset = {0, 0},
                               .extent = mSwapchain->GetExtent()},
	            .clearColor   = vk::ClearColorValue(
                    std::array<float, 4>{0.f, 0.f, 0.f, 1.f}),
	        },

	        dubu::gfx::DrawingCommands::BindPipeline{
	            .pipeline  = mGraphicsPipeline->GetPipeline(),
	            .bindPoint = vk::PipelineBindPoint::eGraphics,
	        },

	        dubu::gfx::DrawingCommands::SetViewport{
	            .viewports = mViewportState->GetViewports(),
	        },

	        dubu::gfx::DrawingCommands::BindVertexBuffers{
	            .buffers = {mVertexBuffer->GetBuffer()},
	            .offsets = {0},
	        },
	        dubu::gfx::DrawingCommands::BindIndexBuffer{
	            .buffer    = mIndexBuffer->GetBuffer(),
	            .offset    = 0,
	            .indexType = vk::IndexType::eUint16,
	        },

	        dubu::gfx::DrawingCommands::DrawIndexed{
	            .indexCount    = static_cast<uint32_t>(INDICES.size()),
	            .instanceCount = 1,
	        },

	        dubu::gfx::DrawingCommands::Custom{
	            .customFunction =
	                [](const vk::CommandBuffer& commandBuffer, std::size_t) {
		                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
		                                                commandBuffer);
	                }},

	        dubu::gfx::DrawingCommands::EndRenderPass{},
	    });
}

void Application::DrawFrame() {
	if (mDevice->GetDevice().waitForFences(
	        *mInFlightFences[mCurrentFrame],
	        VK_TRUE,
	        std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess) {
		DUBU_LOG_FATAL("Failed to wait for fences!");
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
		DUBU_LOG_FATAL("Failed to acquire swap chain image!");
	}

	uint32_t imageIndex = nextImageResult.value;

	if (mImagesInFlight[imageIndex]) {
		if (mDevice->GetDevice().waitForFences(
		        mImagesInFlight[imageIndex],
		        VK_TRUE,
		        std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess) {
			DUBU_LOG_FATAL("Failed to wait for fences!");
		}
	}
	mImagesInFlight[imageIndex] = *mInFlightFences[mCurrentFrame];

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	Update();
	ImGui::Render();
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	RecordCommands(imageIndex);

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
		DUBU_LOG_FATAL("Failed to created Surface!");
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

	auto bindingDescription    = Vertex::GetBindingDescription();
	auto attributeDescriptions = Vertex::GetAttributeDescriptions();
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
	    .vertexBindingDescriptionCount = 1,
	    .pVertexBindingDescriptions    = &bindingDescription,
	    .vertexAttributeDescriptionCount =
	        static_cast<uint32_t>(attributeDescriptions.size()),
	    .pVertexAttributeDescriptions = attributeDescriptions.data(),
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
	        .device        = mDevice->GetDevice(),
	        .queueFamilies = mDevice->GetQueueFamilies(),
	    });
}

void Application::CreateVertexBuffer() {
	{
		dubu::gfx::Buffer stagingBuffer(dubu::gfx::Buffer::CreateInfo{
		    .device         = mDevice->GetDevice(),
		    .physicalDevice = mDevice->GetPhysicalDevice(),
		    .size  = static_cast<uint32_t>(VERTICES.size()) * sizeof(Vertex),
		    .usage = vk::BufferUsageFlagBits::eTransferSrc,
		    .sharingMode      = vk::SharingMode::eExclusive,
		    .memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
		                        vk::MemoryPropertyFlagBits::eHostCoherent,
		});

		stagingBuffer.SetData(VERTICES);

		mVertexBuffer =
		    std::make_unique<dubu::gfx::Buffer>(dubu::gfx::Buffer::CreateInfo{
		        .device         = mDevice->GetDevice(),
		        .physicalDevice = mDevice->GetPhysicalDevice(),
		        .size = static_cast<uint32_t>(VERTICES.size()) * sizeof(Vertex),
		        .usage = vk::BufferUsageFlagBits::eVertexBuffer |
		                 vk::BufferUsageFlagBits::eTransferDst,
		        .sharingMode      = vk::SharingMode::eExclusive,
		        .memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
		    });

		mVertexBuffer->SetData(stagingBuffer.GetBuffer(),
		                       mDevice->GetQueueFamilies(),
		                       mDevice->GetGraphicsQueue());
	}
	{
		dubu::gfx::Buffer stagingBuffer(dubu::gfx::Buffer::CreateInfo{
		    .device         = mDevice->GetDevice(),
		    .physicalDevice = mDevice->GetPhysicalDevice(),
		    .size  = static_cast<uint32_t>(INDICES.size()) * sizeof(uint16_t),
		    .usage = vk::BufferUsageFlagBits::eTransferSrc,
		    .sharingMode      = vk::SharingMode::eExclusive,
		    .memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
		                        vk::MemoryPropertyFlagBits::eHostCoherent,
		});

		stagingBuffer.SetData(INDICES);

		mIndexBuffer =
		    std::make_unique<dubu::gfx::Buffer>(dubu::gfx::Buffer::CreateInfo{
		        .device         = mDevice->GetDevice(),
		        .physicalDevice = mDevice->GetPhysicalDevice(),
		        .size =
		            static_cast<uint32_t>(INDICES.size()) * sizeof(uint16_t),
		        .usage = vk::BufferUsageFlagBits::eIndexBuffer |
		                 vk::BufferUsageFlagBits::eTransferDst,
		        .sharingMode      = vk::SharingMode::eExclusive,
		        .memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
		    });

		mIndexBuffer->SetData(stagingBuffer.GetBuffer(),
		                      mDevice->GetQueueFamilies(),
		                      mDevice->GetGraphicsQueue());
	}
}

void Application::CreateCommandBuffer() {
	mCommandBuffer = std::make_unique<dubu::gfx::CommandBuffer>(
	    dubu::gfx::CommandBuffer::CreateInfo{
	        .device      = mDevice->GetDevice(),
	        .commandPool = mCommandPool->GetCommandPool(),
	        .bufferCount = mFramebuffer->GetFramebufferCount(),
	    });
}

void Application::CreateSyncObjects() {
	mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	mImagesInFlight.resize(mSwapchain->GetImageViews().size());
	for (std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		mRenderFinishedSemaphores[i] =
		    mDevice->GetDevice().createSemaphoreUnique({});
		mImageAvailableSemaphores[i] =
		    mDevice->GetDevice().createSemaphoreUnique({});
		mInFlightFences[i] = mDevice->GetDevice().createFenceUnique(
		    {.flags = vk::FenceCreateFlagBits::eSignaled});
	}
}
