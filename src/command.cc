//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#include "command.h"
#include "graphic.h"

void Command::UpdateCommand() {
  // Update command from key inputs.
  Uint8* key = SDL_GetKeyState(NULL);
  command_[kLeft]  = key[SDLK_LEFT]  ? command_[kLeft] + 1  : 0;
  command_[kRight] = key[SDLK_RIGHT] ? command_[kRight] + 1 : 0;
  command_[kJump]  = key[SDLK_c]     ? command_[kJump] + 1  : 0;
  command_[kDash]  = key[SDLK_x]     ? command_[kDash] + 1  : 0;
}