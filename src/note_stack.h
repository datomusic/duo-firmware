// Copyright 2009 Olivier Gillet.
//
// Author: Olivier Gillet (ol.gillet@gmail.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------
//
// Stack of currently pressed keys.
//
// Currently pressed keys are stored as a linked list. The linked list is used
// as a LIFO stack to allow monosynth-like behaviour. An example of such
// behaviour is:
// player presses and holds C4-> C4 is played.
// player presses and holds C5 (while holding C4) -> C5 is played.
// player presses and holds G4 (while holding C4&C5)-> G4 is played.
// player releases C5 -> G4 is played.
// player releases G4 -> C4 is played.
//
// The nodes used in the linked list are pre-allocated from a pool of 16
// nodes, so the "pointers" (to the root element for example) are not actual
// pointers, but indices of an element in the pool.
//
// Additionally, an array of pointers is stored to allow random access to the
// n-th note, sorted by ascending order of pitch (for arpeggiation).
//
// TODO(pichenettes): having this class implemented as a "static singleton"
// saves almost 300 bytes of code. w00t! But we'd rather move this back to a
// simple class when adding multitimbrality. When will we add multitimbrality?
// Probably never.

#ifndef SHRUTHI_NOTE_STACK_H_
#define SHRUTHI_NOTE_STACK_H_

static const uint8_t kNoteStackSize = 8;

struct NoteEntry {
  uint8_t note;
  uint8_t velocity;
  uint8_t next_ptr;  // Base 1.
};

class NoteStack {
 public: 
  NoteStack() { }
  void Init() { Clear(); }

  void NoteOn(uint8_t note, uint8_t velocity);
  void NoteOff(uint8_t note);
  void Clear();

  uint8_t size() { return size_; }
  uint8_t max_size() { return kNoteStackSize; }
  const NoteEntry& most_recent_note() { return pool_[root_ptr_]; }
  const NoteEntry& least_recent_note() {
    uint8_t current = root_ptr_;
    while (current && pool_[current].next_ptr) {
      current = pool_[current].next_ptr;
    }
    return pool_[current];
  }
  const NoteEntry& sorted_note(uint8_t index) {
    return pool_[sorted_ptr_[index]];
  }
  const NoteEntry& played_note(uint8_t index) const {
    uint8_t current = root_ptr_;
    index = size_ - index - 1;
    for (uint8_t i = 0; i < index; ++i) {
      current = pool_[current].next_ptr;
    }
    return pool_[current];
  }
  const NoteEntry& note(uint8_t index) { return pool_[index]; }
  NoteEntry* mutable_note(uint8_t index) { return &pool_[index]; }
  const NoteEntry& dummy() { return pool_[0]; }

 private:
  uint8_t size_;
  NoteEntry pool_[kNoteStackSize + 1];  // First element is a dummy node!
  uint8_t root_ptr_;  // Base 1.
  uint8_t sorted_ptr_[kNoteStackSize + 1];  // Base 1.

};

static const uint8_t kFreeSlot = 0xff;

void NoteStack::NoteOn(uint8_t note, uint8_t velocity) {
  // Remove the note from the list first (in case it is already here).
  NoteOff(note);
  // In case of saturation, remove the least recently played note from the
  // stack.
  if (size_ == kNoteStackSize) {
    uint8_t least_recent_note;
    for (uint8_t i = 1; i <= kNoteStackSize; ++i) {
      if (pool_[i].next_ptr == 0) {
        least_recent_note = pool_[i].note;
      }
    }
    NoteOff(least_recent_note);
  }
  // Now we are ready to insert the new note. Find a free slot to insert it.
  uint8_t free_slot;
  for (uint8_t i = 1; i <= kNoteStackSize; ++i) {
    if (pool_[i].note == kFreeSlot) {
      free_slot = i;
    }
  }
  pool_[free_slot].next_ptr = root_ptr_;
  pool_[free_slot].note = note;
  pool_[free_slot].velocity = velocity;
  root_ptr_ = free_slot;
  // The last step consists in inserting the note in the sorted list.
  for (uint8_t i = 0; i < size_; ++i) {
    if (pool_[sorted_ptr_[i]].note > note) {
      for (uint8_t j = size_; j > i; --j) {
        sorted_ptr_[j] = sorted_ptr_[j - 1];
      }
      sorted_ptr_[i] = free_slot;
      free_slot = 0;
      break;
    }
  }
  if (free_slot) {
    sorted_ptr_[size_] = free_slot;
  }
  ++size_;
}

void NoteStack::NoteOff(uint8_t note) {
  uint8_t current = root_ptr_;
  uint8_t previous = 0;
  while (current) {
    if (pool_[current].note == note) {
      break;
    }
    previous = current;
    current = pool_[current].next_ptr;
  }
  if (current) {
    if (previous) {
      pool_[previous].next_ptr = pool_[current].next_ptr;
    } else {
      root_ptr_ = pool_[current].next_ptr;
    }
    for (uint8_t i = 0; i < size_; ++i) {
      if (sorted_ptr_[i] == current) {
        for (uint8_t j = i; j < size_ - 1; ++j) {
          sorted_ptr_[j] = sorted_ptr_[j + 1];
        }
        break;
      }
    }
    pool_[current].next_ptr = 0;
    pool_[current].note = kFreeSlot;
    pool_[current].velocity = 0;
    --size_;
  }
}

void NoteStack::Clear() {
  size_ = 0;
  memset(pool_ + 1, 0, sizeof(NoteEntry) * kNoteStackSize);
  memset(sorted_ptr_ + 1, 0, kNoteStackSize);
  root_ptr_ = 0;
  for (uint8_t i = 0; i <= kNoteStackSize; ++i) {
    pool_[i].note = kFreeSlot;
  }
}

#endif // SHRUTHI_NOTE_STACK_H_
