#include "glm/vec3.hpp"
#include <imapp/primitives/drawable.h>
#include <vector>

class Mesh : public Drawable
{
  public:
    using Sptr = std::shared_ptr<Mesh>;
    using Uptr = std::unique_ptr<Mesh>;

    enum DrawMode
    {
        TRIANGLE,
        GRID
    };

    Mesh(const std::string &stlPath)
    {
        const char *vs = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            uniform mat4 pvm;
            uniform mat4 m;

            // out float fragDepth;
            void main()
            {
                // fragDepth = aPos.z
                gl_Position = pvm * m * vec4(aPos, 1.0);
            } 
        )";

        const char *fs = R"(
            #version 330 core 
            // in float fragDepth;
            uniform float alpha;
            out vec4 FragColor;
            void main() 
            { 
                // float zHeight = clamp(fragDepth, -10.0, 10.0);
                // float normalizedHeight = (zHeight + 10.0) / 20.0;  // 将高度映射到 [0, 1] 范围
                vec3 color = vec3(1.0, 1.0, 0.5);  // 使用高度值作为灰度色
                FragColor = vec4(color, alpha);
            } 
        )";
        CompileShader(vs, fs);
        LoadSTLMesh(stlPath);
        UploadDataToGPU();
        setTransform(Eigen::Matrix4f::Identity());
    }

    virtual bool Draw() override
    {
        Shader::Bind();
        SetFloat("alpha", alpha);
        glBindVertexArray(vao);
        switch (mode_)
        {
        case TRIANGLE:
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            break;
        case GRID:
            glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
            break;
        }
        // glDrawArrays(GL_LINES, 0, vertices.size());
        glBindVertexArray(0);
        Shader::UnBind();
        return true;
    }

    inline void SetAlpha(const float a)
    {
        alpha = a;
    }

    inline void SetDrawMode(const DrawMode &mode)
    {
        mode_ = mode;
    }

  private:
    std::vector<glm::vec3> vertices;
    std::vector<uint32_t>  indices;

    float    alpha{0.0};
    DrawMode mode_ = DrawMode::TRIANGLE;

    bool LoadSTLMesh(const std::string &stlPath);
    void UploadDataToGPU();
};
