// bitset_iter.h v1.1.0
// Copyright 2019, Diego Dagum
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef BITSET_ITER_H
#define BITSET_ITER_H

#include <bitset>
#include <iterator>

/**
 * Main namespace.
 */
namespace bitset {

    /**
     * @brief      Wrap around a bitset with iteration status.
     *
     * Ideally, you should never deal with an instance of this type yourself. When
     * you need to visit all bit indices on (or only the ones off) in a bitset, you
     * should hand the result of functions indices_on() or indices_off() to a
     * range-based for loop.
     *
     * Take a look to the following example:
     *
     * @code{.cpp}
     * #include <bitset>
     * #include "bitset_iter.h"
     *
     * std::bitset<32> my_bitset{ 0b10101010101010101010101010101010 };
     *
     * for (const auto& index : bitset::indices_on(my_bitset)) {
     *   std::cout << index << ", ";
     * }
     * // Prints "1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, "
     *
     * for (const auto& index : bitset::indices_off(my_bitset)) {
     *   std::cout << index << ", ";
     * }
     * // Prints "0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, "
     * @endcode
     *
     * Notice that there is no visible instance of this type in the example.
     * However, each range-based for loop holds one internally.
     *
     * @tparam     SIZE       The bitset template size.
     * @tparam     CONTAINER  The type for the container, typically an
     * std::bitset<SIZE> reference but possibly also an std::bitset<SIZE> value.
     * @tparam     FLAG       Boolean to test bits for during iteration.
     */
    template<std::size_t SIZE, typename CONTAINER, bool FLAG = true>
    class index_iterable
    {
    public:
        class const_iterator
        {
        public:
            using iterator_category = std::forward_iterator_tag;

            /**
             * @brief      Main constructor.
             *
             * @param[in]  bitset  An std::bitset<SIZE>.
             */
            explicit const_iterator(const std::bitset<SIZE>& bitset)
                : index{ static_cast<std::size_t>(-1) }
                , bitset{ bitset }
            {}

            /**
             * @brief      Prefix increment.
             *
             * Leaves the iterator pointing to the next bitset index whose status is
             * FLAG. Or past the end if none.
             */
            const_iterator operator++()
            {
                this->seek_next();
                return *this;
            }

            /**
             * @brief      Postfix increment.
             *
             * Leaves the iterator pointing to the next bitset index whose status is
             * FLAG. Or past the end if none.
             */
            const_iterator operator++(int)
            {
                const_iterator prev_this = *this;
                this->seek_next();
                return prev_this;
            }

            /**
             * @brief      Dereference operator.
             *
             * @return     A reference to the current index (whose status is FLAG).
             */
            const std::size_t& operator*() const { return this->index; }

            /**
             * @brief      Operator is-equal.
             */
            bool operator==(const const_iterator& rhs) const
            {
                return (this->index == rhs.index) && (this->bitset == rhs.bitset);
            }

            /**
             * @brief      Operator not-equal.
             */
            bool operator!=(const const_iterator& rhs) const
            {
                return not this->operator==(rhs);
            }

            friend const_iterator index_iterable::begin() const;
            friend const_iterator index_iterable::end() const;

        protected:
            std::size_t index;

        private:
            void seek_next()
            {
                while (++(this->index) < SIZE) {
                    if (bitset[this->index] == FLAG) {
                        break;
                    }
                }
            }

            const std::bitset<SIZE>& bitset;
        };

        /**
         * @brief      Main constructor.
         *
         * @param[in]  bitset  An std::bitset<SIZE> reference.
         */
        explicit index_iterable(const std::bitset<SIZE>& bitset)
            : bitset{ bitset }
        {}

        /**
         * @brief      Creates a const_iterator pointing at the first bitset index
         * whose status is FLAG.
         *
         * @return     An const_iterator<SIZE, CONTAINER, FLAG>.
         */
         // cppcheck-suppress unusedFunction
        const_iterator begin() const
        {
            const_iterator iterator{ this->bitset };
            iterator.seek_next();

            return iterator;
        }

        /**
         * @brief      Creates a const_iterator pointing past the end of the bitset.
         *
         * @return     An const_iterator<SIZE, CONTAINER, FLAG>.
         */
         // cppcheck-suppress unusedFunction
        const_iterator end() const
        {
            const_iterator iterator{ this->bitset };
            iterator.index = SIZE;

            return iterator;
        }

    private:
        CONTAINER bitset;
    };

    /**
     * @brief      Returns its input wrapped into an index_iterable through bits on.
     *
     * In this overload the input reference is held into the iterable.
     *
     * @code{.cpp}
     * #include <bitset>
     * #include "bitset_iter.h"
     *
     * std::bitset<32> my_bitset{ 0b10101010101010101010101010101010 };
     *
     * for (const auto& index : bitset::indices_on(my_bitset)) {
     *   std::cout << index << ", ";
     * }
     * // Prints "1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, "
     * @endcode
     *
     * @param[in]  bitset  An std::bitset<SIZE> reference.
     *
     * @tparam     SIZE    The size of the input.
     *
     * @return     A index_iterable<SIZE, const std::bitset<SIZE>&>.
     */
    template<std::size_t SIZE>
    index_iterable<SIZE, const std::bitset<SIZE>&>
        indices_on(const std::bitset<SIZE>& bitset)
    {
        return index_iterable<SIZE, const std::bitset<SIZE>&>{ bitset };
    }

    /**
     * @brief      Returns an index_iterable for a bitset through bits on.
     *
     * In this overload, the R-value input is copied into the iterable.
     *
     * @code{.cpp}
     * #include <bitset>
     * #include "bitset_iter.h"
     *
     * std::bitset<32> my_bitset{ 0b10101010101010101010101010101010 };
     *
     * for (const auto& index : bitset::indices_on(~my_bitset)) {
     *   std::cout << index << ", ";
     * }
     * // Prints "0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, "
     * @endcode
     *
     * @param[in]  bitset  An std::bitset<SIZE> R-value.
     *
     * @tparam     SIZE    The size of the input.
     *
     * @return     A index_iterable<SIZE, const std::bitset<SIZE>>.
     */
    template<std::size_t SIZE>
    index_iterable<SIZE, const std::bitset<SIZE>>
        indices_on(std::bitset<SIZE>&& bitset)
    {
        return index_iterable<SIZE, const std::bitset<SIZE>>{ bitset };
    }

    /**
     * @brief      Returns its input wrapped into an index_iterable through bits
     * off.
     *
     * In this overload the input reference is held into the iterable.
     *
     * @code{.cpp}
     * #include <bitset>
     * #include "bitset_iter.h"
     *
     * std::bitset<32> my_bitset{ 0b10101010101010101010101010101010 };
     *
     * for (const auto& index : bitset::indices_off(my_bitset)) {
     *   std::cout << index << ", ";
     * }
     * // Prints "0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, "
     * @endcode
     *
     * @param[in]  bitset  An std::bitset<SIZE> reference.
     *
     * @tparam     SIZE    The size of the input.
     *
     * @return     A index_iterable<SIZE, const std::bitset<SIZE>&>.
     */
    template<std::size_t SIZE>
    index_iterable<SIZE, const std::bitset<SIZE>&, false>
        indices_off(const std::bitset<SIZE>& bitset)
    {
        return index_iterable<SIZE, const std::bitset<SIZE>&, false>{ bitset };
    }

    /**
     * @brief      Returns an index_iterable for a bitset through bits off.
     *
     * In this overload, the R-value input is copied into the iterable.
     *
     * @code{.cpp}
     * #include <bitset>
     * #include "bitset_iter.h"
     *
     * std::bitset<32> my_bitset{ 0b10101010101010101010101010101010 };
     *
     * for (const auto& index : bitset::indices_off(~my_bitset)) {
     *   std::cout << index << ", ";
     * }
     * // Prints "1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, "
     * @endcode
     *
     * @param[in]  bitset  An std::bitset<SIZE> R-value.
     *
     * @tparam     SIZE    The size of the input.
     *
     * @return     A index_iterable<SIZE, const std::bitset<SIZE>>.
     */
    template<std::size_t SIZE>
    index_iterable<SIZE, const std::bitset<SIZE>, false>
        indices_off(std::bitset<SIZE>&& bitset)
    {
        return index_iterable<SIZE, const std::bitset<SIZE>, false>{ bitset };
    }

}

#endif // BITSET_ITER_H
