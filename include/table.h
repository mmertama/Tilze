#pragma once

#include "sparseiterator.h"
#include <memory>
#include <array>
#include <cassert>
#include <optional>

template <class T, int ROWS, int STRIPES>
class Table {
private:
    using value_type = std::shared_ptr<T>;
    using ArrayType = std::array<value_type, ROWS * STRIPES>;
    using Array_const_iterator = typename ArrayType::const_iterator;
    using Array_iterator = typename ArrayType::iterator;
    using SparseIteratorType_const = SparseIterator<Array_const_iterator,
        const std::ptrdiff_t,
        const typename ArrayType::value_type,
        const typename ArrayType::value_type*,
        const typename ArrayType::value_type&>;
    using SparseIteratorType = SparseIterator<Array_iterator,
        std::ptrdiff_t,
        typename ArrayType::value_type,
        typename ArrayType::value_type*,
        typename ArrayType::value_type&>;
private:
    int pos(int stripe, int row) const {
        return ROWS * stripe + row;
    }
    const value_type& at(int stripe, int level) const {
        return m_cubes[pos(stripe, level)];
    }
    value_type&& take(int stripe, int level) {
        return std::move(m_cubes[pos(stripe, level)]);
    }
public:
    using iterator = SparseIteratorType;
    using const_iterator = SparseIteratorType_const;
    auto begin() const { return SparseIteratorType_const{m_cubes.begin(), m_cubes.end()}; }
    auto end() const { return SparseIteratorType_const{m_cubes.end(), m_cubes.end()}; }
    auto const_begin() const { return SparseIteratorType_const{m_cubes.begin(), m_cubes.end()}; }
    auto const_end() const { return SparseIteratorType_const{m_cubes.end(), m_cubes.end()}; }
    auto begin() { return SparseIteratorType{m_cubes.begin(), m_cubes.end()}; }
    auto end() {return SparseIteratorType{m_cubes.end(), m_cubes.end()}; }
    Table() = default;
    Table(const Table&) = delete;
    Table& operator=(const Table&) = delete;

    std::optional<value_type> add(int value, int stripe) {
        const auto sz = size(stripe);
        if(sz >= ROWS)
            return std::nullopt;
        const auto p = pos(stripe, sz);
        assert(!m_cubes[p]);
        m_cubes[p] = std::make_shared<T>(value);
        return std::make_optional(m_cubes[p]);
    }

    const_iterator find(const value_type& v) const {
        for(auto it = begin(); it != end(); ++it) {
            if(*it ==  v)
                return it;
        }
        return end();
    }

    value_type move(int stripe_from, int row_from, int stripe_to, int row_to) {
        const auto p = pos(stripe_to, row_to);
        assert(!m_cubes[p]);
        m_cubes[p] = take(stripe_from, row_from);
        assert(m_cubes[p]);
        return m_cubes[p];
    }

    int size(int col) const {
        const auto begin = pos(col, 0);
        const auto end = begin + ROWS;
        for(auto it = begin; it != end; ++it)
            if(!m_cubes[it])
                return it - begin;
        return ROWS;
    }

    int size() const {
        auto count = 0;
        for(auto it = begin(); it != end(); ++it) {
            ++count;
        }
        return count;
    }

    bool empty() const {
        return size() == 0;
    }

    bool full() const {
        return size() == ROWS * STRIPES;
    }

    void clear() {
        for(auto& c : *this) {
            c.reset();
        }
    }

  //  int lastFree(int stripe) const {return size(stripe);}

    bool has(int col, int row) const {
        return at(col, row).operator bool();
    }

    auto takeSisters(const value_type& cube) {
        const auto p = find(cube);

        const auto stripe = column(p);
        const auto level = row(p);

        std::vector<value_type> vec;
        vec.reserve(4);
        const auto check = [&vec, &cube, this](int s, int l) {
            auto c = at(s, l);
            if(!(c && c->isAlive() && c->value() == cube->value()))
                return;
            auto taken = take(s, l);
            taken->kill();
            vec.emplace_back(taken);
        };
        if(stripe > 0)
            check(stripe - 1, level);
        if(stripe + 1 < STRIPES)
            check(stripe + 1, level);
        if(level > 0)
            check(stripe, level - 1);
        if(level + 1 < STRIPES)
            check(stripe, level + 1);
        return vec;
    }

    template<class IT>
    int column(const IT& it) const {
        const int d = distance(it);
        return d / ROWS;
    }

    template<class IT>
    int row(const IT& it) const {
        const int d = distance(it);
        return d % ROWS;
    }

private:
    template<class IT>
    int distance(const IT& it) const {
        return &(*it) - &(*m_cubes.begin());
    }

    template<class IT>
    IT distance(int pos) const {
        return &(*m_cubes.begin()) + pos;
    }

private:
    ArrayType m_cubes;
};
