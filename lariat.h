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

  void shift_down(LNode *node, int index);

  LNode *create_node() const;
};

#ifndef LARIAT_CPP
  #include "lariat.cpp"
#endif

#endif // LARIAT_H
