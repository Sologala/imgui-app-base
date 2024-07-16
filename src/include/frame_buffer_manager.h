#pragma once
#include <imgui_app_base.h>
#include <iostream>

class FrameBuffer
{
  public:
    FrameBuffer(uint32_t w, uint32_t h, float cr = 0.f, float cg = 0.f, float cb = 0.f, float ca = 1.f)
        : width_(w), height_(h), ca_(ca), cr_(cr), cg_(cg), cb_(cb)
    {
        glGenFramebuffers(1, &fbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

        glGenTextures(1, &texture_id_);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id_, 0);

        glGenRenderbuffers(1, &rbo_);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    virtual ~FrameBuffer()
    {
        glDeleteFramebuffers(1, &fbo_);
        glDeleteTextures(1, &texture_id_);
        glDeleteRenderbuffers(1, &rbo_);
    };

    virtual void ScaleTo(uint32_t width, uint32_t height)
    {
        width_  = width;
        height_ = height;

        glBindTexture(GL_TEXTURE_2D, texture_id_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id_, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_);
    }

    inline uint32_t GetTextureId()
    {
        return texture_id_;
    }

    void Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    }

    void UnBind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    virtual void Clean()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        /* glClear(GL_COLOR_BUFFER_BIT); */
        /*  */
        /* glClearColor(cr_, cg_, cb_, ca_); */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    virtual bool Rend() = 0;

  protected:
    GLuint fbo_, rbo_;
    GLuint texture_id_;
    GLuint width_, height_;
    float  cr_, cg_, cb_, ca_;
};
