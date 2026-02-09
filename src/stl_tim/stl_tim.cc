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
//TRACE(resolutionModule, {
  //std::cerr << "newlist befoure simplifications:";
  //newlist.dump(std::cerr);
  //std::cerr << endl;
//});
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
                // innner simplification of new clauses...
                if (rangeSubsumes(newlist.begin(),newlist.end(),*p))
                {
#ifdef COLLECT_STAT
                    ourStatistics.addIndividualSubsumptions(1);
#endif //COLLECT_STAT
                    redundand = true;
                }
                else 
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
                    p = newlist.erase(p);
                }
                else 
                    ++p;
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
                        SortedLiteralList tmpSL((*p)->begin(),(*p)->end());
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
