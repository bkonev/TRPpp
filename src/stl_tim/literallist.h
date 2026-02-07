/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  Copyright (C) 2001-2011                               * */
/* *  Boris Konev                                           * */
/* *  The University of Liverpool                           * */
/* *                                                        * */
/* *  Copyright (C) 2012                                    * */
/* *  Viktor Schuppan                                       * */
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
#ifndef __LITERALLIST_H__
#define __LITERALLIST_H__

#include <vector>
#include <iostream>
#include <algorithm>
#include "literal.h"
#include "config.h"
#include "misc/assert.h"
namespace PropositionalProver
{
    // types used to construct clauses
    // if unsure, use LiteraList, then, by conversion,
    // SortedLiteralList will be created
    typedef std::vector<Literal> LiteralListBase;
    class LiteralList : public LiteralListBase
    {
    public: 
        typedef LiteralListBase::const_iterator const_iterator;
        typedef LiteralListBase::iterator iterator;

        LiteralList()
        : LiteralListBase()
        { }

        LiteralList(const LiteralList& l)
        : LiteralListBase(l)
        { }

        template<typename _InputIterator>
        LiteralList(const _InputIterator __first, const _InputIterator __last)
        : LiteralListBase(__first, __last)
        { }

        std::ostream& dump(std::ostream& os) const
        {
            for(LiteralList::const_iterator it = this->begin();
                    it != this->end();
                    ++it)
                it->dump(os);
            return os;
        }
    };
    // SortedLiteralList might be created directly to avoid 
    // sorting overheads
    class SortedLiteralList : private LiteralList
    {
    public:
        typedef LiteralList::const_iterator const_iterator;
        typedef LiteralList::iterator iterator;

        SortedLiteralList()
        : LiteralList()
        { }

        SortedLiteralList(const LiteralList& l)
        : LiteralList(l)
        {
            this -> maintainStructure();
        }

        template <class _InputIterator> 
        SortedLiteralList(const _InputIterator __first, const _InputIterator __last)
        : LiteralList(__first, __last)
        {
            this -> maintainStructure();
        }

        iterator
        begin() {return LiteralList::begin();}

        const_iterator
        begin() const {return LiteralList::begin();}

        iterator
        end() {return LiteralList::end();}

        const_iterator
        end() const {return LiteralList::end();}

        void 
        maintainStructure()
        {
            std::sort(this->begin(), this->end(), Literal::greater_literal());
            iterator p = std::unique<iterator>(this->begin(), this->end());
            this->erase(p, this->end());
        }

        void 
        push_back(const Literal& literal)
        {
            Assert((LiteralList::size() == 0) || (literal <
                        LiteralList::back()), 
                    "attempt to insert into a wrong place!");
            LiteralList::push_back(literal);
        }

        LiteralList::size_type
        size() const
        {
            return LiteralList::size();
        }
    };
} //namespace PropositionalProver
#endif // __LITERALLIST_H__
