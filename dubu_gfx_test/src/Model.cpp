#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

glm::vec3 AiToGlm(aiVector3D v) {
	return {v.x, v.y, v.z};
}

Mesh::Mesh(const CreateInfo& createInfo) {
	{
		dubu::gfx::Buffer stagingBuffer(dubu::gfx::Buffer::CreateInfo{
		    .device         = createInfo.device,
		    .physicalDevice = createInfo.physicalDevice,
		    .size           = static_cast<uint32_t>(createInfo.vertices.size() *
                                          sizeof(createInfo.vertices[0])),
		    .usage          = vk::BufferUsageFlagBits::eTransferSrc,
		    .sharingMode    = vk::SharingMode::eExclusive,
		    .memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
		                        vk::MemoryPropertyFlagBits::eHostCoherent,
		});

		stagingBuffer.SetData(createInfo.vertices);

		mVertexBuffer =
		    std::make_unique<dubu::gfx::Buffer>(dubu::gfx::Buffer::CreateInfo{
		        .device         = createInfo.device,
		        .physicalDevice = createInfo.physicalDevice,
		        .size  = static_cast<uint32_t>(createInfo.vertices.size() *
                                              sizeof(createInfo.vertices[0])),
		        .usage = vk::BufferUsageFlagBits::eVertexBuffer |
		                 vk::BufferUsageFlagBits::eTransferDst,
		        .sharingMode      = vk::SharingMode::eExclusive,
		        .memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
		    });

		mVertexBuffer->SetData(stagingBuffer.GetBuffer(),
		                       createInfo.queueFamilies,
		                       createInfo.graphicsQueue);
	}
	{
		dubu::gfx::Buffer stagingBuffer(dubu::gfx::Buffer::CreateInfo{
		    .device         = createInfo.device,
		    .physicalDevice = createInfo.physicalDevice,
		    .size           = static_cast<uint32_t>(createInfo.indices.size() *
                                          sizeof(createInfo.indices[0])),
		    .usage          = vk::BufferUsageFlagBits::eTransferSrc,
		    .sharingMode    = vk::SharingMode::eExclusive,
		    .memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
		                        vk::MemoryPropertyFlagBits::eHostCoherent,
		});

		stagingBuffer.SetData(createInfo.indices);

		mIndexBuffer =
		    std::make_unique<dubu::gfx::Buffer>(dubu::gfx::Buffer::CreateInfo{
		        .device         = createInfo.device,
		        .physicalDevice = createInfo.physicalDevice,
		        .size  = static_cast<uint32_t>(createInfo.indices.size() *
                                              sizeof(createInfo.indices[0])),
		        .usage = vk::BufferUsageFlagBits::eIndexBuffer |
		                 vk::BufferUsageFlagBits::eTransferDst,
		        .sharingMode      = vk::SharingMode::eExclusive,
		        .memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
		    });

		mIndexBuffer->SetData(stagingBuffer.GetBuffer(),
		                      createInfo.queueFamilies,
		                      createInfo.graphicsQueue);
	}

	mIndexCount = static_cast<uint32_t>(createInfo.indices.size());
}

Model::Model(const CreateInfo& createInfo) {
	Assimp::Importer importer;
	const aiScene*   scene = importer.ReadFile(
        createInfo.filepath.string().c_str(),
        aiProcessPreset_TargetRealtime_Fast | aiProcess_FlipUVs);

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[i];

		Mesh::CreateInfo meshInfo{
		    .device         = createInfo.device,
		    .physicalDevice = createInfo.physicalDevice,
		    .queueFamilies  = createInfo.queueFamilies,
		    .graphicsQueue  = createInfo.graphicsQueue,
		};

		meshInfo.vertices.resize(mesh->mNumVertices);
		meshInfo.indices.resize(mesh->mNumFaces * 3);

		for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices;
		     ++vertexIndex) {
			meshInfo.vertices[vertexIndex] = Vertex{
			    .position = AiToGlm(mesh->mVertices[vertexIndex]),
			    .normal   = AiToGlm(mesh->mNormals[vertexIndex]),
			    .texCoord = AiToGlm(mesh->mTextureCoords[0][vertexIndex]),
			};
		}
		for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces;
		     ++faceIndex) {
			meshInfo.indices[faceIndex * 3 + 0] =
			    mesh->mFaces[faceIndex].mIndices[0];
			meshInfo.indices[faceIndex * 3 + 1] =
			    mesh->mFaces[faceIndex].mIndices[1];
			meshInfo.indices[faceIndex * 3 + 2] =
			    mesh->mFaces[faceIndex].mIndices[2];
		}

		mMeshes.push_back(Mesh(meshInfo));
	}
}

void Model::RecordCommands(
    std::vector<dubu::gfx::DrawingCommand>& drawingCommands) {
	for (auto& mesh : mMeshes) {
		drawingCommands.push_back(dubu::gfx::DrawingCommands::BindVertexBuffers{
		    .buffers = {mesh.GetVertexBuffer()},
		    .offsets = {0},
		});
		drawingCommands.push_back(dubu::gfx::DrawingCommands::BindIndexBuffer{
		    .buffer    = mesh.GetIndexBuffer(),
		    .offset    = 0,
		    .indexType = vk::IndexType::eUint32,
		});
		drawingCommands.push_back(dubu::gfx::DrawingCommands::DrawIndexed{
		    .indexCount    = mesh.GetIndexCount(),
		    .instanceCount = 1,
		});
	}
}
