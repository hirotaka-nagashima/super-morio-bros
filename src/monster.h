//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef SUPER_MORIO_BROS_MONSTER_H_
#define SUPER_MORIO_BROS_MONSTER_H_

#include "moving_object.h"

class Monster : public MovingObject {
public:
  enum State {
    kWalking = 0,
    kAttacked = 2,
    kNumStates,
  };

  Monster(int id_x, int id_y)
      : MovingObject(kMonster, 32, 32, id_x, id_y) {}

  virtual void Reset() {
    MovingObject::Reset();
    state_ = kWalking;
  }
  virtual void NotifyAttacked();

private:
  virtual int CalculateImageId() const;
  virtual void CalculateForce();

  State state_;
};

#endif  // SUPER_MORIO_BROS_MONSTER_H_