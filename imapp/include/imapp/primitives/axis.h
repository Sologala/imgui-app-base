#pragma once
#include "drawable.h"
#include <vector>

// clang-format on

struct Axis : public Drawable
{
    Axis(float axis_len = 2, float axis_width = 2.0f) : axis_len_(axis_len), axis_width_(axis_width)
    {

        const char *axis_vs = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aColor;
            out vec4 outColor; 
            uniform mat4 pvm;
            uniform mat4 m;
            void main()\
            {
                gl_Position = pvm * m * vec4(aPos, 1.0);
                outColor = vec4(aColor, 1.0);
            }
        )";

        const char *axis_fs = R"(\
            #version 330 core 
            in vec4 outColor;
            out vec4 FragColor;
            void main() 
            { 
                FragColor = outColor; 
            } 
        )";

        CompileShader(axis_vs, axis_fs);

        std::vector<float> axis_vertexs = {
            // x,y ,z ,r, g, b
            0.0, 0.0,       0.0, 1.0, 0.0, 0.0, axis_len_, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0,       0.0, 1.0, 0.0,
            0.0, axis_len_, 0.0, 0.0, 1.0, 0.0, 0.0,       0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, axis_len_, 0.0, 0.0, 1.0,
        };
        // 1. 绑定VAO
        glBindVertexArray(vao);
        // 2. 把顶点数组复制到缓冲中供OpenGL使用
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, axis_vertexs.size() * sizeof(float), &axis_vertexs[0], GL_STATIC_DRAW);
        // 3. 设置顶点属性指针
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    };
    bool Draw()
    {
        Shader::Bind();
        glBindVertexArray(vao);
        glLineWidth(axis_width_);
        glDrawArrays(GL_LINES, 0, 6);
        glBindVertexArray(0);
        Shader::UnBind();
        return true;
    }
    virtual bool Render()
    {
        return true;
    }
    float axis_len_   = 2.0;
    float axis_width_ = 4.0;
};
