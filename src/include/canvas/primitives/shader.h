#pragma once
#include <fstream>
#include <glad/gl.h>
#include <iostream>
#include <sstream>
#include <string>

class Shader
{
  public:
    unsigned int id_program;
    GLuint       vao; // vertex array object
    GLuint       vbo; // vertex buffer object
    GLuint       ebo; // vertex buffer object

    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader() = default;
    Shader(const std::string &vs_str, const std::string &fs_str)
    {
        CompileShader(vs_str, fs_str);
    };
    void CompileShader(const std::string &vs_str, const std::string &fs_str)
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        // 1. retrieve the vertex/fragment source code from filePath
        /* std::string vs_str = LoadShaderFile(vs_path); */
        /* std::string fs_str = LoadShaderFile(fs_path); */
        /* LOGI(vs_str); */
        /* LOGI(vs_str); */

        const char *p_vs_code = vs_str.c_str();
        const char *p_fs_code = fs_str.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &p_vs_code, NULL);
        glCompileShader(vertex);
        CheckCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &p_fs_code, NULL);
        glCompileShader(fragment);
        CheckCompileErrors(fragment, "FRAGMENT");
        // shader Program
        id_program = glCreateProgram();
        glAttachShader(id_program, vertex);
        glAttachShader(id_program, fragment);
        glLinkProgram(id_program);
        CheckCompileErrors(id_program, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    ~Shader()
    {
        glDeleteProgram(id_program);
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void Bind()
    {
        glUseProgram(id_program);
    }
    void UnBind()
    {
        glUseProgram(0);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void SetBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(id_program, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void SetInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(id_program, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void SetFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(id_program, name.c_str()), value);
    }

    void setUniformMatrix4x4(const std::string &var_name, GLfloat *data)
    {
        setUniformMatrix4x4(var_name, 1, GL_FALSE, data);
    }
    void setUniformVec3(const std::string &var_name, GLint count, GLfloat *data)
    {
        Bind();
        GLuint id = glGetUniformLocation(id_program, var_name.c_str());
        if (id != -1)
            glUniform3fv(id, count, data);
        UnBind();
    }
    void setUniformMatrix4x4(const std::string &var_name, GLint count, GLboolean need_transpose, GLfloat *data)
    {
        Bind();
        GLuint id = glGetUniformLocation(id_program, var_name.c_str());
        if (id != -1)
            glUniformMatrix4fv(id, count, need_transpose, data);
        UnBind();
    }

  private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void CheckCompileErrors(unsigned int shader, std::string type)
    {
        int  success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    std::string LoadShaderFile(const std::string &shader_file)
    {
        std::string   ret;
        std::ifstream ifile;
        // ensure ifstream objects can throw exceptions:
        ifile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            ifile.open(shader_file);
            std::stringstream ss;
            // read file's buffer contents into streams
            ss << ifile.rdbuf();
            // close file handlers
            ifile.close();
            // convert stream into string
            ret = ss.str();
        }
        catch (std::ifstream::failure &e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        return ret;
    }
};
