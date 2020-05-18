//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef SUPER_MORIO_BROS_POINT_H_
#define SUPER_MORIO_BROS_POINT_H_

// 2D point, vector.
struct Point {
  Point() : x(0), y(0) {}
  Point(int initial_x, int initial_y) : x(initial_x), y(initial_y) {}

  // Only for std::map<>.
  bool operator<(const Point& point) const {
    return (y < point.y || x < point.x);
  }
  Point Inverse() const {
    Point result(-x, -y);
    return result;
  }
  Point Add(const Point& point) const {
    Point result(x + point.x, y + point.y);
    return result;
  }
  Point Subtract(const Point& point) const {
    return Add(point.Inverse());
  }
  Point Average(const Point& point) const {
    Point result = Add(point);
    result.x /= 2;
    result.y /= 2;
    return result;
  }
  bool Equals(const Point& point) const {
    return (point.x == x && point.y == y);
  }

  int x;
  int y;
};

struct Move {
  Point src;
  Point dest;
};

#endif  // SUPER_MORIO_BROS_POINT_H_