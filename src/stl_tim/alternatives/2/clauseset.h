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
#include <memory>
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

    typedef std::shared_ptr<Clause> PClause;

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

    typedef std::list<PClause> ClauseSetBase;
    typedef std::list<PClause> IndexCarier;
    class IndexedClauseSet : private ClauseSetBase
    {
    public:
        //clauses are kept in a list (may be reaplaces with anything)
        typedef std::map<Literal, IndexCarier, index_key_comparator> index_t;
    private:
        index_t index;
    public:
        typedef ClauseSetBase::iterator iterator;
        typedef ClauseSetBase::const_iterator const_iterator;
        iterator begin() { return ClauseSetBase::begin(); }
        iterator end() { return ClauseSetBase::end(); }
        const_iterator begin() const { return ClauseSetBase::begin(); }
        const_iterator end() const { return ClauseSetBase::end(); }

        bool empty() { return ClauseSetBase::empty(); }

        size_type
        max_size () 
        { 
            size_type s1 = ClauseSetBase::max_size();
            size_type s2 = index.max_size();
            return (s1>s2?s2:s1);  //MIN
        }

        size_type
        size()
        {
            return ClauseSetBase::size();
        }

        reference front() { return ClauseSetBase::front(); }
        const_reference front() const { return ClauseSetBase::front(); }

        reference back() { return ClauseSetBase::back(); }
        const_reference back() const { return ClauseSetBase::back(); }

        // ha-ha reference to pointer... But copying _these_ pointers costs
        // (it's shared)
        // suppose that clause is not empty
        iterator
        insert(iterator __position, const PClause& __x) 
        {
            iterator i = ClauseSetBase::insert(__position, __x);
            // We index maximal literals only. Note that in the propositional
            // case there _is_ and only one maximal literal in each clause
            // old version..
            //index.insert(std::make_pair(__x->getLeadingLiteral(), __x));
            //new one:
            if(__x->size())
            {
                index[__x->getLeadingLiteral()].push_back(__x);
            }
            else
            {
                index[fakeLiteral()].push_back(__x);
            }
            return i;
        }
        // suppose that clause is not empty
        iterator
        erase(iterator __position)
        {
//DEBUG            std::cerr << "LeadingLiteral = ";
//DEBUG            (*__position)->getLeadingLiteral().dump(std::cerr);
//DEBUG            std::cerr<< std::endl;
            index_t::iterator pos = 
                index.find((*__position)->getLeadingLiteral());
            if (pos == index.end())
                throw (IndexInconsistent((*__position)->getLeadingLiteral()));
//DEBUG            std::cerr << "index found" << std::endl;
            bool erased = false;
            //seeking for an occurrence into index...
            for (IndexCarier::iterator p = (pos->second).begin(); p != (pos->second).end(); ++p )
                if (*p == *__position)
                {
                    (pos->second).erase(p);
//DEBUG                    std::cerr << "erased from index";
                    erased = true;
                    break;
                }
            if (!erased)
                throw (IndexInconsistent((*__position)->getLeadingLiteral())); 
            else
            {
                if ((pos->second).size() == 0)
                    index.erase(pos);
//DEBUG                std::cerr << "going to erase from list" << std::endl;
                return ClauseSetBase::erase(__position); //finaly
            }
        }
        void
        clear()
        {
            ClauseSetBase::clear();
            index.clear();
        }
        void
        push_front(const PClause& __x)
        {
            insert(begin(), __x);
        }
        void
        push_back(const PClause& __x)
        {
            insert(end(), __x);
        }
        void
        pop_front()
        {
            erase(begin()); 
        }
        void
        pop_back()
        {
            erase(end()); 
        }

        std::pair< std::list<PClause>::const_iterator, 
                   std::list<PClause>::const_iterator >
        getStartingWith(const Literal& l)  const
        {
            typedef std::list<PClause> ClauseList;
            ClauseList list;
            ClauseList::const_iterator lb = list.begin();
            ClauseList::const_iterator le = list.end();
            index_t::const_iterator pos = index.find(l);
            return (pos != index.end()) ? std::make_pair((pos->second).begin(),(pos->second).end()) : std::make_pair(lb, le);
        }

        std::ostream& 
        dump(std::ostream& os) const
        {
            //
            for (const_iterator p = this->begin();
                    p != this -> end(); 
                    ++p)
            {
                os << "   ";
                (*p)->dump(os);
                os << endl;
            }
#ifdef DEBUG
            os << "Index:  " << endl;
            for (index_t::const_iterator p = index.begin();
                    p != index.end();
                    ++p)
            {
                os << "   ";
                (p->first).dump(os);
                os << " ----> ";
                for(IndexCarier::const_iterator it = p->second.begin();
                        it != p->second.end();
                        it++)
                    (*it)->dump(os);
                os << endl;
            }
#endif
            return os;
        }
        Literal 
        fakeLiteral()
        {
            return Literal(Proposition(""));
        };
    }; //class IndexedClauseSet

    class Active : public IndexedClauseSet
    {
    public:
        std::pair< std::list<PClause>::const_iterator, 
                   std::list<PClause>::const_iterator >
        getComplementary(Literal l)  const
        {
            l.negate();
            return getStartingWith(l);
        }
        std::ostream& 
        dump(std::ostream& os) const
        {
            //
            os << "Active:  " << endl;
            return IndexedClauseSet::dump(os);
        }
    }; // class Active

    // this is not really effective, but for now, let it be

    class Passive: public IndexedClauseSet
    {
    public:
    
        PClause
        select()
        {
            PClause c = *begin();
            erase(begin());
            return c;
        }
        std::ostream& 
        dump(std::ostream& os) const
        {
            //
            os << "Passive:  " << endl;
            return IndexedClauseSet::dump(os);
        }
    }; //class Passive
} //namespace PropositionalProver
#endif // __CLAUSESET_H__
