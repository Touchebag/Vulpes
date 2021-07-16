#pragma once

#include <unordered_map>

template <typename T, typename U>
class Bimap {
  public:
    // Initiation
    Bimap() = default;
    Bimap(std::initializer_list<std::pair<T, U>> list) {
        for (auto elem : list) {
            add(elem.first, elem.second);
        }
    }

    // Inserting
    void add(T t, U u) {
        t_u_[t] = u;
        u_t_[u] = t;
    }
    void add(U u, T t) {
        t_u_[t] = u;
        u_t_[u] = t;
    }

    // Lookup
    T at(U u) const {
        return u_t_.at(u);
    }
    U at(T t) const {
        return t_u_.at(t);
    }

    bool contains(T t) const {
        return t_u_.count(t) > 0;
    }
    bool contains(U u) const {
        return u_t_.count(u) > 0;
    }

  private:
    std::unordered_map<T, U> t_u_;
    std::unordered_map<U, T> u_t_;
};
