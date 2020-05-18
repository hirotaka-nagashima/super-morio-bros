//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#define _CRT_SECURE_NO_WARNINGS
#include "graphic.h"
#include "info.h"
#include "best_scores.h"
#include "world.h"
#include "camera.h"
#include "status.h"

Status Graphic::Initialize() {
  if (window_.Initialize() == kError)
    return kError;

  // Load fonts.
  static const int kFontSize = 16;
  font_ = TTF_OpenFont("resources/font.ttf", kFontSize);
  if (!font_) {
    fprintf(stderr, "ERROR: %s\n", TTF_GetError());
    return kError;
  }

  // Load musics.
  bgm_ = Mix_LoadMUS("resources/bgm.ogg");
  se_jump_ = Mix_LoadWAV("resources/se_jump.ogg");
  se_coin_ = Mix_LoadWAV("resources/se_coin.ogg");
  se_smash_ = Mix_LoadWAV("resources/se_smash.ogg");
  se_up_ = Mix_LoadWAV("resources/se_up.ogg");
  se_down_ = Mix_LoadWAV("resources/se_down.ogg");
  if (!bgm_ || !se_jump_ || !se_coin_ || !se_smash_ ||
      !se_up_ || !se_down_) {
    fprintf(stderr, "ERROR: %s\n", Mix_GetError());
    return kError;
  }

  // Load images.
  image_objects_ = window_.LoadOptimizedImage("resources/objects.png");
  image_infobar_ = window_.LoadOptimizedImage("resources/infobar.png");
  image_title_ = window_.LoadOptimizedImage("resources/title.png");
  image_blood_ = window_.LoadOptimizedImage("resources/blood.png");
  image_foreground_ = window_.LoadOptimizedImage("resources/foreground.png");
  if (!image_objects_ || !image_background_ || !image_infobar_ ||
      !image_title_ || !image_blood_ || !image_foreground_) {
    fprintf(stderr, "ERROR: %s\n", IMG_GetError());
    return kError;
  }

  // Load and optimize backgrounds.
  bool error = false;
  for (int i = 0; i < kNumLayersBackground; ++i) {
    char url[256];
    sprintf(url, "resources/background_%d.png", i);
    image_background_[i] = window_.LoadOptimizedImage(url);
    error |= !image_background_[i];
  }
  if (error) {
    fprintf(stderr, "ERROR: %s\n", IMG_GetError());
    return kError;
  }

  return kSuccess;
}

void Graphic::Terminate() {
  // Release fonts.
  TTF_CloseFont(font_); font_ = NULL;

  // Release musics.
  Mix_FreeMusic(bgm_);      bgm_ = NULL;
  Mix_FreeChunk(se_jump_);  se_jump_ = NULL;
  Mix_FreeChunk(se_coin_);  se_coin_ = NULL;
  Mix_FreeChunk(se_smash_); se_smash_ = NULL;
  Mix_FreeChunk(se_up_);    se_up_ = NULL;
  Mix_FreeChunk(se_down_);  se_down_ = NULL;

  // Release images.
  SDL_FreeSurface(image_objects_);    image_objects_ = NULL;
  SDL_FreeSurface(image_infobar_);    image_infobar_ = NULL;
  SDL_FreeSurface(image_title_);      image_title_ = NULL;
  SDL_FreeSurface(image_blood_);      image_blood_ = NULL;
  SDL_FreeSurface(image_foreground_); image_foreground_ = NULL;
  for (int i = 0; i < kNumLayersBackground; ++i) {
    SDL_FreeSurface(image_background_[i]); image_background_[i] = NULL;
  }

  window_.Terminate();
}

void Graphic::DrawObject(int image_id, const Point& dest,
                         int image_width, int image_height) {
  window_.DrawImage(image_objects_, dest.x, dest.y,
                    image_id, image_width, image_height);
}

Status Graphic::DisplayObjects(const Camera& camera, const World& world,
                               const Info& info, int fps) {
  window_.PlayBGM(bgm_);

  DrawBackground(camera);
  world.Draw(camera, this);
  DrawForeground(camera);

  DrawInfoBar(info);

  window_.Display();
  StabilizeFPS(fps);
  return window_.CheckClose();
}

Status Graphic::DisplayTitle(const Info& info) {
  Mix_HaltMusic();
  window_.DrawSingleImage(image_title_, 0, 0);

  // Draw "PRESS ENTER".
  static const char kMessage[] = "PRESS ENTER";
  int dest_y = height() * 2 / 3;
  window_.DrawStringCenter(kMessage, dest_y, font_);

  window_.Display();
  return window_.WaitEnterKey();
}

Status Graphic::DisplayWorldInfo(int world_id, const Info& info) {
  Mix_HaltMusic();
  window_.ClearScreen();

  // Draw a title.
  static const char kTitle[] = "WORLD";
  int dest_y = height() / 2 - 30;
  window_.DrawStringCenter(kTitle, dest_y, font_);

  // Draw a world id.
  static const int kMaxNumDigits = 10;
  char world_name[kMaxNumDigits + 1];
  sprintf(world_name, "%d", world_id + 1);
  dest_y = height() / 2 + 5;
  window_.DrawStringCenter(world_name, dest_y, font_);

  DrawInfoBar(info);

  window_.Display();
  return window_.Sleep(2500);
}

Status Graphic::DisplayBlood() {
  Mix_HaltMusic();
  PlaySESmash();
  window_.ClearScreen();

  static const int kNumTimesDisplay = 5;
  for (int i = 0; i < kNumTimesDisplay; ++i) {
    // Display blood once.
    static const int kMaxDuration = 100;  // [ms]
    int dest_x = rand() % width() - width() / 2;
    int dest_y = rand() % height() - height() / 2;
    window_.DrawSingleImage(image_blood_, dest_x, dest_y);
    window_.Display();
    if (window_.Sleep(rand() % kMaxDuration) == kClosed)
      return kClosed;
  }

  return window_.Sleep(500);
}

Status Graphic::DisplayGameOver() {
  Mix_HaltMusic();
  window_.ClearScreen();

  // Draw blood as a background.
  int dest_x = width() / 2 - image_blood_->w / 2;
  int dest_y = height() / 2 - image_blood_->h / 2;
  window_.DrawSingleImage(image_blood_, dest_x, dest_y);

  // Draw a message on blood.
  static const char kMessage[] = "GAME OVER";
  int center_y = height() / 2;
  window_.DrawStringCenter(kMessage, center_y, font_);

  window_.Display();
  return window_.Sleep(3000);
}

Status Graphic::DisplayResult(int score, const BestScores& best_scores) {
  Mix_HaltMusic();
  window_.ClearScreen();

  // Draw a title.
  static const char kTitle[] = "TOP SCORES";
  int dest_y = height() / 2 - 60;
  window_.DrawStringCenter(kTitle, dest_y, font_);

  // Draw the best scores.
  static const SDL_Color kYellow = {0xff, 0xff, 0x00};
  static const SDL_Color kWhite = {0xff, 0xff, 0xff};
  bool is_score_registered = false;
  for (int place = 1; place <= best_scores.num_best_scores(); ++place) {
    // String.
    int best_score = best_scores.best_score(place);
    char line[14];
    sprintf(line, "%d.%06d", place, best_score);
    // Destination.
    dest_y = height() / 2 - 30 + 18 * place;
    // Color.
    bool is_new_score = (best_score == score && !is_score_registered);
    SDL_Color color = is_new_score ? kYellow : kWhite;

    window_.DrawStringCenter(line, dest_y, font_, color);
    is_score_registered |= is_new_score;
  }

  // Draw a message.
  if (is_score_registered) {
    static const char kMessage[] = "CONGRATULATIONS!!";
    dest_y = height() / 2 - 110;
    window_.DrawStringCenter(kMessage, dest_y, font_);
  } else {
    // String.
    char line[14];
    sprintf(line, "  %06d", score);
    dest_y = height() / 2 + 70;
    window_.DrawStringCenter(line, dest_y, font_, kWhite);
  }

  window_.Display();
  return window_.Sleep(5000);
}

void Graphic::DrawInfoBar(const Info& info) {
  // Draw a background of the info bar.
  window_.DrawSingleImage(image_infobar_, 0, 0);

  static const int kMaxNumDigits = 10;
  char value[kMaxNumDigits + 1];
  sprintf(value, "%06d", info.score());
  window_.DrawString(value, 48, 48, font_);
  sprintf(value, "%02d", info.num_coins());
  window_.DrawString(value, 208, 48, font_);
  sprintf(value, "%02d", info.num_lives());
  window_.DrawString(value, 328, 48, font_);
  sprintf(value, "%03d", info.remaining_time());
  window_.DrawString(value, 416, 48, font_);
}

void Graphic::DrawForeground(const Camera& camera) {
  SDL_Surface* layer = image_foreground_;
  int dest_x = -((camera.x() * (kNumLayersBackground + 1) /
      kNumLayersBackground) % layer->w);
  int dest_y = -camera.y();
  window_.DrawSingleImage(layer, dest_x, dest_y);
  window_.DrawSingleImage(layer, dest_x + layer->w, dest_y);
}

void Graphic::DrawBackground(const Camera& camera) {
  for (int i = 0; i < kNumLayersBackground; ++i) {
    SDL_Surface* layer = image_background_[i];
    int dest_x = -((camera.x() * i / kNumLayersBackground) % layer->w);
    int dest_y = -camera.y();
    window_.DrawSingleImage(layer, dest_x, dest_y);
    if (0 < i)
      window_.DrawSingleImage(layer, dest_x + layer->w, dest_y);
  }
}

void Graphic::StabilizeFPS(int fps) const {
  static int prev_time = SDL_GetTicks();
  int duration = SDL_GetTicks() - prev_time;
  int given_duration = 1000 / fps;  // [ms]
  if (0 < given_duration - duration)
    SDL_Delay(given_duration - duration);
  prev_time = SDL_GetTicks();
}