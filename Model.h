#pragma once
#include <string>
#include <iostream>
#include <map>
#include <vector>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "utils.h"

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

    // sets the model, view, and projection matrices of each mesh
    void SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) const
    {
        for (auto& mesh : m_Meshes)
            mesh->SetMVP(model, view, proj);
    }

    // draws the model, and thus all its meshes
    void Draw() const
    {
        for (auto& mesh : m_Meshes)
            mesh->Draw();
    }

    // draws the model normals, and thus all of it's meshes normals
    void DrawNormals(const glm::vec4 color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)) const
    {
        for (auto& mesh : m_Meshes)
            mesh->DrawNormals(color);
    }

public:
    // model data 
    std::vector<std::shared_ptr<Texture>> m_TexturesLoaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<std::shared_ptr<TriangleMesh>> m_Meshes;
    std::string m_Directory;
    bool m_GammaCorrection;

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void LoadModel(const std::string& path)
    {
        // read file via ASSIMP
        auto importer = Assimp::Importer();
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
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
            convertedMesh->m_DrawingMode = LIT_OBJECT;
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
        std::vector<std::shared_ptr<Texture>> textures;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex{};
            vertex.Position = glm::vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
            // normals
            if (mesh->HasNormals())
                vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            
            if (mesh->mTextureCoords[0])
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vertex.TexCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
                // Note: tangent and bi-tangent thrown away for the moment

            vertices.emplace_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
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

        auto meshMaterial = std::make_shared<Material>();
        meshMaterial->SetTextures(textures);

        // return a mesh object created from the extracted mesh data
        return std::make_shared<TriangleMesh>(vertices, indices, meshMaterial);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a texture struct.
    std::vector<std::shared_ptr<Texture>> LoadMaterialTextures(const aiMaterial* mat, const aiTextureType type, const std::string& typeName)
    {
        std::vector<std::shared_ptr<Texture>> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (auto& texture : m_TexturesLoaded)
            {
                if (std::strcmp(texture->m_Path.c_str(), str.C_Str()) == 0)
                {
                    textures.emplace_back(texture);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {   // if texture hasn't been loaded already, load it
                auto texture = std::make_shared<Tex2D>((m_Directory + std::string("\\") + std::string(str.C_Str())).c_str());
                texture->m_Tag = typeName;
                texture->m_Path = str.C_Str();
                textures.emplace_back(texture);
                m_TexturesLoaded.emplace_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
            }
        }

        return textures;
    }
};
