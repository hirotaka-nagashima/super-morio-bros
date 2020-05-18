//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef SUPER_MORIO_BROS_TYPE_H_
#define SUPER_MORIO_BROS_TYPE_H_

enum Type {
  // Blocks.
  kBeginBlocks = -1,
    kCrackedStone = 0,
    kBricks,
    kBox,
    kEmptyBox,
    kInvisible,
    kStone,
  kEndBlocks,

  // Moving objects.
  kBeginMovingObjects,
    kBeginItems,
      kCoin = 10,
      kProtein,  // Yellow.
      kPotion,   // Red.
      kDrug,     // Purple.
    kEndItems,
    kBeginPlayers,
      kNormalPlayer = 20,
      kFirePlayer = 40,
    kEndPlayers,
    kBeginMonsters,
      kMonster = 60,
      kTurtleMonster = 70,
    kEndMonsters,
  kEndMovingObjects,
};

static bool IsBlock(Type type) {
  return kBeginBlocks < type && type < kEndBlocks;
}
static bool IsMovingObject(Type type) {
  return kBeginMovingObjects < type && type < kEndMovingObjects;
}
static bool IsPlayer(Type type) {
  return kBeginPlayers < type && type < kEndPlayers;
}
static bool IsMonster(Type type) {
  return kBeginMonsters < type && type < kEndMonsters;
}
static bool IsItem(Type type) {
  return kBeginItems < type && type < kEndItems;
}

#endif  // SUPER_MORIO_BROS_TYPE_H_