//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef SUPER_MORIO_BROS_ITEM_H_
#define SUPER_MORIO_BROS_ITEM_H_

#include "moving_object.h"

class Item : public MovingObject {
public:
  enum State {
    kStored,
    kPushedOut,
    kMoving,
  };

  Item(Type type, int id_x, int id_y)
      : MovingObject(type, 32, 32, id_x, id_y) {}

  virtual void Reset() {
    MovingObject::Reset();
    is_vanished_ = true;
    state_ = kStored;
  }

  State state() const { return state_; }
  void set_state(State state) { state_ = state; }
  void appear() { is_vanished_ = false; }

private:
  virtual void CalculateForce();

  State state_;
};

#endif  // SUPER_MORIO_BROS_ITEM_H_