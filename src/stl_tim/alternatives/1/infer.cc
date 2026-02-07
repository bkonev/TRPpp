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
#include <utility>
#include "infer.h"
#include "config.h"

namespace PropositionalProver
{
    ////////////////////////////////////////////////////////////////////
    //
    //                         SUBSUMPTION                            //
    //
    // /////////////////////////////////////////////////////////////////
    //
    // I know that this is ugly, but it is vital for the performance.
    // The only thing I can do is to separated ATTRIBUTE-influenced 
    // staff into separate (implementationDependent...) functions.
    //
    // The problem: attributes encode terms.
    // initial             <-->   0
    // universal, step_now <-->   X
    // step_next           <--> S(X)
    // Hence, universal(step_now) clause can subsume a clause of any
    // type. Even more, there are clauses consisting of step_now and
    // step_next literals, thus, there could be two cases: Universal
    // clause subsumes a step clause by X->S(X), or by X->X substitutions.
    //
    // Thus, if the attribute of the leading literal 
    // is universal, we should consider cases.
    //
    // RECALL: all attributes but step_now, step_next are incompatible,
    // step_now is always less than step_next.
    //
    // Implementation SUGGESTS that clauses are ordered from maximal
    // to minimal literal!
    inline bool
    implementationDependentSubsumesHelper(const PClause& c, const PClause& d)
    {
        typedef Clause::const_iterator CI;
        CI p = c->begin();
        CI pend = c->end();
        CI q = d->begin(); 
        CI qend = d->end(); 
        //
        while ((p != pend) && (q != qend))
        {
            if (*p < *q) //prefix 
            {
                ++q;
            }
            else
            {
                if (*p == *q ) // Same literal
                {
                    ++p;
                    ++q;
                }
                else 
                {
// DEBUG
#ifdef SUBSUMPTION_DEBUG
std::cerr << "returning false" << std::endl;
#endif
// SUBSUMPTION_DEBUG
                    return false;
                }
            }
        }
        if (p == pend) 
        {
// SUBSUMPTION_DEBUG
#ifdef SUBSUMPTION_DEBUG
std::cerr << "returning true " << std::endl;
#endif
// SUBSUMPTION_DEBUG
            return true;
        }
        else 
        {
// SUBSUMPTION_DEBUG
#ifdef SUBSUMPTION_DEBUG
std::cerr << "returning false" << std::endl;
#endif
// SUBSUMPTION_DEBUG
            return false;
        }
    }

    inline bool
    implementationDependentSubsumes(const PClause& c, const PClause& d)
    {
// SUBSUMPTION_DEBUG
#ifdef SUBSUMPTION_DEBUG
std::cerr << "subsumption test for C= " ;
c->dump(std::cerr);
std::cerr << " D= ";
d->dump(std::cerr);
std::cerr << std::endl;
#endif
        // functionality depends on the attribute types of C and D
        Attribute::AttributeType leadingLiteralAttributeC(c->getLeadingLiteral().getAttribute().getType());
        Attribute::AttributeType leadingLiteralAttributeD(d->getLeadingLiteral().getAttribute().getType());


        // simple cases: initial and step_next
        // (a subtile thing: checks up to attribute comparator!)
        if (c->getLeadingLiteral().getAttribute() != universal_attr)
        {
#ifdef SUBSUMPTION_DEBUG
std::cerr << " simple case " <<  std::endl;
#endif
            if(leadingLiteralAttributeC == leadingLiteralAttributeD)
            {
                bool result = implementationDependentSubsumesHelper(c, d);
#ifdef SUBSUMPTION_DEBUG
std::cerr << " returning " << result << std::endl;
#endif
                return result;
            }
            else 
            {
// SUBSUMPTION_DEBUG
#ifdef SUBSUMPTION_DEBUG
std::cerr << "returning false" << std::endl;
#endif
// SUBSUMPTION_DEBUG
                return false;
            }
        }
        // else
        if (leadingLiteralAttributeD != Attribute::step_next)
        {
            return implementationDependentSubsumesHelper(c, d); 
        }
// SUBSUMPTION_DEBUG
#ifdef SUBSUMPTION_DEBUG
std::cerr << "The ugliest case.." << std::endl ;
#endif // SUBSUMPTION_DEBUG
// SUBSUMPTION_DEBUG

        //else
        // The ugliest thing: C is universal(step_now), D is step_next
        //
        // Once considering C as step_next, then as step_now
        //
        // Assume that C consists of step-next literals only
        typedef Clause::const_iterator CI;
        CI p = c->begin(); CI pend = c->end();
        CI q = d->begin(); CI qend = d->end(); 
        while ((p != pend) && (q != qend) && 
                (q->getAttribute() == step_next_attr))
        {
            if (p->getProposition() < q->getProposition())
                // prefix
            {
                ++q;
            }
            else 
            {
                if ((p->getProposition() == 
                            q->getProposition()) &&
                        (p->getSign() == q->getSign())) 
                    // Same literal
                {
                    ++p; ++q;
                }
                else 
                {
                    break; // not the case
                }
            }
        }
        if (p == pend) 
        {
// SUBSUMPTION_DEBUG
#ifdef SUBSUMPTION_DEBUG
std::cerr << "returning true" << std::endl;
#endif
// SUBSUMPTION_DEBUG
            return true; // this is the first case
        }
        // If we are here, the first case does not work
        // There is still a possibility that C subsumes D as step_now
        //
        // Assume that C consists of step_now literals only
// SUBSUMPTION_DEBUG
#ifdef SUBSUMPTION_DEBUG
std::cerr << "First ugliest case didn't work" << std::endl ;
#endif // SUBSUMPTION_DEBUG
// SUBSUMPTION_DEBUG
        p = c->begin(); 
        while ((q != qend) && (q->getAttribute() == step_next_attr))
            q++; // moving q until *q is a step_now literal
        while((p != pend) && (q != qend))
        {
            if (p->getProposition() < q->getProposition())
                // prefix
            {
                ++q;
            }
            else 
            {
                if ((p->getProposition() == 
                            q->getProposition()) &&
                        (p->getSign() == q->getSign())) 
                    // Same literal
                {
                    ++p; ++q;
                }
                else 
                {
// SUBSUMPTION_DEBUG
#ifdef SUBSUMPTION_DEBUG
std::cerr << "returning false" << std::endl;
#endif
// SUBSUMPTION_DEBUG
                    return false; // now we can be sure
                }
            }
        }
        if (p == pend) 
        {
// SUBSUMPTION_DEBUG
#ifdef SUBSUMPTION_DEBUG
std::cerr << "returning true" << std::endl;
#endif
// SUBSUMPTION_DEBUG
            return true; // this is the first case
        }
        else 
        {
// SUBSUMPTION_DEBUG
#ifdef SUBSUMPTION_DEBUG
std::cerr << "returning false" << std::endl;
#endif
// SUBSUMPTION_DEBUG
            return false; // now we are sure
        }
    }

    // returns true if clause C subsumes clause D
    inline bool
    clauseSubsumesHelper(const PClause& c, const PClause& d)
    {
// SUBSUMPTION_DEBUG
#ifdef SUBSUMPTION_DEBUG
std::cerr << "clauseSubsumesHelper: " << std::endl << "C = ";
c->dump(std::cerr);
std::cerr << std::endl << " D = ";
d->dump(std::cerr);
std::cerr << std::endl;
#endif
// SUBSUMPTION_DEBUG
        unsigned int cSize = c->size();
        if(!cSize)
        {
// SUBSUMPTION_DEBUG
#ifdef SUBSUMPTION_DEBUG
std::cerr << "     Empty clause subsumes everything" << std::endl;
#endif
// SUBSUMPTION_DEBUG
            return true; // the empty clause subsumes everything
        }
        // else
        if (cSize > d->size()) 
        {
// SUBSUMPTION_DEBUG
#ifdef SUBSUMPTION_DEBUG
std::cerr << "     C.size() > D.size(); returning false" << std::endl;
#endif
// SUBSUMPTION_DEBUG
            return false; // c must be a subset, but it greater in size..
        }
        // else
// SUBSUMPTION_DEBUG
#ifdef SUBSUMPTION_DEBUG
std::cerr << "     Calling implementationDependentSubsumes(c, d)" << std::endl;
#endif
// SUBSUMPTION_DEBUG
        return implementationDependentSubsumes(c, d);
    }

    // returns true if clause C subsumes clause D but they are 
    // not clauses with  the same id (needed for inner simplification)
    inline bool
    clauseSubsumes(const PClause& c, const PClause& d)
    {
        // simple checks first
        if (c->getId() == d->getId()) // c and d is the same entity (in inner
                                      // simplification)
            return false;
        bool result = clauseSubsumesHelper(c, d);
#ifdef RESOLUTION_DEBUG
// this is a part of resulution, hense, RESOLUTION_DEBUG
if(result)
{
std::cerr << "clause C = ";
c->dump(std::cerr);
std::cerr << std::endl << " subsumes clause D = ";
d->dump(std::cerr);
std::cerr << std::endl;
}
#endif
        return result;
    }

    // returns true if clause C subsumes clause D or they are
    // equal
    bool 
    clauseSubsumesOrEqual(const PClause& c, const PClause& d)
    {
        if (c->getId() == d->getId()) // this is faster then check subsumption
            return true;
        else
            return clauseSubsumesHelper(c, d);
    }
    //Iterator iterates overe a range of PClause
    template<typename Iterator>
    inline bool
    rangeSubsumes (Iterator __begin, Iterator __end, const PClause& d)
    {
        while (__begin != __end)
            if (clauseSubsumes(*__begin++, d)) // passing pointers 
                return true;
        return false;
    }
    ///////////////////////////////////////////////////////////////////////
    //
    //                        (Binary) RESOLUTION                        //
    //
    ///////////////////////////////////////////////////////////////////////


    // This function checks whether we are alowed to resolve upon this literal
    inline bool
    checkConditions(const Literal& l, const Literal& m)
    {
        if (areCompatible(l.getAttribute(),m.getAttribute()))
            return true;
        else 
            return false;
    }
    inline std::string 
    make_info(const clauseid_t id1, const clauseid_t id2)
    {
        std::ostringstream ost;
        ost << id1 << "." << id2;
        return ost.str();
    }
    // returns the resolvent of c and d or 0 if not constructed
    // l is the selected literal (resolve upon)
    // checks conditions on literals
    //
    // Never ever call this function by itself (bad return type!!)
    //
    // assume that neither of the clauses is empty
    inline Clause*
    resolveMaximals(const PClause& c, const PClause& d)
    {
        typedef Clause::const_iterator CI; //Clause contains literals themselves
#ifdef RESOLUTION_DEBUG
   std::cerr << "(binary) resolve called with " << endl;
   std::cerr << "C = ";
   c->dump(std::cerr);
   std::cerr << endl;
   std::cerr << "D = ";
   d->dump(std::cerr);
   std::cerr << endl;
#endif
        CI citer = c->begin();
        CI diter = d->begin();
        if (!checkConditions(*citer,*diter))
        {
#ifdef RESOLUTION_DEBUG
  std::cerr << "clauses cannot be resolved" << endl;
#endif
            return 0;
        }

        ASSERT(areComplementary(*citer,*diter), "resolveMaximals Called with wrong arguments");

        citer++; diter++; // first literals are maximal; we resolve on it
        CI cend  = c->end();
        CI dend  = d->end();
        // AttributeTransformer models unification
        AttributeTransformer cTransformer(c->getLeadingLiteral().getAttribute(),
                d->getLeadingLiteral().getAttribute());
        AttributeTransformer dTransformer(d->getLeadingLiteral().getAttribute(),
                c->getLeadingLiteral().getAttribute());

        //SortedLiteralList result;
        LiteralList result;
        // the following code asumes that literals are ordered within
        // clauses. Resulting clause is ordered as well
        while ((citer != cend) && (diter != dend))
        {
//            if(*citer == *diter) //exactly the same literal
            if ((citer->getSign() == diter->getSign()) &&
                (citer->getProposition() == diter->getProposition()) && 
                (cTransformer(citer->getAttribute()) ==
                   dTransformer(diter->getAttribute())))
            {
#ifdef RESOLUTION_DEBUG
 std::cerr << "same literals" << endl;
#endif
                Literal tempLiteral = *citer;
                tempLiteral.setAttribute(cTransformer(tempLiteral.getAttribute()));
                result.push_back(tempLiteral); // (implicit) factor
                ++citer;
                ++diter;
            }
            else
                if ((citer->getSign() != diter->getSign()) &&
                    (citer->getProposition() == diter->getProposition()) && 
                    (cTransformer(citer->getAttribute()) ==
                       dTransformer(diter->getAttribute())))
                {
#ifdef RESOLUTION_DEBUG
  std::cerr << "complementary, not allowed to resolve" << endl;
#endif
                    return 0;
                }
                else 
                {
#ifdef RESOLUTION_DEBUG
  std::cerr << "different" << endl;
#endif
                    if (*citer < *diter)
                    {
                        Literal tempLiteral = *diter++;
                        tempLiteral.setAttribute(dTransformer(tempLiteral.getAttribute()));
                        result.push_back(tempLiteral); 
                    }
                    else
                    {
                        Literal tempLiteral = *citer++;
                        tempLiteral.setAttribute(cTransformer(tempLiteral.getAttribute()));
                        result.push_back(tempLiteral); 
                    }
                }
        }
        while (citer != cend)
        {
            Literal tempLiteral = *citer++;
            tempLiteral.setAttribute(cTransformer(tempLiteral.getAttribute()));
            result.push_back(tempLiteral); 
        }
        while (diter != dend)
        {
            Literal tempLiteral = *diter++;
            tempLiteral.setAttribute(dTransformer(tempLiteral.getAttribute()));
            result.push_back(tempLiteral);
        }
#ifdef RESOLUTION_DEBUG
  //std::cerr << "at the end of (binary) resolve:" << endl;
  //std::cerr << "result = ";
  //result.dump(std::cerr);
  //std::cerr << endl;
#endif 
        return new Clause(result, make_info(c->getId(), d->getId()));
    }

    // assume that the clause c is not empty
    inline void
    resolve(const Active &active, const PClause& c, std::list<PClause>& result)
    {
        Clause* rc; //result clause
        Literal leading = c->getLeadingLiteral();
        std::pair< Active::const_iterator, 
                   Active::const_iterator >
            candidates = active.getComplementary(leading);
#ifdef RESOLUTION_DEBUG
  std::cerr << "resolve called with" << endl;
  active.dump(std::cerr);
  std::cerr << endl;
  c->dump(std::cerr);
  std::cerr << endl << "candidates are:";
  for (Active::const_iterator __p=candidates.first; 
          __p!=candidates.second; __p++)
      (*__p)->dump(std::cerr);
  std::cerr << endl;
#endif 
        for (Active::const_iterator p = candidates.first;
                p != candidates.second;
                ++p)
        {
            rc = resolveMaximals(*p, c);
            if(rc)
            {
                PClause prc(rc);
#ifdef RESOLUTION_DEBUG
  std::cerr << "binary resolveMaximals produced:";
  prc->dump(std::cerr);
  std::cerr << endl;
#endif
                result.push_back(prc);
            }
        }
    } //resolve

    ////////////////////////////////////////////////////////////////////
    //
    //                         RESOLUTION CYCLE                       //
    //
    // /////////////////////////////////////////////////////////////////
    //
    // returns a pair of boolean values
    // The first value is true, if the clause set is unsatisfiable
    // If the second value is true, no new information was generated during
    // the run
    //
    
    bool
    backwardSubsumptionOnActive(Active& active, const PClause& clause, Statistics& stat)
    {
        bool result = false;
        Active::iterator activeEnd = active.end();
        Active::iterator i = active.begin();
        while ( i != activeEnd)
        {
            //RESOLUTION_DEBUG std::cerr << "           i = ";
            //RESOLUTION_DEBUG (*i)->dump(std::cerr); 
            //RESOLUTION_DEBUG std::cerr << std::endl;
            if (clauseSubsumes(clause, *i))
            {
#ifdef RESOLUTION_DEBUG
std::cerr << "going to erase ";
(*i)->dump(std::cerr);
std::cerr << std::endl;
std::cerr << "because of ";
clause->dump(std::cerr);
std::cerr << std::endl;
#endif
           //RESOLUTION_DEBUG      std::cerr << "Active: ";
           //RESOLUTION_DEBUG      active.dump(std::cerr);
                result = true;
#ifdef COLLECT_STAT
                stat.addSubsumptions(1);
#endif
                i = active.erase(i);
           //RESOLUTION_DEBUG      std::cerr << " erased " << std::endl;
           //RESOLUTION_DEBUG      active.dump(std::cerr);
            }
            else
                i++;
        }
        return result;
    }

    std::pair<bool,bool>
    resolution(Active& active, Passive& passive, Statistics& stat)
    {
        bool anythingNew = false;
        while (passive.size())
        {
            // getting the next clause
            PClause current = passive.select();
            // checking whether it is redundand: DISCOUNT algorithm
            if (rangeSubsumes(active.begin(), active.end(), current))
            {
#ifdef COLLECT_STAT
                stat.addSubsumptions(1);
#endif
                continue;
            }
            // else
            // simplifying Active by current..
            anythingNew |= backwardSubsumptionOnActive(active, current, stat);
            // adding to active..
            active.push_back(current);
            //std::cerr << "going further" << std::endl;
            //        active.dump(std::cerr);
            // generating of new clauses...
            // returns a pointer on a list of generated clauses
            std::list<PClause> newlist;
            resolve(active, current, newlist);
#ifdef RESOLUTION_DEBUG
  //std::cerr << "newlist befoure simplifications:";
  //newlist.dump(std::cerr);
  //std::cerr << endl;
#endif // RESOLUTION_DEBUG
            for (std::list<PClause>::iterator p = newlist.begin();
                    p != newlist.end();)
            {
#ifdef COLLECT_STAT
                stat.addResolvents(1);
#endif
                bool redundand = false;
                // innner simplification of new clauses...
                if (rangeSubsumes(newlist.begin(),newlist.end(),*p))
                {
                    redundand = true;
                }
                else 
                {
                    // simplificateion of new clauses by active
                    Clause::const_iterator ciend = (*p)->end();
                    for(Clause::const_iterator ci = (*p)->begin(); 
                            ci != ciend; ci++)
                    {
                        std::pair<Active::const_iterator,
                                  Active::const_iterator> 
                                      candidates = active.getStartingWith(*ci);
                        if(rangeSubsumes(candidates.first, 
                                    candidates.second, *p))
                        {
                            redundand = true;
                            break;
                        }
                    }
                    if(!redundand)
                    {
                        Clause::const_iterator ciend = (*p)->end();
                        for(Clause::const_iterator ci = (*p)->begin(); 
                                ci != ciend; ci++)
                        {
                            std::pair<Passive::const_iterator,
                                      Passive::const_iterator> 
                                          candidates = passive.getStartingWith(*ci);
                            if(rangeSubsumes(candidates.first, 
                                        candidates.second, *p))
                            {
                                redundand = true;
                                break;
                            }
                        }
                    }
                }
                if (redundand)
                {
#ifdef COLLECT_STAT
                    stat.addSubsumptions(1);
#endif
                    p = newlist.erase(p);
                }
                else 
                    ++p;
            }
            // put what rests into passive
#ifdef RESOLUTION_DEBUG            
  //std::cerr << "newlist after simplifications:";
  //newlist.dump(std::cerr);
  //std::cerr << endl;
#endif // RESOLUTION_DEBUG
            bool success = false; //is empty clause generated?
            std::list<PClause>::const_iterator cpend = newlist.end();
            for (std::list<PClause>::const_iterator p = newlist.begin();
                    p != cpend;
                    ++p)
            {
                // only if we put anything new into passive, 
                // we consider that something new was generated
                anythingNew = true;
                if ((*p)->size() == 0)
                    success = true;
                // simplifying Active by the newly generated clauses
                backwardSubsumptionOnActive(active, current, stat);
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
} //namespace PropositionalProver
