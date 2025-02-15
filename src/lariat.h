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

  /**
   * @brief Constructs an empty Lariat
   */
  Lariat();

  /**
   * @brief Copy contructor for Lariat
   */
  Lariat(Lariat const &rhs);

  /**
   * @brief Copy assignment operator for Lariat
   */
  Lariat &operator=(const Lariat &rhs);

  ~Lariat();

  // Templatized Constructor and Assignment Operator

  /**
   * @brief Templatized copy contructor for Lariat
   */
  template<typename OtherT, int OtherSize>
  Lariat(const Lariat<OtherT, OtherSize> &rhs);

  /**
   * @brief Templatized copy assignment operator for Lariat
   */
  template<typename OtherT, int OtherSize>
  Lariat &operator=(const Lariat<OtherT, OtherSize> &rhs);

  // Deleting move constructor and assignment (Not specified in the handout)
  Lariat(Lariat &&rhs) = delete;
  Lariat &operator=(Lariat &&rhs) = delete;

  // Insertion Methods

  /**
   * @brief Insert a value of type T into the Lariat
   *
   * @param index Location to insert
   * @param value Value to insert
   */
  void insert(int index, const T &value);

  /**
   * @brief Insert a value of T at the end of the Lariat
   *
   * @param value Value to insert
   */
  void push_back(const T &value);

  /**
   * @brief Insert a value of T at the front of the Lariat
   *
   * @param value Value to insert
   */
  void push_front(const T &value);

  // Deletion Methods

  /**
   * @brief Erase the value at index
   *
   * @param index Index of value to delete
   */
  void erase(int index);

  /**
   * @brief Erase the last element in the Lariat
   */
  void pop_back();

  /**
   * @brief Erase the first element in the Lariat
   */
  void pop_front();

  // Access Methods

  /**
   * @brief Retrieves the element at index
   *
   * @param index The index of the element to retrieve
   * @return Reference to retrieved value
   */
  T &operator[](int index); // for l-values

  /**
   * @brief Retrieves the element at index with a const reference
   *
   * @param index The index of the element to retrieve
   * @return Const reference to retrieved value
   */
  const T &operator[](int index) const; // for r-values

  /**
   * @brief Retrieves the element at the front of the Lariat
   *
   * @return Reference to retrieved value
   */
  T &first();

  /**
   * @brief Retrieves the element at the front of the Lariat
   *
   * @return Const reference to retrieved value
   */
  T const &first() const;

  /**
   * @brief Retrieves the element at the end of the Lariat
   *
   * @return Reference to retrieved value
   */
  T &last();

  /**
   * @brief Retrieves the element at the end of the Lariat
   *
   * @return Const reference to retrieved value
   */
  T const &last() const;

  /**
   * @brief Retrieve the element where value T is
   *
   * @param value The value to find
   * @return index of the element
   */
  unsigned find(const T &value) const; // returns index, size (one past last) if not found

  friend std::ostream &operator<< <T, Size>(std::ostream &os, Lariat<T, Size> const &list);

  // Miscelaneous Methods

  /**
   * @brief Retrieve the element where value T is
   *
   * @return The amount of elements contained in the data structure
   */
  size_t size(void) const; // total number of items (not nodes)

  /**
   * @brief Clear the Lariat
   */
  void clear(void); // make it empty

  /**
   * @brief Removes all empty spaces in the data structure
   */
  void compact(); // push data in front reusing empty positions and delete remaining nodes

private:
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

  /**
   * @brief Splits the current node so that each node has an even amount of elements after inserting a new one.
   *
   * @param to_split The node to split
   * @return Pointer to the second half of the split
   */
  LNode *split(LNode &to_split);

  /**
   * @brief Finds the element at the given index
   *
   * @param index The index to look in
   * @return A struct containing the results of the search.
   */
  ElementSearch find_element(int index) const;

  /**
   * @brief Shifts all elements in the node up by 1 index.
   *
   * @param node The node to shift up in.
   * @param index The index to shift up from.
   */
  void shift_up(LNode *node, int index);

  /**
   * @brief Shifts all elements in the node down by 1 index.
   *
   * @param node The node to shift down in.
   * @param index The index to shift down from.
   */
  void shift_down(LNode *node, int index);

  /**
   * @brief Factory method for a node. This will throw an exception if it fails.
   */
  LNode *create_node() const;
};

#ifndef LARIAT_CPP
  #include "lariat.cpp"
#endif

#endif // LARIAT_H
