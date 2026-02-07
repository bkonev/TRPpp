/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  Copyright (C) 2001-2011                               * */
/* *  Boris Konev                                           * */
/* *  The University of Liverpool                           * */
/* *                                                        * */
/* *  Copyright (C) 2012                                    * */
/* *  Viktor Schuppan                                       * */
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
#ifndef __LITERAL_H__
#define __LITERAL_H__

#include <functional>
#include <iostream>
#include "attribute.h"
#include "proposition.h"
namespace PropositionalProver
{
    class Literal
    {
    private:
        Proposition myProposition;
        bool mySign;
        Attribute myAttribute;

    public:
        Literal (const Proposition& proposition = Proposition(""),
                 const bool sign = true, 
                 const Attribute& attribute = default_attr) 
        : myProposition(proposition), mySign(sign), myAttribute(attribute)
        { }

        void negate()
        {
            mySign = !mySign;
        }

        // requesting internals
        const Proposition& 
        getProposition() const
        {
            return myProposition;
        }

        const Attribute& 
        getAttribute() const
        {
            return myAttribute;
        }

        void 
        setAttribute(const Attribute& attribute)
        {
            myAttribute = attribute;
        }

        bool 
        getSign() const
        {
            return mySign;
        }

        bool 
        isPositive() const
        {
            return getSign();
        }

        bool 
        isNegative() const
        {
            return !getSign();
        }
        
        bool 
        operator== (const Literal& literal) const
        {
            return ((this->myProposition == literal.myProposition) && 
                    (this->mySign == literal.mySign) && 
                    (myAttribute == literal.myAttribute));
        }

        bool
        operator< (const Literal& literal) const
        {
            if (literal.myAttribute < myAttribute) 
            { 
                return false;
            } 
            else 
                if (myAttribute < literal.myAttribute) 
                { 
                    return true;
                } 
                else 
                {
                    return (myProposition < literal.myProposition);
                }
        }

        // strict weak ordering to be used in std::sort of
        // SortedLiteralList::maintainStructure.
        struct greater_literal: public std::greater<Literal>
        {
          bool
            operator()(Literal const & left, Literal const & right) const
          {
            if (right.myAttribute < left.myAttribute) {
              return true;
            } else if (left.myAttribute < right.myAttribute) {
              return false;
            } else {
              return (right.myProposition < left.myProposition);
            }
          }
        };

        std::ostream& 
        dump(std::ostream& os) const
        {
            os << "( " << (mySign ? "+" : "-") ;
            myProposition.dump(os);
            os << " : " << myAttribute << " )";
            return os;
        }

        friend bool areComplementary(const Literal& l, const Literal& m);
    }; //class Literal

    inline bool 
    areComplementary(const Literal& l, const Literal&m)
    {
        return ((l.mySign == (!m.mySign)) 
             && areCompatible(l.myAttribute, m.myAttribute) 
             && (l.myProposition == m.myProposition));
    }
    // note that sign does not participate in the order definition!
} // namespace PropositionalProver
#endif // __LITERAL_H__
