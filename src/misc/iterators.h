/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  Minimal iterator adapters to replace Boost iterators   * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/
#ifndef __MISC_ITERATORS_H__
#define __MISC_ITERATORS_H__

#include <iterator>
#include <type_traits>
#include <utility>
#include <functional>

namespace Misc
{
    template <typename Pred, typename Iter>
    class filter_iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename std::iterator_traits<Iter>::value_type;
        using difference_type = typename std::iterator_traits<Iter>::difference_type;
        using pointer = typename std::iterator_traits<Iter>::pointer;
        using reference = typename std::iterator_traits<Iter>::reference;

        filter_iterator() = default;
        filter_iterator(Iter begin, Iter end, Pred pred = Pred())
        : current(begin), last(end), predicate(pred)
        {
            satisfy();
        }

        reference operator*() const { return *current; }
        pointer operator->() const { return std::addressof(*current); }

        filter_iterator& operator++()
        {
            ++current;
            satisfy();
            return *this;
        }

        filter_iterator operator++(int)
        {
            filter_iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        friend bool operator==(const filter_iterator& a, const filter_iterator& b)
        {
            return a.current == b.current;
        }

        friend bool operator!=(const filter_iterator& a, const filter_iterator& b)
        {
            return !(a == b);
        }

    private:
        void satisfy()
        {
            while (current != last && !predicate(*current))
            {
                ++current;
            }
        }

        Iter current{};
        Iter last{};
        Pred predicate{};
    };

    template <typename Func, typename Iter>
    class transform_iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = typename std::iterator_traits<Iter>::difference_type;
        using value_type = std::remove_cv_t<std::remove_reference_t<decltype(std::invoke(std::declval<Func>(), *std::declval<Iter>()))>>;
        using reference = decltype(std::invoke(std::declval<Func>(), *std::declval<Iter>()));
        using pointer = void;

        transform_iterator() = default;
        explicit transform_iterator(Iter it, Func f = Func())
        : current(it), func(f)
        {
        }

        reference operator*() const { return std::invoke(func, *current); }

        transform_iterator& operator++()
        {
            ++current;
            return *this;
        }

        transform_iterator operator++(int)
        {
            transform_iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        friend bool operator==(const transform_iterator& a, const transform_iterator& b)
        {
            return a.current == b.current;
        }

        friend bool operator!=(const transform_iterator& a, const transform_iterator& b)
        {
            return !(a == b);
        }

    private:
        Iter current{};
        Func func{};
    };

    template <typename Pred, typename Iter>
    inline filter_iterator<Pred, Iter> make_filter_iterator(Iter begin, Iter end)
    {
        return filter_iterator<Pred, Iter>(begin, end, Pred());
    }
}

#endif // __MISC_ITERATORS_H__
