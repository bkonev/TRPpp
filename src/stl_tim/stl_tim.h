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
#ifndef __STL_TIM_H__    
#define __STL_TIM_H__    

#include <iostream>
#include <string>
#include <utility>
#include "misc/compound_container.h"
#include "proposition.h"
#include "literal.h"
#include "literallist.h"
#include "clause.h"
#include "clauseset.h"
#include "infer.h"
#include "timstat.h"


namespace PropositionalProver
{
    class ClauseSet : public compound_container<Active, Passive>
    {
    private:
        static Statistics ourStatistics;
    public:
        // this is a way to say to the inference machine what and how to do
        struct Flags
        { 
            // by default, no Forward Subsumption Resolution, normal mode
            Flags()
            : useFSR(false), loopSearch(false)
            { }
            bool useFSR;
            bool loopSearch;
        };
        static PClause 
        make_clause(const LiteralList& ll, const std::string& str)
        {
            return PClause(new Clause(ll, str));
        }
        // saturate the set of clauses..
        std::pair<bool, bool>
        saturate(const Flags& f = Flags());
        // dump kept information
        std::ostream& dump(std::ostream& os) const;

        const Statistics&
        getStatistics() const
        {
            return ourStatistics;
        }
    }; // class ClauseSet
} // namespace PropositionalProver
#endif // __STL_TIM_H__    
