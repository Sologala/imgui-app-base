#pragma once
#include "shader.h"
#include <Eigen/Core>
#include <memory>

class Drawable : public Shader
{
  public:
    using Sptr          = std::shared_ptr<Drawable>;
    using Uptr          = std::unique_ptr<Drawable>;
    virtual bool Draw() = 0;
    template <typename Scalar>
    void setTransform(const Eigen::MatrixBase<Scalar> &T_wt)
    {
        Eigen::Matrix4f T = Eigen::Matrix4f::Identity();
        T                 = T_wt.template cast<float>();
        setUniformMatrix4x4("m", 1, false, T.data());
    }

    template <typename Scalar, typename Scalar1>
    void setTransform(const Eigen::MatrixBase<Scalar> &R_wt, const Eigen::MatrixBase<Scalar1> &t_wt)
    {
        Eigen::Matrix4f T        = Eigen::Matrix4f::Identity();
        T.topLeftCorner<3, 3>()  = R_wt.template cast<float>();
        T.topRightCorner<3, 1>() = t_wt.template cast<float>();
        setUniformMatrix4x4("m", 1, false, T.data());
    }
    template <typename Scalar>
    void setRotation(const Eigen::MatrixBase<Scalar> &R_wt)
    {
        Eigen::Matrix4f T       = Eigen::Matrix4f::Identity();
        T.topLeftCorner<3, 3>() = R_wt.template cast<float>();
        setUniformMatrix4x4("m", 1, false, T.data());
    }

    template <typename Scalar>
    void setTranslation(const Eigen::MatrixBase<Scalar> &t_wt)
    {
        Eigen::Matrix4f T        = Eigen::Matrix4f::Identity();
        T.topRightCorner<3, 1>() = t_wt.template cast<float>();
        setUniformMatrix4x4("m", 1, false, T.data());
    }
};
