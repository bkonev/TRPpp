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
#ifndef __CLAUSESET_H__
#define __CLAUSESET_H__

#include <queue>
#include <vector>
#include "misc/exception.h"
#include "boost/smart_ptr.hpp"
#include "boost/iterator_adaptors.hpp"
#include "config.h"

namespace  Exceptions
{
    class ClauseSetException : public Exception
    {
    public:
        virtual void 
        module(std::ostream& os) const
        {
            os << "Clause set";
        }
    }; // class ClauseSetException
    class IndexInconsistent : public ClauseSetException
    {
    private:
        const PropositionalProver::Literal lit;
    public:
        IndexInconsistent(const PropositionalProver::Literal& l)
        : lit (l)
        { }
        virtual void 
        what (std::ostream& os) const
        {
            os << "Index inconsistent for literal: ";
            lit.dump(os);
            os << endl;
        }
    }; //class IndexInconsistent
} // namespace Exceptions

namespace PropositionalProver
{
    using namespace Exceptions;
    using std::endl;

    typedef boost::shared_ptr<Clause> PClause;

    // used for index construction
    class index_key_comparator{
    public:
        bool 
        operator() (const Literal& l, const Literal& m) const
        {
            // only proposition and its sign contribute but not
            // attribute!
            return ((l.getProposition() < m.getProposition()) || 
                    ( (l.getProposition() == m.getProposition()) && 
                      l.isPositive() && 
                      m.isNegative()));
        }
    };

    typedef std::multimap<Literal, PClause, index_key_comparator> IndexedClauseSetBase;

    class IndexedClauseSet : public IndexedClauseSetBase
    {
    private:
        struct selectPClause
        {
            typedef IndexedClauseSetBase::value_type argument_type;
            typedef argument_type::second_type result_type;
    
            const result_type&
            operator() (const argument_type& arg) const
            {
                return arg.second;
            }
    
            result_type&
            operator() (argument_type& arg) const
            {
                return arg.second;
            }
        };
    public:
        typedef boost::projection_iterator_pair_generator<selectPClause,
          IndexedClauseSetBase::iterator, IndexedClauseSetBase::const_iterator> PairGen;
        typedef PairGen::iterator iterator;
        typedef PairGen::const_iterator const_iterator;

        const_iterator
        begin() const
        {
            return const_iterator(IndexedClauseSetBase::begin());
        }
        iterator
        begin()
        {
            return iterator(IndexedClauseSetBase::begin());
        }

        const_iterator
        end() const
        {
            return const_iterator(IndexedClauseSetBase::end());
        }
        iterator
        end()
        {
            return iterator(IndexedClauseSetBase::end());
        }

        void 
        push_back(const PClause& pc)
        {
            // there is one "bad" case: when pc is an empty clause, it 
            // has no leading literal...
            if(pc->size())
            {
                IndexedClauseSetBase::insert(std::make_pair(pc->getLeadingLiteral(), pc));
            }
            else
            { // empty clause
                IndexedClauseSetBase::insert(std::make_pair(fakeLiteral(), pc));
            }
        }

        iterator
        erase(iterator __position)
        {
            IndexedClauseSetBase::iterator newPosition = __position.base();
            IndexedClauseSetBase::iterator erasingPosition = newPosition++;
            IndexedClauseSetBase::erase(erasingPosition);
            return iterator(newPosition);
        }
    
        std::pair< const_iterator, 
                   const_iterator >
        getStartingWith(const Literal& l)  const
        {
            std::pair<IndexedClauseSetBase::const_iterator, IndexedClauseSetBase::const_iterator>
                beginend = equal_range(l);
            return std::make_pair<const_iterator, const_iterator>(const_iterator(beginend.first), const_iterator(beginend.second));
        }
        std::ostream& dump(std::ostream& os)const
        {
            for (const_iterator p = this->begin();
                    p != this->end();
                    ++p)
            {
                os << "   ";
                (*p)->dump(os);
                os << endl;
            }
            return os;
        }
        Literal 
        fakeLiteral()
        {
            return Literal(Proposition(""));
        };
    };

    class Active : public IndexedClauseSet
    {
    public:
        typedef PClause value_type;
        std::pair< const_iterator, 
                   const_iterator >
        getComplementary(Literal l)  const
        {
            l.negate();
            return getStartingWith(l);
        }
        std::ostream& dump(std::ostream& os) const
        {
            os << "Active: " << endl;
            return IndexedClauseSet::dump(os);
        }
    }; //class Active


    class Passive: public IndexedClauseSet
    {
    public:
        typedef PClause value_type;
        PClause
        select()
        {
            PClause c = *begin();
            erase(begin());
            return c;
        }

        std::ostream& dump(std::ostream& os) const
        {
            os << "Passive: " << endl;
            return IndexedClauseSet::dump(os);
        }
    }; //class Passive
} //namespace PropositionalProver
#endif // __CLAUSESET_H__
