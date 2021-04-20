namespace hft {
template <class T, uint32_t LENGTH>
List<T, LENGTH>::List(const uint32_t& min_length) : m_min_length{min_length}, m_list{0}, m_itr{m_list}, m_cum_size{0} {
    // printf("%s %d\n", __FUNCTION__, __LINE__);
    memset(m_list, 0, sizeof(m_list));
}

template <class T, uint32_t LENGTH>
const T* List<T, LENGTH>::Append(const T& value) {
    m_cum_size++;
    // m_last_value = value;
    if (UNLIKELY(m_itr - m_list >= LENGTH - 1)) {
        for (int i = m_min_length - 1; i >= 0; --i) {
            --m_itr;
            m_list[i] = *m_itr;
        }
        m_itr = m_list + m_min_length;
        *m_itr = value;
        m_itr++;
    } else {
        *m_itr = value;
        m_itr++;
    }
    return m_itr - 1;
}

template <class T, uint32_t LENGTH>
const T* List<T, LENGTH>::begin() const {
    return m_cum_size >= m_min_length ? m_itr - m_min_length : m_list;
}
template <class T, uint32_t LENGTH>
const T* List<T, LENGTH>::end() const {
    return m_itr;
}
template <class T, uint32_t LENGTH>
T& List<T, LENGTH>::operator[](size_t i) {
    return m_cum_size >= m_min_length ? *(m_itr - m_min_length + i) : *(m_list + i);
}
template <class T, uint32_t LENGTH>
const T& List<T, LENGTH>::operator[](size_t i) const {
    return *(this->begin() + i);
}

template <class T, uint32_t LENGTH>
List<T, LENGTH>& List<T, LENGTH>::operator=(const List<T, LENGTH>& other) {
    this->m_min_length = other.GetMinLength();
    for (auto v : other) {
        this->Append(v);
    }
    this->m_cum_size = other.cum_size();
    return *this;
}

template <class T, uint32_t LENGTH>
List<T, LENGTH> List<T, LENGTH>::copy() const {
    List<T, LENGTH> tmp{this->GetMinLength()};
    for (auto i = this->begin(); i < this->end(); ++i) {
        tmp.Append(*i);
    }
    return tmp;
}

template <class T, uint32_t LENGTH>
List<T, LENGTH>& List<T, LENGTH>::operator+=(const List<T, LENGTH>& other) {
    T* j = const_cast<T*>(other.end() - 1);
    for (T *i = m_itr - 1; i >= m_itr - m_min_length; --i, --j) {
        *i += *j;
        if (j == other.begin()) break;
    }
    return *this;
}

template <class T, uint32_t LENGTH>
template <typename Tintput>
List<T, LENGTH>& List<T, LENGTH>::operator+=(const Tintput& b) {
    for (T* i = this->itr(); i >= this->begin(); --i) {
        *i += b;
    }
    return *this;
}

template <class T, uint32_t LENGTH>
template <typename Tintput>
List<T, LENGTH>& List<T, LENGTH>::operator-=(const Tintput& b) {
    for (T* i = this->itr(); i >= this->begin(); --i) {
        *i -= b;
    }
    return *this;
}

template <class T, uint32_t LENGTH>
List<T, LENGTH>& List<T, LENGTH>::operator*=(const List<T, LENGTH>& other) {
    T* j = const_cast<T*>(other.end() - 1);
    for (T *i = m_itr - 1; i >= m_itr - m_min_length; --i, --j) {
        *i *= *j;
        if (j == other.begin()) break;
    }
    return *this;
}

template <class T, uint32_t LENGTH>
List<T, LENGTH>& List<T, LENGTH>::operator/=(const List<T, LENGTH>& other) {
    T* j = const_cast<T*>(other.end() - 1);
    for (T *i = m_itr - 1; i >= m_itr - m_min_length; --i, --j) {
        *i /= *j;
        if (j == other.begin()) break;
    }
    return *this;
}

template <class T, uint32_t LENGTH>
template <typename Tintput>
List<T, LENGTH>& List<T, LENGTH>::operator*=(const Tintput& v0) {
    for (T* i = m_itr - 1; i >= m_itr - m_min_length; --i) {
        if (i < m_list) break;
        *i *= v0;
    }
    return *this;
}

template <class T, uint32_t LENGTH, class Tintput>
List<T, LENGTH> operator*(const List<T, LENGTH>& other, const Tintput& v0) {
    auto tmp = other.copy();
    tmp *= v0;
    return tmp;
}

template <class Tintput, class T, uint32_t LENGTH>
List<T, LENGTH> operator*(const Tintput& v0, const List<T, LENGTH>& other) {
    auto tmp = other.copy();
    tmp *= v0;
    return tmp;
}

template <class T, uint32_t LENGTH>
template <typename Tintput>
List<T, LENGTH>& List<T, LENGTH>::operator/=(const Tintput& v0) {
    for (T* i = m_itr - 1; i >= m_itr - m_min_length; --i) {
        *i /= v0;
    }
    return *this;
}

template <class T, uint32_t LENGTH>
List<T, LENGTH>& List<T, LENGTH>::operator-=(const List<T, LENGTH>& other) {
    T* j = const_cast<T*>(other.end() - 1);
    for (T *i = m_itr - 1; i >= m_itr - m_min_length; --i, --j) {
        *i -= *j;
        if (j == other.begin()) break;
    }
    return *this;
}

template <class T, uint32_t LENGTH>
const List<T, LENGTH> operator+(const List<T, LENGTH>& a, const List<T, LENGTH>& b) {
    List<T, LENGTH> tmp = a.copy();
    auto n = MIN(a.cum_size(), a.GetMinLength());
    for (size_t i = 0; i < n; ++i) {
        tmp[i] += b[i];
    }
    return tmp;
}

template <class Tintput, class T, uint32_t LENGTH>
const List<T, LENGTH> operator+(const List<T, LENGTH>& a, const Tintput& b) {
    auto tmp = a.copy();
    tmp += b;
    return tmp;
}

template <class Tintput, class T, uint32_t LENGTH>
const List<T, LENGTH> operator+(const Tintput& b, const List<T, LENGTH>& a) {
    auto tmp = a.copy();
    tmp += b;
    return tmp;
}

template <class T, uint32_t LENGTH, class Tintput>
const List<T, LENGTH> operator-(const List<T, LENGTH>& a, const Tintput& b) {
    auto tmp = a.copy();
    tmp += b;
    return tmp;
}

template <typename Type, uint32_t Len>
const List<Type, Len> operator-(const List<Type, Len>& a, const List<Type, Len>& b) {
    List<Type, Len> tmp = a.copy();
    auto n = MIN(a.cum_size(), a.GetMinLength());
    for (size_t i = 0; i < n; ++i) {
        tmp[i] -= b[i];
    }
    return tmp;
}

// template <typename Tintput, class T, uint32_t LENGTH>
// List<T, LENGTH> operator*(const List<T, LENGTH>& other, const Tintput& v0) {
//    auto tmp = other.copy();
//    tmp *= v0;
//    return tmp;
//}

template <typename T, uint32_t LENGTH>
List<T, LENGTH> operator*(const List<T, LENGTH>& other, const int32_t& v0) {
    auto tmp = other.copy();
    tmp *= v0;
    return tmp;
}

};  // namespace hft
