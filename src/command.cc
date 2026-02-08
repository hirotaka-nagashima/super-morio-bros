//-----------------------------------------------------------------------------
// Copyright (c) 2013 Hirotaka Nagashima. All rights reserved.
//-----------------------------------------------------------------------------

#include "command.h"
#include "graphic.h"

void Command::UpdateCommand() {
  // Update command from key inputs.
  const Uint8* key = SDL_GetKeyboardState(NULL);
  command_[kLeft]  = key[SDL_SCANCODE_LEFT]  ? command_[kLeft] + 1  : 0;
  command_[kRight] = key[SDL_SCANCODE_RIGHT] ? command_[kRight] + 1 : 0;
  command_[kJump]  = key[SDL_SCANCODE_C]     ? command_[kJump] + 1  : 0;
  command_[kDash]  = key[SDL_SCANCODE_X]     ? command_[kDash] + 1  : 0;
}