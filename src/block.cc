//-----------------------------------------------------------------------------
// Copyright (c) 2013 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#include "block.h"
#include "graphic.h"
#include "item.h"
#include "camera.h"

void Block::Draw(Graphic* graphic, const Camera& camera) const {
  graphic->DrawObject(type(), camera.dp(position()), kWidth, kHeight);
}

void Block::Reset() {
  type_ = kDefaultType;
  for (auto item : items_)
    item->Reset();
}

void Block::PushOutItem() {
  // Push out an item once.
  for (int i = 0; i < static_cast<int>(items_.size()); ++i) {
    if (items_[i]->state() != Item::kStored)
      continue;

    items_[i]->set_state(Item::kPushedOut);
    items_[i]->appear();

    bool was_last_item = (i + 1 == static_cast<int>(items_.size()));
    if (was_last_item)
      type_ = kEmptyBox;

    break;
  }
}