#include "Model.h"

#include <iostream>

#include <assimp\postprocess.h>

#include "Components\Renderable\TriangleMeshComponent.h"
#include "Components\MaterialComponent.h"
#include "Components\ModelComponent.h"

#include "Entity.h"

void Model::LoadModel(const std::string& path)
{
    // read file via ASSIMP
    auto importer = Assimp::Importer();
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs); // | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP: " << importer.GetErrorString() << std::endl;
        return;
    }
    // retrieve the directory path of the filepath
    m_Directory = path.substr(0, path.find_last_of('\\'));

    // process ASSIMP's root node recursively
    ProcessNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::ProcessNode(const aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
        ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene);

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        ProcessNode(node->mChildren[i], scene);
}

void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    auto entity = m_Scene.lock()->CreateEntity();
    entity.AddComponent<ModelComponent>(this);
    
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::shared_ptr<Tex2D>> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex{};

        vertex.Position = glm::vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);

        if (mesh->HasNormals())
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        if (mesh->mTextureCoords[0])
            vertex.TexCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

        vertices.emplace_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.emplace_back(face.mIndices[j]);
    }

    entity.AddComponent<TriangleMeshComponent>(vertices, indices);

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // Batch load all the different types of textures
    auto diffuseMaps  = LoadMaterialTextures(material, aiTextureType_DIFFUSE,  "diffuse");
    auto specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
    auto normalMaps   = LoadMaterialTextures(material, aiTextureType_HEIGHT,   "normal");
    auto heightMaps   = LoadMaterialTextures(material, aiTextureType_AMBIENT,  "height");
    auto emissiveMaps = LoadMaterialTextures(material, aiTextureType_EMISSIVE, "emissive");

    // Reserve space for textures
    textures.reserve(diffuseMaps.size() + specularMaps.size() + normalMaps.size() + heightMaps.size() + emissiveMaps.size());

    // Move the loaded textures into the textures vector
    for (auto& texture : diffuseMaps)
        textures.emplace_back(std::move(texture));

    for (auto& texture : specularMaps)
        textures.emplace_back(std::move(texture));

    for (auto& texture : normalMaps)
        textures.emplace_back(std::move(texture));

    for (auto& texture : heightMaps)
        textures.emplace_back(std::move(texture));

    for (auto& texture : emissiveMaps)
        textures.emplace_back(std::move(texture));

    entity.AddComponent<MaterialComponent>(std::move(textures), 0.5f);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a texture struct.
std::vector<std::shared_ptr<Tex2D>> Model::LoadMaterialTextures(const aiMaterial* mat, const aiTextureType type, const std::string& typeName)
{
    auto materialTextures = std::vector<std::shared_ptr<Tex2D>>();

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        auto filename = std::string(str.C_Str());

        // Check if texture has been loaded
        if (auto iterator = m_TexturesLoaded.find(filename); iterator != m_TexturesLoaded.end())
        {
            materialTextures.emplace_back(iterator->second);
        }
        else
        {
            auto texture = std::make_shared<Tex2D>(m_Directory + std::string("\\") + filename, typeName);
            materialTextures.emplace_back(texture);
            m_TexturesLoaded.insert(std::make_pair(filename, texture));
        }
    }

    return materialTextures;
}