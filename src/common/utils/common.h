#pragma once

namespace util {

class TypesafeInt {
  public:
    operator int() {
        return d_;
    }

    int operator +(int d) {
        return d_ + d;
    }

    int operator -(int d) {
        return d_ - d;
    }

    int operator +=(int d) {
        d_ += d;
        return d_;
    }

    int operator -=(int d) {
        d_ -= d;
        return d_;
    }

    bool operator <(int d) {
        return d_ < d;
    }

    bool operator >(int d) {
        return d_ > d;
    }

  protected:
    TypesafeInt() {};

    int d_ = 0;
};

class X : public TypesafeInt {
  public:
    explicit X(int x) { d_ = x; };
    void operator= (int x) { d_ = x; };
};

class Y : public TypesafeInt{
  public:
    explicit Y(int y) { d_ = y; };
    void operator= (int y) { d_ = y; };
};

class Right : public TypesafeInt{
  public:
    explicit Right(int r) { d_ = r; };
};

class Left : public TypesafeInt{
  public:
    explicit Left(int l) { d_ = l; };
};

class Top : public TypesafeInt{
  public:
    explicit Top(int t) { d_ = t; };
};

class Bottom : public TypesafeInt{
  public:
    explicit Bottom(int b) { d_ = b; };
};

class Point {
  public:
    Point(int x, int y) : x_(x), y_(y) {}

    const int x_;
    const int y_;
};

} // namespace
