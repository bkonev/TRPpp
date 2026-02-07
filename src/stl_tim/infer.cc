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
    bool
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
TRACE(subsumptionModule, {
std::cerr << "returning false" << std::endl;
});
                    return false;
                }
            }
        }
        if (p == pend) 
        {
TRACE(subsumptionModule, {
std::cerr << "returning true " << std::endl;
});
            return true;
        }
        else 
        {
TRACE(subsumptionModule, {
std::cerr << "returning false" << std::endl;
});
            return false;
        }
    }

    inline bool
    implementationDependentSubsumes(const PClause& c, const PClause& d)
    {
TRACE(subsumptionModule, {
std::cerr << "subsumption test for C= " ;
c->dump(std::cerr);
std::cerr << " D= ";
d->dump(std::cerr);
std::cerr << std::endl;
});
        // functionality depends on the attribute types of C and D
        Attribute::AttributeType leadingLiteralAttributeTypeC(c->getLeadingLiteral().getAttribute().getType());
        Attribute::AttributeType leadingLiteralAttributeTypeD(d->getLeadingLiteral().getAttribute().getType());


        // simple cases: initial and step_next
        // (a subtile thing: checks up to attribute comparator!)
        if (c->getLeadingLiteral().getAttribute() != universal_attr)
        {
TRACE(subsumptionModule, {
std::cerr << " simple case " <<  std::endl;
});
            // comment: I do not understand, why I am comparing 
            // attribute types and not attributes. Seems that 
            // Universal does not subsume StepNow.... Check!
            if(leadingLiteralAttributeTypeC == leadingLiteralAttributeTypeD)
            {
                bool result = implementationDependentSubsumesHelper(c, d);
TRACE(subsumptionModule, {
std::cerr << " returning " << result << std::endl;
});
                return result;
            }
            else 
            {
TRACE(subsumptionModule, {
std::cerr << "returning false" << std::endl;
});
                return false;
            }
        }
        // else Attribute of c is universal_attr (ie either of universal
        // and step_now in normal mode, universal in loopsearch)
        if (leadingLiteralAttributeTypeD != Attribute::step_next)
        {
            return implementationDependentSubsumesHelper(c, d); 
        }
TRACE(subsumptionModule, {
std::cerr << "The ugliest case.." << std::endl ;
});

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
TRACE(subsumptionModule, {
std::cerr << "returning true" << std::endl;
});
            return true; // this is the first case
        }
        // If we are here, the first case does not work
        // There is still a possibility that C subsumes D as step_now
        //
        // Assume that C consists of step_now literals only
TRACE(subsumptionModule, {
std::cerr << "First ugliest case didn't work" << std::endl ;
});
        p = c->begin(); 
        while ((q != qend) && (q->getAttribute() == step_next_attr))
            ++q; // moving q until *q is a step_now literal
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
TRACE(subsumptionModule, {
std::cerr << "returning false" << std::endl;
});
                    return false; // now we can be sure
                }
            }
        }
        if (p == pend) 
        {
TRACE(subsumptionModule, {
std::cerr << "returning true" << std::endl;
});
            return true; // this is the first case
        }
        else 
        {
TRACE(subsumptionModule, {
std::cerr << "returning false" << std::endl;
});
            return false; // now we are sure
        }
    }

    // returns true if clause C subsumes clause D
    inline bool
    clauseSubsumesHelper(const PClause& c, const PClause& d)
    {
TRACE(subsumptionModule, {
std::cerr << "clauseSubsumesHelper: " << std::endl << "C = ";
c->dump(std::cerr);
std::cerr << std::endl << " D = ";
d->dump(std::cerr);
std::cerr << std::endl;
});
        unsigned int cSize = c->size();
        if(!cSize)
        {
TRACE(subsumptionModule, {
std::cerr << "     Empty clause subsumes everything" << std::endl;
});
            return true; // the empty clause subsumes everything
        }
        // else
        if (cSize > d->size()) 
        {
TRACE(subsumptionModule, {
std::cerr << "     C.size() > D.size(); returning false" << std::endl;
});
            return false; // c must be a subset, but it greater in size..
        }
        // else
TRACE(subsumptionModule, {
std::cerr << "     Calling implementationDependentSubsumes(c, d)" << std::endl;
});
        return implementationDependentSubsumes(c, d);
    }

    // returns true if clause C subsumes clause D but they are 
    // not clauses with  the same id (needed for inner simplification)
    bool
    clauseSubsumes(const PClause& c, const PClause& d)
    {
        // simple checks first
        if (c->getId() == d->getId()) // c and d is the same entity (in inner
                                      // simplification)
            return false;
        bool result = clauseSubsumesHelper(c, d);
TRACE(resolutionModule, {
if(result)
{
std::cerr << "clause C = ";
c->dump(std::cerr);
std::cerr << std::endl << " subsumes clause D = ";
d->dump(std::cerr);
std::cerr << std::endl;
}
});
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
    Clause*
    resolveMaximals(const PClause& c, const PClause& d)
    {
        typedef Clause::const_iterator CI; //Clause contains literals themselves
TRACE(resolutionModule, {
   std::cerr << "(binary) resolve called with " << std::endl;
   std::cerr << "C = ";
   c->dump(std::cerr);
   std::cerr << std::endl;
   std::cerr << "D = ";
   d->dump(std::cerr);
   std::cerr << std::endl;
});
        CI citer = c->begin();
        CI diter = d->begin();
        if (!checkConditions(*citer,*diter))
        {
TRACE(resolutionModule, {
  std::cerr << "clauses cannot be resolved" << std::endl;
});
            return 0;
        }

        Assert(areComplementary(*citer,*diter), "resolveMaximals Called with wrong arguments");

        ++citer; ++diter; // first literals are maximal; we resolve on it
        CI cend  = c->end();
        CI dend  = d->end();
        // AttributeTransformer models unification
        AttributeTransformer cTransformer(c->getLeadingLiteral().getAttribute(),
                d->getLeadingLiteral().getAttribute());
        AttributeTransformer dTransformer(d->getLeadingLiteral().getAttribute(),
                c->getLeadingLiteral().getAttribute());

        SortedLiteralList result;
        //LiteralList result;
        // the following code asumes that literals are ordered within
        // clauses. Resulting clause is ordered as well
        while ((citer != cend) && (diter != dend))
        {
//            if(*citer == *diter) //exactly the same literal
            bool cSign = citer->getSign();
            bool dSign = diter->getSign();

            // after all, these are only parts of some complex data 
            // structures. Thus, I am creating const references in 
            // order to not to copy them.
            const Proposition &cProposition = citer->getProposition();
            const Proposition &dProposition = diter->getProposition();
            const Attribute &cAttribute = citer->getAttribute();
            const Attribute &dAttribute = diter->getAttribute();

            Attribute cTransformedAttribute = cTransformer(cAttribute);
            Attribute dTransformedAttribute = dTransformer(dAttribute);

            if ((cSign == dSign) &&
                (cProposition == dProposition) && 
                (cTransformedAttribute ==
                   dTransformedAttribute))
            {
TRACE(resolutionModule, {
 std::cerr << "same literals" << std::endl;
});
                Literal tempLiteral = *citer;
                tempLiteral.setAttribute(cTransformedAttribute);
                result.push_back(tempLiteral); // (implicit) factor
                ++citer;
                ++diter;
            }
            else
                if ((cSign != dSign) &&
                    (cProposition == dProposition) && 
                    (cTransformedAttribute ==
                       dTransformedAttribute))
                {
TRACE(resolutionModule, {
  std::cerr << "complementary, not allowed to resolve" << std::endl;
});
                    return 0;
                }
                else 
                {
TRACE(resolutionModule, {
  std::cerr << "different" << std::endl;
});
                    if (!(dTransformedAttribute < cTransformedAttribute)&&( (cTransformedAttribute < dTransformedAttribute) || (cProposition < dProposition)))
                    {
                        Literal tempLiteral = *diter++;
                        tempLiteral.setAttribute(dTransformedAttribute);
                        result.push_back(tempLiteral); 
                    }
                    else
                    {
                        Literal tempLiteral = *citer++;
                        tempLiteral.setAttribute(cTransformedAttribute);
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
TRACE(resolutionModule, {
  //std::cerr << "at the end of (binary) resolve:" << std::endl;
  //std::cerr << "result = ";
  //result.dump(std::cerr);
  //std::cerr << std::endl;
});
        return new Clause(result, make_info(c->getId(), d->getId()));
    }

} //namespace PropositionalProver
