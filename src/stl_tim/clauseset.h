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

#include <vector>
#include <set>
#include "misc/exception.h"
#include "config.h"
#include "modules.h"
#include "misc/tracer.h"
#include "keycomparator.h"
#include "pclause.h"
#include "infer.h"
#include "timstat.h"
#include "subsumptionindex.h"
#include "resolutionindex.h"

namespace PropositionalProver
{
    // since no clause is kept twice in the resolution index, I use
    // that class as a base class for Active
    class Active
    {
    public:
        typedef ResolutionIndex::const_iterator   const_iterator;
        typedef ResolutionIndex::iterator         iterator;
        typedef ResolutionIndex::value_type       value_type;
        typedef ResolutionIndex::difference_type  difference_type;
        typedef ResolutionIndex::size_type        size_type;
        typedef ResolutionIndex::reference        reference;
        typedef ResolutionIndex::const_reference  const_reference;
        typedef ResolutionIndex::pointer          pointer;
    public:
        const_iterator 
        begin() const { return myResolutionIndex.begin(); }

        const_iterator
        end() const { return myResolutionIndex.end(); }

        void
        push_back(const PClause& pclause);

        /*void
        remove(const PClause& pclause);*/

        // Returns true if some clauses from Active were subsumed 
        // ought to clause
        bool
        backwardSubsumption(const PClause& clause, Statistics& stat);
    
        // Returns the number of a clause from Active that subsumes the given 
        // clause or 0
        unsigned int
        forwardSubsumption(const PClause& clause) const;

        std::pair< IndexCarier::const_iterator, 
                   IndexCarier::const_iterator >
        getComplementary(Literal l)  const { return myResolutionIndex.getComplementary(l); }


        // assume that the clause c is not empty
        // returns all resolvents of c and clauses in Active
        void
        resolve(const PClause& c, std::list<PClause>& result) const;

        std::ostream& 
        dump(std::ostream& os) const;
    private:
        // data fields
        ResolutionIndex myResolutionIndex;
        SubsumptionIndex mySubsumptionIndex;
    }; //class Active

    // this is not really effective, but for now, let it be
    
    struct passiveClauseComparator
    {
        bool 
        operator() (const PClause& c1, const PClause& c2) const
        {
            // only proposition and its sign contribute but not
            // attribute!
            return ((c1->getWeight() < c2->getWeight()) || 
                    ((c1->getWeight() == c2->getWeight()) &&
                    (cmp(c1->getLeadingLiteral(), c2->getLeadingLiteral()) || 
                     ((c1->getLeadingLiteral().getProposition() == 
                          c2->getLeadingLiteral().getProposition() ) && 
                      (c1->getLeadingLiteral().getSign() == 
                          c2->getLeadingLiteral().getSign()) &&
                      (c1->getId() < c2->getId())))
                    )
                   );
        }
    private:
        static index_key_comparator cmp;
    };

    typedef std::set<PClause, passiveClauseComparator> PassiveBase;

    class Passive: public PassiveBase
    {
    public:
        typedef PassiveBase::iterator iterator;
        typedef PassiveBase::const_iterator const_iterator;

        const_iterator
        begin() const { return const_iterator(PassiveBase::begin()); }

        iterator
        begin() { return iterator(PassiveBase::begin()); }

        const_iterator
        end() const { return const_iterator(PassiveBase::end()); }

        iterator
        end() { return iterator(PassiveBase::end()); }

        void 
        push_back(const PClause& pc);
    
        PClause
        select();

/*        std::pair< const_iterator, 
                   const_iterator >
        getStartingWith(const Literal& l)  const;*/

        // Returns true if some clauses from Passive were subsumed 
        // ought to clause
        bool
        backwardSubsumption(const PClause& clause, Statistics& stat);

        // Returns the number of a clause from Passive that subsumes the given 
        // clause or 0
        unsigned int
        forwardSubsumption(const PClause& clause) const;

        std::ostream& 
        dump(std::ostream& os) const;

    private:
        Literal 
        fakeLiteral() { return Literal(Proposition("")); };

        SubsumptionIndex mySubsumptionIndex;
    }; //class Passive
} //namespace PropositionalProver
#endif // __CLAUSESET_H__
