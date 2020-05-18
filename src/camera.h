//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef SUPER_MORIO_BROS_CAMERA_H_
#define SUPER_MORIO_BROS_CAMERA_H_

#include "point.h"

class Camera {
public:
  Camera(int screen_width) : kScreenWidth(screen_width) { Reset(); }

  void Reset() { target_.x = target_.y = 0; }
  void SlideHorizontally(int player_x, int world_width);

  int screen_width() const { return kScreenWidth; }
  Point dp(const Point& p) const { return p.Subtract(target_); }
  int dx(int x) const { return x - target_.x; }
  int dy(int y) const { return y - target_.y; }
  int x() const { return target_.x; }
  int y() const { return target_.y; }
  int left() const { return x(); }
  int right() const { return x() + screen_width(); }

private:
  const int kScreenWidth;
  Point target_;
};

#endif  // SUPER_MORIO_BROS_CAMERA_H_
