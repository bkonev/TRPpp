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
#include "stl_tim.h"
#include "subsumptionindex.h"
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include "misc/i2s.h"
#include "interrupthandler.h"

namespace PropositionalProver
{
    std::pair<bool, bool>
    ClauseSet::saturate(const ClauseSet::Flags& f)
    {
        Active& active = get_first();    // pritty names
        Passive& passive = get_second();

        bool anythingNew = false;

        while (passive.size())
        {
            SignalHandling::processPending();
            // getting the next clause

//                        TRACE(resolutionModule, {
//                        std::cerr << "Selecting from:" << std::endl;
//                        passive.dump(std::cerr);
//                        std::cerr <<  std::endl;
//                        });
            PClause current = passive.select();
                         TRACE(resolutionModule, {
                                 std::cerr << "Selected clause: ";
                                 current->dump(std::cerr);
                                 std::cerr << std::endl;
                                 });
            // checking whether it is redundand: DISCOUNT algorithm
            unsigned int tmpResult = active.forwardSubsumption(current);
            if (tmpResult)
            {
#ifdef COLLECT_STAT
                ourStatistics.addForwardSubsumptions(1);
#endif //COLLECT_STAT
                        TRACE(resolutionModule, {
                                std::cerr << "Clause ";
                                current->dump(std::cerr); 
                                std::cerr << " is subsumed by active clause " <<
                                    tmpResult << std::endl;
                                });
                continue;
            }
            // else
            // simplifying Active by current..
            anythingNew |= active.backwardSubsumption(current, ourStatistics);
            // adding to active..
            active.push_back(current);
            //std::cerr << "going further" << std::endl;
            //        active.dump(std::cerr);
            // generating of new clauses...
            // returns a pointer on a list of generated clauses
            std::list<PClause> newlist;
            active.resolve(current, newlist);

            bool hasEmptyClause = false;
            for (std::list<PClause>::const_iterator it = newlist.begin();
                    it != newlist.end();
                    ++it)
            {
                if ((*it)->size() == 0)
                {
                    hasEmptyClause = true;
                    break;
                }
            }

            // Index new clauses once to avoid O(n^2) inner subsumption checks.
            // Build candidate buckets by length for exact fallback.
            // SubsumptionIndex cannot safely index empty clauses.
            SubsumptionIndex innerIndex;
            struct Candidate
            {
                Literal leading;
                PClause clause;
                Candidate(const Literal& l, const PClause& c)
                : leading(l), clause(c)
                { }
            };
            std::vector<std::vector<Candidate> > candidatesBySize;
            std::unordered_map<clauseid_t, PClause> idToClause;
            std::unordered_set<Clause*> removedCandidates;
            const bool useInnerIndex = (newlist.size() > 1) && (!hasEmptyClause);
            if (useInnerIndex)
            {
                for (std::list<PClause>::const_iterator it = newlist.begin();
                        it != newlist.end();
                        ++it)
                {
                    if ((*it)->size() != 0)
                    {
                        const size_t len = (*it)->size();
                        innerIndex.insert(*it);
                        if (candidatesBySize.size() <= len)
                        {
                            candidatesBySize.resize(len + 1);
                        }
                        candidatesBySize[len].push_back(
                            Candidate((*it)->getLeadingLiteral(), *it));
                        idToClause[(*it)->getId()] = *it;
                    }
                }
            }
//TRACE(resolutionModule, {
  //std::cerr << "newlist befoure simplifications:";
  //newlist.dump(std::cerr);
  //std::cerr << endl;
//});
            bool emptyKept = false;
            for (std::list<PClause>::iterator p = newlist.begin();
                    p != newlist.end();)
            {
                SignalHandling::processPending();
#ifdef COLLECT_STAT
                ourStatistics.addResolvents(1);
                if(((*p)->size() !=0) && ((*p)->getLeadingLiteral().getAttribute() == PropositionalProver::universal_attr))
                {
                    ourStatistics.addUniversalResolvents(1);
                }
#endif //COLLECT_STAT
                bool redundand = false;
                bool innerSubsumed = false;
                // inner simplification of new clauses...
                if ((*p)->size() == 0)
                {
                    if (emptyKept)
                    {
#ifdef COLLECT_STAT
                        ourStatistics.addIndividualSubsumptions(1);
#endif //COLLECT_STAT
                        innerSubsumed = true;
                        redundand = true;
                    }
                    else
                    {
                        emptyKept = true;
                    }
                }
                else if (hasEmptyClause)
                {
#ifdef COLLECT_STAT
                    ourStatistics.addIndividualSubsumptions(1);
#endif //COLLECT_STAT
                    innerSubsumed = true;
                    redundand = true;
                }
                else if (useInnerIndex)
                {
                    innerIndex.remove(*p); // exclude self
                    unsigned int indexId = innerIndex.forwardSubsumes(*p);
                    if (indexId)
                    {
                        std::unordered_map<clauseid_t, PClause>::iterator cand =
                            idToClause.find(indexId);
                        if (cand != idToClause.end() &&
                                (removedCandidates.find(cand->second.get()) == removedCandidates.end()) &&
                                clauseSubsumes(cand->second, *p))
                        {
#ifdef COLLECT_STAT
                            ourStatistics.addIndividualSubsumptions(1);
#endif //COLLECT_STAT
                            innerSubsumed = true;
                            redundand = true;
                        }
                    }
                    if (!innerSubsumed)
                    {
                        const size_t maxSize = (*p)->size();
                        for (size_t len = 1; (len <= maxSize) && (!innerSubsumed); ++len)
                        {
                            if (len >= candidatesBySize.size())
                            {
                                continue;
                            }
                            const std::vector<Candidate>& bucket = candidatesBySize[len];
                            for (std::vector<Candidate>::const_iterator candIt = bucket.begin();
                                    candIt != bucket.end();
                                    ++candIt)
                            {
                                if (candIt->clause.get() == (*p).get())
                                {
                                    continue;
                                }
                                if (removedCandidates.find(candIt->clause.get()) != removedCandidates.end())
                                {
                                    continue;
                                }
                                if (clauseSubsumes(candIt->clause, *p))
                                {
#ifdef COLLECT_STAT
                                    ourStatistics.addIndividualSubsumptions(1);
#endif //COLLECT_STAT
                                    innerSubsumed = true;
                                    redundand = true;
                                    break;
                                }
                            }
                        }
                    }
                }

                if (!redundand)
                {
                    // simplification of new clauses by active
                    tmpResult = active.forwardSubsumption(*p);
                    if(tmpResult)
                    {
#ifdef COLLECT_STAT
                        ourStatistics.addForwardSubsumptions(1);
#endif //COLLECT_STAT
                        redundand = true;
                        TRACE(resolutionModule, {
                                std::cerr << "Clause ";
                                current->dump(std::cerr); 
                                std::cerr << " is subsumed by active clause " <<
                                    tmpResult << std::endl;
                                });
                    }
                    else
                    {
                        tmpResult = passive.forwardSubsumption(*p);
                        if(tmpResult)
                        {
#ifdef COLLECT_STAT
                            ourStatistics.addForwardSubsumptions(1);
#endif //COLLECT_STAT
                            redundand = true;
                            TRACE(resolutionModule, {
                                    std::cerr << "Clause ";
                                    current->dump(std::cerr); 
                                    std::cerr << " is subsumed by passive clause " <<
                                        tmpResult << std::endl;
                                    });
                        }
                    }
                }

                if (redundand)
                {
                    if (useInnerIndex)
                    {
                        removedCandidates.insert((*p).get());
                    }
                    p = newlist.erase(p);
                }
                else 
                {
                    if (useInnerIndex)
                    {
                        innerIndex.insert(*p);
                    }
                    ++p;
                }
            }
            // put what rests into passive
//TRACE(resolutionModule, {            
  //std::cerr << "newlist after simplifications:";
  //newlist.dump(std::cerr);
  //std::cerr << endl;
//});
            bool success = false; //is empty clause generated?
            std::list<PClause>::const_iterator cpend = newlist.end();
            for (std::list<PClause>::const_iterator p = newlist.begin();
                    p != cpend;
                    ++p)
            {
                SignalHandling::processPending();
                // only if we put anything new into passive, 
                // we consider that something new was generated
                anythingNew = true;
                if ((*p)->size() == 0)
                    success = true;
                // simplifying Active by the newly generated clauses
                // depricated: it was not here all the time and works 
                // resonably well...
                // active.backwardSubsumption(*p, ourStatistics);
                // 
                //
                //if (((!success) && (f.useFSR)) && (!(f.loopSearch&&((*p)->getLeadingLiteral().getAttribute() == step_now_attr))))
                if (((!success) && (f.useFSR)) && (!f.loopSearch))
                {
                    // SimplifyByFS
                    //
                    // for efficiency
                    for(Clause::iterator tCi = (*p)->begin();
                            tCi != (*p)->end(); )
                    { 
                        SignalHandling::processPending();
                        // if in loop search,
                        // do not allow resolutions in step_now part 
                        // (goes into next loop candidate)
                        //if(f.loopSearch&&(tCi->getAttribute() == step_now_attr))
                        //{
                        //    break;
                        //}
std::string keptTmpStr;
TRACE(FSRModule, {
        std::ostringstream ost;
        (*p)->dump(ost);
        keptTmpStr = ost.str();

        /*std::cerr << "FSR Current literal: ";
        tCi->dump(std::cerr);
        std::cerr << std::endl;*/
        });
                        tCi->negate();
/*
        std::cerr << "FSR negated literal: ";
        tCi->dump(std::cerr);
        std::cerr << std::endl << "*p = ";
        (*p)->dump(std::cerr);
        std::cerr << std::endl;
*/
                        SortedLiteralList tmpSL;
                        for(Clause::const_iterator litIt = (*p)->begin();
                                litIt != (*p)->end();
                                ++litIt)
                        {
                            tmpSL.push_back(*litIt);
                        }
                        PClause tmpClause(new Clause(tmpSL));
/*
TRACE(FSRModule, {
        std::cerr << "tmpclause = ";
        tmpClause -> dump(std::cerr);
        std::cerr << std::endl;
        });
*/
                        tmpResult = active.forwardSubsumption(tmpClause);
                        // changing it back
                        tCi->negate();
                        if(tmpResult)
                        {
                            tCi = (*p)->erase(tCi);
/*
        std::cerr << std::endl << "*p = ";
        (*p)->dump(std::cerr);
        std::cerr << std::endl;
*/
                            (*p)->addInfo(std::string("+FSR(")+itos(tmpResult)+")");
                            if ((*p)->size() == 0)
                                success = true;
TRACE(FSRModule, {
                                    std::cerr << "clause " << keptTmpStr;
                                    //(*p)->dump(std::cerr); 
                                    std::cerr << " simplified by FSR(" << tmpResult <<") to " ;
                                    (*p)->dump(std::cerr);
                                    std::cerr << std::endl;
});
#ifdef COLLECT_STAT
                            ourStatistics.addFSR(1);
#endif //COLLECT_STAT
                        }
                        else
                        {
                            ++tCi;
                        }
                    }
                }

                passive.push_back(*p); // still, we do not want to loos
                                       // the generated clauses (might
                                       // be useful)
            }
            if (success)
                // it is always the case: if empty clause is here, it's a 
                // new information
                return std::make_pair(true, true);
            // the following command would put everything into passive,
            // but we need to ch
            //std::copy(newlist.begin(),newlist.end(),back_inserter(passive));
        } // while(passive.size())
        return std::make_pair(false, anythingNew); // passive is empty, 
                                                   // empty clause not derived
    }
    
    std::ostream& ClauseSet::dump(std::ostream& os) const    
    {
        os << "order(" ;
        Proposition::const_iterator i = Proposition::begin();
        os << (i++)->second;
        for(; i != Proposition::end(); ++i)
        {
            os << ", " << i->second;
        }
        os << ")." << std::endl;
        get_first().dump(os);
        get_second().dump(os);
        return os;
    }

    Statistics ClauseSet::ourStatistics;
}
