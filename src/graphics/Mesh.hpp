#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>
#include "ShaderManager.hpp"
#include "TextureManager.hpp"

class aiScene;
class aiMesh;
class aiVertexWeight;

namespace mesh_impl {
    class Vertex {
    public:
        Vertex() = default;

        Vertex(const aiVector3D& _pos,
               const aiVector3D& _normals,
               const aiVector3D& _uv,
               const aiVector3D& _tangent,
               const aiVector3D& _bitangent
            ):      pos      (_pos.x, _pos.y, _pos.z),
                    normals  (_normals.x, _normals.y, _normals.z),
                    uv       (_uv.x, _uv.y),
                    tangent  (_tangent.x, _tangent.y, _tangent.z),
                    bitangent(_bitangent.x, _bitangent.y, _bitangent.z)
        {}

    public:
        glm::vec3 pos;
        glm::vec2 uv;
        glm::vec3 normals;
        glm::vec3 tangent;
        glm::vec3 bitangent;

        using PositionT  = decltype(pos);
        using UVT        = decltype(uv);
        using NormalT    = decltype(normals);
        using TangentT   = decltype(tangent);
        using BitangentT = decltype(bitangent);

        enum DataIndex {
            POSITION = 0,
            UV,
            NORMAL,
            TANGENT,
            BITANGENT
        };
    };

    //using VertexWeight = std::pair<unsigned, float>; // id / weight
    //struct Bone {
    //    Bone(aiVertexWeight* iweights, unsigned weights_count, const aiMatrix4x4& ioffset);
    //    glm::mat4 offset;
    //    std::vector<VertexWeight> weights;
    //};

    struct MeshEntry {
        MeshEntry(
                unsigned indices_count,
                unsigned material_index,
                unsigned start_vertex_pos,
                unsigned start_index_pos):
            _indices_count   (indices_count),
            _material_index  (material_index),
            _start_vertex_pos(start_vertex_pos),
            _start_index_pos (start_index_pos) {}

        unsigned _indices_count;
        unsigned _material_index;
        unsigned _start_vertex_pos;
        unsigned _start_index_pos;

        //std::vector<Bone> _bones;
    };
}

namespace grx {

    class Mesh {
        using VP_T         = std::pair<glm::mat4, glm::mat4>;
        using Vertex       = mesh_impl::Vertex;
        using UnpackedVertexVector = std::tuple<
                std::vector<Vertex::PositionT>,
                std::vector<Vertex::UVT>,
                std::vector<Vertex::NormalT>,
                std::vector<Vertex::TangentT>,
                std::vector<Vertex::BitangentT>>;

        using MeshEntry    = mesh_impl::MeshEntry;
        //using Bone         = mesh_impl::Bone;
        //using VertexWeight = mesh_impl::VertexWeight;

    public:
        explicit Mesh(const char* filepath);
        void render(const glm::mat4& view, const glm::mat4& projection, grx::ShaderProgram& shader_program);

        void render(const VP_T& view_projection, grx::ShaderProgram& shader_program) {
            render(view_projection.first, view_projection.second, shader_program);
        }

        void render(const glm::mat4& view, const glm::mat4& projection, grx::ShaderProgram& shader_program, unsigned instances);

        void render(const VP_T& view_projection, grx::ShaderProgram& shader_program, unsigned instances) {
            render(view_projection.first, view_projection.second, shader_program, instances);
        }

        void addNormals(unsigned materialID, const grx::Texture& texture) {
            if (textures_normals.size() <= materialID)
                textures_normals.resize(materialID + 1);

            textures_normals[materialID] = texture;
        }

        glm::vec3 pos = {0.f, 0.f, 0.f};

        enum BufferNumbers {
            INDEX_BUFFER      = 0,
            POSITION_VB       = 1,
            NORMAL_VB         = 2,
            UV_VB             = 3,
            TANGENT_VB        = 4,
            BITANGENT_VB      = 5,
            MVP_MATRIX_VB     = 6,
            MODEL_MATRIX_VB   = 7,
            BufferNumbersSize = 8
        };

        enum AttributesLocation {
            POSITION_LOCATION     = 0,
            UV_LOCATION           = 1,
            NORMAL_LOCATION       = 2,
            TANGENT_LOCATION      = 3,
            BITANGENT_LOCATION    = 4,
            MVP_MATRIX_LOCATION   = 5,
            MODEL_MATRIX_LOCATION = 9
        };

    protected:
        void initFromScene(const aiScene* scene, const char* filepath);
        void initMesh(const aiMesh* mesh, UnpackedVertexVector& vertices, std::vector<unsigned>& indices);

        std::vector<MeshEntry> mesh_entries;
        std::vector<grx::Texture>  textures;
        std::vector<grx::Texture>  textures_normals;
        std::array <unsigned, BufferNumbersSize> _glBuffers;
        glm::vec3 _aa = {0.f, 0.f, 0.f};
        glm::vec3 _bb = {0.f, 0.f, 0.f};

        unsigned _glVAO;


    };


} // namespace grx
