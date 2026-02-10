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
#include "subsumptionindex.h"

namespace PropositionalProver
{

    SubsumptionIndex::SubsumptionIndex(const SubsumptionIndex& subsumptionIndex)
    { 
        if(subsumptionIndex.myRoot) 
        {
            myRoot = new Node(*(subsumptionIndex.myRoot));
        }
        else
        {
            myRoot = 0;
        }
    }

    void
    SubsumptionIndex::insert(const PClause& pclause)
    {

        if((pclause->size() == 0) || (pclause->getLeadingLiteral().getAttribute().getType() != Attribute::step_next))
        {
            return insert(pclause->begin(), pclause->end(), myRoot, pclause);
        }
        else
        {
            Clause::const_iterator i = nextEnd(pclause);
            if(i != pclause->end())
            {
                // This is not good for backwars subsumption, but
                // else step clauses may be checked for subsumption
                // twice.
                insert(i, pclause->end(), myRoot, pclause);
            }
            else  // in this case, step_now part is empty and if I do not
                  // put the clause into the index like this, I will not 
                  // put it elsewhere..
            {
                if(pclause->begin() != i)
                {
                    insert(pclause->begin(), i, myRoot, pclause);
                }
            }
        }
    }

    void
    SubsumptionIndex::insert(Clause::const_iterator startPosition, const
            Clause::const_iterator endPosition, PNode& root, 
            const PClause& pclause)
    // inserts the clause pclause into the index in the subtree starting
    // at root, and reading from position till endposition 
    {
//std::cerr << "Insert called" << std::endl;
        if(root == 0)
        {
//std::cerr << "node created " << std::endl;
            root = new Node;
        }
//this->dump(std::cerr);
//std::cerr<<std::endl;
        if(startPosition == endPosition)
        {
//std::cerr << "start == end" << std::endl;
            root->data.push_back(pclause);
        }
        else
        {
//std::cerr << "moving by ";
//startPosition->dump(std::cerr);
//std::cerr << "here" << std::endl;
            std::pair<Node::Next::iterator, bool> nextNode = 
             root->next.insert(std::make_pair(*startPosition, PNode()));
            if(nextNode.second)
            {
                root->myNumberOfSubBranches++; 
            }
//std::cerr << "link retrieved" << std::endl;
            insert(startPosition+1, endPosition,
                    nextNode.first->second, pclause);
        }
    }


    void
    SubsumptionIndex::remove(const PClause& pclause)
    {

        if(pclause->getLeadingLiteral().getAttribute().getType() != Attribute::step_next)
        {
            return remove(pclause->begin(), pclause->end(), myRoot, pclause);
        }
        else
        {
            Clause::const_iterator i = nextEnd(pclause);
            if(i != pclause->end())
            {
                // This is not good for backwars subsumption, but
                // else step clauses may be checked for subsumption
                // twice.
                remove(i, pclause->end(), myRoot, pclause);
            }
            else  // in this case, step_now part is empty and if I do not
                  // put the clause into the index like this, I will not 
                  // put it elsewhere..
            {
                if(pclause->begin() != i)
                {
                    remove(pclause->begin(), i, myRoot, pclause);
                }
            }
        }
    }

    void
    SubsumptionIndex::remove(Clause::const_iterator startPosition, 
            const Clause::const_iterator endPosition, const PNode root, 
            const PClause& pclause)
    {
        if(root == 0)
        {
            return;
        }
        if(startPosition == endPosition)
        {
            root->data.remove(pclause);
        }
        else
        {

            Node::Next::iterator pos = root->next.find(*startPosition);
            if(pos == root->next.end())
            {
                return;
            }
            remove(startPosition+1, endPosition, pos->second, pclause);

            if((pos->second->myNumberOfSubBranches == 0) &&
                        (pos->second->data.size() == 0)) 
            {
                delete pos->second;
                root->next.erase(pos);
                --(root->myNumberOfSubBranches);
            }
        }
    }

    unsigned int
    SubsumptionIndex::forwardSubsumes(const PClause& pclause) const
    {
                       TRACE(forwardsubsumptionModule, {
                       std::cerr << "Forward subsumption\n Index:" ;
                       this->dump(std::cerr);
                       std::cerr << std::endl;
                       std::cerr << "PClause:" ;
                       pclause->dump(std::cerr);
                       std::cerr << std::endl;
                       });
        Clause::const_iterator pcbegin = pclause->begin();
        Clause::const_iterator pcend = pclause->end();

        if(pclause->getLeadingLiteral().getAttribute() != step_next_attr)
        {
            return forwardSubsumes(pcbegin, pcend, myRoot, pclause);
        }
        else
        {
            Clause::const_iterator i = nextEnd(pclause);
            int tmpRes = forwardSubsumes(pcbegin, i, myRoot, pclause);
            if(tmpRes)
            {
                return tmpRes;
            }
            else
            return (forwardSubsumes(i, pclause->end(), myRoot, pclause));
        }
    }

    unsigned int
    SubsumptionIndex::forwardSubsumes(Clause::const_iterator startPosition,
            const Clause::const_iterator endPosition, 
            const PNode root, const PClause& pclause) const
    {
        // termination conditions:
        if(root == 0)
            return 0;

        if(root->data.size()) 
        {
            int tmpResult = forwardTestData(root->data.begin(), root->data.end(), pclause);
            if(tmpResult)
            {
                return tmpResult;
            }
        }
        if(startPosition == endPosition)
            return 0;

        /*
        // simplified subsumption procedure: will subsume MUCH less
        // clauses. Wanna see, how it works...
        // Works badly.

        Node::Next::const_iterator ii = root->next.find(*startPosition);
        if(ii == root->next.end())
            return false;
        else
            return forwardSubsumes(startPosition+1, endPosition,
                            ii->second, pclause);

         */

        // Full subsumption procedure
        // Use reverse iterator in order to check all nodes that
        // are less than *startPosition
        //
        Node::Next::const_reverse_iterator
            ri(root->next.upper_bound(*startPosition));
        Node::Next::const_reverse_iterator riend = root->next.rend();

        index_key_comparator comparator;

        for( ; ri != riend; ++ri)
        {
            while ((startPosition != endPosition) &&
                    (comparator(ri->first, *startPosition)))  
                // i.e. ri->first < *startPosition
            {
                ++startPosition;
            }
            if (startPosition == endPosition)
                return 0;

            // Damn ATTRIBUTES!
            if((ri->first.getProposition() == startPosition->getProposition())
                    && (ri->first.getSign() == startPosition->getSign()))
            {
                        TRACE(forwardsubsumptionModule, {
                        ri->first.dump(std::cerr);
                        });
                int tmpResult = forwardSubsumes(startPosition+1, endPosition, ri->second, pclause);
                if(tmpResult)
                {
                        TRACE(forwardsubsumptionModule, {
                        std::cerr << std::endl;
                        });
                    return tmpResult;
                }
            }
                        TRACE(forwardsubsumptionModule, {
                        std::cerr << std::endl;
                        });
        }
        return 0;
    }

    std::pair<SubsumptionIndex::BackwardSubsumptionList::iterator,
              SubsumptionIndex::BackwardSubsumptionList::iterator>
    SubsumptionIndex::backwardSubsumes(const PClause& pclause) 
    {
                       TRACE(backwardsubsumptionModule, {
                       std::cerr << "Backward subsumption\n Index:" ;
                       this->dump(std::cerr);
                       std::cerr << std::endl;
                       std::cerr << "PClause:" ;
                       pclause->dump(std::cerr);
                       std::cerr << std::endl;
                       });

        Clause::const_iterator pcbegin = pclause->begin();
        Clause::const_iterator pcend = pclause->end();
        myBackwardSubsumptionList.clear();

        if(pclause->getLeadingLiteral().getAttribute() != step_next_attr)
        {
            backwardSubsumes(pcbegin, pcend, myRoot, pclause);
        }
        else
        {
            Clause::const_iterator i = nextEnd(pclause);
            
            // THINK!
            if(i != pclause->end())
            {
                backwardSubsumes(i, pclause->end(), myRoot, pclause);
            }
            else
            {
                if(i != pclause->begin())
                    backwardSubsumes(pcbegin, i, myRoot, pclause);
            }
        }
        // output: Unfortunatelly, sometimes (e.g. for (P => next(P)))
        // a clause is put twice into the list. Hence, uniqueness needed
        // first sort
        std::sort(myBackwardSubsumptionList.begin(),
                                myBackwardSubsumptionList.end(), FastCmp());

        // then move not-unique members into the list end
        BackwardSubsumptionList::iterator newEnd = 
            std::unique(myBackwardSubsumptionList.begin(),
                                myBackwardSubsumptionList.end());

        // return the list starting from begin up to the end of 
        // unique clauses
        return std::make_pair(myBackwardSubsumptionList.begin(),
                newEnd);
        /*return std::make_pair(myBackwardSubsumptionList.begin(),
                myBackwardSubsumptionList.end());*/
    }

    void
    SubsumptionIndex::backwardSubsumes(Clause::const_iterator startPosition,
            const Clause::const_iterator endPosition, const PNode root, 
            const PClause& pclause) 
    {
        // termination conditions:
        if(root == 0)
            return; // shorter than pclause

        if(startPosition == endPosition)
        // A-ha: end of pclause riched: it might subsume any/all of the
        // clauses under this node (including those in data)
        {
            if(root->data.size())
            {
                // test all clauses at this node and all its successors
                // put result into myBackwardSubsumptionList
                backwardTestData(root->data, pclause);
            }

            for(Node::Next::iterator ii=root->next.begin();
                    ii != root->next.end(); )
            {
                Assert(ii->second,
                        "SubsumptionIndex: data contains null pointer");
                        TRACE(backwardsubsumptionModule, {
                            ii->first.dump(std::cerr);
                        });
                // go into subnodes
                backwardSubsumes(startPosition, endPosition, ii->second,
                        pclause);
                // the call of backwardSubsumes may delete all clauses
                // under the node. Then, there is no reason to keep this
                // link (in some rare cases it causes a major time
                // consuming problem); therefore, the time spent on
                // this deletion is worth.
                if((ii->second->myNumberOfSubBranches == 0) &&
                        (ii->second->data.size() == 0))
                {
                    delete ii->second;
                    // accrording to the standard, after an element
                    // is deleted from a map, the iterator is not valid
                    // any more and should not be used elsewhere. 
                    // Therefore, I first move it (creating copy),
                    // than delete its previouse value
                    Node::Next::iterator tmpiter = ii++;
                    root->next.erase(tmpiter);
                    (root->myNumberOfSubBranches)--;
                }
                else
                {
                    ++ii;
                }
            }
                       TRACE(backwardsubsumptionModule, {
                       std::cerr << std::endl;
                       });
            return;
        }

        // pclause is not read untill end, there are nodes under this,
        // we have to check only those clause whose literal under 
        // observation is greater than or equal to the literal in
        // *startPosition
        Node::Next::iterator
            i = root->next.lower_bound(*startPosition);
        Node::Next::iterator iend = root->next.end();

        if(i == iend)
            return;
        // must be defined...
        Assert(i->second, "SubsumptionIndex: data contains null pointer");

        // Since i is the lower_bound, the equal literal can only
        // be the first one..
        // if the symbol occures, check the subtree
        // Damn ATTRIBUTES!
        if(startPosition->getProposition() == i->first.getProposition())
        {
            // two possibilities: either it is the same literal..
            if(startPosition->getSign() == i->first.getSign())
            {
                       TRACE(backwardsubsumptionModule, {
                       i->first.dump(std::cerr);
                       });
                // check nodes positioned under root
                backwardSubsumes(startPosition+1, endPosition,
                        i->second, pclause);
                // this call may delete subbranches of i->second
                if((i->second->myNumberOfSubBranches == 0) &&
                    (i->second->data.size() == 0))
                {
                    // delete empty subbranch
                    delete i->second;
                    // accrording to the standard, after an element
                    // is deleted from a map, the iterator is not valid
                    // any more and should not be used elsewhere. 
                    // Therefore, I first move it (creating copy),
                    // than delete its previouse value
                    Node::Next::iterator tmpiter = i++;
                    root->next.erase(tmpiter);
                    root->myNumberOfSubBranches--;
                }
                else
                {
                    // subbranch is not empty
                    ++i;
                }
            }
            else
            {
                // .. or one more exceptional case: it might be that the
                // literal *i differs from *startPosition only by its sign.
                // Note that in no clause containing the negation of
                // *startPositon can be subsumed by the given clause. Due to
                // our order, this may only happen when in the previouse case,
                // *i differs from *startPosition (and only by its sign)
                // skip this i
                i++;
            }
        }

        // check all the rest
        for( ;  i !=  iend;  )
        {
            // go into subtrees but with the same value of 
            // startPosition.
            Assert(i->second, "SubsumptionIndex: data contains null pointer");
            //if(i->second)
               TRACE(backwardsubsumptionModule, {
               i->first.dump(std::cerr);
               });
            backwardSubsumes(startPosition, endPosition, i->second, 
                    pclause);
            if((i->second->myNumberOfSubBranches == 0) &&
                    (i->second->data.size() == 0))
            {
                delete i->second;
                // accrording to the standard, after an element
                // is deleted from a map, the iterator is not valid
                // any more and should not be used elsewhere. 
                // Therefore, I first move it (creating copy),
                // than delete its previouse value
                Node::Next::iterator tmpiter = i++;
                root->next.erase(tmpiter);
                root->myNumberOfSubBranches--;
            }
            else
            {
                ++i;
            }
        }
                       TRACE(backwardsubsumptionModule, {
                       std::cerr << std::endl;
                       });
        return;
    }

    Clause::const_iterator
    SubsumptionIndex::nextEnd(const PClause& pclause) const
    {
        Clause::const_iterator iend = pclause->end();
        Clause::const_iterator i = pclause->begin();
        for( ; i != iend; ++i)
        {
            if(i->getAttribute().getType() == Attribute::step_now)
            {
                break;
            }
        }
        return i;
    }

    unsigned int
    SubsumptionIndex::forwardTestData(std::list<PClause>::const_iterator
            startClause, const std::list<PClause>::const_iterator endClause, 
            const PClause& pclause) const
    {
        const Attribute tmp1Attribute =
            pclause->getLeadingLiteral().getAttribute();
        
        if( tmp1Attribute == initial_attr)
        {
            // initial clause can be subsumed either by universal
            // or by initial clause. If one of these clauses is in 
            // the list, return true, else do nothing (return false
            // in the end
            for( ; startClause != endClause; ++startClause)
            {
                const Attribute &tmpAttribute =
                    (*startClause)->getLeadingLiteral().getAttribute();

                if((tmpAttribute == initial_attr || (tmpAttribute ==
                                universal_attr) ))
                {
                    return (*startClause)->getId();
                }
            }
        }
        else if( tmp1Attribute == universal_attr)
        {
            // univeral clause can be subsumed by universal
            // clause only. If one of universal clauses is in 
            // the list, return true, else do nothing (return false
            // in the end
            for( ; startClause != endClause; ++startClause)
            {
                if((*startClause)->getLeadingLiteral().getAttribute() ==
                        universal_attr)
                {
                    return (*startClause)->getId();
                }
            }
        }
        else if( tmp1Attribute == step_now_attr) 
        {
            // step_now clause can be subsumed by step_now
            // clause only (in case of normal mode, never reached)
            for( ; startClause != endClause; ++startClause)
            {
                if((*startClause)->getLeadingLiteral().getAttribute() ==
                        step_now_attr)
                {
                    return (*startClause)->getId();
                }
            }
        }
        else // if ( tmp1Attribute == step_next_attr )
        {
            // step_next clause can be subsumed by universal or step_next
            // clauses. 
            for( ; startClause != endClause; ++startClause)
            {
                const Attribute &tmpAttribute =
                    (*startClause)->getLeadingLiteral().getAttribute();

                if ((tmpAttribute == universal_attr))
                {
                    // universal clause subsumed one of the 
                    // parts of the step_next clause. Must improve
                    // "the ugliest case".
                    // HAHA
                    return (*startClause)->getId();
                }
                // otherwise, I need check. But instead of general 
                // clauseSubsumes, I use implementation dependend thing
                // that would be eventually called anyway.

                else if (tmpAttribute == step_next_attr)
                {
                    TRACE(subsumptionModule, {
                            std::cerr << "\nsubsumption test for C = ";
                           (*startClause)->dump(std::cerr);
                           std::cerr << " and D = ";
                           pclause->dump(std::cerr);
                           std::cerr << std::endl;
                       });

                    if(implementationDependentSubsumesHelper(*startClause, pclause))
                    {
                        return (*startClause)->getId();
                    }
                }
            }
        }
        // the list is checked, true is not returned
        return 0;
    }

    void
    SubsumptionIndex::backwardTestData(std::list<PClause>& data,
            const PClause& pclause) 
    {
        std::list<PClause>::iterator startClause = data.begin();
        std::list<PClause>::iterator endClause = data.end();

        const Attribute tmp1Attribute =
            pclause->getLeadingLiteral().getAttribute();
        
        if( tmp1Attribute == initial_attr)
        {
            // initial clause can subsume initial clauses only. 
            while ( startClause != endClause )
            {
                if((*startClause)->getLeadingLiteral().getAttribute() ==
                        initial_attr)
                {
                    myBackwardSubsumptionList.push_back(*startClause);
                        TRACE(backwardsubsumptionModule, { 
                            std::cerr  << std::endl << "collecting ";
                            (*startClause)->dump(std::cerr);
                            std::cerr << std::endl;
                            });
                    startClause = data.erase(startClause);
                }
                else
                {
                    ++startClause;
                }
            }
        }
        else if( tmp1Attribute == universal_attr)
        {
            // univeral clause can subsume any kind of clause
            // step_now and step_next parts of step clauses are
            // kept separately anyway, so this is safe.
            while( startClause != endClause )
            {
                if((*startClause)->getLeadingLiteral().getAttribute() ==
                        universal_attr)
                {
                    myBackwardSubsumptionList.push_back(*startClause);
                        TRACE(backwardsubsumptionModule, { 
                            std::cerr  << std::endl << "collecting ";
                            (*startClause)->dump(std::cerr);
                            std::cerr << std::endl;
                            });
                    startClause = data.erase(startClause);
                }
                else
                {
                    ++startClause;
                }
            }
        }
        else if( tmp1Attribute == step_now_attr) 
        {
            // step_now clause can subsume step_now and step next
            // clauses only (in case of normal mode, never reached)
            while (  startClause != endClause )
            {
                const Attribute &tmpAttribute =
                    (*startClause)->getLeadingLiteral().getAttribute();

                if(tmpAttribute == step_now_attr)
                {
                    myBackwardSubsumptionList.push_back(*startClause);
                        TRACE(backwardsubsumptionModule, { 
                            std::cerr  << std::endl << "collecting ";
                            (*startClause)->dump(std::cerr);
                            std::cerr << std::endl;
                            });
                    startClause = data.erase(startClause);
                }
                // to be safe (might be here because of step_next part)
                else if((tmpAttribute == step_next_attr) &&
                  (implementationDependentSubsumesHelper(pclause,*startClause)))
                {
                    myBackwardSubsumptionList.push_back(*startClause);
                        TRACE(backwardsubsumptionModule, { 
                            std::cerr  << std::endl << "collecting ";
                            (*startClause)->dump(std::cerr);
                            std::cerr << std::endl;
                            });
                    startClause = data.erase(startClause);
                }
                else
                {
                    ++startClause;
                }

            }
        }
        else // if (tmp1Attribute == step_next_attr)
        {
            // step_next clause can subsume step_next clauses only. 
            // step_now and step_next parts are kept separately, hence,
            // extra check is needed
            while ( startClause != endClause )
            {
                const Attribute &tmpAttribute =
                    (*startClause)->getLeadingLiteral().getAttribute();
                
                if((tmpAttribute == step_next_attr) && 
                  (implementationDependentSubsumesHelper(pclause,*startClause)))
                {
                    myBackwardSubsumptionList.push_back(*startClause);
                        TRACE(backwardsubsumptionModule, { 
                            std::cerr  << std::endl << "collecting ";
                            (*startClause)->dump(std::cerr);
                            std::cerr << std::endl;
                            });
                    startClause = data.erase(startClause);
                }
                else
                {
                    ++startClause;
                }
            }
        }
        // the list is checked
        return;
    }

    SubsumptionIndex::Node::~Node()
    {
        for (Next::iterator i = next.begin(); i != next.end(); ++i)
        {
            if(i->second)
                delete i->second;
        }
    }

    std::ostream& // I do not want to have extra files...
    SubsumptionIndex::Node::dump(std::ostream& os, 
            const int alignment) const
    {
        if (this->data.size())
        {
            os << "--->";
            for(std::list<PClause>::const_iterator cj = data.begin();
                    cj != data.end(); ++cj)
            {
                (*cj)->dump(os);
                os << " ";
            }
        }
        os << std::endl;

        for( Next::const_iterator i = next.begin(); i != next.end(); ++i)
        {
            for (int align = alignment; align; align--)
                os << " ";

            i->first.dump(os);
            if(i->second)
                i->second->dump(os, alignment+1);
        }
        return os;
    }

    // static members:

//    TODO
//    LIST
//    SubsumptionIndex::backwardSubsumes(const PClause& pclause) const
//    {
//    }
//
//    LIST
//    SubsumptionIndex::backwardSubsumes(Clause::const_iterator startPosition, 
//            const Clause::const_iterator endPosition, PNode root, 
//            const PClause& pclause) const
//    {
//    }
} // namespace PropositionalProver
