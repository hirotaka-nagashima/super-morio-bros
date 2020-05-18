//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef SUPER_MORIO_BROS_COMMAND_H_
#define SUPER_MORIO_BROS_COMMAND_H_

#include <cstring>

class Command {
public:
  enum Key {
    kLeft,
    kRight,
    kJump,
    kDash,
    kNumKeys,
  };

  Command() { Reset(); }

  void UpdateCommand();
  void Reset() { memset(command_, 0, sizeof(command_)); }

  int command(Key key) const { return command_[key]; }
  int left() const { return command_[kLeft]; }
  int right() const { return command_[kRight]; }
  int jump() const { return command_[kJump]; }
  int dash() const { return command_[kDash]; }

private:
  int command_[kNumKeys];  // How long each key is held down.
};

#endif  // SUPER_MORIO_BROS_COMMAND_H_