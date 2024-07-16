#pragma once
#include "canvas/primitives/axis.h"
#include "canvas/primitives/drawable.h"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <frame_buffer_manager.h>

#define M_PI 3.1415926
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
        model.rotate(Eigen::AngleAxisf(m_x_rot / 180 * M_PI, Eigen::Vector3f::UnitX())); // 绕X轴旋转
        model.rotate(Eigen::AngleAxisf(m_z_rot / 180 * M_PI, Eigen::Vector3f::UnitZ())); // 绕X轴旋转

        // 设置视图矩阵
        Eigen::Affine3f view = Eigen::Affine3f::Identity();
        Eigen::Vector3f eye(0, 0, 50.0);   // 相机位置
        Eigen::Vector3f center(0, 0, 1.0); // 相机看向的点
        Eigen::Vector3f up(0, 1, 0);       // 相机的向上向量
        view = lookAt(eye, center, up);

        // 设置投影矩阵
        float       near_        = 0.1f;                               // 近裁剪面
        float       far_         = 100.0f;                             // 远裁剪面
        float       fov         = m_zoom / 180 * M_PI;                // 垂直视场角度 (弧度)
        float       aspectRatio = (float)fb_width / (float)fb_height; // 宽高比
        const float tanHalfFovy = tan(0.5 * fov);

        Eigen::Matrix4f projection = Eigen::Matrix4f::Zero();
        projection                 = perspective(m_zoom, aspectRatio, near_, far_);
        // 计算总的MVP矩阵
        return (projection * view.matrix() * model.matrix());
    }

    Eigen::Matrix4f perspective(float fovy, float aspect, float zNear, float zFar)
    {
        Eigen::Transform<float, 3, Eigen::Projective> tr;
        tr.matrix().setZero();
        assert(aspect > 0);
        assert(zFar > zNear);
        assert(zNear > 0);
        float radf          = M_PI * fovy / 180.0;
        float tan_half_fovy = std::tan(radf / 2.0);
        tr(0, 0)            = 1.0 / (aspect * tan_half_fovy);
        tr(1, 1)            = 1.0 / (tan_half_fovy);
        tr(2, 2)            = -(zFar + zNear) / (zFar - zNear);
        tr(3, 2)            = -1.0;
        tr(2, 3)            = -(2.0 * zFar * zNear) / (zFar - zNear);
        return tr.matrix();
    }

    Eigen::Matrix4f lookAt(Eigen::Vector3f const &eye, Eigen::Vector3f const &center, Eigen::Vector3f const &up)
    {
        Eigen::Vector3f f   = (center - eye).normalized();
        Eigen::Vector3f u   = up.normalized();
        Eigen::Vector3f s   = f.cross(u).normalized();
        u                   = s.cross(f);
        Eigen::Matrix4f mat = Eigen::Matrix4f::Zero();
        mat(0, 0)           = s.x();
        mat(0, 1)           = s.y();
        mat(0, 2)           = s.z();
        mat(0, 3)           = -s.dot(eye);
        mat(1, 0)           = u.x();
        mat(1, 1)           = u.y();
        mat(1, 2)           = u.z();
        mat(1, 3)           = -u.dot(eye);
        mat(2, 0)           = -f.x();
        mat(2, 1)           = -f.y();
        mat(2, 2)           = -f.z();
        mat(2, 3)           = f.dot(eye);
        mat.row(3) << 0, 0, 0, 1;
        return mat;
    }

    float m_x_rot   = 0; // deg
    float m_z_rot   = 0;
    float m_x_trans = 0.0;
    float m_y_trans = 0.0;
    float m_zoom    = 45.0; // deg
    float fb_width  = 1280;
    float fb_height = 960;
};

class Canvas : public FrameBuffer
{
  public:
    Canvas(uint32_t width, uint32_t height) : FrameBuffer(width, height), cam(width, height)
    {
    }
    ~Canvas()
    {
    }

    virtual bool Rend() override
    {
        Clean();

        Bind();
        /* glClearColor(0.0f, 0.0f, 0.0f, 1.0f); */
        /* glClear(GL_COLOR_BUFFER_BIT); */
        glViewport(0, 0, width_, height_);
        /* Clean(); */
        Eigen::Matrix4f pvm_eigen = cam.getPVM();
        axis.setUniformMatrix4x4("pvm", 1, GL_FALSE, pvm_eigen.data());
        if (!axis.Draw())
            return false;
        UnBind();
        return true;
    }
    virtual void ScaleTo(uint32_t width, uint32_t height) override
    {
        cam.fb_height = height;
        cam.fb_width  = width;
        FrameBuffer::ScaleTo(width, height);
    }

  public:
    Camera cam;

  private:
    Axis axis;
};
