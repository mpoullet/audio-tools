#pragma once

// Taken from: http://www.fluentcpp.com/2017/05/05/news-strong-types-are-free/

template <typename T, typename Parameter> class NamedType {
  public:
    explicit NamedType(T const &value) : value_(value) {}
    T &get() { return value_; }
    T const &get() const { return value_; }

  private:
    T value_;
};

