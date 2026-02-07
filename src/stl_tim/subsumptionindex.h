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
#ifndef __SUBSUMPTIONINDEX_H__
#define __SUBSUMPTIONINDEX_H__

#include <map>
#include <list>
#include "attribute.h"
#include "clause.h"
#include "pclause.h"
#include "infer.h"
#include "keycomparator.h"

namespace PropositionalProver
{
    
    // this class implements trie-based imperfect indexing augmented with
    // a 'real' subsumption test for candidates. 
    // Indexed clauses are kept in a trie; retrieval conditions are tuned
    // to subsumption (in fact).
    class SubsumptionIndex 
    {
    private:
        struct Node; // forward declaration
        typedef Node* PNode;
    public:
        SubsumptionIndex()
        : myRoot(0)
        { }

        SubsumptionIndex(const SubsumptionIndex& subsumptionIndex);

        void
        clear() { if(myRoot) delete myRoot; myRoot = 0; }

        ~SubsumptionIndex() { this->clear(); } 

        void
        insert(const PClause& pclause);

        void
        remove(const PClause& pclause);

        // returns true if the clause pclause is subsumed by some
        // indexed clause
        unsigned int 
        forwardSubsumes(const PClause& pclause) const;

        // finds clauses subsumed by pclause, DELETES them from the 
        // index, creates internal list containing them and 
        // returns the sequence of removed clauses.
        typedef std::vector<PClause> BackwardSubsumptionList;

        std::pair<BackwardSubsumptionList::iterator,
                  BackwardSubsumptionList::iterator>
        backwardSubsumes(const PClause& pclause);

        std::ostream&
        dump(std::ostream& os) const { if(myRoot) return myRoot->dump(os); return os;}
    private:
        void
        insert(Clause::const_iterator startPosition, 
                const Clause::const_iterator endPosition, PNode& root, 
                const PClause& pclause);
        void
        remove(Clause::const_iterator startPosition, 
                const Clause::const_iterator endPosition, const PNode root, 
                const PClause& pclause);
        unsigned int
        forwardSubsumes(Clause::const_iterator startPosition, 
                const Clause::const_iterator endPosition, const PNode root,
                const PClause& pclause) const;

        void
        backwardSubsumes(Clause::const_iterator startPosition, 
                const Clause::const_iterator endPosition, const PNode root,
                const PClause& pclause);

        Clause::const_iterator
        nextEnd(const PClause& pclause) const;

        // returns the Id of a clause from the sequence subsumes pclause
        unsigned int 
        forwardTestData(std::list<PClause>::const_iterator startClause, 
                const std::list<PClause>::const_iterator endPosition,
                const PClause& pclause) const;
        // for all the clauses from the list, checks whether the 
        // clause is subsumed by pclause. If yes, adds this clause 
        // into myBackwardSubsumptionList and deletes it from the 
        // index
        void
        backwardTestData(std::list<PClause>& data,
                const PClause& pclause);
    private:
        struct Node
        {
            // a map of pointers to nodes
            typedef std::map<Literal, PNode, index_key_comparator> Next;
            std::list<PClause> data;
            Next next;

            // contains the number of subbranches originating at
            // this node.  if 0, the node should be deleted..
            int myNumberOfSubBranches; 

            // constructrors:
            //   default
            Node()
            : data(), next(), myNumberOfSubBranches(0)
            { }

            //   copy
            Node(const Node& node)
            : data(node.data), next(), myNumberOfSubBranches(node.myNumberOfSubBranches)
            {
                for (Next::const_iterator i = node.next.begin();
                        i != node.next.end(); ++i)
                {
                    if(i->second)
                    {
                        next[i->first] = new Node(*(i->second));
                    }
                }
            }

            ~Node();
            std::ostream&
            dump(std::ostream& os, const int alignment = 0) const;
        };
    private:
        PNode myRoot;
        BackwardSubsumptionList myBackwardSubsumptionList;
    };
} // namespace PropositionalProver


#endif // __SUBSUMPTIONINDEX_H__
