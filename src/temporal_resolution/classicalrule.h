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
#ifndef __CLASSICALRULE_H__
#define __CLASSICALRULE_H__

#include "temporalrule.h"

namespace TemporalProver
{
    template<typename ClauseSet>
    class ClassicalRule : public TemporalRule<ClauseSet>
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
    ClassicalRule<ClauseSet>::augment(ClauseSet& clauseSet)
    {
        // making a list of eventuality literals
        std::vector<Literal> eventualities;
        for (typename ClauseSet::const_e_iterator i = clauseSet.eventuality_begin();
                i != clauseSet.eventuality_end();
                ++i)
            eventualities.push_back(i->getEventuality());
        // eventualities.sort();
        std::sort(eventualities.begin(), eventualities.end());
        std::sort(eventualities.begin(), eventualities.end());
        // eventualities.unique();
        typename std::vector<Literal>::iterator newEventualitiesEnd =
            std::unique(eventualities.begin(), eventualities.end());

        // for each eventuality LITERAL we generate a step clause and
        // put it into the clauseSet
        for (typename std::vector<Literal>::const_iterator i = eventualities.begin();
                i != newEventualitiesEnd; ++i)
        {
            Literal wl = TemporalRule<ClauseSet>::makeLiteral(*i);
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
        // and put it into teh clauseSet
        for (typename ClauseSet::const_e_iterator i = clauseSet.eventuality_begin();
                i != clauseSet.eventuality_end();
                ++i)
        {
            // getting the eventuality literal
            Literal eventualityLiteral = i->getEventuality();
            // creating a new literal  (note that makeLiteral returns
            // the same literal for the same name --> the same as when
            // we process eventuality literals above)
            Literal wl = TemporalRule<ClauseSet>::makeLiteral(eventualityLiteral);
            typename ClauseSet::LiteralList list(i->present_begin(), i->present_end());
            // creating a universal clause
            list.push_back(eventualityLiteral);
            list.push_back(wl);
            typename ClauseSet::UClause universalClause = ClauseSet::make_uclause(list, "aug U");
            // putting it into the clause set
            clauseSet.push_back(universalClause);
        }
    }

    // returns true if anything was generated
    template<typename ClauseSet>
        template<typename IN>
            void
            ClassicalRule<ClauseSet>::generateResolvents(IN loopBegin, IN
                    loopEnd, const EClause& eventualityClause,
                    ClauseSet& clauseSet)
            {
                // definitions:
                typedef typename ClauseSet::LiteralList LiteralList;
    
                Literal eventualityLiteral = eventualityClause.getEventuality();
    
                LiteralList
                    eventualityClauseList(eventualityClause.present_begin(),
                            eventualityClause.present_end());
                // here we have two options:
                // ether the loop consists of an empty list (loop in true)
                // or there are some clauses
                Literal notWLiteral = TemporalRule<ClauseSet>::makeLiteral(eventualityLiteral);
                notWLiteral.negate();

                LiteralList tmpList; // empty list
                tmpList.push_back(notWLiteral);
                // First alternative:
                if(loopBegin == loopEnd)
                {
                    // generate two clauses
                    // a step clause
                    LiteralList tmpList2;
                    tmpList2.push_back(eventualityLiteral);
                    typename ClauseSet::SClause stepClause = ClauseSet::make_sclause(tmpList, tmpList2, "tres S");
                    // putting into the clause set
                    clauseSet.push_back(stepClause);
                    // a universal clause
                    eventualityClauseList.push_back(eventualityLiteral);
                    typename ClauseSet::UClause universalClause = ClauseSet::make_uclause(eventualityClauseList, "tres U");
                    // putting into the clause set
                    clauseSet.push_back(universalClause);
                }
                else
                {
                    for(IN i = loopBegin;
                            i != loopEnd;
                            ++i)
                    {
                        LiteralList loopLiterals(i->begin(), i->end());
                        // generating a step clause
                        loopLiterals.push_back(eventualityLiteral);
                        typename ClauseSet::SClause stepClause =
                            ClauseSet::make_sclause(tmpList, loopLiterals,
                                    "tres S");
                        // putting into the clause set
                        clauseSet.push_back(stepClause);
                        // generating a universal clause
                        std::copy(eventualityClauseList.begin(), 
                                eventualityClauseList.end(),
                                  back_inserter(loopLiterals));
                        typename ClauseSet::UClause universalClause =
                            ClauseSet::make_uclause(loopLiterals, "tres U");
                        // putting into the clause set
                        clauseSet.push_back(universalClause);
                    }
                }
            }
    template<typename ClauseSet>
    inline void
    ClassicalRule<ClauseSet>::preprocess(ClauseSet& clauseSet)
    {
        return augment(clauseSet);
    }

} // namespace TemporalProver
#endif // __CLASSICALRULE_H__  
