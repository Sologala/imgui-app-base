#pragma once
#include "imapp/primitives/axis.h"
#include "imapp/primitives/drawable.h"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <imapp/canvas/frame_buffer_manager.h>
#include <unordered_map>

struct MeshGrid : public Drawable
{
    float mesh_size  = 1.0f;
    int   mesh_count = 20;
    MeshGrid()
    {

        const char *mesh_vs = R"(#version 330 core 
            layout (location = 0) in vec3 aPos;
            uniform mat4 pvm;
            void main()
            {
                gl_Position = pvm * vec4(aPos, 1.0);
            }
        )";
        const char *mesh_fs = R"(\
            #version 330 core
            out vec4 FragColor;
            void main()
            {
                FragColor = vec4(1.f, 1.0f, 1.0f, 1.0f);
            }
        )";
        CompileShader(mesh_vs, mesh_fs);
        init(mesh_size, mesh_count);
    };

    void init(float size, int count)
    {
        std::vector<float> mesh_vertexs;
        unsigned int       vertex_count = 0;

        float start = count * (size / 2);
        float pos_x = start, pos_z = start;

        for (int i = 0; i <= count; ++i)
        {
            mesh_vertexs.push_back(pos_x);
            mesh_vertexs.push_back(start);
            mesh_vertexs.push_back(0);

            mesh_vertexs.push_back(pos_x);
            mesh_vertexs.push_back(-start);
            mesh_vertexs.push_back(0);

            mesh_vertexs.push_back(start);
            mesh_vertexs.push_back(pos_z);
            mesh_vertexs.push_back(0);

            mesh_vertexs.push_back(-start);
            mesh_vertexs.push_back(pos_z);
            mesh_vertexs.push_back(0);

            pos_x = pos_x - size;
            pos_z = pos_z - size;
        }

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, mesh_vertexs.size() * sizeof(float), &mesh_vertexs[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        draw_count = (int)mesh_vertexs.size() / 3;
    }
    bool Draw()
    {
        Shader::Bind();
        glBindVertexArray(vao);
        glLineWidth(1.0f);
        glDrawArrays(GL_LINES, 0, draw_count);
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
    int   draw_count;
};

class Camera
{
  public:
    Camera(const float width, const float height) : fb_width(width), fb_height(height)
    {
    }
    Eigen::Matrix4f getPVM()
    {
        Eigen::Affine3f model = Eigen::Affine3f::Identity();
        model.translate(Eigen::Vector3f(m_x_trans, m_y_trans, 0.0));                     // 平移
        model.rotate(Eigen::AngleAxisf(m_y_rot / 180 * EIGEN_PI, Eigen::Vector3f::UnitY())); // 绕X轴旋转
        model.rotate(Eigen::AngleAxisf(m_z_rot / 180 * EIGEN_PI, Eigen::Vector3f::UnitZ())); // 绕Z轴旋转

        Eigen::Affine3f view = Eigen::Affine3f::Identity();
        Eigen::Vector3f eye(-20.0, 0, 30.0);
        Eigen::Vector3f center(0, 0., 0);
        Eigen::Vector3f up(0, 0, 1);
        view = Camera::lookAt(eye, center, up);

        // 设置投影矩阵
        float       mnear        = 0.1f;                               // 近裁剪面
        float       mfar         = 1000.0f;                            // 远裁剪面
        float       fov         = m_zoom / 180 * EIGEN_PI;                // 垂直视场角度 (弧度)
        float       aspectRatio = (float)fb_width / (float)fb_height; // 宽高比
        const float tanHalfFovy = tan(0.5 * fov);

        Eigen::Matrix4f projection = Eigen::Matrix4f::Zero();
        projection                 = perspective(m_zoom, aspectRatio, mnear, mfar);
        // 计算总的MVP矩阵
        return (projection * view.matrix() * model.matrix());
    }

    // opengl camera: RUB  NDC coord: RUF
    Eigen::Matrix4f perspective(float fovy, float aspect, float zNear, float zFar)
    {
        Eigen::Matrix4f P = Eigen::Matrix4f::Zero();
        assert(aspect > 0);
        assert(zFar > zNear);
        assert(zNear > 0);
        float fovy_rad      = EIGEN_PI * fovy / 180.0;
        float fovy_rad_half = fovy_rad / 2;
        float fy            = 1.0 / std::tan(fovy_rad_half);
        float fx            = 1.0 / (aspect * std::tan(fovy_rad_half));
        P(0, 0)             = fx;
        P(1, 1)             = fy;
        P(2, 2)             = -(zFar + zNear) / (zFar - zNear);
        P(3, 2)             = -1.0;
        P(2, 3)             = -(2.0 * zFar * zNear) / (zFar - zNear);
        return P.matrix();
    }

    static Eigen::Matrix4f lookAt(Eigen::Vector3f const &eye, Eigen::Vector3f const &center, Eigen::Vector3f const &up)
    {
        Eigen::Vector3f z = (center - eye).normalized();
        Eigen::Vector3f x = -(up.cross(z).normalized());
        Eigen::Vector3f y = -(x.cross(z));

        Eigen::Matrix4f T_wc = Eigen::Matrix4f::Identity();

        T_wc.block<3, 1>(0, 0)      = x;
        T_wc.block<3, 1>(0, 1)      = y;
        T_wc.block<3, 1>(0, 2)      = z;
        T_wc.topRightCorner<3, 1>() = eye;
        Eigen::Matrix4f T_cw        = T_wc.inverse();
        Eigen::Matrix4f T_gl_cam;
        T_gl_cam << 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1;
        return T_gl_cam * T_cw;
    }

    float m_y_rot   = 0; // deg
    float m_z_rot   = 0;
    float m_x_trans = 0.0;
    float m_y_trans = 0.0;
    float m_zoom    = 45.0; // deg
    float fb_width  = 1280;
    float fb_height = 960;
};

class Canvas : public FrameBufferShader
{
  private:
    struct DrawInfo
    {
        std::shared_ptr<Drawable> draw_p;
        bool                      draw_flag{true};
    };

  public:
    Canvas(const int width = 1280, const int height = 960) : FrameBufferShader(width, height), cam(width, height)
    {
        Axis::Sptr axis_world = std::make_shared<Axis>(2.0f, 2.f);
        axis_world->setTransform(Eigen::Matrix4f::Identity());
        AddDrawable("world", axis_world);
    }
    ~Canvas()
    {
    }
    void Resize(const int width, const int height)
    {
        fb_width      = width;
        fb_height     = height;
        cam.fb_width  = fb_width;
        cam.fb_height = fb_height;
    }
    virtual void Clean()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(0.18f, 0.18f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    virtual void AddDrawable(const std::string &name, Drawable::Sptr pdrawable, bool dft_show = true)
    {
        draw_lists[name].draw_p    = pdrawable;
        draw_lists[name].draw_flag = dft_show;
    }
    virtual void RemoveDrawable(const std::string &name, Drawable::Sptr pdrawable)
    {
        if (draw_lists.count(name))
        {
            draw_lists.erase(name);
        }
    }
    virtual bool Render()
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, fb_width, fb_height);
        Eigen::Matrix4f pvm_eigen = cam.getPVM();
        Eigen::Matrix4f identity  = Eigen::Matrix4f::Identity();
        mesh_grid.setUniformMatrix4x4("pvm", 1, GL_FALSE, pvm_eigen.data());
        if (!mesh_grid.Draw())
            return false;
        // draw all drawables in draw_lists;
        for (auto &draw : draw_lists)
        {
            if (draw.second.draw_flag == false)
                continue;
            draw.second.draw_p->setUniformMatrix4x4("pvm", 1, GL_FALSE, pvm_eigen.data());
            if (!draw.second.draw_p->Draw())
            {
                return false;
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        FrameBufferShader::RenderToTexture();
        return true;
    }

  public:
    Camera                                    cam;
    std::unordered_map<std::string, DrawInfo> draw_lists;

  private:
    MeshGrid mesh_grid;
};

class ImGuiCanvas : public Canvas
{
  public:
    using Sptr = std::shared_ptr<ImGuiCanvas>;
    using Uptr = std::unique_ptr<ImGuiCanvas>;
    ImGuiCanvas(const std::string &name) : name_(name), Canvas()
    {
    }
    bool Draw()
    {
        if (!ImGui::Begin(name_.c_str()))
        {
            ImGui::End();
            return true;
        }
        if (ImGui::TreeNode("DrawTree"))
        {
            for (auto &d : draw_lists)
            {
                ImGui::Selectable(d.first.c_str(), &d.second.draw_flag);
            }
            ImGui::TreePop();
        }

        ImVec2 pos          = ImGui::GetCursorScreenPos();
        ImVec2 avalid_regin = ImGui::GetContentRegionAvail();
        avalid_regin.x      = std::max(200.f, avalid_regin.x);
        avalid_regin.y      = std::max(200.f, avalid_regin.y);
        ImGui::InvisibleButton("iv_button", ImVec2(avalid_regin.x, avalid_regin.y));
        Resize(avalid_regin.x, avalid_regin.y);
        pos.x -= 5;
        pos.y -= 2;

        if (ImGui::IsItemHovered())
        {
            cam.m_zoom -= ImGui::GetIO().MouseWheel;
            cam.m_zoom = std::max(1.0f, cam.m_zoom);
            cam.m_zoom = std::min(50.0f, cam.m_zoom);
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            {
                cam.m_y_rot += (ImGui::GetIO().MouseDelta.y < 0 ? 1.f : -1.f) * 0.1 *
                               std::min(std::abs(ImGui::GetIO().MouseDelta.y), 50.f);
                cam.m_z_rot += (ImGui::GetIO().MouseDelta.x > 0 ? 1.f : -1.f) * 0.2 *
                               std::min(std::abs(ImGui::GetIO().MouseDelta.x), 50.f);
            }
            {
                if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
                {
                    cam.m_y_trans -= 0.03 * ImGui::GetIO().MouseDelta.x;
                    cam.m_x_trans -= 0.03 * ImGui::GetIO().MouseDelta.y;
                }
            }
        }

        ImGui::GetWindowDrawList()->AddImage(
            texture, ImVec2(pos.x, pos.y), ImVec2(pos.x + fb_width, pos.y + cam.fb_height), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
        Clean();
        Render();
        return true;
    }

    std::string name_;
};
