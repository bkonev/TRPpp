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
#ifndef __CLAUSE_H
#define __CLAUSE_H
#include <iostream>
#include <algorithm>
#include <vector>
#include "literal.h"
#include "literallist.h"
#include "symbol.h"
#include "config.h"
#include <boost/pool/object_pool.hpp>
#include "misc/assert.h"


namespace PropositionalProver
{
    typedef std::vector< Literal > ClauseBase;
    class Clause: public ClauseBase
    {
    private:
        // some other info to add
        clauseid_t myId;
        std::string myInfo; //
        static clauseid_t id_unused;            // First currently not used id  
        static const clauseid_t id_none = 0;    // Non-existent symbol    
        static const clauseid_t init_unused = 1;// First not used id on startup 
        static const clauseid_t id_last = static_cast<clauseid_t>(-1); // Max possible id
        static clauseid_t get_unused_id()
        {
            Assert(id_unused != id_last, "maximum symbol id reached");
            return id_unused++;
        }

        typedef boost::object_pool<Clause> ClausePool;
        static ClausePool ourPool;
    public:
        typedef ClauseBase::iterator iterator;
        typedef ClauseBase::const_iterator const_iterator;
        //constructor
        Clause(const SortedLiteralList& sortedLiteralList, 
                const std::string& info=std::string(""))
        : ClauseBase(sortedLiteralList.begin(),sortedLiteralList.end()), 
          myId(get_unused_id()), myInfo(info) 
        { }

        //needed for effective select
        int 
        getWeight() const
        {
            return size();
        }

        //accessing information via iterators + the following
        const Literal&
        getLeadingLiteral() const
        {
            return front();
        }

        const clauseid_t& 
        getId() const
        {
            return this->myId;
        }
        const std::string&
        getInfo() const
        {
            return this->myInfo;
        }

        void
        addInfo(const std::string& info) 
        {
            myInfo += info;
        }
        // Returns true if the clause is correct (does not have any
        // contrary literals)
        // This function must be called befoure inserting a clause
        // into a clause collection unless the correctness is guaranteed
        // by other means
        //
        // Presumes that the literals are sorted within the clause
        bool
        isCorrect() const
        {
            const_iterator j = this->begin();
            const_iterator clauseEnd = this->end();
            if (j==clauseEnd) { return true; }

            const_iterator i = j++; // get value and move
            for(; j != clauseEnd; ++i, ++j)
            {
                // as literal are ordered, contrary pair might occure only
                // in raw 
                // I cannot use areContrary() as that function 
                // checks whether attributes are compatible, 
                // while this needs them to be exactly the same
                if((i->getSign() != j->getSign())&&
                   (i->getProposition() == j->getProposition())&&
                   (i->getAttribute() == j->getAttribute()))
                {
                    // not correct
                    return false;
                }
            }
            // else
            return true;
        }

        //raw printing
        std::ostream& dump(std::ostream& os) const
        {
            typedef const_iterator CI;
            os << " {" << myId << ": " << myInfo;
            for (CI p = this->begin(); p != this->end(); ++p)
            {
                p->dump(os);
            }
            os << "} ";
            return os;
        }

        // memory management (in chunks)
        void*
        operator new(size_t)
        {
            return ourPool.malloc();
        }

        void
        operator delete(void* p, size_t)
        {
            // boost requires the argument of free to be of the
            // type Node*, not void*
            return ourPool.free(static_cast<Clause *>(p));
        }

    };
} //namespace PropositionalProver

#endif // __CLAUSE_H
