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
#ifndef __SEARCH_H__
#define __SEARCH_H__

#include <iostream>
#include "trim/trim.h"
#include "temporal_resolution/temporalrule.h"
#include "temporal_resolution/classicalrule.h"
//#include "temporal_resolution/DFKrule.h"
#include "temporalstat.h"
#include "getopt/options.h"
#include "modules.h"
#include "interrupthandler.h"

namespace TemporalProver
{

    // this class is instantiated with classes that implement
    // basic Clause operations (ClauseSet), loopsearch (LoopList), and
    // temporal resolution (TemporalRule)
    //
    template<typename ClauseSet, typename LoopList, typename TemporalRule>
    class Prover
    {
    public:
        // constructor: gets the clause set
        Prover(const ClauseSet&);
        // search for contradiction
        bool 
        search(const TRPPPOptions&);

        // used by outputStatistics
        const TemporalStatistics&
        getStatistics() const
        {
            return myStatistics;
        }

        // returns the kept clause set (used for e.g. output)
        const ClauseSet&
        getClauseSet() const
        {
            return myClauseSet;
        }

        // prints statistics
        void 
        outputStatistics(std::ostream& os) const
        {
            os << this->getClauseSet().getInternalStatistics();
            os << this->getStatistics();
        }

    private:
        // private data structures: 
        // statistics
        TemporalStatistics myStatistics;
        // set of clauses
        ClauseSet myClauseSet;
    };


    // IMPLEMENTATION
    template<typename ClauseSet, typename LoopList, typename TemporalRule>
    Prover<ClauseSet, LoopList, TemporalRule>::Prover(const ClauseSet& clauseSet)
    : myClauseSet(clauseSet)
    { }

    template<typename ClauseSet, typename LoopList, typename TemporalRule>
    bool
    Prover<ClauseSet, LoopList, TemporalRule>::search(const TRPPPOptions& options)
    {
        // first, trying to solve by pure step resolution
        ClauseSet::setMode(ClauseSet::normal);
        std::pair<bool, bool> result = myClauseSet.saturate(options); // saturate 
                                                               // with step
                                                               // rules
        if (result.first)
            return true; // done
                                TRACE(tempresModule, {
								std::cerr << "after saturation by step rules"
                                               << std::endl;
								myClauseSet.dump(std::cerr);
								std::cerr << std::endl;
                                });
        // this fails.
        //
        // Then, augmentation
        TemporalRule::preprocess(myClauseSet);
                                TRACE(tempresModule, {
								std::cerr<< "after augmentation" << std::endl;
								myClauseSet.dump(std::cerr);
								std::cerr << std::endl;
                                });
        // again, saturate with step rules
        result = myClauseSet.saturate(options); 
        if (result.first)
                return true; // should not work, but who knows..
                                TRACE(tempresModule, {
								std::cerr << "after saturation by step rules" <<
                                                std::endl;
								myClauseSet.dump(std::cerr);
								std::cerr << std::endl;
                                });
        // main loop
        while(true)
        {
            SignalHandling::processPending();
            bool somethingNew = false;
            bool pretestOk = true;
            // for each eventuality clause
            // for optimality:
            typename ClauseSet::const_e_iterator iend =
                                            myClauseSet.eventuality_end();
            typename ClauseSet::const_e_iterator ibegin =
                                            myClauseSet.eventuality_begin();

            // At each iteration of the main loop, we test whether the 
            // left-hand-side at least one eventuality rule is not 
            // guaranteed to be false.
            if (options.isUsePretest())
            {
                for(typename ClauseSet::const_e_iterator i = ibegin;
                        i != iend;
                        ++i)
                {
                    if(! myClauseSet.notMaximal(i->present_begin(), i->present_end()))
                    {
                                TRACE(pretestModule, {
                                std::cerr << "Pretest: negation of one of ";
                                for(typename ClauseSet::EClause::const_present_iterator iii = i->present_begin(); iii != i->present_end(); ++iii)
                                {
                                   iii->dump(std::cerr);
                                   std::cerr << " ";
                                }
                                std::cerr << "is maximal in the cluase set." << std::endl;
                                });
                        pretestOk = true;                 
                        break;
                    }
                }
            TRACE(pretestModule, {
                    std::cerr << (pretestOk ? "Pretest passed: continue search"
                            : "Pretest failed: satisfiable") << std::endl;
                    });
            }

            if(! pretestOk)
            {
                return false; // our pretest shows that none of the 
                              // possible eventuality rules can be
                              // applies

            }

            for(typename ClauseSet::const_e_iterator i = ibegin;
                    i != iend;
                    ++i)
            {
                SignalHandling::processPending();
                // pre-test on whether we want to find this loop
                // if any of the literals in the list are not maximal,
                // we do not need to consider this eventuality fule
                // trying to find a loop in i->getEventuality()
                LoopList loopList(myClauseSet, i->getEventuality(), options);
                bool loopExists = loopList.search();
#ifdef COLLECT_STAT
                myStatistics.addToLoopSearches(1);
                if(loopExists)
                {
                    myStatistics.addToSuccessfulLoopSearches(1);
                }
#endif // COLLECT_STAT
TRACE(loopsearchModule, {
								if (loopExists)
								{
								std::cerr << "Loop found for eventuality literal " ;
								i->getEventuality().dump(std::cerr);
								std::cerr << std::endl;
								std::cerr << "Loop:" << std::endl;
								for (typename LoopList::const_iterator iii = loopList.begin();
								         iii != loopList.end();
								         ++iii)
								    iii->getInternalRep()->dump(std::cerr);
								std::cerr << std::endl;
								}
								else
								{
								
								std::cerr << "no loop for " << std::endl;
								i->getEventuality().dump(std::cerr);
								std::cerr << std::endl;
								}
});
                if (loopExists)
                {
                    TemporalRule::generateResolvents(loopList.begin(), loopList.end(), *i, myClauseSet);
TRACE(tempresModule, {
								std::cerr << "after temporal resolution application " << std::endl;
								myClauseSet.dump(std::cerr);
								std::cerr << std::endl;
});
                    ClauseSet::setMode(ClauseSet::normal);
                    // checking whether it gives a proof or is really new..
                    result = myClauseSet.saturate(options); // saturate with step rules
TRACE(tempresModule, {
								std::cerr << "after saturation by step rules" << std::endl;
								myClauseSet.dump(std::cerr);
								std::cerr << std::endl;
});
                    if (result.first) // we deduced a contradiction
                    {
#ifdef COLLECT_STAT
                        myStatistics.addToNonredundandLoopSearches(1);
#endif
                        return true;
                    }
                    if(result.second) // no condradiction, but generated
                                      // clauses are not subsumed by old ones
                    {
                        myStatistics.addToNonredundandLoopSearches(1);
                        somethingNew = true;
                        if(options.isDFS())
                        {
                            break;
                        }
                    }
                }
            }
            if (!somethingNew)
            {
TRACE(tempresModule, {
								std::cerr << "nothing new was generated" << std::endl;;
});
                return false; // nothing new -> no proof (clauseset is saturated)
            }
TRACE(tempresModule, {
								std::cerr << "after applications of temporal resolutions" << std::endl;
								myClauseSet.dump(std::cerr);
								std::cerr << std::endl;
});
        }
    }
} // namespace TemporalProver
#endif // __SEARCH_H__
