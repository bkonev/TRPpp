/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  Copyright (C) 2001-2011                               * */
/* *  Boris Konev                                           * */
/* *  The University of Liverpool                           * */
/* *                                                        * */
/* *  This program is free software; you can redistribute   * */
/* *  it and/or modify it under the terms of the GNU        * */
/* *  General Public License as published by the Free       * */
/* *  Software Foundation; either version 2 of the License, * */
/* *  or (at your option) any later version.                * */
/* *                                                        * */
/* *  This program is distributed in the hope that it will  * */
/* *  be useful, but WITHOUT ANY WARRANTY; without even     * */
/* *  the implied warranty of MERCHANTABILITY or FITNESS    * */
/* *  FOR A PARTICULAR PURPOSE.  See the GNU General Public * */
/* *  License for more details.                             * */
/* *                                                        * */
/* *  You should have received a copy of the GNU General    * */
/* *  Public License along with this program; if not, write * */
/* *  to the Free Software Foundation, Inc., 59 Temple      * */
/* *  Place, Suite 330, Boston, MA  02111-1307  USA         * */
/* *                                                        * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/
#ifndef __COMPOUND_CONTAINER_H__
#define __COMPOUND_CONTAINER_H__
#include <utility>
#include <iterator>
#include <iostream>

template < typename _Iterator1, typename _Iterator2 >
    struct compound_iterator 
    {
        typedef size_t                      size_type;
        typedef ptrdiff_t                   difference_type;
        typedef std::input_iterator_tag     iterator_category;
        typedef typename _Iterator1::value_type      value_type;
        typedef typename _Iterator1::pointer         pointer;
        typedef typename _Iterator1::reference       reference;

        typedef compound_iterator<_Iterator1, _Iterator2> _Self;

        //compound_iterator()
        //{ }

        // looks strange but only one of these "currents" is correct
        // the second one is given as I do not see any other way 
        // to find the problem that _Iterator1 == _Iterator2 (then,
        // I cannot select between them by one template parameters)
        compound_iterator(_Iterator1 current1, _Iterator2 current2,
                _Iterator1 end, _Iterator2 begin, bool first)
        : first_end(end), second_begin(begin), current_first(current1), 
          current_second(current2) // of course, this is not entirely 
              // correct: values of current1/2 will be updated later
              // durti hack in order to avoid default constructors
        {
            if (first)
            {
                // to add _Iter == _Iterator1
                if(current1 != end)
                {
                    selected_first = true;
                    current_first = current1;
                }
                else 
                {
                    selected_first = false;
                    current_second = begin;
                }
            }
            else // first == false
            {
                selected_first = false;
                current_second = current2;
            }
        }

        bool 
        operator== (const _Self& __x) const
        {
            if (this->selected_first && __x.selected_first)
                    return (this->current_first == __x.current_first);
                else
                    if (!(this->selected_first) && !(__x.selected_first)) // 
                        return (this->current_second == __x.current_second);
            // else
            return false;
        }

        bool 
        operator!= (const _Self& __x) const
        {
            return !this->operator==(__x);
        }

        pointer 
        operator-> ()  const 
        {
            if (selected_first)
                return current_first.operator->();
            else // selected_first == false
                return current_second.operator->();
        }

        reference 
        operator* () const
        {
            return *operator->();
        }

        _Self& 
        operator++()
        {
            if (selected_first)
            {
                ++current_first;
                if (current_first == first_end) 
                {
                    selected_first = false;
                    current_second = second_begin;
                }
            }
            else // selected_first == false
            {
                ++current_second;
            }
            return *this;
        }

        _Self&
        operator--()
        {
            if (!selected_first)
            {
                if (current_second == second_begin)
                {
                    selected_first = true;
                    current_first = first_end;
                }
                else // current_second != second_begin
                    --current_second;
            }
            else // selected_first == true
                --current_first;
            return *this;
        }

        _Self
        operator++(int)
        {
            _Self old(*this); 
            ++*this;
            return old;
        }

        _Self
        operator--(int)
        {
            _Self old(*this);
            --*this;
            return old;
        }
        
    private: 
        _Iterator1 first_end; // keeping the end of the first range
        _Iterator2 second_begin; // keeping the beginning of the second 
        _Iterator1 current_first; // current pointer within the first range
        _Iterator2 current_second; // current pointer within the second
        bool selected_first; // still within the first range
    };

// this is not a _real_ container, only some really needed members are
// implemented. If something will be needed, add. A am laizy, I know..
template < typename _First, typename _Second > 
   class compound_container
    {
    private:
        _First first;
        _Second second;
    public:
        // not all of container functions!
        typedef typename _First::value_type value_type;
        typedef typename _First::difference_type difference_type;
        typedef typename _First::size_type size_type;
        typedef typename _First::const_reference const_reference;
        typedef typename _First::reference reference;
        typedef typename _First::pointer pointer;
        typedef compound_iterator<typename _First::iterator, typename _Second::iterator> iterator;
        typedef compound_iterator<typename _First::const_iterator, typename _Second::const_iterator> const_iterator;
    
        typedef compound_container<_First, _Second> _Self;

        iterator
        begin()
        {
            return iterator(first.begin(), /*fake*/ second.begin(), first.end(), second.begin(), true);
        }
        const_iterator begin() const
        {
            return const_iterator(first.begin(), /*fake*/ second.begin(), first.end(), second.begin(), true);
        }

        iterator
        end()
        {
            return iterator(/*fake*/ first.end(), second.end(), first.end(), second.begin(), false);
        }

        const_iterator
        end() const
        {
            return const_iterator(/*fake*/ first.end(), second.end(), first.end(), second.begin(), false);
        }

        bool
        empty() const
        {
            return (first.empty() && second.empty());
        }

        size_type
        size() const
        {
            return (first.size() + second.size());
        }

        size_type
        max_size() const
        {
            size_type s1 = first.max_size();
            size_type s2 = second.max_size();
            return (s1>s2?s2:s1);  //MIN
        }

        void 
        toSecond(const value_type& __x) 
        {
            second.push_back(__x);
        }   

        void 
        toFirst(const value_type& __x)
        {
            first.push_back(__x);
        }

        void 
        push_back(const value_type& __x) 
        {
            // this is my choice
            toSecond(__x);
        }   

        // descendents of this type may wish to access to first and second
        // directly. 
    protected:
        _First&
        get_first()
        {
            return first;
        }

        const _First& 
        get_first() const
        {
            return first;
        }

        _Second& 
        get_second() 
        {
            return second;
        }

        const _Second& 
        get_second() const
        {
            return second;
        }
    }; // class compound_container
#endif // __COMPOUND_CONTAINER_H__
