#pragma once

namespace util {

class TypesafeDouble {
  public:
    operator double() {
        return d_;
    }

    operator +(double d) {
        return d_ + d;
    }

    operator -(double d) {
        return d_ - d;
    }

    operator +=(double d) {
        d_ += d;
        return d_;
    }

    operator -=(double d) {
        d_ -= d;
        return d_;
    }

  protected:
    TypesafeDouble() {};

    double d_ = 0.0;
};

class X : public TypesafeDouble {
  public:
    explicit X(double x) { d_ = x; };
};

class Y : public TypesafeDouble{
  public:
    explicit Y(double y) { d_ = y; };
};

} // namespace
