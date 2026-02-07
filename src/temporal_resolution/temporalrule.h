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
#ifndef __TEMPORALRULE_H__
#define __TEMPORALRULE_H__

#include <string>
#include "trim/trim.h"
#include "loop_search/loopsearch.h"

  
// Note: functions of this class are deliberately not made virtual
// as for now I do not have any plans on having more than one rule 
// in one program.
// THIS IS NOT AN INTERFACE TO BE USED! (IT JUST SAYS WHICH FUNCTIONS
// A TEMPORAL RULE CLASS SHOULD IMPLEMENT (SEE E.G. ClassicalRule))
// and provides with a couple of useful functions
//
namespace TemporalProver
{
    template<typename ClauseSet>
    class TemporalRule
    {
    public:
        typedef typename ClauseSet::Literal Literal;
        typedef typename ClauseSet::EClause EClause;
        // changes the given ClauseSet as needed before temporal resolution
        // steps start (e.g., augmentation)
        //
        // To be defined in a derived class
        static void 
        preprocess(ClauseSet&);

        // given a loop (as a range and EClause, generates temporal 
        // resolvents and put them into ClauseSet
        //
        // To be defined in a derived class
        template<typename IN>
            static void 
            generateResolvents(IN, IN, const EClause&, ClauseSet&);

    protected:
        // generates a new literal with the name generated from literal's name
        static typename ClauseSet::Literal
        makeLiteral(const Literal& literal);
    };

    template<typename ClauseSet>
    inline typename ClauseSet::Literal
    TemporalRule<ClauseSet>::makeLiteral(const Literal& literal)
    {
        std::string newName = (literal.isPositive()? std::string("w_") :
                std::string("w_not_")) + literal.getProposition().getName();
        return Literal(typename ClauseSet::Proposition(newName));
    }

} // namespace TemporalProver
#endif //  __TEMPORALRULE_H__
