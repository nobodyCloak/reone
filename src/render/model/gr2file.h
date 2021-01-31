/*
 * Copyright (c) 2020-2021 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "../../render/model/model.h"
#include "../../resource/format/binfile.h"

namespace reone {

namespace render {

/**
 * Encapsulates reading GR2 model files, used by Star Wars: The Old Republic.
 * This is highly experimental.
 */
class Gr2File : public resource::BinaryFile {
public:
    Gr2File();

    std::shared_ptr<render::Model> model() const { return _model; }

private:
    struct MeshHeader {
        std::string name;
        uint16_t numPieces { 0 };
        uint16_t numUsedBones { 0 };
        uint16_t vertexMask { 0 };
        uint16_t vertexSize { 0 };
        uint32_t numVertices { 0 };
        uint32_t numIndices { 0 };
        uint32_t offsetVertices { 0 };
        uint32_t offsetPieces { 0 };
        uint32_t offsetIndices { 0 };
        uint32_t offsetBones { 0 };
    };

    struct MeshPiece {
        uint32_t startFaceIdx { 0 };
        uint32_t numFaces { 0 };
        uint32_t materialIndex { 0 };
        uint32_t pieceIndex { 0 };
    };

    struct MeshBone {
        std::string name;
        std::vector<float> bounds;
    };

    struct Gr2Mesh {
        MeshHeader header;
        std::vector<std::shared_ptr<MeshPiece>> pieces;
        std::shared_ptr<ModelMesh> mesh;
        std::vector<std::shared_ptr<MeshBone>> bones;
    };

    struct SkeletonBone {
        std::string name;
        uint32_t parentIndex { 0 };
        std::vector<float> rootToBone;
    };

    uint16_t _numMeshes { 0 };
    uint16_t _numMaterials { 0 };
    uint16_t _numBones { 0 };
    uint32_t _offsetMeshHeader { 0 };
    uint32_t _offsetMaterialHeader { 0 };
    uint32_t _offsetBoneStructure { 0 };

    std::vector<std::shared_ptr<Gr2Mesh>> _meshes;
    std::vector<std::shared_ptr<SkeletonBone>> _bones;
    std::shared_ptr<render::Model> _model;

    void doLoad() override;

    void loadMeshes();
    void loadMaterials();
    void loadSkeletonBones();
    void loadModel();

    std::unique_ptr<Gr2Mesh> readMesh();
    std::unique_ptr<MeshPiece> readMeshPiece();
    std::unique_ptr<ModelMesh> readModelMesh(const Gr2Mesh &mesh);
    std::unique_ptr<MeshBone> readMeshBone();
    std::unique_ptr<SkeletonBone> readSkeletonBone();
};

} // namespace render

} // namespace reone
