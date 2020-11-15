#pragma once

#include <dubu_gfx/dubu_gfx.h>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
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
		        .offset   = offsetof(Vertex, position),
		    },
		    vk::VertexInputAttributeDescription{
		        .location = 1,
		        .binding  = 0,
		        .format   = vk::Format::eR32G32B32Sfloat,
		        .offset   = offsetof(Vertex, normal),
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

struct Texture {
	uint32_t    id;
	std::string type;
};

class Mesh {
public:
	struct CreateInfo {
		vk::Device                    device         = {};
		vk::PhysicalDevice            physicalDevice = {};
		dubu::gfx::QueueFamilyIndices queueFamilies  = {};
		vk::Queue                     graphicsQueue  = {};
		std::vector<Vertex>           vertices       = {};
		std::vector<uint32_t>         indices        = {};
		std::vector<Texture>          textures       = {};
	};

public:
	Mesh(const CreateInfo& createInfo);

	[[nodiscard]] const vk::Buffer& GetVertexBuffer() const {
		return mVertexBuffer->GetBuffer();
	}
	[[nodiscard]] const vk::Buffer& GetIndexBuffer() const {
		return mIndexBuffer->GetBuffer();
	}
	[[nodiscard]] const uint32_t GetIndexCount() const { return mIndexCount; }

private:
	std::unique_ptr<dubu::gfx::Buffer> mVertexBuffer = {};
	std::unique_ptr<dubu::gfx::Buffer> mIndexBuffer  = {};
	uint32_t                           mIndexCount   = {};
};

class Model {
public:
	struct CreateInfo {
		vk::Device                    device         = {};
		vk::PhysicalDevice            physicalDevice = {};
		dubu::gfx::QueueFamilyIndices queueFamilies  = {};
		vk::Queue                     graphicsQueue  = {};
		std::filesystem::path         filepath       = {};
	};

public:
	Model(const CreateInfo& createInfo);

	void RecordCommands(
	    std::vector<dubu::gfx::DrawingCommand>& drawingCommands);

private:
	std::vector<Mesh> mMeshes;
};