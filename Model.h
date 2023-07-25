#pragma once
#include <string>
#include <iostream>
#include <map>
#include <vector>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "TM.h"
#include "Texture.h"
#include "Vertex.h"

class Model
{
public:
    // constructor, expects a filepath to a 3D model.
    explicit Model(std::string const& path, const bool correctGamma = false)
        : m_GammaCorrection(correctGamma)
    {
        LoadModel(path);
    }

public:
    std::unordered_map<std::string, std::shared_ptr<Tex2D>> m_TexturesLoaded = std::unordered_map<std::string, std::shared_ptr<Tex2D>>(); // Stores all loaded textures with their file names as keys
    std::vector<std::shared_ptr<TriangleMesh>> m_Meshes = std::vector<std::shared_ptr<TriangleMesh>>(); // Vector of meshes within the model
    std::string m_Directory = std::string(); // The location of the directory containing all model assets
    bool m_GammaCorrection = false; // Flag for whether gamma should be corrected

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void LoadModel(const std::string& path)
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
    void ProcessNode(const aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            auto convertedMesh = ProcessMesh(mesh, scene);
            convertedMesh->m_DrawingMode = DrawingMode::LIT_OBJECT;
            convertedMesh->SetVertexCount(mesh->mNumVertices);
            m_Meshes.emplace_back(convertedMesh);
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            ProcessNode(node->mChildren[i], scene);
    }

    std::shared_ptr<TriangleMesh> ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill
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
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vertex.TexCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

            vertices.emplace_back(vertex);
        }
        // now walk through each of the mesh's faces and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.emplace_back(face.mIndices[j]);
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // 1. diffuse maps
        auto diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        auto specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        auto normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        auto heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        // 5. height maps
        auto emissiveMaps = LoadMaterialTextures(material, aiTextureType_EMISSIVE, "emissive");
        textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());

        auto meshMaterial = std::make_shared<Material>(textures, 0.5f);

        // return a mesh object created from the extracted mesh data
        return std::make_shared<TriangleMesh>(vertices, indices, meshMaterial);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a texture struct.
    std::vector<std::shared_ptr<Tex2D>> LoadMaterialTextures(const aiMaterial* mat, const aiTextureType type, const std::string& typeName)
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
};
