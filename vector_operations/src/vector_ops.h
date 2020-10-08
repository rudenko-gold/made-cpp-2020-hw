#pragma once
#include <vector>
#include <iostream>

namespace task {
    
const double eps = 1e-7;

template <typename T>
std::vector<T> operator+(const std::vector<T>& lhs, const std::vector<T>& rhs) {
    std::vector<T> answer(lhs.size());

    for (size_t i = 0; i < lhs.size(); ++i) {
        answer[i] = lhs[i] + rhs[i];
    }

    return answer;
}

template <typename T>
std::vector<T> operator-(const std::vector<T>& lhs, const std::vector<T>& rhs) {
    std::vector<T> answer(lhs.size());

    for (size_t i = 0; i < lhs.size(); ++i) {
        answer[i] = lhs[i] - rhs[i];
    }

    return answer;
}

template <typename T>
std::vector<T> operator+(const std::vector<T>& v) {
    return v;
}

template <typename T>
std::vector<T> operator-(const std::vector<T>& v) {
    std::vector<T> answer(v.size());

    for (size_t i = 0; i < v.size(); ++i) {
        answer[i] = -v[i];
    }

    return answer;
}

template <typename T>
T operator*(const std::vector<T>& lhs, const std::vector<T>& rhs) {
    T answer = T();

    for (size_t i = 0; i < lhs.size(); ++i) {
        answer += lhs[i] * rhs[i];
    }

    return answer;
}

template <typename T>
std::vector<T> operator%(const std::vector<T>& lhs, const std::vector<T>& rhs) {
    return std::vector<T> {
            lhs[1] * rhs[2] - lhs[2] * rhs[1],
            -(lhs[0] * rhs[2] - lhs[2] * rhs[0]),
            lhs[0] * rhs[1] - lhs[1] * rhs[0]
    };
}

bool is_zero(const std::vector<int>& v) {
    for (size_t i = 0; i < v.size(); ++i) {
        if (v[i] != 0) {
            return false;
        }
    }
    return true;
}

bool is_zero(const std::vector<double>& v) {
    for (size_t i = 0; i < v.size(); ++i) {
        if (v[i] >= eps) {
            return false;
        }
    }
    return true;
}

bool operator||(const std::vector<int>& lhs, const std::vector<int>& rhs) {
    if (is_zero(lhs) || is_zero(rhs)) {
        return true;
    }

    for (size_t i = 1; i < lhs.size(); ++i) {
        if (rhs[i] * lhs[i - 1] != rhs[i - 1] * lhs[i]) {
            return false;
        }
    }
    return true;
}

bool operator||(const std::vector<double>& lhs, const std::vector<double>& rhs) {
    if (is_zero(lhs) || is_zero(rhs)) {
        return true;
    }

    for (size_t i = 1; i < lhs.size(); ++i) {
        if (fabs(rhs[i] * lhs[i - 1] - rhs[i - 1] * lhs[i]) >= eps) {
            return false;
        }
    }
    return true;
}

template <typename T>
bool operator&&(const std::vector<T>& lhs, const std::vector<T>& rhs) {
    for (size_t i = 0; i < lhs.size(); ++i) {
        if ((lhs[i] > 0) != (rhs[i] > 0)) {
            return false;
        }
    }
    return lhs || rhs;
}

template <typename T>
std::istream& operator>>(std::istream& is, std::vector<T>& v) {
    size_t v_size;
    is >> v_size;

    v.resize(v_size);

    for (auto& item : v) {
        is >> item;
    }

    return is;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    for (const auto& item : v) {
        os << item << " ";
    }
    os << "\n";
    return os;
}

template <typename T>
void reverse(std::vector<T>& v) {
    for (size_t i = 0; i < v.size() / 2; ++i) {
        std::swap(v[i], v[v.size() - 1u - i]);
    }
}

std::vector<int> operator&(const std::vector<int>& lhs, const std::vector<int>& rhs) {
    std::vector<int> answer(lhs.size());

    for (size_t i = 0; i < answer.size(); ++i) {
        answer[i] = lhs[i] & rhs[i];
    }

    return answer;
}

std::vector<int> operator|(const std::vector<int>& lhs, const std::vector<int>& rhs) {
    std::vector<int> answer(lhs.size());

    for (size_t i = 0; i < answer.size(); ++i) {
        answer[i] = lhs[i] | rhs[i];
    }

    return answer;
}

}  // namespace task

