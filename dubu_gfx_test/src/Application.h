#pragma once

#include <dubu_gfx/dubu_gfx.h>
#include <dubu_window/dubu_window.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static auto GetBindingDescription() {
		return vk::VertexInputBindingDescription{
		    .binding   = 0,
		    .stride    = sizeof(Vertex),
		    .inputRate = vk::VertexInputRate::eVertex,
		};
	}

	static auto GetAttributeDescriptions() {
		return std::array<vk::VertexInputAttributeDescription, 3>{
		    vk::VertexInputAttributeDescription{
		        .location = 0,
		        .binding  = 0,
		        .format   = vk::Format::eR32G32B32Sfloat,
		        .offset   = offsetof(Vertex, pos),
		    },
		    vk::VertexInputAttributeDescription{
		        .location = 1,
		        .binding  = 0,
		        .format   = vk::Format::eR32G32B32Sfloat,
		        .offset   = offsetof(Vertex, color),
		    },
		    vk::VertexInputAttributeDescription{
		        .location = 2,
		        .binding  = 0,
		        .format   = vk::Format::eR32G32Sfloat,
		        .offset   = offsetof(Vertex, texCoord),
		    },
		};
	}
};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

class Application : public dubu::event::EventSubscriber {
public:
	void Run();

private:
	void MainLoop();

	void Update();

	void InitFramework();
	void InitImGui();
	void RecreateSwapchain();

	void RecordCommands(uint32_t imageIndex);
	void UpdateUniformBuffer(uint32_t imageIndex);
	void DrawFrame();

	void CreateInstance();
	void CreateSurface();
	void CreateDevice();
	void CreateSwapchain();
	void CreateRenderPass();
	void CreateDescriptorSetLayout();
	void CreateGraphicsPipeline();
	void CreateFramebuffer();
	void CreateCommandPool();
	void CreateDepthResources();
	void CreateTextureImage();
	void CreateVertexBuffer();
	void CreateUniformBuffers();
	void CreateDescriptorPool();
	void CreateDescriptorSets();
	void CreateCommandBuffer();
	void CreateSyncObjects();

	std::unique_ptr<dubu::window::GLFWWindow> mWindow;

	std::unique_ptr<dubu::gfx::Instance>            mInstance            = {};
	std::unique_ptr<dubu::gfx::Surface>             mSurface             = {};
	std::unique_ptr<dubu::gfx::Device>              mDevice              = {};
	std::unique_ptr<dubu::gfx::Swapchain>           mSwapchain           = {};
	std::unique_ptr<dubu::gfx::RenderPass>          mRenderPass          = {};
	std::unique_ptr<dubu::gfx::ViewportState>       mViewportState       = {};
	std::unique_ptr<dubu::gfx::DescriptorSetLayout> mDescriptorSetLayout = {};
	vk::UniquePipelineLayout                        mPipelineLayout      = {};
	std::unique_ptr<dubu::gfx::GraphicsPipeline>    mGraphicsPipeline    = {};
	std::unique_ptr<dubu::gfx::Framebuffer>         mFramebuffer         = {};
	std::unique_ptr<dubu::gfx::CommandPool>         mCommandPool         = {};
	std::unique_ptr<dubu::gfx::Image>               mDepthImage          = {};
	std::unique_ptr<dubu::gfx::Image>               mTextureImage        = {};
	std::unique_ptr<dubu::gfx::Sampler>             mTextureSampler      = {};
	std::unique_ptr<dubu::gfx::Buffer>              mVertexBuffer        = {};
	std::unique_ptr<dubu::gfx::Buffer>              mIndexBuffer         = {};
	std::vector<std::unique_ptr<dubu::gfx::Buffer>> mUniformBuffers      = {};
	std::unique_ptr<dubu::gfx::DescriptorPool>      mDescriptorPool      = {};
	std::unique_ptr<dubu::gfx::DescriptorSet>       mDescriptorSet       = {};
	std::unique_ptr<dubu::gfx::CommandBuffer>       mCommandBuffer       = {};
	std::vector<vk::UniqueSemaphore> mRenderFinishedSemaphores           = {};
	std::vector<vk::UniqueSemaphore> mImageAvailableSemaphores           = {};
	std::vector<vk::UniqueFence>     mInFlightFences                     = {};
	std::vector<vk::Fence>           mImagesInFlight                     = {};
	std::size_t                      mCurrentFrame                       = {};
	bool                             mIsMinimized = false;

	std::unique_ptr<dubu::gfx::DescriptorPool> mImGuiDescriptorPool = {};
	bool                                       mShowDemoWindow      = false;
};