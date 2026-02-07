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
#ifndef __LOOPSEARCH_H__
#define __LOOPSEARCH_H__

#include <vector>
#include <utility>
#include <iostream>
//#include <map>
#include "trim/trim.h"
#include "config.h"
#include "modules.h"
#include "misc/tracer.h"
#include "misc/assert.h"

namespace TemporalProver
{
    using namespace TRIMs;

    // this class implements loop search. 
    // When created, the object grabs into internal data structures
    // the clause set and the eventuality literal. 
    // The actual loop search is implemented by search method.
    //
    template<typename ClauseSet>
    class LoopList : private std::vector<typename ClauseSet::UClause>
    {
    public:
        typedef std::vector<typename ClauseSet::UClause> _Base;
        typedef typename ClauseSet::Literal Literal;
        typedef typename _Base::iterator iterator;
        typedef typename _Base::const_iterator const_iterator;
        // Note
        // I tried this data structure to keep found loops and to 
        // return false in case when the recently found loop and the 
        // kept loop coiside. However, it turned out that it does not
        // pay of.
        //
        //typedef std::map<Literal, _Base> Loops;
        // End of note

        // constructor: gets needed clauses from clauseSet into internal data
        LoopList(const ClauseSet&, const Literal&, const TRPPPOptions& );

        // returns true, if a loop was found
        bool 
        search();

        // true, if loop was found (thus, it is possible to check later)
        bool 
        loopFound() const
        {
            return myLoopFound;
        }

        // The output is taken through the following functions.
        // If loop search was successful, one can use iterator in order
        // to find all the clauses forming the loop one by one
        // E.g. for(const_iterator i = loop.begin(); i != loop.end(), ++i)
        // {
        //   ....
        // }
        //
        // in this case, i will point to a clause.
        
        // loop begins
        const_iterator
        begin() const
        {
            // this is allowed if a loop was found. Note that 
            // release version of the program does not include this
            // test.
            Assert(loopFound(), "Attempt to get loop wich is not found");
            return internal_begin();
        }

        // loop ends
        const_iterator
        end() const
        {
            // this is allowed if a loop was found. Note that 
            // release version of the program does not include this
            // test.
            Assert(loopFound(), "Attempt to get loop wich is not found");
            return internal_end();
        }

        // used internally in order to put clauses into it
        void
        push_back(typename ClauseSet::UClause uclause)
        {
            return _Base::push_back(uclause);
        }
    private:
        // internal data structures
        ClauseSet myClauseSet;
        Literal myLiteral;
        bool myLoopFound;
        TRPPPOptions myOptions;

        // deactivated
        // static Loops ourLoops;
        


        // this procedure actually searches for a loop
        bool 
        inferenceBFS();

        // these functions look ugly, but I suppose this is the
        // safest way of preventing begin/end to be called in the case
        // when assertion is "on" and myLoopFound == false
        // as this functions are private, they cannot be used from
        // outside of the class
        const_iterator 
        internal_begin() const { return _Base::begin(); }

        const_iterator
        internal_end() const { return _Base::end(); }
    };
    // IMPLEMENTATION
    template<typename ClauseSet> 
    LoopList<ClauseSet>::LoopList(
            const ClauseSet& clauseSet, 
            const Literal& literal, const TRPPPOptions& options)
    : myLiteral(literal), myLoopFound(false), myOptions(options)
    {
        // selecting needed clauses into myClauseSet
        myClauseSet.getForLoop(clauseSet);
    }

    template<typename ClauseSet> 
    inline bool 
    LoopList<ClauseSet>::inferenceBFS()
    {
        // definitions
        typedef typename ClauseSet::LiteralList LiteralList;

        // initially, the list contains false (empty clause)
        static LiteralList emptyLiteralList;
        this->push_back(ClauseSet::make_uclause(emptyLiteralList, "loop U"));

//int loopSearchIterations = 0;

        // main loop
        while(true)
        {
                        TRACE(loopsearchModule, {
                                    std::cerr << "Next LoopSearch iteration" << std::endl;
                                });
//if(++loopSearchIterations == 2)
//{
//    setTrace(resolutionModule);
//    setTrace(FSRModule);
//}
            // For each iteration, we need a fresh copy of myClauseSet
            ClauseSet tmpClauseSet = myClauseSet;
            typename _Base::const_iterator iend = this->internal_end();

            // adding myLiteral to each clause from the list clauseList,
            for (typename _Base::const_iterator i = this->internal_begin();
                    i != iend;
                    i++)
            {
                LiteralList literalList(i->begin(), i->end());
                literalList.push_back(myLiteral);
                // making the result a step clause and putting it into the
                // clause set
                typename ClauseSet::SClause tmpClause =
                    ClauseSet::make_sclause(emptyLiteralList, literalList,
                            std::string("loop S ") + itos(i->getId()));
                tmpClauseSet.push_back(tmpClause);
            }
            // recall: mode is loopSearch
            if (tmpClauseSet.saturate(myOptions).first)
            {
                // empty loop
                this->clear(); 
                return true;
            }

            // extract clauses of the form ...=>next(False), i.e.
            // with empty 'step_next' part
            _Base newClauseList(tmpClauseSet.next_false_as_universal_begin(), tmpClauseSet.next_false_as_universal_end());
            // 
                                    TRACE(loopsearchModule, {
                                            std::cerr << "Loop candidate: ";
                                            for (typename _Base::const_iterator ixi = newClauseList.begin(); ixi != newClauseList.end(); ++ixi) 
                                            {
                                                std::cerr << "  ";
                                                ixi->getInternalRep()->dump(std::cerr);
                                                std::cerr << std::endl;
                                            }
                                            std::cerr << std::endl;
                                            });
            // 
            if (newClauseList.size() == 0)
            {
                // no loop
                this->clear();
                return false;
            }
            // subsumption check
            // I need these constants because newClauseList{Begin,End} must
            // be constant iterators. 
            typename _Base::const_iterator newClauseListBegin = newClauseList.begin();
            typename _Base::const_iterator newClauseListEnd = newClauseList.end();
            if (ClauseSet::subsumesOrEqual(newClauseListBegin,
                        newClauseListEnd, this->internal_begin(), this->internal_end()))
            {
                // although this is not needed (any of the lists form 
                // a loop, I get the new one for compatibility reasons)
                this->clear();
                //std::copy(newClauseList.begin(), newClauseList.end(), std::back_inserter(*this));

                for (typename _Base::const_iterator ixi = newClauseList.begin(); ixi != newClauseList.end(); ++ixi) 
                {
                    this->_Base::push_back(*ixi);
                }
                return true;
            }
            // else 
            // copying newClauseList into *this
            this->clear();
            //std::copy(newClauseList.begin(), newClauseList.end(), std::back_inserter(*this));
            for (typename _Base::const_iterator ixi = newClauseList.begin(); ixi != newClauseList.end(); ++ixi) 
            {
                this->_Base::push_back(*ixi);
            }
        }
        
    }

    template<typename ClauseSet> 
    bool
    LoopList<ClauseSet>::search()
    {
        // preparation to search..
        // saving the operation mode // TODO: mode should not be global
        typename ClauseSet::OperationMode saveMode = ClauseSet::getMode();
        ClauseSet::setMode(ClauseSet::loopSearch);
        // perform search
        myLoopFound = inferenceBFS();
        // restoring operation mode
        ClauseSet::setMode(saveMode);

        /* this code is deactivated as it brings very little gain in 
         * case when there are redundand loops and causes quite some
         * slow down for the case when there are no redundand loops
         *
        // befoure we return the loop, we check whether we have already tried
        // it for the same literal and if so, whether we have found the 
        // same loop. In the later case, we return false
        Loops::const_iterator oldLoop = ourLoops.find(myLiteral);
        if(oldLoop != ourLoops.end()) // we already did something for this literal
        {
            //
            if ((ClauseSet::subsumesOrEqual(oldLoop->second.begin(),
                  oldLoop->second.end(), this->internal_begin(),
                  this->internal_end())) &&
                (ClauseSet::subsumesOrEqual(this->internal_begin(),
                  this->internal_end(), oldLoop->second.begin(),
                  oldLoop->second.end())))
            {
                this->clear();
                myLoopFound = false;
            }
        }
        if(myLoopFound)
        {
            // this means that the newly founded loop is valuable,
            // I have to keep it
            ourLoops[myLiteral] = *this;
        }
        */

        return myLoopFound;
    }

    /*
    // I do not know, whether this would work if I include 
    // this file from more than one place. For now it seems to be
    // OK, if a problem appears, it should be solved by other
    // template instantiation method.
    // 
    // Anyway, this is deactivated 
    // template<typename ClauseSet> typename LoopList<ClauseSet>::Loops LoopList<ClauseSet>::ourLoops; 
    */

} // namespace LoopSearchs
#endif // __LOOPSEARCH_H__
