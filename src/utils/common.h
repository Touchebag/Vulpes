#pragma once

namespace util {

class TypesafeDouble {
  public:
    operator double() {
        return d_;
    }

    double operator +(double d) {
        return d_ + d;
    }

    double operator -(double d) {
        return d_ - d;
    }

    double operator +=(double d) {
        d_ += d;
        return d_;
    }

    double operator -=(double d) {
        d_ -= d;
        return d_;
    }

    bool operator <(double d) {
        return d_ < d;
    }

    bool operator >(double d) {
        return d_ > d;
    }

  protected:
    TypesafeDouble() {};

    double d_ = 0.0;
};

class X : public TypesafeDouble {
  public:
    explicit X(double x) { d_ = x; };
    void operator= (double x) { d_ = x; };
};

class Y : public TypesafeDouble{
  public:
    explicit Y(double y) { d_ = y; };
    void operator= (double y) { d_ = y; };
};

class Right : public TypesafeDouble{
  public:
    explicit Right(double r) { d_ = r; };
};

class Left : public TypesafeDouble{
  public:
    explicit Left(double l) { d_ = l; };
};

class Top : public TypesafeDouble{
  public:
    explicit Top(double t) { d_ = t; };
};

class Bottom : public TypesafeDouble{
  public:
    explicit Bottom(double b) { d_ = b; };
};

class Point {
  public:
    Point(int x, int y) : x(x), y(y) {}

    const int x;
    const int y;
};

} // namespace
