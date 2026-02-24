#include "myclass.h"

MyClass::MyClass() {
}

MyClass::MyClass(int x_) { 
  x = x_; 
}

int MyClass::getX() { 
  return x; 
}

int MyClass::getY() { 
  return y; 
}