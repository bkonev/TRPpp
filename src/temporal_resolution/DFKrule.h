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
#ifndef __DFKRULE_H__
#define __DFKRULE_H__

#include "temporalrule.h"

namespace TemporalProver
{
    // Degtyarev, Fisher, Konev rule
    template<typename ClauseSet>
    class DFKRule : public TemporalRule<ClauseSet>
    {
    public:
        typedef typename ClauseSet::Literal Literal;
        typedef typename ClauseSet::EClause EClause;
        // changes the given ClauseSet as needed before temporal resolution
        // steps start (e.g., augmentation
        static void 
        preprocess(ClauseSet&);

        // given a loop (as a range and EClause, generates temporal 
        // resolvents and put them into ClauseSet
        template<typename IN>
            static void 
            generateResolvents(IN, IN, const EClause&, ClauseSet&);
    private:
        // this name is kept for easier reference to the original paper
        // of M.Fisher
        static void
        augment(ClauseSet&);
    };

    // IMPLEMENTATION
    template<typename ClauseSet>
    inline void
    DFKRule<ClauseSet>::augment(ClauseSet& clauseSet)
    {
        // making a list of eventuality literals
        std::list<Literal> eventualities;
        for (typename ClauseSet::const_e_iterator i = clauseSet.eventuality_begin();
                i != clauseSet.eventuality_end();
                i++)
            eventualities.push_back(i->getEventuality());
        eventualities.sort();
        eventualities.unique();

        // for each eventuality LITERAL we generate a step clause and
        // put it into the clauseSet
        for (std::list<Literal>::const_iterator i = eventualities.begin();
                i != eventualities.end();
                i++)
        {
            Literal wl = makeLiteral(*i);
            typename ClauseSet::LiteralList nowList, nextList; // making empty 
                                                               // lists

            Literal notwl = wl; // negation of wl
            notwl.negate();

            nowList.push_back(notwl);
            nextList.push_back(*i);
            nextList.push_back(wl);
            // creating a step clause
            typename ClauseSet::SClause stepClause = ClauseSet::make_sclause(nowList, nextList, "aug S");
            // putting it into the clause set
            clauseSet.push_back(stepClause);
        }

        // for each eventuality CLAUSE we generate a universal clause
        // and put it into the clauseSet
        // The clause itself is deleted, instead, we generate new 
        // unconditional eventuality clause next(not w_l)
        typename ClauseSet::e_iterator iend = clauseSet.eventuality_end();
        std::list<EClause> tmpEClauseList;
        for (typename ClauseSet::e_iterator i = clauseSet.eventuality_begin();
                i != iend;
                i = clauseSet.erase(i) )
        {
            // getting the eventuality literal
            Literal eventualityLiteral = i->getEventuality();
            // creating a new literal  (note that makeLiteral returns
            // the same literal for the same name --> the same as when
            // we process eventuality literals above)
            Literal wl = makeLiteral(eventualityLiteral);
            typename ClauseSet::LiteralList list(i->present_begin(), i->present_end());
            // creating a universal clause
            list.push_back(eventualityLiteral);
            list.push_back(wl);
            typename ClauseSet::UClause universalClause = ClauseSet::make_uclause(list, "aug U");
            // putting it into the clause set
            clauseSet.push_back(universalClause);
            // generating new (unconditional) eventuality clauses
            Literal notwl = wl;
            notwl.negate();
            typename ClauseSet::EClause newEventualityClause = ClauseSet::make_eclause(typename ClauseSet::LiteralList(), notwl, i->getInfo()+"\'");
//std::cerr << "going to push back" << std::endl;
            tmpEClauseList.push_back(newEventualityClause);
        }
        for(std::list<EClause>::iterator ii = tmpEClauseList.begin();
                ii != tmpEClauseList.end();
                ii++)
        {
            clauseSet.push_back(*ii);
        }
    }

    // returns true if anything was generated
    template<typename ClauseSet>
        template<typename IN>
            void
            DFKRule<ClauseSet>::generateResolvents(IN loopBegin, IN
                    loopEnd, const EClause& eventualityClause,
                    ClauseSet& clauseSet)
            {
                // definitions:
                typedef typename ClauseSet::LiteralList LiteralList;
    
                // here we have two options:
                // ether the loop consists of an empty list (loop in true)
                // or there are some clauses
                LiteralList tmpList; // empty list
                // First alternative:
                // proof suceed 
                if(loopBegin == loopEnd)
                {
                    // a universal emty clause
                    typename ClauseSet::UClause universalClause = ClauseSet::make_uclause(tmpList, "tres U");
                    // putting into the clause set
                    clauseSet.push_back(universalClause);
                }
                else
                {
                    // for optimization
                    for(std::list<typename ClauseSet::UClause>::const_iterator i = loopBegin;
                            i != loopEnd;
                            i++)
                    {
                        LiteralList loopLiterals(i->begin(), i->end());
                        // generating a step clause
                        typename ClauseSet::UClause universalClause =
                            ClauseSet::make_uclause(loopLiterals, "tres U");
                        // putting into the clause set
                        clauseSet.push_back(universalClause);
                    }
                }
            }
    template<typename ClauseSet>
    inline void
    DFKRule<ClauseSet>::preprocess(ClauseSet& clauseSet)
    {
        return augment(clauseSet);
    }

} // namespace TemporalProver
#endif // __DFKRULE_H__  
