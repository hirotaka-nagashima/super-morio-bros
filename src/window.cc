//-----------------------------------------------------------------------------
// Copyright (c) 2013 Hirotaka Nagashima. All rights reserved.
//-----------------------------------------------------------------------------

#define _CRT_SECURE_NO_WARNINGS
#include "window.h"
#include <cstdio>   // fprintf()
#include "point.h"
#include "status.h"

Status Window::Initialize() {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    fprintf(stderr, "ERROR: %s\n", SDL_GetError());
    return kError;
  }
  if (TTF_Init() < 0) {
    fprintf(stderr, "ERROR: %s\n", TTF_GetError());
    return kError;
  }
  if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    fprintf(stderr, "ERROR: %s\n", Mix_GetError());
    return kError;
  }

  window_ = SDL_CreateWindow(kTitle.c_str(), SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, kWidth, kHeight,
                             SDL_WINDOW_SHOWN);
  if (window_ == NULL) {
    fprintf(stderr, "ERROR: %s\n", SDL_GetError());
    return kError;
  }
  if (!kIconUrl.empty()) {
    SDL_Surface* icon = IMG_Load(kIconUrl.c_str());
    if (icon) {
      SDL_SetWindowIcon(window_, icon);
      SDL_FreeSurface(icon);
    }
  }
  video_surface_ = SDL_GetWindowSurface(window_);
  if (video_surface_ == NULL) {
    fprintf(stderr, "ERROR: %s\n", SDL_GetError());
    return kError;
  }
  return kSuccess;
}

void Window::Terminate() {
  if (window_) {
    SDL_DestroyWindow(window_);
    window_ = NULL;
    video_surface_ = NULL;
  }
  Mix_CloseAudio();
  TTF_Quit();
  SDL_Quit();
}

SDL_Surface* Window::LoadOptimizedImage(const char* url) const {
  SDL_Surface* original = IMG_Load(url);
  if (!original) return NULL;
  SDL_Surface* optimized =
      SDL_ConvertSurfaceFormat(original, SDL_PIXELFORMAT_RGBA8888, 0);
  SDL_FreeSurface(original);
  return optimized;
}

void Window::ClearScreen() {
  // Fill with a black plane.
  ClearScreenWith(0x00, 0x00, 0x00);
}

void Window::ClearScreenWith(Uint8 r, Uint8 g, Uint8 b) {
  SDL_FillRect(video_surface_, NULL,
               SDL_MapRGB(video_surface_->format, r, g, b));
}

void Window::DrawSingleImage(SDL_Surface* image, int dest_x, int dest_y) {
  SDL_Rect src, dest;
  src.y = src.x = 0;
  src.w = image->w;
  src.h = image->h;
  dest.x = dest_x;
  dest.y = dest_y;
  SDL_BlitSurface(image, &src, video_surface_, &dest);
}

void Window::DrawImage(SDL_Surface* image, int dest_x, int dest_y,
                       int id, int width, int height) {
  SDL_Rect src, dest;
  src.x = (id % (image->w / width)) * width;
  src.y = (id / (image->w / width)) * height;
  src.w = width;
  src.h = height;
  dest.x = dest_x;
  dest.y = dest_y;
  SDL_BlitSurface(image, &src, video_surface_, &dest);
}

void Window::DrawString(const char* text, int dest_x, int dest_y,
                        TTF_Font* font, const SDL_Color& color) {
  SDL_Surface* temp_text = TTF_RenderUTF8_Blended(font, text, color);
  SDL_Rect src, dest;
  src.x = 0;
  src.y = 0;
  src.w = temp_text->w;
  src.h = temp_text->h;
  dest.x = dest_x;
  dest.y = dest_y;
  SDL_BlitSurface(temp_text, &src, video_surface_, &dest);
  SDL_FreeSurface(temp_text);
}

void Window::DrawStringCenter(const char* text, int dest_y,
                              TTF_Font* font, const SDL_Color& color) {
  SDL_Surface* temp_text = TTF_RenderUTF8_Blended(font, text, color);
  SDL_Rect src, dest;
  src.x = 0;
  src.y = 0;
  src.w = temp_text->w;
  src.h = temp_text->h;
  dest.x = (kWidth - temp_text->w) / 2;
  dest.y = dest_y;
  SDL_BlitSurface(temp_text, &src, video_surface_, &dest);
  SDL_FreeSurface(temp_text);
}

void Window::Display() {
  SDL_UpdateWindowSurface(window_);
}

void Window::PlayBGM(Mix_Music* bgm) {
  bool is_music_playing = (Mix_PlayingMusic() == 1);
  if (!is_music_playing)
    Mix_PlayMusic(bgm, -1);
}

void Window::PlaySEOnce(Mix_Chunk* se) {
  Mix_PlayChannel(-1, se, 0);
}

Status Window::Sleep(int msec) const {
  // Divide duration by 5 for lighter processing.
  static const int kDt = 5;
  for (int i = 0; i < msec / kDt; ++i) {
    SDL_Delay(kDt);
    if (CheckClose() == kClosed)
      return kClosed;
  }
  return kSuccess;
}

Status Window::CheckClose() const {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT)
      return kClosed;
  }
  return kSuccess;
}

Status Window::WaitClick(bool distinguish_mouse_down, Point* cursor) {
  // Get the current state of the mouse button.
  while (true) {
    SDL_Event event;
    SDL_WaitEvent(&event);
    if (event.type == SDL_QUIT)
      return kClosed;
    if ((event.type == SDL_MOUSEBUTTONDOWN && distinguish_mouse_down) ||
        event.type == SDL_MOUSEBUTTONUP) {
      // Return clicked coordinates.
      cursor->x = event.button.x;
      cursor->y = event.button.y;
      break;
    }
  }
  return kSuccess;
}

Status Window::WaitEnterKey() const {
  while (true) {
    static const int kCheckDuration = 50;
    if (Sleep(kCheckDuration) == kClosed)
      return kClosed;
    const Uint8* key = SDL_GetKeyboardState(NULL);
    if (key[SDL_SCANCODE_RETURN] || key[SDL_SCANCODE_KP_ENTER])
      break;
  }
  return kSuccess;
}