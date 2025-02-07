#include <iostream>

template<typename T>
void swap(T &a, T &b) {
  T temp = a;
  a = b;
  b = temp;
}

int main() {
  char array[9]{
      'a',
      'b',
      'c',
      'd',
      'e',
      'f',
      'g',
      'h',
      'i',
  };

  int index = 2;
  // Shift Up
  for (int i = index; i + 1 < 9; i++) {
    swap(array[index], array[i + 1]);
  }

  // Shift Down
  for (int i = index; i + 1 < 9; i++) {
    swap(array[i], array[i + 1]);
  }

  for (int i = 0; i < 9; i++) {
    std::cout << array[i] << std::endl;
  }

  return 0;
}
