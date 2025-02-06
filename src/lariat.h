////////////////////////////////////////////////////////////////////////////////
#ifndef LARIAT_H
#define LARIAT_H
////////////////////////////////////////////////////////////////////////////////

#include <cstring> // memcpy
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

  template<int OtherSize>
  Lariat(const Lariat<T, OtherSize> &rhs);

  /// TODO: Check that this is the same signature as in the original
  template<int OtherSize>
  Lariat &operator=(const Lariat<T, OtherSize> &rhs);

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
  struct LNode { // DO NOT modify provided code
    LNode *next = nullptr;
    LNode *prev = nullptr;
    int count = 0; // number of items currently in the node
    T values[Size];
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
    size_t index{0};
  };

  // Helper Functions

  LNode *split(LNode &to_split);

  ElementSearch find_element(int index) const;

  void shift_up(LNode *node, int index);

  void shift_down(LNode *node, int index);
};

// TODO: Move everything below here into `lariat.cpp` and document it.

// Utility Implementation

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
Lariat<T, Size>::Lariat(const Lariat<T, Size> &other) {
  // TODO: Utilize push_back() to copy all the the elements into the copy correctly
}

template<typename T, int Size>
template<int OtherSize>
Lariat<T, Size>::Lariat(const Lariat<T, OtherSize> &other) {
  // TODO: Utilize push_back() to copy all the the elements into the copy correctly (templated)
}

template<typename T, int Size>
Lariat<T, Size> &Lariat<T, Size>::operator=(const Lariat &other) {
  // TODO: Cleanup data, then do the same functionality as the copy constructor
};

template<typename T, int Size>
template<int OtherSize>
Lariat<T, Size> &Lariat<T, Size>::operator=(const Lariat<T, OtherSize> &other) {
  // TODO: Cleanup data, then do the same functionality as the copy constructor (templated)
};

template<typename T, int Size>
Lariat<T, Size>::~Lariat() {
  // TODO: Do node cleanup
}

// Insertion Methods

template<typename T, int Size>
void Lariat<T, Size>::insert(int index, const T &value) {
  // TODO: Implement insert
}

template<typename T, int Size>
void Lariat<T, Size>::push_front(const T &value) {
  // TODO: Implement push front
}

template<typename T, int Size>
void Lariat<T, Size>::push_back(const T &value) {
  // TODO: Implement push back
}

// Deletion Methods

template<typename T, int Size>
void Lariat<T, Size>::erase(int index) {
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
T &Lariat<T, Size>::operator[](int index) {}

template<typename T, int Size>
const T &Lariat<T, Size>::operator[](int index) const {}

template<typename T, int Size>
T &Lariat<T, Size>::first() {}

template<typename T, int Size>
T const &Lariat<T, Size>::first() const {}

template<typename T, int Size>
T &Lariat<T, Size>::last() {}

template<typename T, int Size>
T const &Lariat<T, Size>::last() const {}

template<typename T, int Size>
unsigned Lariat<T, Size>::find(const T &value) const {}

// Miscelaneous Methods

template<typename T, int Size>
size_t Lariat<T, Size>::size() const {}

template<typename T, int Size>
void Lariat<T, Size>::clear() {}

template<typename T, int Size>
void Lariat<T, Size>::compact() {}

// Helper Functions

template<typename T, int Size>
typename Lariat<T, Size>::LNode *Lariat<T, Size>::split(Lariat<T, Size>::LNode &to_split) {}

template<typename T, int Size>
typename Lariat<T, Size>::ElementSearch Lariat<T, Size>::find_element(int index) const {}

template<typename T, int Size>
void Lariat<T, Size>::shift_up(Lariat<T, Size>::LNode *node, int index) {}

template<typename T, int Size>
void Lariat<T, Size>::shift_down(Lariat<T, Size>::LNode *node, int index) {}

#include "lariat.cpp"

#endif // LARIAT_H
