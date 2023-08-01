#pragma once
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <memory>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>

#include "Components\Texture.h"

#include "Scene.h"

class Model
{
public:
    // constructor, expects a filepath to a 3D model.
    explicit Model(std::string const& path, std::weak_ptr<Scene> scene, const bool correctGamma = false)
        : m_GammaCorrection(correctGamma), m_Scene(std::move(scene))
    {
        LoadModel(path);
        s_UUID++;
    }

public:
    inline static unsigned int s_UUID = 0;
    std::string m_Directory = std::string(); // The location of the directory containing all model assets
    bool m_GammaCorrection = false; // Flag for whether gamma should be corrected
    std::weak_ptr<Scene> m_Scene;

private:
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void LoadModel(const std::string& path);
    // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void ProcessNode(const aiNode* node, const aiScene* scene);
    // Processes a mesh, then returns an entity with triangle mesh and material components.
    void ProcessMesh(aiMesh* mesh, const aiScene* scene);
    // Checks all material textures of a given type and loads the textures if they're not loaded yet
    std::vector<std::shared_ptr<Tex2D>> LoadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName);

private:
    std::unordered_map<std::string, std::shared_ptr<Tex2D>> m_TexturesLoaded = std::unordered_map<std::string, std::shared_ptr<Tex2D>>(); // Stores all loaded textures with their file names as keys
};
