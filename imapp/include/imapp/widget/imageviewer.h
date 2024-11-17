#pragma once
#include <cassert>
#include <cstdint>
#include <glad/gl.h>
#include <imapp/utils/threadpool.h>
#include <imgui.h>
#include <iostream>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
class ImageViewer
{
  public:
    ImageViewer(const std::string img_name = "default_name")
    {
        glGenTextures(1, &texture);
        file_name = img_name;
    };
    ~ImageViewer() = default;
    int draw(const cv::Mat &img)
    {
        cv::Mat nimg = img;
        if (img.cols < 200 || img.rows < 200)
        {
            // inp img size < 200, the texture will be wrong.
            nimg        = img.clone();
            float ratio = 200 / fmax(1.f, fmin(img.cols, img.rows));
            cv::resize(nimg, nimg, cv::Size{}, ratio, ratio);
        }
        if (save_flag)
        {
            nimg = img.clone();
            cv::circle(nimg, cv::Point(5, 5), 3, {0, 0, 255}, 2);
        }
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nimg.cols, nimg.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, nimg.data);
        ImVec2 avail_size = ImGui::GetContentRegionAvail();
        double ratio      = static_cast<double>(avail_size.x) / nimg.cols;
        avail_size.y      = ratio * nimg.rows;
        ImGui::Image(texture, avail_size);
        hovered = ImGui::IsItemHovered();
        if (hovered)
        {
            // 计算鼠标相对于图片的坐标
            ImVec2 mouse_pos_in_window = ImGui::GetMousePos();
            ImVec2 window_pos          = ImGui::GetWindowPos();
            // 获取图片绘制区域的开始坐标
            ImVec2 imageStartPos = ImGui::GetItemRectMin();

            // 计算并显示鼠标在图像上的相对坐标
            ImVec2 mouse_pos_in_image(mouse_pos_in_window.x - imageStartPos.x, mouse_pos_in_window.y - imageStartPos.y);

            // 限定鼠标坐标不超过图片尺寸
            if (mouse_pos_in_image.x >= 0.0f && mouse_pos_in_image.x < avail_size.x && mouse_pos_in_image.y >= 0.0f &&
                mouse_pos_in_image.y < avail_size.y)
            {
                mouse_norm = ImVec2{mouse_pos_in_image.x / avail_size.x, mouse_pos_in_image.y / avail_size.y};
            }
            if (ImGui::IsKeyPressed(ImGuiKey_S))
            {
                ToggleSave(file_name);
            }
        }
        if (save_flag)
        {
            if (writer == nullptr)
            {
                writer = std::make_shared<cv::VideoWriter>(cv::String(file_name + ".mp4"),
                                                           cv::VideoWriter::fourcc('D', 'I', 'V', 'X'), 30,
                                                           cv::Size{img.cols, img.rows});
            }
            writer_worker->enqueue([](std::shared_ptr<cv::VideoWriter> _writer, cv::Mat img) { _writer->write(img); },
                                   writer, img.clone());
        }
        return 0;
    }
    bool Hovered()
    {
        return hovered;
    }
    ImVec2 MousePosNorm()
    {
        return mouse_norm;
    }
    void ToggleSave(const std::string &filename)
    {
        file_name = filename;
        if (save_flag == false)
        {
            writer_worker = std::make_shared<ThreadPool>(1);
            save_flag     = true;
        }
        else
        {
            writer_worker.reset();
            writer->release();
            printf("save %s video to %s.mp4\n", file_name.c_str(), file_name.c_str());
            writer.reset();
            save_flag = false;
        }
    }

  private:
    GLuint                           texture;
    bool                             hovered{false};
    ImVec2                           mouse_norm;
    std::shared_ptr<cv::VideoWriter> writer;
    bool                             save_flag{false};
    std::shared_ptr<ThreadPool>      writer_worker;
    std::string                      file_name;
};
