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
#include "clauseset.h"

namespace PropositionalProver
{
    void
    Active::push_back(const PClause& pclause) 
    {
        myResolutionIndex.insert(pclause);
        mySubsumptionIndex.insert(pclause); 
    }

    /*
    void
    Active::remove(const PClause& pclause)
    {
        // removed by the index itself
        //mySubsumptionIndex.remove(pclause);
        myResolutionIndex.remove(pclause);
    }
    */

    bool
    Active::backwardSubsumption(const PClause& clause, Statistics& stat) 
    {
        std::pair<SubsumptionIndex::BackwardSubsumptionList::iterator,
                  SubsumptionIndex::BackwardSubsumptionList::iterator>
                      ipair = mySubsumptionIndex.backwardSubsumes(clause);

        if(ipair.first != ipair.second)                 
        {
                            TRACE(resolutionModule, {
                                std::cerr << "Active clause(s)" ;
                                std::cerr << std::endl;
                            });
            for(SubsumptionIndex::BackwardSubsumptionList::iterator ii = ipair.first; ii != ipair.second; ++ii)
            {
                            TRACE(resolutionModule, {
                                (*ii)->dump(std::cerr);
                                std::cerr << "." << std::endl;
                            });
                myResolutionIndex.remove(*ii);
#ifdef COLLECT_STAT
                stat.addBackwardSubsumptions(1);
#endif
            }
                            TRACE(resolutionModule, {
                                std::cerr << "are backward subsumed because of";
                                clause->dump(std::cerr);
                                std::cerr << std::endl;
                            });
            return true;
        }
        else 
            return false;
    }

    unsigned int
    Active::forwardSubsumption(const PClause& clause) const
    {
        if(!clause->size())
            return 0; // it would be pitty to subsume the empty clause :-)
        unsigned int result = mySubsumptionIndex.forwardSubsumes(clause);

                            TRACE(resolutionModule, {
                                if(result)
                                {
                                    std::cerr << "clause " ;
                                    clause->dump(std::cerr);
                                    std::cerr << " is subsumed by active clause "<< result <<std::endl;
                                }
                            });
        return result;
//std::cerr << "forwardSubsumption: clause = ";
//clause->dump(std::cerr);
//std::cerr << std::endl;
//std::cerr << "new test says..." << (mySubsumptionIndex.forwardSubsumes(clause)?
/*
std::cerr  << (mySubsumptionIndex.forwardSubsumes(clause)?
        "Subsumed":"Not") << std::endl;
        Clause::const_iterator ciend = clause->end();
        for(Clause::const_iterator ci = clause->begin(); 
                ci != ciend; ++ci)
        {
            std::pair<IndexCarier::const_iterator,
                      IndexCarier::const_iterator> 
                          candidates = this->getStartingWith(*ci);
            if(rangeSubsumes(candidates.first, 
                        candidates.second, clause))
            {
std::cout << "Subsumed" << std::endl;
                return true;
            }
        }
std::cout << "Not" << std::endl;
        return false;
        */
    }

    std::ostream& 
    Active::dump(std::ostream& os) const
    {
        //
        os << "Active:  " << std::endl;
        for (const_iterator p = this->begin();
                p != this -> end(); 
                ++p)
        {
            os << "   ";
            (*p)->dump(os);
            os << std::endl;
        }
TRACE(indexModule, {
        os << "Index:  " << std::endl;
        myResolutionIndex.dump(os);
        os << "Subsumption Index:  " << std::endl;
        mySubsumptionIndex.dump(os);
});
        return os;
    }

    // assume that the clause c is not empty
    void
    Active::resolve(const PClause& c, std::list<PClause>& result) const
    {
        Clause* rc; //result clause
        Literal leading = c->getLeadingLiteral();
        std::pair< IndexCarier::const_iterator, 
                   IndexCarier::const_iterator >
            candidates = this->getComplementary(leading);
TRACE(resolutionModule, {
  std::cerr << "resolve called with" << std::endl;
  std::cerr << "Active (suppressed)";
  //this->dump(std::cerr);
  std::cerr << std::endl;
  c->dump(std::cerr);
  std::cerr << std::endl << "candidates are:";
  for (IndexCarier::const_iterator __p=candidates.first; 
          __p!=candidates.second; ++__p)
      (*__p)->dump(std::cerr);
  std::cerr << std::endl;
});
        for (IndexCarier::const_iterator p = candidates.first;
                p != candidates.second;
                ++p)
        {
            rc = resolveMaximals(*p, c);
            if(rc)
            {
                PClause prc(rc);
TRACE(resolutionModule, {
  std::cerr << "binary resolveMaximals produced:";
  prc->dump(std::cerr);
  std::cerr << std::endl;
});
                result.push_back(prc);
            }
        }
    } //resolve

    // IMPLEMENTATION: Passive
    void 
    Passive::push_back(const PClause& pc)
    {
        PassiveBase::insert(pc);
        mySubsumptionIndex.insert(pc); 
    }

    PClause
    Passive::select()
    {
        PClause c = *begin();
        erase(PassiveBase::begin());
        mySubsumptionIndex.remove(c);
        return c;
    }

    bool
    Passive::backwardSubsumption(const PClause& clause, Statistics& stat) 
    {
        std::pair<SubsumptionIndex::BackwardSubsumptionList::iterator,
                  SubsumptionIndex::BackwardSubsumptionList::iterator>
                      ipair = mySubsumptionIndex.backwardSubsumes(clause);

        if(ipair.first != ipair.second)                 
        {
                            TRACE(resolutionModule, {
                                std::cerr << "Passive clause(s)" ;
                                std::cerr << std::endl;
                            });
            for(SubsumptionIndex::BackwardSubsumptionList::iterator ii = ipair.first; ii != ipair.second; ++ii)
            {
                            TRACE(resolutionModule, {
                                (*ii)->dump(std::cerr);
                                std::cerr << "." << std::endl;
                            });

                PassiveBase::erase(*ii);
                
#ifdef COLLECT_STAT
                stat.addBackwardSubsumptions(1);
#endif
            }
                            TRACE(resolutionModule, {
                                std::cerr << "are backward subsumed because of";
                                clause->dump(std::cerr);
                                std::cerr << std::endl;
                            });
            return true;
        }
        else 
            return false;
    }

    unsigned int
    Passive::forwardSubsumption(const PClause& clause) const
    {
        if(!clause->size())
            return 0; // it would be pitty to subsume the empty clause :-)
        unsigned int result = mySubsumptionIndex.forwardSubsumes(clause);

                            TRACE(resolutionModule, {
                                if(result)
                                {
                                    std::cerr << "clause " ;
                                    clause->dump(std::cerr);
                                    std::cerr << " is subsumed by passive clause "  << result << std::endl;
                                }
                            });
        return result;
    }

    std::ostream& 
    Passive::dump(std::ostream& os) const
    {
        os << "Passive:" << std::endl;
        for (const_iterator p = this->begin();
                p != this->end();
                ++p)
        {
            os << "   ";
            (*p)->dump(os);
            os << std::endl;
        }
TRACE(indexModule, {
        os << "Subsumption Index:  " << std::endl;
        mySubsumptionIndex.dump(os);
});
        return os;
    }
    
    index_key_comparator passiveClauseComparator::cmp;

} // namespace PropositionalProver
