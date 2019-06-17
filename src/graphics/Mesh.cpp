#include "Mesh.hpp"

#include "TextureManager.hpp"
#include "ShaderManager.hpp"

#include <iostream>

#include <GL/glew.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <glm/ext/matrix_transform.hpp>

#include "filesystem.hpp"
#include "configs.hpp"

grx::Mesh::Mesh(const char* filepath) {
    auto realPath = base::fs::to_data_path(base::cfg::read<ftl::String>("models_dir") / std::string_view(filepath));

    glGenVertexArrays(1, &_glVAO);
    glBindVertexArray(_glVAO);

    glGenBuffers(_glBuffers.size(), _glBuffers.data());

    auto importer = Assimp::Importer();

    auto scene = importer.ReadFile(realPath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals);

    // Todo: assert
    if (!scene)
        std::cerr << "Error parsing mesh file " << realPath.c_str() << ": " << importer.GetErrorString() << std::endl;

    initFromScene(scene, realPath.c_str());

    glBindVertexArray(0);
}

void grx::Mesh::initFromScene(const aiScene* scene, const char* filepath) {
    mesh_entries.reserve(scene->mNumMeshes);

    UnpackedVertexVector   vertices;
    auto& positions = std::get<Vertex::POSITION>(vertices);
    auto& uvs       = std::get<Vertex::UV      >(vertices);
    auto& normals   = std::get<Vertex::NORMAL  >(vertices);

    std::vector<unsigned>  indices;

    std::cout << "Load mesh data '" << filepath << "'..." << std::endl;
    std::cout << "Has meshes:     " << (scene->HasMeshes() ? "true" : "false") << std::endl;
    std::cout << "Has textures:   " << (scene->HasTextures() ? "true" : "false") << std::endl;
    std::cout << "Has materials:  " << (scene->HasMaterials() ? "true" : "false") << std::endl;
    std::cout << "Has animations: " << (scene->HasAnimations() ? "true" : "false") << std::endl;
    std::cout << "Has lights:     " << (scene->HasLights() ? "true" : "false") << std::endl;
    std::cout << "Has cameras:    " << (scene->HasCameras() ? "true" : "false") << std::endl;

    std::cout << std::endl;
    std::cout << "Meshes    count:  " << scene->mNumMeshes << std::endl;
    std::cout << "Textures count:   " << scene->mNumTextures << std::endl;
    std::cout << "Materials count:  " << scene->mNumMaterials << std::endl;
    std::cout << "Animations count: " << scene->mNumAnimations << std::endl;
    std::cout << "Lights count      " << scene->mNumLights << std::endl;
    std::cout << "Cameras count     " << scene->mNumCameras << std::endl;
    std::cout << std::endl;

    // Lights
    for (unsigned i = 0; i < scene->mNumLights; ++i) {
        auto& light = *scene->mLights[i];
        std::cout << "\tLight " << light.mName.C_Str() << std::endl;
        std::cout << "\t\tAttenuation constant: " << light.mAttenuationConstant << std::endl;
        std::cout << "\t\tAttenuation linear  : " << light.mAttenuationLinear   << std::endl;
        std::cout << "\t\tAttenuation quad    : " << light.mAttenuationQuadratic << std::endl;
        std::cout << "\t\tLight position      : " << light.mPosition.x << " " << light.mPosition.y << " " << light.mPosition.z << std::endl;
    }
    std::cout << std::endl;

    unsigned verticesCount = 0;
    unsigned indicesCount  = 0;

    // Calculate vertices, indices count and their offsets in every mesh entry
    for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
        mesh_entries.emplace_back(
                scene->mMeshes[i]->mNumFaces * 3,  // indices count
                scene->mMeshes[i]->mMaterialIndex, // material index
                verticesCount,                     // start vertex position
                indicesCount                       // start index position
        );

        verticesCount += scene->mMeshes[i]->mNumVertices;
        indicesCount  += mesh_entries.back()._indices_count;
    }
    std::cout << "Total vertices: " << verticesCount << std::endl;
    std::cout << "Total indices : " << indicesCount  << std::endl << std::endl;

    // Reserve memory for vertices and indices
    positions.reserve(verticesCount);
    uvs      .reserve(verticesCount);
    normals  .reserve(verticesCount);
    indices  .reserve(indicesCount );

    // Load vertices and indices
    for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
        std::cout << "Load mesh[" << i << "]..." << std::endl;
        initMesh(scene->mMeshes[i], vertices, indices);
    }

    // Init materials
    textures.reserve(scene->mNumMaterials);
    for (unsigned i = 0; i < scene->mNumMaterials; ++i) {
        std::cout << "Load material[" << i << "]..." << std::endl;
        auto material = scene->mMaterials[i];
        std::cout << "\tName: " << material->GetName().data << std::endl;

        auto diffuseCount    = material->GetTextureCount(aiTextureType_DIFFUSE);
        auto normalCount     = material->GetTextureCount(aiTextureType_NORMALS);
        auto specularCount   = material->GetTextureCount(aiTextureType_SPECULAR);

        std::cout << "\tDiffuse count:    " << diffuseCount << std::endl;
        std::cout << "\tNormal count:     " << normalCount << std::endl;
        std::cout << "\tSpecular count:   " << specularCount << std::endl;


        if (diffuseCount > 0) {
            auto path = aiString();
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                std::cout << "\t\tLoad diffuse " << path.C_Str() << std::endl;
                textures.emplace_back(path.data);
            }
            else
                textures.emplace_back();
        } else {
            // Todo: set dummy texture
        }
    }


    // Generate and populate buffers
    glBindBuffer(GL_ARRAY_BUFFER, _glBuffers[POSITION_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex::PositionT) * positions.size(), positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, _glBuffers[UV_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex::UVT) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(UV_LOCATION);
    glVertexAttribPointer(UV_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, _glBuffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex::NormalT) * normals.size(), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _glBuffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(decltype(indices)::value_type) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, _glBuffers[MVP_MATRIX_VB]);
    for (unsigned i = 0; i < 4; ++i) {
        glEnableVertexAttribArray(MVP_MATRIX_LOCATION + i);
        glVertexAttribPointer(MVP_MATRIX_LOCATION + i, 4, GL_FLOAT, GL_FALSE,
                sizeof(glm::mat4), (GLvoid*)(sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(MVP_MATRIX_LOCATION + i, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, _glBuffers[MODEL_MATRIX_VB]);
    for (unsigned i = 0; i < 4; ++i) {
        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + i);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION + i, 4, GL_FLOAT, GL_FALSE,
                sizeof(glm::mat4), (GLvoid*)(sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION + i, 1);
    }

    /*
    std::cout << "Indices: " << std::endl;
    for (auto& i : indices)
        std::cout << i << std::endl;

    std::cout << "\n Vertices: " << std::endl;
    for (unsigned v = 0; v < positions.size(); ++v) {
        std::cout << "pos: { " << positions[v].x << ", " << positions[v].y << ", " << positions[v].z << " }, ";
        std::cout << "uv: { " << uvs[v].x << ", " << uvs[v].y <<  " }, ";
        std::cout << "normals: { " << normals[v].x << ", " << normals[v].y << ", " << normals[v].z << " }" << std::endl;
    }
     */
}

void grx::Mesh::initMesh(const aiMesh* mesh, UnpackedVertexVector& vertices, std::vector<unsigned>& indices) {
    std::cout << "\tName:              " << mesh->mName.data << std::endl;
    std::cout << "\tHas positions:     " << (mesh->HasPositions() ? "true" : "false") << std::endl;
    std::cout << "\tHas bones:         " << (mesh->HasBones() ? "true" : "false") << std::endl;
    std::cout << "\tHas faces:         " << (mesh->HasFaces() ? "true" : "false") << std::endl;
    std::cout << "\tHas normals:       " << (mesh->HasNormals() ? "true" : "false") << std::endl;
    std::cout << "\tHas tan and bitan: " << (mesh->HasTangentsAndBitangents() ? "true" : "false") << std::endl;

    auto colorChannelsCount = mesh->GetNumColorChannels();
    auto uvChannelsCount    = mesh->GetNumUVChannels();
    std::cout << "\tColor channels count: " << colorChannelsCount << std::endl;
    std::cout << "\tUV channels count:    " << uvChannelsCount << std::endl;
    std::cout << "\tVertices count:       " << mesh->mNumVertices << std::endl;

    // Todo: calculate AABB!
    _aa = {-1, -1, -1};
    _bb = {1, 1, 1};

    //Assimp::FindAABBTransformed(mesh, aa, bb, )

    //std::vector<Bone>     bones;

    //if (mesh->HasTangentsAndBitangents())
    //    for (unsigned i = 0; i < mesh->mNumVertices; ++i)
    //        vertices.emplace_back(
    //                mesh->mVertices[i],
    //                mesh->mNormals[i],
    //                mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : aiVector3D(0.f, 0.f, 0.f),
    //                mesh->mTangents[i],
    //                mesh->mBitangents[i]);
    //else
    for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
        // Todo: assert mesh->HasTextureCoords(0)
        std::get<Vertex::POSITION>(vertices).emplace_back(
                mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        std::get<Vertex::NORMAL  >(vertices).emplace_back(
                mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        if (mesh->HasTextureCoords(0))
            std::get<Vertex::UV>(vertices).emplace_back(
                    mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            std::get<Vertex::UV>(vertices).emplace_back(0.f, 0.f);
    }
        /*
            vertices.emplace_back(
                    mesh->mVertices[i],
                    mesh->mNormals[i],
                    // Todo: more than one texture coordinates support
                    mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : aiVector3D(0.f, 0.f, 0.f));
                    */

    if (mesh->HasFaces()) {
        std::cout << "\tFaces count:          " << mesh->mNumFaces << std::endl;
        for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
            auto face = mesh->mFaces[i];
            // todo: assert face.mNumIndices == 3
            indices.emplace_back(face.mIndices[0]);
            indices.emplace_back(face.mIndices[1]);
            indices.emplace_back(face.mIndices[2]);
        }
    }

    //if (mesh->HasBones()) {
    //    bones.reserve(mesh->mNumBones);
    //    std::cout << "\tBones count:          " << mesh->mNumBones << std::endl;
    //    for (unsigned i = 0; i < mesh->mNumBones; ++i) {
    //        auto bone = mesh->mBones[i];
    //        std::cout << "\t\tLoad bone [" << i << "]..." << std::endl;
    //        std::cout << "\t\t\tBone name:     " << bone->mName.data << std::endl;
    //        std::cout << "\t\t\tWeights count: " << bone->mNumWeights << std::endl;
    //        bones.emplace_back(bone->mWeights, bone->mNumWeights, bone->mOffsetMatrix);
    //    }
    //}
}

/*
mesh_impl::Bone::Bone(aiVertexWeight* iweights, unsigned weights_count, const aiMatrix4x4& o):
    offset(o.a1, o.a2, o.a3, o.a4,
           o.b1, o.b2, o.b3, o.b4,
           o.c1, o.c2, o.c3, o.c4,
           o.d1, o.d2, o.d3, o.d4 )
{
    weights.reserve(weights_count);
    for (unsigned i = 0; i < weights_count; ++i)
        weights.emplace_back(iweights->mVertexId, iweights->mWeight);
}
 */

/*
mesh_impl::MeshEntry::MeshEntry(
        unsigned                        material_index,
        std::vector<mesh_impl::Vertex>& vertices,
        std::vector<unsigned>&          indices,
        std::vector<Bone>&              bones):
    _material_index(material_index),
    _indices_count (indices.size()),
    _bones         (bones)
{
    glGenBuffers(1, &_vb);
        glBindBuffer(GL_ARRAY_BUFFER, _vb);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &_ib);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ib);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * _indices_count, indices.data(), GL_STATIC_DRAW);
}*/

void grx::Mesh::render(const glm::mat4& view, const glm::mat4& projection, grx::ShaderProgram& sp) {
    auto model = glm::translate(glm::mat4(1), pos);
    auto MVP   = projection * view * model;

    sp.uniform("_MVP", MVP);
    sp.uniform("_M",   model);
    sp.uniform("_textureSampler", 0);

    glBindVertexArray(_glVAO);

    for (auto& me : mesh_entries) {
        auto materialIndex = me._material_index;

        if (materialIndex < textures.size() && textures[materialIndex].valid()) {
            glActiveTexture(GL_TEXTURE0);
            textures[materialIndex].bind();
        }

        glDrawElementsBaseVertex(
                GL_TRIANGLES,
                me._indices_count,
                GL_UNSIGNED_INT,
                (void*)(sizeof(decltype(me._start_index_pos)) * me._start_index_pos),
                me._start_vertex_pos);
    }

    glBindVertexArray(0);
}

bool frustrum_test(const glm::vec3& min, const glm::vec3& max, const std::array<glm::vec4, 6>& planes) {
    bool pass = true;
    for (auto& plane : planes)
        pass &= (std::max(min.x * plane.x, max.x * plane.x) +
                 std::max(min.y * plane.y, max.y * plane.y) +
                 std::max(min.z * plane.z, max.z * plane.z) + plane.w) > 0;

    return pass;
}

void grx::Mesh::render(const glm::mat4& view, const glm::mat4& projection, grx::ShaderProgram& sp, unsigned instancesNum,
                       const std::array<glm::vec4, 6>& frustumPlanes) {
    std::vector<glm::mat4> models; models.reserve(instancesNum);
    std::vector<glm::mat4> mvps; mvps.reserve(instancesNum);


    for (unsigned i = 0; i < instancesNum; ++i) {
        models.emplace_back(glm::translate(glm::mat4(1), glm::vec3((i << 2)+4, 0.f, 0.f)));
        mvps.emplace_back(projection * view * models.back());
    }

    sp.uniform("textureSampler", 0);

    glBindBuffer(GL_ARRAY_BUFFER, _glBuffers[MVP_MATRIX_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * instancesNum, mvps.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, _glBuffers[MODEL_MATRIX_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * instancesNum, models.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(_glVAO);

    for (auto& me : mesh_entries) {
        auto materialIndex = me._material_index;

        if (materialIndex < textures.size() && textures[materialIndex].valid()) {
            glActiveTexture(GL_TEXTURE0);
            textures[materialIndex].bind();
        }

        glDrawElementsInstancedBaseVertex(
                GL_TRIANGLES,
                me._indices_count,
                GL_UNSIGNED_INT,
                (void*)(sizeof(decltype(me._start_index_pos)) * me._start_index_pos),
                instancesNum,
                me._start_vertex_pos);
    }

    glBindVertexArray(0);
}