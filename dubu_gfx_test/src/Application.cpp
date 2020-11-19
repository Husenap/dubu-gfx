#include "Application.h"

#include <cmath>
#include <filesystem>
#include <fstream>

#include <dubu_gfx/vulkan/QueueFamilyIndices.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_vulkan.h>
#include <imgui/imgui.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

constexpr uint32_t WIDTH                = 1600;
constexpr uint32_t HEIGHT               = 900;
constexpr int      MAX_FRAMES_IN_FLIGHT = 2;

dubu::gfx::blob ReadFile(std::filesystem::path filepath) {
	std::ifstream file(filepath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		DUBU_LOG_ERROR("Failed to open file: {}", filepath);
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
	CreateDepthResources();
	CreateRenderPass();
	CreateDescriptorSetLayouts();
	CreateGraphicsPipeline();
	CreateFramebuffer();
	CreateCommandPool();
	CreateUniformBuffers();
	CreateDescriptorPool();
	CreateDescriptorSets();
	CreateCommandBuffer();
	CreateSyncObjects();
	CreateModel();
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
	    .MSAASamples     = VK_SAMPLE_COUNT_1_BIT,
	    .Allocator       = nullptr,
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
	mDescriptorSet.reset();
	mFramebuffer.reset();
	mGraphicsPipeline.reset();
	mPipelineLayout.reset();
	mRenderPass.reset();
	mDepthImage.reset();
	mSwapchain.reset();

	CreateSwapchain();
	CreateDepthResources();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateFramebuffer();
	CreateDescriptorSets();
	CreateCommandBuffer();

	ImGui_ImplVulkan_SetMinImageCount(mFramebuffer->GetFramebufferCount());
}

void Application::RecordCommands(uint32_t imageIndex) {
	std::vector<dubu::gfx::DrawingCommand> drawingCommands;

	drawingCommands.push_back(dubu::gfx::DrawingCommands::BeginRenderPass{
	    .renderPass   = mRenderPass->GetRenderPass(),
	    .framebuffers = mFramebuffer->GetFramebuffers(),
	    .renderArea   = {.offset = {0, 0}, .extent = mSwapchain->GetExtent()},
	    .clearValues =
	        {
	            vk::ClearColorValue(std::array<float, 4>{0.f, 0.f, 0.f, 1.f}),
	            vk::ClearDepthStencilValue{.depth = 1.f},
	        },
	});
	drawingCommands.push_back(dubu::gfx::DrawingCommands::BindPipeline{
	    .pipeline  = mGraphicsPipeline->GetPipeline(),
	    .bindPoint = vk::PipelineBindPoint::eGraphics,
	});
	drawingCommands.push_back(dubu::gfx::DrawingCommands::SetViewport{
	    .viewports = mViewportState->GetViewports(),
	});
	drawingCommands.push_back(dubu::gfx::DrawingCommands::SetViewport{
	    .viewports = mViewportState->GetViewports(),
	});

	drawingCommands.push_back(dubu::gfx::DrawingCommands::BindDescriptorSets{
	    .bindPoint      = vk::PipelineBindPoint::eGraphics,
	    .pipelineLayout = *mPipelineLayout,
	    .descriptorSets = {mDescriptorSet->GetDescriptorSet(
	        static_cast<std::size_t>(imageIndex))},
	});

	mModel->RecordCommands(*mPipelineLayout, drawingCommands);

	drawingCommands.push_back(dubu::gfx::DrawingCommands::Custom{
	    .customFunction =
	        [](const vk::CommandBuffer& commandBuffer, std::size_t) {
		        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
		                                        commandBuffer);
	        },
	});
	drawingCommands.push_back(dubu::gfx::DrawingCommands::EndRenderPass{});

	mCommandBuffer->RecordCommands(static_cast<std::size_t>(imageIndex),
	                               drawingCommands);
}

void Application::UpdateUniformBuffer(uint32_t imageIndex) {
	auto time = static_cast<float>(glfwGetTime());
	time = 0.f;

	UniformBufferObject ubo{
	    .model = glm::mat4(1.f),
	    .view = glm::lookAt(glm::vec3(2.f, 0.f, 2.f),
	                        glm::vec3(0.f, 0.f, 0.f),
	                        glm::vec3(0.f, 1.f, 0.f)),
	    .projection = glm::perspective(
	        glm::radians(45.f),
	        static_cast<float>(mSwapchain->GetExtent().width) /
	            static_cast<float>(mSwapchain->GetExtent().height),
	        0.1f,
	        100.f),
	};

	ubo.projection[1][1] *= -1.f;

	mUniformBuffers[imageIndex]->SetData(ubo);
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

	UpdateUniformBuffer(imageIndex);

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
	        .device = mDevice->GetDevice(),
	        .attachments =
	            {{
	                 .format         = mSwapchain->GetImageFormat(),
	                 .samples        = vk::SampleCountFlagBits::e1,
	                 .loadOp         = vk::AttachmentLoadOp::eClear,
	                 .storeOp        = vk::AttachmentStoreOp::eStore,
	                 .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
	                 .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
	                 .initialLayout  = vk::ImageLayout::eUndefined,
	                 .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
	                 .referenceLayout =
	                     vk::ImageLayout::eColorAttachmentOptimal,
	             },
	             {
	                 .format         = mDepthImage->GetFormat(),
	                 .samples        = vk::SampleCountFlagBits::e1,
	                 .loadOp         = vk::AttachmentLoadOp::eClear,
	                 .storeOp        = vk::AttachmentStoreOp::eDontCare,
	                 .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
	                 .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
	                 .initialLayout  = vk::ImageLayout::eUndefined,
	                 .finalLayout =
	                     vk::ImageLayout::eDepthStencilAttachmentOptimal,
	                 .referenceLayout =
	                     vk::ImageLayout::eDepthStencilAttachmentOptimal,
	             }},
	        .subpasses = {{
	            .bindPoint              = vk::PipelineBindPoint::eGraphics,
	            .colorAttachmentIndices = {0},
	            .depthAttachmentIndex   = 1,
	        }},
	    });
}

void Application::CreateDescriptorSetLayouts() {
	mDescriptorSetLayouts.scene =
	    std::make_unique<dubu::gfx::DescriptorSetLayout>(
	        dubu::gfx::DescriptorSetLayout::CreateInfo{
	            .device   = mDevice->GetDevice(),
	            .bindings = {{
	                .binding         = 0,
	                .descriptorType  = vk::DescriptorType::eUniformBuffer,
	                .descriptorCount = 1,
	                .stageFlags      = vk::ShaderStageFlagBits::eVertex,
	            }},
	        });
	mDescriptorSetLayouts.material =
	    std::make_unique<dubu::gfx::DescriptorSetLayout>(
	        dubu::gfx::DescriptorSetLayout::CreateInfo{
	            .device = mDevice->GetDevice(),
	            .bindings =
	                {
	                    {
	                        .binding = 0,
	                        .descriptorType =
	                            vk::DescriptorType::eCombinedImageSampler,
	                        .descriptorCount = 1,
	                        .stageFlags = vk::ShaderStageFlagBits::eFragment,
	                    },
	                    {
	                        .binding = 1,
	                        .descriptorType =
	                            vk::DescriptorType::eCombinedImageSampler,
	                        .descriptorCount = 1,
	                        .stageFlags = vk::ShaderStageFlagBits::eFragment,
	                    },
	                    {
	                        .binding = 2,
	                        .descriptorType =
	                            vk::DescriptorType::eCombinedImageSampler,
	                        .descriptorCount = 1,
	                        .stageFlags = vk::ShaderStageFlagBits::eFragment,
	                    },
	                },
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

	const auto bindingDescription    = Vertex::GetBindingDescription();
	const auto attributeDescriptions = Vertex::GetAttributeDescriptions();
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
	    .cullMode                = vk::CullModeFlagBits::eNone,
	    .frontFace               = vk::FrontFace::eClockwise,
	    .depthBiasEnable         = VK_FALSE,
	    .lineWidth               = 1.f,
	};

	vk::PipelineMultisampleStateCreateInfo multisampleInfo{
	    .rasterizationSamples = vk::SampleCountFlagBits::e1,
	    .sampleShadingEnable  = VK_FALSE,
	};

	vk::PipelineDepthStencilStateCreateInfo depthStencilInfo{
	    .depthTestEnable       = VK_TRUE,
	    .depthWriteEnable      = VK_TRUE,
	    .depthCompareOp        = vk::CompareOp::eLess,
	    .depthBoundsTestEnable = VK_FALSE,
	    .stencilTestEnable     = VK_FALSE,
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

	std::vector<vk::DescriptorSetLayout> descriptorSetLayouts{
	    mDescriptorSetLayouts.scene->GetDescriptorSetLayout(),
	    mDescriptorSetLayouts.material->GetDescriptorSetLayout(),
	};
	mPipelineLayout = mDevice->GetDevice().createPipelineLayoutUnique(
	    vk::PipelineLayoutCreateInfo{
	        .setLayoutCount =
	            static_cast<uint32_t>(descriptorSetLayouts.size()),
	        .pSetLayouts = descriptorSetLayouts.data(),
	    });

	mGraphicsPipeline = std::make_unique<dubu::gfx::GraphicsPipeline>(
	    dubu::gfx::GraphicsPipeline::CreateInfo{
	        .device             = mDevice->GetDevice(),
	        .shaderStages       = shader.GetShaderStages(),
	        .vertexInputState   = vertexInputInfo,
	        .inputAssemblyState = inputAssemblyInfo,
	        .viewportState      = mViewportState->GetViewportState(),
	        .rasterizationState = rasterizerInfo,
	        .multisampleState   = multisampleInfo,
	        .depthStencilState  = depthStencilInfo,
	        .colorBlendState    = colorBlendInfo,
	        .dynamicState       = dynamicState.GetDynamicState(),
	        .pipelineLayout     = *mPipelineLayout,
	        .renderPass         = mRenderPass->GetRenderPass(),
	        .subpass            = 0,
	    });
}

void Application::CreateFramebuffer() {
	mFramebuffer = std::make_unique<dubu::gfx::Framebuffer>(
	    dubu::gfx::Framebuffer::CreateInfo{
	        .device         = mDevice->GetDevice(),
	        .renderPass     = mRenderPass->GetRenderPass(),
	        .imageViews     = mSwapchain->GetImageViews(),
	        .depthImageView = mDepthImage->GetImageView(),
	        .extent         = mSwapchain->GetExtent(),
	    });
}

void Application::CreateCommandPool() {
	mCommandPool = std::make_unique<dubu::gfx::CommandPool>(
	    dubu::gfx::CommandPool::CreateInfo{
	        .device        = mDevice->GetDevice(),
	        .queueFamilies = mDevice->GetQueueFamilies(),
	    });
}

void Application::CreateDepthResources() {
	mDepthImage =
	    std::make_unique<dubu::gfx::Image>(dubu::gfx::Image::CreateInfo{
	        .device         = mDevice->GetDevice(),
	        .physicalDevice = mDevice->GetPhysicalDevice(),
	        .imageInfo =
	            {
	                .imageType = vk::ImageType::e2D,
	                .format    = vk::Format::eD32Sfloat,
	                .extent =
	                    {
	                        .width  = mSwapchain->GetExtent().width,
	                        .height = mSwapchain->GetExtent().height,
	                        .depth  = 1,
	                    },
	                .mipLevels   = 1,
	                .arrayLayers = 1,
	                .samples     = vk::SampleCountFlagBits::e1,
	                .tiling      = vk::ImageTiling::eOptimal,
	                .usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
	                .sharingMode   = vk::SharingMode::eExclusive,
	                .initialLayout = vk::ImageLayout::eUndefined,
	            },
	        .memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
	        .aspectMask       = vk::ImageAspectFlagBits::eDepth,
	    });

	mDepthImage->TransitionImageLayout(
	    vk::ImageLayout::eUndefined,
	    vk::ImageLayout::eDepthStencilAttachmentOptimal,
	    mDevice->GetQueueFamilies(),
	    mDevice->GetGraphicsQueue());
}

void Application::CreateModel() {
	mModel = std::make_unique<Model>(Model::CreateInfo{
	    .device         = mDevice->GetDevice(),
	    .physicalDevice = mDevice->GetPhysicalDevice(),
	    .queueFamilies  = mDevice->GetQueueFamilies(),
	    .graphicsQueue  = mDevice->GetGraphicsQueue(),
	    .filepath       = "assets/models/cerberus.glb",
	    .descriptorPool = mDescriptorPool->GetDescriptorPool(),
	    .descriptorSetLayout =
	        mDescriptorSetLayouts.material->GetDescriptorSetLayout(),
	});
}

void Application::CreateUniformBuffers() {
	for (std::size_t i = 0; i < mSwapchain->GetImageCount(); ++i) {
		mUniformBuffers.push_back(
		    std::make_unique<dubu::gfx::Buffer>(dubu::gfx::Buffer::CreateInfo{
		        .device           = mDevice->GetDevice(),
		        .physicalDevice   = mDevice->GetPhysicalDevice(),
		        .size             = sizeof(UniformBufferObject),
		        .usage            = vk::BufferUsageFlagBits::eUniformBuffer,
		        .sharingMode      = vk::SharingMode::eExclusive,
		        .memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
		                            vk::MemoryPropertyFlagBits::eHostCoherent,
		    }));
	}
}

void Application::CreateDescriptorPool() {
	mDescriptorPool = std::make_unique<dubu::gfx::DescriptorPool>(
	    dubu::gfx::DescriptorPool::CreateInfo{
	        .device = mDevice->GetDevice(),
	        .poolSizes =
	            {
	                {vk::DescriptorType::eUniformBuffer, 100},
	                {vk::DescriptorType::eCombinedImageSampler, 100},
	            },
	        .maxSets = 2 * 100,
	    });
}

void Application::CreateDescriptorSets() {
	mDescriptorSet = std::make_unique<dubu::gfx::DescriptorSet>(
	    dubu::gfx::DescriptorSet::CreateInfo{
	        .device         = mDevice->GetDevice(),
	        .descriptorPool = mDescriptorPool->GetDescriptorPool(),
	        .descriptorSetCount =
	            static_cast<uint32_t>(mSwapchain->GetImageCount()),
	        .layouts = std::vector<vk::DescriptorSetLayout>(
	            mSwapchain->GetImageCount(),
	            mDescriptorSetLayouts.scene->GetDescriptorSetLayout()),
	    });

	for (std::size_t i = 0; i < mSwapchain->GetImageCount(); ++i) {
		std::vector<vk::WriteDescriptorSet> descriptorWrites;
		vk::DescriptorBufferInfo            bufferInfo{
            .buffer = mUniformBuffers[i]->GetBuffer(),
            .offset = 0,
            .range  = VK_WHOLE_SIZE,
        };
		descriptorWrites.push_back(vk::WriteDescriptorSet{
		    .dstBinding      = 0,
		    .dstArrayElement = 0,
		    .descriptorCount = 1,
		    .descriptorType  = vk::DescriptorType::eUniformBuffer,
		    .pBufferInfo     = &bufferInfo});

		mDescriptorSet->UpdateDescriptorSets(i, descriptorWrites);
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
	mImagesInFlight.resize(mSwapchain->GetImageCount());
	for (std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		mRenderFinishedSemaphores[i] =
		    mDevice->GetDevice().createSemaphoreUnique({});
		mImageAvailableSemaphores[i] =
		    mDevice->GetDevice().createSemaphoreUnique({});
		mInFlightFences[i] = mDevice->GetDevice().createFenceUnique(
		    {.flags = vk::FenceCreateFlagBits::eSignaled});
	}
}
