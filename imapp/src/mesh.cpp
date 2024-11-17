#include "stlreader.h"
#include <glad/gl.h> // Make sure to initialize GLEW #include <glm/glm.hpp> // You may need to install GLM for this
#include <imapp/imapp.h>
#include <imapp/primitives/mesh.h>
#include <map>
#include <vector>

struct Triangle
{
    glm::vec3 normal;
    glm::vec3 vertices[3];
};

std::vector<Triangle> readBinaryStl(const std::string &filename)
{
    stl_reader::StlMesh<float, unsigned int> mesh(filename);

    std::vector<Triangle> triangles(mesh.num_tris());
    for (size_t itri = 0; itri < mesh.num_tris(); ++itri)
    {
        // std::cout << "coordinates of triangle " << itri << ": ";
        for (size_t icorner = 0; icorner < 3; ++icorner)
        {
            const float *c = mesh.tri_corner_coords(itri, icorner);
            // or alternatively:
            // float* c = mesh.vrt_coords (mesh.tri_corner_ind (itri, icorner));
            triangles[itri].vertices[icorner].x = c[0];
            triangles[itri].vertices[icorner].y = c[1];
            triangles[itri].vertices[icorner].z = c[2];
            // triangles[itri].vertices[icorner] /= 40;
        }
        const float *n = mesh.tri_normal(itri);

        triangles[itri].normal.x = n[0];
        triangles[itri].normal.y = n[1];
        triangles[itri].normal.z = n[2];
        // triangles[itri].normal /= 40;
    }
    return triangles;
}

// Comparator for glm::vec3 used in std::map
struct lessVec3
{
    bool operator()(const glm::vec3 &lhs, const glm::vec3 &rhs) const
    {
        constexpr float thro = 0.0001;
        if (abs(lhs.x - rhs.x) > thro)
        {
            return lhs.x < rhs.x;
        }
        if (abs(lhs.y - rhs.y) > thro)
        {
            return lhs.y < rhs.y;
        }
        return lhs.z < rhs.z;
    }
};
void buildVertexAndIndexData(const std::vector<Triangle> &triangles, std::vector<glm::vec3> &vertices,
                             std::vector<uint32_t> &indices)
{
    std::map<glm::vec3, uint32_t, lessVec3> vertex_map;

    for (const auto &triangle : triangles)
    {
        for (const auto &vertex : triangle.vertices)
        {
            auto it = vertex_map.find(vertex);
            if (it != vertex_map.end())
            {
                indices.push_back(it->second);
            }
            else
            {
                uint32_t index = vertices.size();
                vertices.push_back(vertex);
                indices.push_back(index);
                vertex_map[vertex] = index;
            }
        }
    }
}

// And set up your VAO and drawing routines accordingly...
bool Mesh::LoadSTLMesh(const std::string &stlPath)
{
    std::vector<Triangle> tris = readBinaryStl(stlPath);
    buildVertexAndIndexData(tris, vertices, indices);
    std::cout << vertices.size() << " index size " << indices.size() << std::endl;
    return true;
}
void Mesh::UploadDataToGPU()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
