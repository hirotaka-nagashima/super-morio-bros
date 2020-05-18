//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------
// 2017/03/13: Reconstructed with C++.
// 2017/01/13:
// 2013/0?/??: The project was created for "S2 Programming I."
//-----------------------------------------------------------------------------
// [Å©][Å®]     : Move.
// [x]+[Å©][Å®] : Dash.
// [c]        : Jump.
//-----------------------------------------------------------------------------

#include "game.h"
#include <ctime>  // time()
#include "status.h"
#include "world.h"
#include "best_scores.h"
#include "info.h"
#include "camera.h"
#include "command.h"

int g_frame_id;

// NOTE: Comment out below to use SDL in VS2015 or later versions.
// https://stackoverflow.com/questions/30412951/
FILE _iob[] = {*stdin, *stdout, *stderr};
extern "C" FILE * __cdecl __iob_func() {
  return _iob;
}

int main(int argc, char* argv[]) {
  srand(static_cast<unsigned int>(time(NULL)));
  Game game;
  game.Play();
  return 0;
}

void Game::Play() {
  // Initialization.
  if (graphic_.Initialize() == kError)
    return;
  BestScores best_scores;
  file_handler_.LoadBestScores(&best_scores);

  while (PlayTillGameOver(&best_scores) == kSuccess) {}

  // Termination.
  file_handler_.SaveBestScores(best_scores);
  graphic_.Terminate();
}

Status Game::PlayTillGameOver(BestScores* best_scores) {
  // Initialize information and display a title.
  Info info(kDefaultNumLives, kDefaultRemainingTime);
  if (graphic_.DisplayTitle(info) == kClosed)
    return kClosed;

  // Until a game is over.
  for (int i = 0; 1 <= info.num_lives(); ++i) {  // Each world.
    Status status = PlayInWorld(i, &info);
    if (status == kClosed) return kClosed;
    if (status == kError)  break;
  }

  // Display result.
  if (1 <= info.num_lives()) {
    best_scores->UpdateBestScores(info.score());
    if (graphic_.DisplayResult(info.score(), *best_scores) == kClosed)
      return kClosed;
  } else {
    if (graphic_.DisplayGameOver() == kClosed)
      return kClosed;
  }
  return kSuccess;
}

Status Game::PlayInWorld(int world_id, Info* info) {
  // Initialize a world.
  World* world = NULL;
  if (file_handler_.LoadWorld(world_id, &world) == kError)
    return kError;

  // Until zero life or the world is cleared.
  while (1 <= info->num_lives()) {  // Each life.
    int prev_num_lives = info->num_lives();
    if (PlayWithLife(world, info) == kClosed) {
      delete world;
      return kClosed;
    }

    bool is_world_over = (info->num_lives() == prev_num_lives);
    if (is_world_over)
      break;
  }

  // Player achieved a goal.
  info->ConvertTimeToScore();
  delete world;
  return kSuccess;
}

Status Game::PlayWithLife(World* world, Info* info) {
  world->Reset(info->player_type());
  info->ResetTime();

  // Display world information.
  if (graphic_.DisplayWorldInfo(world->id(), *info) == kClosed)
    return kClosed;

  // Play in the world.
  Command command;
  Camera camera(graphic_.width());
  g_frame_id = -1;
  while (true) {
    // Decrement time.
    if (++g_frame_id % kFPS == kFPS - 1) {
      if (info->DecrementTime())  // Time is up.
        break;
    }

    // Display game screen.
    if (graphic_.DisplayObjects(camera, *world, *info, kFPS) == kClosed)
      return kClosed;

    // Move characters.
    command.UpdateCommand();
    Player::Message message = world->PlayIn(command, &camera);
    if (message.achieved_goal)   return kSuccess;
    if (message.is_dead)         break;
    if (message.got_coin)        info->IncrementCoin();
    if (message.got_protein)     info->Strengthen();
    if (message.got_potion)      info->IncrementLife();
    if (message.got_drug)        info->AddScore(1000);
    if (message.stamped_monster) info->AddScore(100);
    if (message.is_attacked)     info->Weaken();
  }

  // Player is dead.
  if (graphic_.DisplayBlood() == kClosed)
    return kClosed;
  info->DecrementLife();
  info->Weaken();
  return kSuccess;
}