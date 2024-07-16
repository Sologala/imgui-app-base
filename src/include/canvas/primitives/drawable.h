#pragma once
#include "shader.h"
#include <memory>


class Drawable : public Shader
{
  public:
    using Sptr = std::shared_ptr<Drawable>;
    using Uptr = std::unique_ptr<Drawable>;

    Drawable(const std::string &vs_str, const std::string &fs_str) : Shader(vs_str, fs_str)
    {
    }

    virtual bool Draw() = 0;
};


