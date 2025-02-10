#include <iostream>
#include <ostream>
#include <utility>

#define LARIAT_CPP

#ifndef LARIAT_H
  #include "lariat.h"
#endif

// NOTE: Remaining Tests = Probably related issues so first deal with the memory leak.
// - 24 (The operations are not consistent)
// - 26 (There is 1 node being lost made from a call to split)

// TODO: Document the code.
// TODO: Throw Exceptions for the required things.

// Utility Implementation

template<typename T>
void swap(T &a, T &b) {
  T temp = std::move(a);
  a = std::move(b);
  b = std::move(temp);
}

// Constructors + Destructor

template<typename T, int Size>
Lariat<T, Size>::Lariat() : head_(nullptr), tail_(nullptr), size_(0), nodecount_(0), asize_(0) {}

template<typename T, int Size>
Lariat<T, Size>::Lariat(const Lariat<T, Size> &other) : head_(nullptr), tail_(nullptr), size_(0), nodecount_(0) {
  clear();

  for (int i = 0; i < other.size_; i++) {
    push_back(other[i]);
  }
}

template<typename T, int Size>
template<typename OtherT, int OtherSize>
Lariat<T, Size>::Lariat(const Lariat<OtherT, OtherSize> &other) :
    head_(nullptr), tail_(nullptr), size_(0), nodecount_(0) {
  clear();

  for (int i = 0; i < other.size_; i++) {
    push_back(static_cast<T>(other[i]));
  }
}

template<typename T, int Size>
Lariat<T, Size> &Lariat<T, Size>::operator=(const Lariat &other) {
  clear();

  for (int i = 0; i < other.size_; i++) {
    push_back(other[i]);
  }
  return *this;
}

template<typename T, int Size>
template<typename OtherT, int OtherSize>
Lariat<T, Size> &Lariat<T, Size>::operator=(const Lariat<OtherT, OtherSize> &other) {
  clear();

  for (int i = 0; i < other.size_; i++) {
    push_back(static_cast<T>(other[i]));
  }
  return *this;
}

template<typename T, int Size>
Lariat<T, Size>::~Lariat() {
  clear();
}

// Insertion Methods

template<typename T, int Size>
void Lariat<T, Size>::insert(int index, const T &value) {
  if (index < 0 || index > size_) {
    throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
  }

  if (index == 0) {
    push_front(value);
    return;
  }

  if (index == size_) {
    push_back(value);
    return;
  }

  ElementSearch search = find_element(index);
  if (search.node == nullptr) {
    // TODO: Throw an exception or something
  }

  LNode *node = search.node;

  shift_up(node, search.index);

  if (node->count < Size) {
    node->values[search.index] = value;
    node->count++;

  } else {
    T overflow = node->values[search.index];
    node->values[search.index] = value;

    LNode *new_half = split(*node);
    new_half->values[new_half->count] = overflow;
    new_half->count++;
  }

  size_++;
}

template<typename T, int Size>
void Lariat<T, Size>::push_front(const T &value) {
  if (head_ == nullptr) {
    head_ = create_node();
    head_->values[0] = value;
    head_->count++;

    tail_ = head_;
    nodecount_++;
    size_++;
    return;
  }

  LNode *node = head_;
  shift_up(node, 0);

  if (node->count < Size) {
    node->values[0] = value;
    node->count++;

  } else {
    T overflow = node->values[0];
    node->values[0] = value;

    LNode *new_half = split(*node);
    new_half->values[new_half->count] = overflow;
    new_half->count++;

    if (head_ == tail_) {
      tail_ = new_half;
    }
  }

  size_++;
}

template<typename T, int Size>
void Lariat<T, Size>::push_back(const T &value) {
  if (tail_ == nullptr) {
    head_ = create_node();
    head_->values[0] = value;
    head_->count++;

    tail_ = head_;
    nodecount_++;
    size_++;
    return;
  }

  if (tail_->count == Size) {
    tail_ = split(*tail_);
  }

  tail_->values[tail_->count] = value;
  tail_->count++;

  size_++;
}

// Deletion Methods

template<typename T, int Size>
void Lariat<T, Size>::erase(int index) {
  if (size_ == 0) {
    // TODO: Check that this is the right Error Code
    throw LariatException(LariatException::E_DATA_ERROR, "Cannot delete in an empty Lariat");
  }

  if (index < 0 || index >= size_) {
    throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
  }

  if (index == 0) {
    pop_front();
    return;
  }

  if (index == size_ - 1) {
    pop_back();
    return;
  }

  ElementSearch search = find_element(index);
  if (search.node == nullptr) {
    // TODO: Throw an exception or sth
  }

  shift_down(search.node, search.index);
  search.node->count--;
  size_--;

  if (search.node->count == 0) {
    search.node->prev->next = search.node->next;
    search.node->next->prev = search.node->prev;

    delete search.node;
    nodecount_--;
  }
}

template<typename T, int Size>
void Lariat<T, Size>::pop_front() {
  if (size_ == 0) {
    // TODO: Check that this is the right Error Code
    throw LariatException(LariatException::E_DATA_ERROR, "Cannot delete in an empty Lariat");
  }

  shift_down(head_, 0);
  head_->count--;
  size_--;

  if (head_->count == 0) {
    LNode *new_head = head_->next;

    delete head_;
    nodecount_--;

    if (tail_ == head_) {
      tail_ = new_head;
    }
    head_ = new_head;

    if (head_ != nullptr) {
      head_->prev = nullptr;
    }
  }
}

template<typename T, int Size>
void Lariat<T, Size>::pop_back() {
  if (size_ == 0) {
    // TODO: Check that this is the right Error Code
    throw LariatException(LariatException::E_DATA_ERROR, "Cannot delete in an empty Lariat");
  }

  tail_->count--;
  size_--;

  // TODO: Check what would be the faster way to do this
  if (tail_->count == 0) {
    LNode *new_tail = tail_->prev;

    delete tail_;
    nodecount_--;

    if (tail_ == head_) {
      head_ = new_tail;
    }
    tail_ = new_tail;

    if (tail_ != nullptr) {
      tail_->next = nullptr;
    }
  }
}

// Access Methods

template<typename T, int Size>
T &Lariat<T, Size>::operator[](int index) {
  ElementSearch search = find_element(index);
  if (search.node == nullptr) {
    // TODO: Throw exception or sth
  }

  return search.node->values[search.index];
}

template<typename T, int Size>
const T &Lariat<T, Size>::operator[](int index) const {
  ElementSearch search = find_element(index);
  if (search.node == nullptr) {
    // TODO: Throw exception or sth
  }

  return search.node->values[search.index];
}

template<typename T, int Size>
T &Lariat<T, Size>::first() {
  if (head_ == nullptr) {
    // TODO: Throw exception or sth
  }

  return head_->values[0];
}

template<typename T, int Size>
T const &Lariat<T, Size>::first() const {
  if (head_ == nullptr) {
    // TODO: Throw exception or sth
  }

  return head_->values[0];
}

template<typename T, int Size>
T &Lariat<T, Size>::last() {
  if (tail_ == nullptr) {
    // TODO: Throw exception or sth
  }

  return tail_->values[tail_->count - 1];
}

template<typename T, int Size>
T const &Lariat<T, Size>::last() const {
  if (tail_ == nullptr) {
    // TODO: Throw exception or sth
  }

  return tail_->values[tail_->count - 1];
}

template<typename T, int Size>
unsigned Lariat<T, Size>::find(const T &value) const {

  unsigned stepped_indexes = 0;
  for (LNode *current = head_; current != nullptr; current = current->next) {

    for (int i = 0; i < current->count; i++) {
      if (current->values[i] == value) {
        return stepped_indexes + static_cast<unsigned>(i);
      }
    }

    stepped_indexes += current->count;
  }

  return static_cast<unsigned>(size_);
}

// Miscelaneous Methods

template<typename T, int Size>
size_t Lariat<T, Size>::size() const {
  return size_;
}

template<typename T, int Size>
void Lariat<T, Size>::clear() {
  LNode *current = head_;
  while (current != nullptr) {
    LNode *to_delete = current;

    size_ -= current->count;
    current = current->next;
    delete to_delete;

    nodecount_--;
  }

  head_ = nullptr;
  tail_ = nullptr;
}

template<typename T, int Size>
void Lariat<T, Size>::compact() {
  for (LNode *current = head_; current != nullptr; current = current->next) {
    for (LNode *inner = current->next; inner != nullptr; inner = inner->next) {
      if (inner->count > 0) {

        while (current->count != Size) {

          current->values[current->count] = inner->values[0];
          current->count++;

          shift_down(inner, 0);

          inner->count--;
          if (inner->count == 0) {
            break;
          }
        }
      }
    }
  }

  LNode *current = tail_;
  while (current != head_) {
    LNode *to_delete = current;

    tail_ = current->prev;
    current = current->prev;
    delete to_delete;

    tail_->next = nullptr;
    nodecount_--;
  }
}

// Helper Functions

template<typename T, int Size>
typename Lariat<T, Size>::LNode *Lariat<T, Size>::split(LNode &to_split) {
  LNode *second_half = create_node();

  // NOTE: The count to split for
  int expected_count = to_split.count + 1;

  // NOTE: Whether the split will be equal
  int extra_whole = (expected_count % 2);

  int split_point = (expected_count / 2) + extra_whole;

  for (int i = 0; i < split_point - 1 - extra_whole; i++) {
    second_half->values[i] = to_split.values[split_point + i];
    second_half->count++;
  }
  to_split.count = split_point;

  second_half->next = to_split.next;
  second_half->prev = &to_split;

  if (to_split.next != nullptr) {
    to_split.next->prev = second_half;
  }
  to_split.next = second_half;

  nodecount_++;
  return second_half;
}

template<typename T, int Size>
typename Lariat<T, Size>::ElementSearch Lariat<T, Size>::find_element(int index) const {
  if (index < 0 || index >= size_) {
    throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
  }

  int traversed_indexes = 0;
  for (LNode *current = head_; current != nullptr; current = current->next) {

    if (traversed_indexes + current->count > index) {
      return ElementSearch{current, index - traversed_indexes};
    }

    traversed_indexes += current->count;
  }

  return {nullptr, 0};
}

// NOTE: The index provided will hold the overflow value if there is one
template<typename T, int Size>
void Lariat<T, Size>::shift_up(Lariat<T, Size>::LNode *node, int index) {
  if (node == nullptr) {
    return;
  }

  for (int i = index; i + 1 < Size; i++) {
    swap(node->values[index], node->values[i + 1]);
  }
}

template<typename T, int Size>
void Lariat<T, Size>::shift_down(Lariat<T, Size>::LNode *node, int index) {
  if (node == nullptr) {
    return;
  }

  for (int i = index; i + 1 < Size; i++) {
    swap(node->values[i], node->values[i + 1]);
  }
}

template<typename T, int Size>
typename Lariat<T, Size>::LNode *Lariat<T, Size>::create_node() const {
  LNode *output = nullptr;
  try {
    output = new LNode;

  } catch (const std::bad_alloc &) {
    throw LariatException(LariatException::E_NO_MEMORY, "Unable to allocate a new node. Check if memory is leaking.");
  }

  return output;
}

#if 1
template<typename T, int Size>
std::ostream &operator<<(std::ostream &os, Lariat<T, Size> const &list) {
  typename Lariat<T, Size>::LNode *current = list.head_;
  int index = 0;
  while (current) {
    os << "Node starting (count " << current->count << ")\n";
    for (int local_index = 0; local_index < current->count; ++local_index) {
      os << index << " -> " << current->values[local_index] << std::endl;
      ++index;
    }
    os << "-----------\n";
    current = current->next;
  }
  return os;
}
#else // fancier

#endif
