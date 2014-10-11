#pragma once
#include <opencv2/core/core.hpp>


namespace rsdt { namespace protovis {


class Canvas
{
public:
  Canvas(cv::Mat const& canvas)
  : canvas_(canvas),
    pen_thickness_(1),
    pen_color_(cv::Scalar(0, 0, 0)),
    bg_color_(cv::Scalar(255, 255, 255)),

    origin_(0, 0),
    pos_(0, 0),
    x_scale_(1.0),
    y_scale_(1.0)
  {
    canvas_ = bg_color_;
  }

  void set_scale(float sx, float sy)
  {
    x_scale_ = sx;
    y_scale_ = sy;
  }

  void set_origin(int ix, int iy)
  {
    origin_.x = ix;
    origin_.y = iy;
  }

  void shift_origin(int dix, int diy)
  {
    origin_.x += dix;
    origin_.y += diy;
  }

  void flip_y_axis()
  {
    y_scale_ *= -1;
    origin_.y = canvas_.rows - origin_.y - 1;
  }

  void convert_domain_to_image(int & ix, int & iy,
                               float dx, float dy)
  {
    ix = static_cast<int>(dx * x_scale_ + 0.5) + origin_.x;
    iy = static_cast<int>(dy * y_scale_ + 0.5) + origin_.y;
  }

  void move_to(float x, float y)
  {
    convert_domain_to_image(pos_.x, pos_.y, x, y);
  }

  void line_to(float x, float y)
  {
    auto const prev = pos_;
    move_to(x, y);
    cv::line(canvas_, prev, pos_, pen_color_, pen_thickness_);
  }

private:
  // graphics
  cv::Mat    canvas_;
  int        pen_thickness_;
  cv::Scalar pen_color_;
  cv::Scalar bg_color_;

  // coordinates
  cv::Point origin_;
  cv::Point pos_;
  float x_scale_;
  float y_scale_;
};

void line_plot
(
  Canvas          & canvas,
  std::vector<float> const& data
);

void multiline_surface_plot
(
  Canvas       & canvas,
  cv::Mat const& data,
  int            step,
  int            phase,
  float          z_scale
);

}} // rsdt::protovis
