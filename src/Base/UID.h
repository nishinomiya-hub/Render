#pragma once

namespace Learn {

template<typename T>
class UID {
 public:
  UID() : uid_(uidCounter_++) {}

  inline int get() const {
    return uid_;
  }

 private:
  int uid_ = -1;
  static int uidCounter_;
};

template<typename T>
int UID<T>::uidCounter_ = 0;

}