////////////////////////////////////////////////////////////////////////////////
#ifndef LARIAT_H
#define LARIAT_H
////////////////////////////////////////////////////////////////////////////////

#include <cstring> // memcpy
#include <iostream>
#include <new>
#include <ostream>
#include <string> // error strings
#include <utility> // error strings

class LariatException : public std::exception {
private:
  int m_ErrCode;
  std::string m_Description;

public:
  LariatException(int ErrCode, const std::string &Description) : m_ErrCode(ErrCode), m_Description(Description) {}

  virtual int code(void) const { return m_ErrCode; }

  virtual const char *what(void) const throw() { return m_Description.c_str(); }

  virtual ~LariatException() throw() {}

  enum LARIAT_EXCEPTION { E_NO_MEMORY, E_BAD_INDEX, E_DATA_ERROR };
};

// forward declaration for 1-1 operator<<
template<typename T, int Size>
class Lariat;

template<typename T, int Size>
std::ostream &operator<<(std::ostream &os, Lariat<T, Size> const &rhs);

template<typename T, int Size>
class Lariat {
public:
  // Friending other instantiations of Lariat

  template<typename S, int OtherSize>
  friend class Lariat;

  // Constructors + Destructor

  Lariat();

  Lariat(Lariat const &rhs);

  /// TODO: Check that this is the same signature as in the original
  Lariat &operator=(const Lariat &rhs);

  ~Lariat();

  // Templatized Constructor and Assignment Operator

  template<typename OtherT, int OtherSize>
  Lariat(const Lariat<OtherT, OtherSize> &rhs);

  /// TODO: Check that this is the same signature as in the original
  template<typename OtherT, int OtherSize>
  Lariat &operator=(const Lariat<OtherT, OtherSize> &rhs);

  // Deleting move constructor and assignment (Not specified in the handout)
  Lariat(Lariat &&rhs) = delete;

  Lariat &operator=(Lariat &&rhs) = delete;

  // Insertion Methods

  void insert(int index, const T &value);

  void push_back(const T &value);

  void push_front(const T &value);

  // Deletion Methods

  void erase(int index);

  void pop_back();

  void pop_front();

  // Access Methods

  T &operator[](int index); // for l-values

  const T &operator[](int index) const; // for r-values

  T &first();

  T const &first() const;

  T &last();

  T const &last() const;

  unsigned find(const T &value) const; // returns index, size (one past last) if not found

  friend std::ostream &operator<< <T, Size>(std::ostream &os, Lariat<T, Size> const &list);

  // Miscelaneous Methods

  size_t size(void) const; // total number of items (not nodes)

  void clear(void); // make it empty

  void compact(); // push data in front reusing empty positions and delete remaining nodes

private:
  // TODO: Make sure that all this stuff is NOT modified

  struct LNode { // DO NOT modify provided code
    LNode *next{nullptr};
    LNode *prev{nullptr};
    int count{0}; // number of items currently in the node
    T values[Size]{};
  };

  // DO NOT modify provided code
  LNode *head_; // points to the first node
  LNode *tail_; // points to the last node
  int size_; // the number of items (not nodes) in the list
  mutable int nodecount_; // the number of nodes in the list
  int asize_; // the size of the array within the nodes

  // Helper Struct

  struct ElementSearch {
    LNode *node{nullptr};
    int index{0};
  };

  // Helper Functions

  LNode *split(LNode &to_split);

  ElementSearch find_element(int index) const;

  void shift_up(LNode *node, int index);

  void shift_down(LNode *node);

  LNode *create_node() const;
};

// TODO: Move everything below here into `lariat.cpp` and document it.
// TODO: Throw Exceptions for the required things.

// Utility Implementation

// TODO: Consider utilizing move here.
template<typename T>
void swap(T &a, T &b) {
  T temp = a;
  a = b;
  b = temp;
}

// Constructors + Destructor

template<typename T, int Size>
Lariat<T, Size>::Lariat() : head_(nullptr), tail_(nullptr), size_(0), nodecount_(0), asize_(0) {}

template<typename T, int Size>
Lariat<T, Size>::Lariat(const Lariat<T, Size> &) {
  // TODO: Utilize push_back() to copy all the the elements into the copy correctly
  head_ = nullptr;
  size_ = 0;
}

template<typename T, int Size>
template<typename OtherT, int OtherSize>
Lariat<T, Size>::Lariat(const Lariat<OtherT, OtherSize> &) {
  // TODO: Utilize push_back() to copy all the the elements into the copy correctly (templated)
  head_ = nullptr;
  size_ = 0;
}

template<typename T, int Size>
Lariat<T, Size> &Lariat<T, Size>::operator=(const Lariat &) {
  clear();
  // TODO: Cleanup data, then do the same functionality as the copy constructor
  return *this;
};

template<typename T, int Size>
template<typename OtherT, int OtherSize>
Lariat<T, Size> &Lariat<T, Size>::operator=(const Lariat<OtherT, OtherSize> &) {
  // TODO: Cleanup data, then do the same functionality as the copy constructor (templated)
  return *this;
};

template<typename T, int Size>
Lariat<T, Size>::~Lariat() {
  clear();
}

// Insertion Methods

template<typename T, int Size>
void Lariat<T, Size>::insert(int index, const T &value) {
  if (index < 0 || index >= size_ + 1) {
    throw LariatException(LariatException::E_BAD_INDEX, "Insert Error: Invalid index for insertion.");
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

  } else {
    T overflow = node->values[search.index];
    node->values[search.index] = value;

    LNode *new_half = split(*node);
    new_half->values[new_half->count] = overflow;
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

  shift_up(head_, 0);

  if (node->count < Size) {
    node->values[0] = value;

  } else {
    T overflow = node->values[0];
    node->values[0] = value;

    LNode *new_half = split(*node);
    new_half->values[new_half->count] = overflow;

    if (nodecount_ == 1) {
      tail_ = new_half;
    }
  }

  node->count++;
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
void Lariat<T, Size>::erase(int) {
  // TODO: Implement pop front
}

template<typename T, int Size>
void Lariat<T, Size>::pop_front() {
  // TODO: Implement pop front
}

template<typename T, int Size>
void Lariat<T, Size>::pop_back() {
  // TODO: Implement pop back
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

    current = current->next;
    delete to_delete;

    nodecount_--;
  }
}

template<typename T, int Size>
void Lariat<T, Size>::compact() {}

// Helper Functions

template<typename T, int Size>
typename Lariat<T, Size>::LNode *Lariat<T, Size>::split(LNode &to_split) {
  LNode *second_half = create_node();

  int split_point = (to_split.count / 2) + 1;
  for (int i = split_point; i < to_split.count; i++) {
    second_half->values[i - split_point] = to_split.values[i];
    second_half->count++;
  }
  to_split.count = split_point;

  second_half->next = to_split.next;
  second_half->prev = &to_split;

  to_split.next = second_half;
  if (to_split.next != nullptr) {
    to_split.next->prev = second_half;
  }

  nodecount_++;
  return second_half;
}

template<typename T, int Size>
typename Lariat<T, Size>::ElementSearch Lariat<T, Size>::find_element(int index) const {
  // TODO: Add bound checking here

  int traversed_indexes = 0;
  for (LNode *current = head_; current != nullptr; current = current->next) {

    if (traversed_indexes + current->count >= index) {
      return ElementSearch{current, index - traversed_indexes};
    }

    traversed_indexes += current->count;
  }

  return {nullptr, 0};
}

// HACK: The index provided will hold the overflow value
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
void Lariat<T, Size>::shift_down(Lariat<T, Size>::LNode *node) {
  if (node == nullptr) {
    return;
  }

  for (int i = 0; i + 1 < Size; i++) {
    swap(node->values[i], node->values[i + 1]);
  }
}

template<typename T, int Size>
typename Lariat<T, Size>::LNode *Lariat<T, Size>::create_node() const {
  LNode *output = nullptr;
  try {
    output = new LNode;
    output->prev = nullptr;
    output->next = nullptr;
    output->count = 0;

  } catch (const std::bad_alloc &) {
    throw LariatException(LariatException::E_NO_MEMORY, "Unable to allocate a new node. Check if memory is leaking.");
  }

  return output;
}

#include "lariat.cpp"

#endif // LARIAT_H
