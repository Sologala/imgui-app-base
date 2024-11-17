#pragma once

#include <cmath>
#include <imapp/primitives/drawable.h>
#include <vector>

struct Cylinder : public Drawable
{
    Cylinder(float radius = 2, float height = 1.75f, float color_r = 0.5, float color_g = 0.5, float color_b = 0.5)
        : Drawable(), radius_(radius), height_(height)
    {
        const std::string vs = R"( #version 330 core
            layout (location = 0) in vec3 aPos; 
            uniform mat4 pvm;
            uniform mat4 m;
            void main()
            {
                gl_Position = pvm * m * vec4(aPos, 1.0);
            }
        )";

        const std::string fs = R"(
            #version 330 core
            out vec4 FragColor;
            uniform vec3 objectColor;
            void main()
            {
                FragColor = vec4(objectColor, 1.0);
            }
        )";

        Drawable::CompileShader(vs, fs);

        color_[0] = (color_r);

        color_[1] = (color_g);
        color_[2] = (color_b);
        // 顶点和索引数据

        auto generateCylinderMesh = [&]() {
            // 生成圆柱体的顶点数据
            for (int i = 0; i <= slices_; ++i)
            {
                float angle = 2.0f * EIGEN_PI * i / slices_;
                float x     = radius * cos(angle);
                float y     = radius * sin(angle);

                // 顶面点
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(height);

                // 底面点
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(0);
            }

            // 最后添加中心点
            vertices.push_back(0.0f);
            vertices.push_back(0.f);
            vertices.push_back(height);

            vertices.push_back(0.0f);
            vertices.push_back(0.0);
            vertices.push_back(0.0);

            int centerTopIndex    = slices_ * 2;
            int centerBottomIndex = centerTopIndex + 1;

            // 生成圆柱体的索引数据
            for (int i = 0; i < slices_; ++i)
            {
                // 侧面
                indices.push_back(i * 2);
                indices.push_back(i * 2 + 1);
                indices.push_back((i * 2 + 2) % (slices_ * 2));

                indices.push_back((i * 2 + 2) % (slices_ * 2));
                indices.push_back(i * 2 + 1);
                indices.push_back((i * 2 + 3) % (slices_ * 2));

                // 顶面
                indices.push_back(centerTopIndex);
                indices.push_back(i * 2);
                indices.push_back((i * 2 + 2) % (slices_ * 2));

                // 底面
                indices.push_back(centerBottomIndex);
                indices.push_back((i * 2 + 3) % (slices_ * 2));
                indices.push_back(i * 2 + 1);
            }
        };
        generateCylinderMesh();
        // 1. 绑定VAO
        glBindVertexArray(vao);
        // 2. 把顶点数组复制到缓冲中供OpenGL使用
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        setTransform(Eigen::Matrix4f::Identity());
    };
    virtual bool Draw() override
    {
        setUniformVec3("objectColor", 1, color_);

        Shader::Bind();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        Shader::UnBind();
        return true;
    }
    virtual bool Render()
    {
        return true;
    }
    float                radius_ = 2.0;
    float                height_ = 4.0;
    int                  slices_ = 36;
    float                color_[3]{0.f};
    std::vector<GLfloat> vertices;
    std::vector<GLuint>  indices;
};
