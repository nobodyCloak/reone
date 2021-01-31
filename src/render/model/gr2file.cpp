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

#include "gr2file.h"

#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "../../common/log.h"
#include "../../render/textures.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace render {

Gr2File::Gr2File() : BinaryFile(4, "GAWB") {
}

void Gr2File::doLoad() {
    // Adapted from multiple sources:
    //
    // https://github.com/SWTOR-Extractors-Modders-Dataminers/Granny2-Plug-In-Blender-2.8x/blob/v1.0.0.1/io_scene_gr2/import_gr2.py
    // https://forum.xentax.com/viewtopic.php?f=16&t=9703&start=30#p94880
    // https://forum.xentax.com/viewtopic.php?f=16&t=11317&start=15#p128702

    seek(0x10);

    uint32_t num50Offsets = readUint32();
    uint32_t gr2Type = readUint32();

    _numMeshes = readUint16();
    _numMaterials = readUint16();
    _numBones = readUint16();

    uint16_t numAttachments = readUint16();

    seek(0x50);

    uint32_t offset50Offset = readUint32();

    _offsetMeshHeader = readUint32();
    _offsetMaterialHeader = readUint32();
    _offsetBoneStructure = readUint32();

    uint32_t offsetAttachments = readUint32();

    loadMeshes();
    loadMaterials();
    loadSkeletonBones();

    // TODO: load attachments

    loadModel();
}

void Gr2File::loadMeshes() {
    for (uint16_t i = 0; i < _numMeshes; ++i) {
        seek(_offsetMeshHeader + i * 0x28);
        _meshes.push_back(readMesh());
    }
}

unique_ptr<Gr2File::Gr2Mesh> Gr2File::readMesh() {
    uint32_t offsetName = readUint32();
    string name(readCStringAt(offsetName));
    if (boost::contains(name, "collision")) return nullptr;

    auto mesh = make_unique<Gr2Mesh>();
    mesh->header.name = name;

    ignore(4);

    mesh->header.numPieces = readUint16();
    mesh->header.numUsedBones = readUint16();
    mesh->header.vertexMask = readUint16();
    mesh->header.vertexSize = readUint16();
    mesh->header.numVertices = readUint32();
    mesh->header.numIndices = readUint32();
    mesh->header.offsetVertices = readUint32();
    mesh->header.offsetPieces = readUint32();
    mesh->header.offsetIndices = readUint32();
    mesh->header.offsetBones = readUint32();

    for (uint16_t i = 0; i < mesh->header.numPieces; ++i) {
        seek(mesh->header.offsetPieces + i * 0x30);
        mesh->pieces.push_back(readMeshPiece());
    }

    mesh->mesh = readModelMesh(*mesh);

    for (uint16_t i = 0; i < mesh->header.numUsedBones; ++i) {
        seek(mesh->header.offsetBones + i * 0x1c);
        mesh->bones.push_back(readMeshBone());
    }

    return move(mesh);
}

unique_ptr<Gr2File::MeshPiece> Gr2File::readMeshPiece() {
    auto piece = make_unique<MeshPiece>();
    piece->startFaceIdx = readUint32();
    piece->numFaces = readUint32();
    piece->materialIndex = readUint32();
    piece->pieceIndex = readUint32();

    ignore(0x24); // bounding box

    return move(piece);
}

static float convertByteToFloat(uint8_t value) {
    return 2.0f * value / 255.0f - 1.0f;
}

static float convertHalfFloatToFloat(uint16_t value) {
    uint32_t sign = (value & 0x8000) ? 1 : 0;
    uint32_t exponent = ((value & 0x7c00) >> 10) - 16;
    uint32_t mantissa = value & 0x03ff;
    uint32_t tmp = ((sign << 31) | ((exponent + 127) << 23) | (mantissa << 13));
    float result = 2.0f * *reinterpret_cast<float *>(&tmp);
    return result;
}

static glm::vec3 computeBitangent(const glm::vec3 &normal, const glm::vec3 &tangent) {
    return glm::cross(tangent, normal);
}

unique_ptr<ModelMesh> Gr2File::readModelMesh(const Gr2Mesh &mesh) {
    int unkFlags = mesh.header.vertexMask & ~0x1f2;
    if (unkFlags != 0) {
        warn(boost::format("GR2: unrecognized vertex flags: 0x%x") % unkFlags);
    }

    Mesh::VertexOffsets offsets;

    vector<float> vertices;
    seek(mesh.header.offsetVertices);
    vector<uint8_t> gr2Vertices(readArray<uint8_t>(static_cast<size_t>(mesh.header.numVertices) * mesh.header.vertexSize));
    const uint8_t *gr2VerticesPtr = &gr2Vertices[0];
    for (uint32_t i = 0; i < mesh.header.numVertices; ++i) {
        int stride = 0;
        int gr2Stride = 0;

        // Vertex coordinates
        vertices.push_back(*reinterpret_cast<const float *>(gr2VerticesPtr + gr2Stride + 0));
        vertices.push_back(*reinterpret_cast<const float *>(gr2VerticesPtr + gr2Stride + 4));
        vertices.push_back(*reinterpret_cast<const float *>(gr2VerticesPtr + gr2Stride + 8));
        stride += 3 * sizeof(float);
        gr2Stride += 3 * sizeof(float);

        // Bone weights and indices
        if (mesh.header.vertexMask & 0x100) {
            vertices.push_back(static_cast<float>(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 0)));
            vertices.push_back(static_cast<float>(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 1)));
            vertices.push_back(static_cast<float>(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 2)));
            vertices.push_back(static_cast<float>(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 3)));
            vertices.push_back(static_cast<float>(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 4)));
            vertices.push_back(static_cast<float>(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 5)));
            vertices.push_back(static_cast<float>(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 6)));
            vertices.push_back(static_cast<float>(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 7)));
            offsets.boneWeights = stride;
            offsets.boneIndices = stride + 4 * sizeof(float);
            stride += 8 * sizeof(float);
            gr2Stride += 8;
        }

        // Normal and tangent space (?)
        if (mesh.header.vertexMask & 0x2) {
            glm::vec3 normal;
            normal.x = convertByteToFloat(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 0));
            normal.y = convertByteToFloat(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 1));
            normal.z = convertByteToFloat(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 2));
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);

            glm::vec3 tangent;
            tangent.x = convertByteToFloat(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 4));
            tangent.y = convertByteToFloat(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 5));
            tangent.z = convertByteToFloat(*reinterpret_cast<const uint8_t *>(gr2VerticesPtr + gr2Stride + 6));
            vertices.push_back(tangent.x);
            vertices.push_back(tangent.y);
            vertices.push_back(tangent.z);

            glm::vec3 bitangent(computeBitangent(normal, tangent));
            vertices.push_back(bitangent.x);
            vertices.push_back(bitangent.y);
            vertices.push_back(bitangent.z);

            offsets.normals = stride;
            offsets.tangents = stride + 3 * sizeof(float);
            offsets.bitangents = stride + 6 * sizeof(float);
            stride += 9 * sizeof(float);
            gr2Stride += 8;
        }

        // Color
        if (mesh.header.vertexMask & 0x10) {
            gr2Stride += 4;
        }

        // Texture 1 coordinates
        if (mesh.header.vertexMask & 0x20) {
            vertices.push_back(convertHalfFloatToFloat(*reinterpret_cast<const uint16_t *>(gr2VerticesPtr + gr2Stride + 0)));
            vertices.push_back(-1.0f * convertHalfFloatToFloat(*reinterpret_cast<const uint16_t *>(gr2VerticesPtr + gr2Stride + 2)));
            offsets.texCoords1 = stride;
            stride += 2 * sizeof(float);
            gr2Stride += 2 * sizeof(uint16_t);
        }

        // Texture 2 coordinates
        if (mesh.header.vertexMask & 0x40) {
            gr2Stride += 4;
        }

        // Texture 3 coordinates
        if (mesh.header.vertexMask & 0x80) {
            gr2Stride += 4;
        }

        gr2VerticesPtr += mesh.header.vertexSize;
        offsets.stride = stride;
    }

    vector<uint16_t> indices;
    seek(mesh.header.offsetIndices);
    for (uint16_t i = 0; i < mesh.header.numPieces; ++i) {
        vector<uint16_t> pieceIndices(readArray<uint16_t>(3 * mesh.pieces[i]->numFaces));
        indices.insert(indices.end(), pieceIndices.begin(), pieceIndices.end());
    }

    auto modelMesh = make_unique<ModelMesh>(true, 0, true);
    modelMesh->_vertexCount = mesh.header.numVertices;
    modelMesh->_vertices = move(vertices);
    modelMesh->_offsets = move(offsets);
    modelMesh->_indices = move(indices);
    modelMesh->_diffuseColor = glm::vec3(0.8f);
    modelMesh->_ambientColor = glm::vec3(0.2f);

    // TODO: load textures from model
    modelMesh->_diffuse = Textures::instance().get("acklay", TextureType::Diffuse);

    modelMesh->computeAABB();

    return move(modelMesh);
}

unique_ptr<Gr2File::MeshBone> Gr2File::readMeshBone() {
    uint32_t offsetName = readUint32();

    auto bone = make_unique<MeshBone>();
    bone->name = readCStringAt(offsetName);
    bone->bounds = readArray<float>(6);

    return move(bone);
}

void Gr2File::loadMaterials() {
}

void Gr2File::loadSkeletonBones() {
    for (uint16_t i = 0; i < _numBones; ++i) {
        seek(_offsetBoneStructure + i * 0x88);
        _bones.push_back(readSkeletonBone());
    }
}

unique_ptr<Gr2File::SkeletonBone> Gr2File::readSkeletonBone() {
    uint32_t offsetName = readUint32();

    auto bone = make_unique<SkeletonBone>();
    bone->name = readCStringAt(offsetName);
    bone->parentIndex = readUint32();

    ignore(0x40);

    bone->rootToBone = readArray<float>(16);

    return move(bone);
}

void Gr2File::loadModel() {
    if (_meshes.empty()) return;

    glm::mat4 transform(1.0f);
    transform *= glm::mat4_cast(glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    transform *= glm::mat4_cast(glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0, 0.0f)));
    transform = glm::scale(transform, glm::vec3(10.0f));

    int index = 0;
    auto rootNode = make_shared<ModelNode>(index++);
    rootNode->_localTransform = transform;
    rootNode->_absTransform = rootNode->_localTransform;
    rootNode->_absTransformInv = glm::inverse(rootNode->_absTransform);

    for (uint16_t i = 0; i < _numMeshes; ++i) {
        auto node = make_shared<ModelNode>(index);
        node->_nodeNumber = index;
        node->_name = _meshes[i]->header.name;
        node->_mesh = _meshes[i]->mesh;
        node->_absTransform = rootNode->_absTransform;
        node->_absTransformInv = glm::inverse(node->_absTransform);
        rootNode->_children.push_back(move(node));

        ++index;
    }

    vector<unique_ptr<Animation>> anims;
    _model = make_shared<Model>("", rootNode, anims);
    _model->_classification = Model::Classification::Character; // prevent shading
    _model->initGL();
}

} // namespace render

} // namespace reone
