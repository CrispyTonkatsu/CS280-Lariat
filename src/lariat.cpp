#include <iostream>
#include <ostream>
#include <utility>

#define LARIAT_CPP

#ifndef LARIAT_H
  #include "lariat.h"
#endif

// Utility Implementation

/**
 * @brief A replacement of the std::swap function
 *
 * @param a The value to swap with b
 * @param b The value to swap with a
 */
template<typename T>
void swap(T &a, T &b) {
  T temp = std::move(a);
  a = std::move(b);
  b = std::move(temp);
}

// Constructors + Destructor

/**
 * @brief Constructs an empty Lariat
 */
template<typename T, int Size>
Lariat<T, Size>::Lariat() : head_(nullptr), tail_(nullptr), size_(0), nodecount_(0), asize_(0) {}

/**
 * @brief Copy contructor for Lariat
 */
template<typename T, int Size>
Lariat<T, Size>::Lariat(const Lariat &other) : head_(nullptr), tail_(nullptr), size_(0), nodecount_(0) {
  clear();

  for (int i = 0; i < other.size_; i++) {
    push_back(other[i]);
  }
}

/**
 * @brief Templatized copy contructor for Lariat
 */
template<typename T, int Size>
template<typename OtherT, int OtherSize>
Lariat<T, Size>::Lariat(const Lariat<OtherT, OtherSize> &other) :
    head_(nullptr), tail_(nullptr), size_(0), nodecount_(0) {
  clear();

  for (int i = 0; i < other.size_; i++) {
    push_back(static_cast<T>(other[i]));
  }
}

/**
 * @brief Copy assignment operator for Lariat
 */
template<typename T, int Size>
Lariat<T, Size> &Lariat<T, Size>::operator=(const Lariat &other) {
  clear();

  for (int i = 0; i < other.size_; i++) {
    push_back(other[i]);
  }
  return *this;
}

/**
 * @brief Templatized copy assignment operator for Lariat
 */
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

/**
 * @brief Insert a value of type T into the Lariat
 *
 * @param index Location to insert
 * @param value Value to insert
 */
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

  LNode *node = search.node;

  shift_up(node, search.index);

  if (node->count < Size) {
    node->values[search.index] = value;
    ++node->count;

  } else {
    T overflow = node->values[search.index];
    node->values[search.index] = value;

    LNode *new_half = split(*node);
    if (node == tail_) {
      tail_ = new_half;
    }

    new_half->values[new_half->count] = overflow;
    ++new_half->count;
  }

  size_++;
}

/**
 * @brief Insert a value of T at the front of the Lariat
 *
 * @param value Value to insert
 */
template<typename T, int Size>
void Lariat<T, Size>::push_front(const T &value) {
  if (head_ == nullptr) {
    head_ = create_node();
    head_->values[0] = value;
    ++head_->count;

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

/**
 * @brief Insert a value of T at the end of the Lariat
 *
 * @param value Value to insert
 */
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

/**
 * @brief Erase the value at index
 *
 * @param index Index of value to delete
 */
template<typename T, int Size>
void Lariat<T, Size>::erase(int index) {
  if (size_ == 0) {
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

/**
 * @brief Erase the first element in the Lariat
 */
template<typename T, int Size>
void Lariat<T, Size>::pop_front() {
  if (size_ == 0) {
    throw LariatException(LariatException::E_DATA_ERROR, "Cannot delete in an empty Lariat");
  }

  shift_down(head_, 0);
  --head_->count;
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

/**
 * @brief Erase the last element in the Lariat
 */
template<typename T, int Size>
void Lariat<T, Size>::pop_back() {
  if (size_ == 0) {
    throw LariatException(LariatException::E_DATA_ERROR, "Cannot delete in an empty Lariat");
  }

  --tail_->count;
  size_--;

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

/**
 * @brief Retrieves the element at index
 *
 * @param index The index of the element to retrieve
 * @return Reference to retrieved value
 */
template<typename T, int Size>
T &Lariat<T, Size>::operator[](int index) {
  ElementSearch search = find_element(index);

  return search.node->values[search.index];
}

/**
 * @brief Retrieves the element at index with a const reference
 *
 * @param index The index of the element to retrieve
 * @return Const reference to retrieved value
 */
template<typename T, int Size>
const T &Lariat<T, Size>::operator[](int index) const {
  ElementSearch search = find_element(index);

  return search.node->values[search.index];
}

/**
 * @brief Retrieves the element at the front of the Lariat
 *
 * @return Reference to retrieved value
 */
template<typename T, int Size>
T &Lariat<T, Size>::first() {
  if (head_ == nullptr) {
    throw LariatException(LariatException::E_BAD_INDEX, "Empty lariat, cannot access first element");
  }

  return head_->values[0];
}

/**
 * @brief Retrieves the element at the front of the Lariat
 *
 * @return Const reference to retrieved value
 */
template<typename T, int Size>
T const &Lariat<T, Size>::first() const {
  if (head_ == nullptr) {
    throw LariatException(LariatException::E_BAD_INDEX, "Empty lariat, cannot access first element");
  }

  return head_->values[0];
}

/**
 * @brief Retrieves the element at the end of the Lariat
 *
 * @return Reference to retrieved value
 */
template<typename T, int Size>
T &Lariat<T, Size>::last() {
  if (tail_ == nullptr) {
    throw LariatException(LariatException::E_BAD_INDEX, "Empty lariat, cannot access last element");
  }

  return tail_->values[tail_->count - 1];
}

/**
 * @brief Retrieves the element at the end of the Lariat
 *
 * @return Const reference to retrieved value
 */
template<typename T, int Size>
T const &Lariat<T, Size>::last() const {
  if (tail_ == nullptr) {
    throw LariatException(LariatException::E_BAD_INDEX, "Empty lariat, cannot access last element");
  }

  return tail_->values[tail_->count - 1];
}

/**
 * @brief Retrieve the element where value T is
 *
 * @param value The value to find
 * @return index of the element
 */
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

/**
 * @brief Retrieve the element where value T is
 *
 * @return The amount of elements contained in the data structure
 */
template<typename T, int Size>
size_t Lariat<T, Size>::size() const {
  return size_;
}

/**
 * @brief Clear the Lariat
 */
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

/**
 * @brief Removes all empty spaces in the data structure
 */
template<typename T, int Size>
void Lariat<T, Size>::compact() {
  for (LNode *current = head_; current != nullptr; current = current->next) {
    for (LNode *inner = current->next; inner != nullptr; inner = inner->next) {
      if (inner->count > 0) {

        while (current->count != Size) {

          current->values[current->count] = inner->values[0];
          ++current->count;

          shift_down(inner, 0);

          --inner->count;
          if (inner->count == 0) {
            break;
          }
        }
      }
    }
  }

  LNode *current = tail_;
  while (current != nullptr && current->count == 0) {
    LNode *to_delete = current;

    tail_ = current->prev;
    current = current->prev;
    delete to_delete;

    tail_->next = nullptr;
    nodecount_--;
  }
}

// Helper Functions

/**
 * @brief Splits the current node so that each node has an even amount of elements after inserting a new one.
 *
 * @param to_split The node to split
 * @return Pointer to the second half of the split
 */
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
    ++second_half->count;
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

/**
 * @brief Finds the element at the given index
 *
 * @param index The index to look in
 * @return A struct containing the results of the search.
 */
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

/**
 * @brief Shifts all elements in the node up by 1 index.
 *
 * @param node The node to shift up in.
 * @param index The index to shift up from.
 */
template<typename T, int Size>
void Lariat<T, Size>::shift_up(LNode *node, int index) {
  if (node == nullptr) {
    return;
  }

  for (int i = index; i + 1 < Size; i++) {
    swap(node->values[index], node->values[i + 1]);
  }
}

/**
 * @brief Shifts all elements in the node down by 1 index.
 *
 * @param node The node to shift down in.
 * @param index The index to shift down from.
 */
template<typename T, int Size>
void Lariat<T, Size>::shift_down(LNode *node, const int index) {
  if (node == nullptr) {
    return;
  }

  for (int i = index; i + 1 < Size; i++) {
    swap(node->values[i], node->values[i + 1]);
  }
}

/**
 * @brief Factory method for a node. This will throw an exception if it fails.
 */
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
