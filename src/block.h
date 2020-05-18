//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef SUPER_MORIO_BROS_BLOCK_H_
#define SUPER_MORIO_BROS_BLOCK_H_

#include <vector>
#include "point.h"
#include "type.h"

class Graphic;
class Item;
class Camera;

class Block {
public:
  static const int kWidth = 32;
  static const int kHeight = 32;

  Block(Type type, int id_x, int id_y)
      : kDefaultType(type),
        kPosition(id_x * kWidth, id_y * kHeight) {
    Reset();
  }
  Block(Type type, int id_x, int id_y, const std::vector<Item*>& items)
      : kDefaultType(type),
        kPosition(id_x * kWidth, id_y * kHeight),
        items_(items) {
    Reset();
  }
  virtual ~Block() {}

  virtual void Draw(Graphic* graphic, const Camera& camera) const;
  virtual void Reset();
  virtual void PushOutItem();

  Point position() const { return kPosition; }
  Type type() const { return type_; }

protected:
  const Type kDefaultType;
  const Point kPosition;
  Type type_;
  std::vector<Item*> items_;
};

#endif  // SUPER_MORIO_BROS_BLOCK_H_