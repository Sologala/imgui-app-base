#pragma once
#include "drawable.h"
#include <vector>

const char *axis_vs = "\
    #version 330 core\n\
    layout (location = 0) in vec3 aPos;\n\
    layout (location = 0) in vec3 aColor;\n\
    out vec3 vertexColor; \n\
    uniform mat4 pvm;\n\
    void main()\
    {\n\
        gl_Position = pvm * vec4(aPos, 1.0);\n\
        vertexColor = aColor;\n\
    }\n\
";

const char *axis_fs = "\
    #version 330 core \n\
    in vec3 vertexColor;\n\
    out vec4 FragColor;\n\
    void main() \n\
    { \n\
        FragColor = vec4(vertexColor, 1.0); \n\
    } \n\
";
// clang-format on

struct Axis : public Drawable
{

    Axis() : Drawable(axis_vs, axis_fs)
    {
        init();
    };

    void init()
    {
        std::vector<float> axis_vertexs = {
            // x,y ,z ,r, g, b
            0.0, 0.0, 0.0, 1.0, 0.0, 0.0, m_axis_len, 0.0,        0.0,        1.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,        m_axis_len, 0.0,        0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,        0.0,        m_axis_len, 0.0, 0.0, 1.0,
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
    }
    bool Draw()
    {
        Shader::Bind();
        glBindVertexArray(vao);
        glLineWidth(5.0f);
        glDrawArrays(GL_LINE_STRIP, 0, 6);
        glBindVertexArray(0);
        Shader::UnBind();
        return true;
    }
    virtual bool Render()
    {
        return true;
    }
    /* const float vertices[9] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f}; */
    float m_axis_len = 2.0;
};
