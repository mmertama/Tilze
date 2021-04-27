#pragma once

template<typename T,
    typename DT = typename T::difference_type,
    typename VT = typename T::value_type,
    typename PT = typename T::pointer,
    typename RT = typename T::reference>
struct SparseIterator  {
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = DT;
    using value_type        = VT;
    using pointer           = PT;
    using reference         = RT;
    reference operator*() const {return *m_it;}
    pointer operator->() {return m_it;}
    SparseIterator& operator++() {
        do {
            if(m_it == m_it_end)
                break;
            ++m_it;
        } while(*m_it == nullptr);
        return *this;
        }  
    SparseIterator operator++(int) {
        auto tmp = *this;
        ++(*this);
        return tmp;
        }
    friend bool operator== (const SparseIterator& a, const SparseIterator& b) { return a.m_it == b.m_it; };
    friend bool operator!= (const SparseIterator& a, const SparseIterator& b) { return a.m_it != b.m_it; };
    SparseIterator(T i, T e) : m_it(i), m_it_end(e) {
        while(m_it != m_it_end && !*m_it) {
            ++m_it;
        }
    }
private:
        T m_it;
        T m_it_end;
};
