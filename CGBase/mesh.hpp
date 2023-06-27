/**
 * @file mesh.hpp
 * @author Jovan Ivosevic
 * @brief Model mesh, a renderable piece of a model
 * @version 0.1
 * @date 2022-10-09
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <assimp/scene.h>
#include<vector>
#include <GL/glew.h>
#include <iostream>
#include "texture.hpp"

class Mesh {
public:
    std::vector<unsigned> mIndices;
    std::vector<float> mVertices;

    Mesh(const aiMesh* mesh, const aiMaterial* material, const std::string& resPath);

    void Render() const;

private:
    unsigned mVAO;
    unsigned mVBO;
    unsigned mEBO;
    unsigned mVertexCount;
    unsigned mIndexCount;
    unsigned mDiffuseTexture;
    unsigned mSpecularTexture;
    unsigned loadMeshTexture(const aiMaterial* material, const std::string& resPath, aiTextureType type);
    void processMesh(const aiMesh* mesh, const aiMaterial* material, const std::string& resPath);
};