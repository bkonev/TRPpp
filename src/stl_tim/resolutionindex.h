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
#ifndef __RESOLUTIONINDEX_H__
#define __RESOLUTIONINDEX_H__

#include <map>
#include <vector>
#include <iterator>
#include "config.h"
#include "modules.h"
#include "misc/exception.h"
#include "misc/tracer.h"
#include "keycomparator.h"
#include "pclause.h"

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
            os << std::endl;
        }
    }; //class IndexInconsistent
} // namespace Exceptions

namespace PropositionalProver
{
    using namespace Exceptions;

    typedef std::vector<PClause> IndexCarier;

    class ResolutionIndex
    {
    private:
        // since I do have map of arrays, I do not want this class to
        // be a derivation of map. Internal type used instead
        typedef std::map<Literal, IndexCarier, index_key_comparator> index_t;
    public:
        struct resolution_const_iterator
        {
            resolution_const_iterator(const index_t::const_iterator& idxCurrent, 
                    const index_t::const_iterator& idxEnd, 
                    const IndexCarier::const_iterator& crrCurrent)
            : myIndexIterator(idxCurrent), myIndexEnd(idxEnd),
               myCarierIterator(crrCurrent)
            { }

            bool
            operator==(const resolution_const_iterator& __x) const
            {
                if((this->myIndexIterator == this->myIndexEnd) &&
                        (__x.myIndexIterator == this->myIndexEnd))
                {
                    return true;
                }
                else
                {
                    return ((myIndexIterator == __x.myIndexIterator) && 
                        (myCarierIterator == __x.myCarierIterator));
                }
            }

            bool
            operator!=(const resolution_const_iterator& __x) const
            {
                return ! this->operator==(__x);
            }

            typedef std::iterator_traits<IndexCarier::const_iterator>::value_type value_type;
            typedef std::iterator_traits<IndexCarier::const_iterator>::pointer pointer;
            typedef std::iterator_traits<IndexCarier::const_iterator>::reference reference;

            pointer
            operator-> () const
            {
                return &(operator*());
            }

            reference
            operator* () const
            {
                return *myCarierIterator;
            }

            resolution_const_iterator&
            operator++()
            {
                myCarierIterator++;
                if(myCarierIterator == (myIndexIterator->second.end()))
                {
                    myIndexIterator++;
                    if(myIndexIterator != myIndexEnd)
                    {
                        // else, who cares? :-)
                        myCarierIterator = (myIndexIterator->second.begin());
                    }
                }
                return *this;
            }

        private:
            index_t::const_iterator myIndexIterator, myIndexEnd;
            IndexCarier::const_iterator myCarierIterator;
        };

        typedef resolution_const_iterator const_iterator;
        typedef resolution_const_iterator iterator;
        typedef IndexCarier::value_type       value_type;
        typedef IndexCarier::difference_type  difference_type;
        typedef IndexCarier::size_type        size_type;
        typedef IndexCarier::reference        reference;
        typedef IndexCarier::const_reference  const_reference;
        typedef IndexCarier::pointer          pointer;

        const_iterator 
        begin() const
        {
            return const_iterator(index.begin(), index.end(),
                    index.begin()->second.begin());
        }

        const_iterator 
        end() const
        {
            index_t::const_iterator tmpIndex = index.end();
            return const_iterator(tmpIndex, tmpIndex, tmpIndex->second.end());
        }

        // ha-ha reference to pointer... But copying _these_ pointers costs
        // (it's shared)
        // suppose that clause is not empty
        void
        insert(const PClause& __x);

        void 
        remove(const PClause& __x);

        std::pair< IndexCarier::const_iterator, 
                   IndexCarier::const_iterator >
        getStartingWith(const Literal& l)  const;

        std::pair< IndexCarier::const_iterator, 
                   IndexCarier::const_iterator >
        getComplementary(Literal l)  const;

        std::ostream& 
        dump(std::ostream& os) const;
    private:
        index_t index;
    };
} // namespace PropositionalProver

#endif // __RESOLUTIONINDEX_H__
