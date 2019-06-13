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
               const aiVector3D& _uv
            ):      pos      (_pos.x, _pos.y, _pos.z),
                    normals  (_normals.x, _normals.y, _normals.z),
                    uv       (_uv.x, _uv.y)
        {}

    public:
        glm::vec3 pos;
        glm::vec2 uv;
        glm::vec3 normals;

        using PositionT = decltype(pos);
        using UVT       = decltype(uv);
        using NormalT   = decltype(normals);
        enum DataIndex {
            POSITION = 0,
            UV,
            NORMAL
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
                std::vector<Vertex::NormalT>>;

        using MeshEntry    = mesh_impl::MeshEntry;
        //using Bone         = mesh_impl::Bone;
        //using VertexWeight = mesh_impl::VertexWeight;

    public:
        explicit Mesh(const char* filepath);
        void render(const glm::mat4& view, const glm::mat4& projection, grx::ShaderProgram& shader_program);

        void render(const VP_T& view_projection, grx::ShaderProgram& shader_program) {
            render(view_projection.first, view_projection.second, shader_program);
        }

        void render(const glm::mat4& view, const glm::mat4& projection, grx::ShaderProgram& shader_program, unsigned instances,
                const std::array<glm::vec4, 6>& frustumPlanes);

        void render(const VP_T& view_projection, grx::ShaderProgram& shader_program, unsigned instances, const std::array<glm::vec4, 6>& frustumPlanes) {
            render(view_projection.first, view_projection.second, shader_program, instances, frustumPlanes);
        }

        glm::vec3 pos = {0.f, 0.f, 0.f};

        enum BufferNumbers {
            INDEX_BUFFER = 0,
            POSITION_VB = 1,
            NORMAL_VB = 2,
            UV_VB = 3,
            MVP_MATRIX_VB = 4,
            MODEL_MATRIX_VB = 5
        };

        enum AttributesLocation {
            POSITION_LOCATION = 0,
            UV_LOCATION = 1,
            NORMAL_LOCATION = 2,
            MVP_MATRIX_LOCATION = 3,
            MODEL_MATRIX_LOCATION = 7
        };

    protected:
        void initFromScene(const aiScene* scene, const char* filepath);
        void initMesh(const aiMesh* mesh, UnpackedVertexVector& vertices, std::vector<unsigned>& indices);

        std::vector<MeshEntry> mesh_entries;
        std::vector<grx::Texture>  textures;
        std::array <unsigned, 6> _glBuffers;
        glm::vec3 _aa = {0.f, 0.f, 0.f};
        glm::vec3 _bb = {0.f, 0.f, 0.f};

        unsigned _glVAO;


    };


} // namespace grx
