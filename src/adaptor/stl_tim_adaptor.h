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
#ifndef __STL_TIM_ADAPTOR_H__
#define __STL_TIM_ADAPTOR_H__

#include <string>
#include <iostream>
#include "misc/iterators.h"
#include <utility>
#include <iterator>
#include "stl_tim/stl_tim.h"
#include "misc/exception.h"
#include "misc/assert.h"
#include "config.h"
#include "getopt/options.h"
#include "misc/i2s.h"

namespace Adaptors
{
    // I am not happy with the way how all this is written. 
    // Needed: take parts out of the class
    // make Adaptor an Adaptor Generator with policies
    // this will be possibly done after I will have the current version 
    // working
    //
    // definition
    class Adaptor : public PropositionalProver::ClauseSet
    {
    public:
        // public types
        typedef PropositionalProver::PClause internal_clause_rep;
        typedef ClauseSet internal_clause_collection_rep;
        // all adaptors _must provide_ the following types:
        //
        typedef PropositionalProver::Proposition Proposition;
        typedef PropositionalProver::Literal Literal;
        typedef PropositionalProver::LiteralList LiteralList;
        // const_clause_iterator iterates over internal representation of
        // clauses (internal_clause_rep*)
        typedef PropositionalProver::Clause::const_iterator clause_const_iterator;
        typedef PropositionalProver::Statistics InternalStatistics;
    private:
        // some internal data structures and functions:
        // recall that we keep clauses in some internal representation
        // Thus, we need selecting function objects to distinguish them
        struct IsInitial;
        struct IsUniversal;
        struct IsStep;
        struct IsStepNextFalse;
    public:
        // all adaptors _must define_ the following types:
        // IClause: I:(literal, literal,...)
        // UClause: U:(literal, literal,...)
        // SClause: S(literal, literal,..., next(literal, literal, ...))
        class IClause;
        class UClause;
        class SClause;
        // entities generation public interface:
        // these functions take LiteralList, not const LiteralList& for
        // purpose
        static inline IClause make_iclause(LiteralList ll, const std::string s = "");
        static inline UClause make_uclause(LiteralList ll, const std::string s = "");
        static inline SClause make_sclause(LiteralList ll1, LiteralList ll2, const std::string s = "");

    private:
        // private accessing information:
        // used to translate clauses from Adaptee internal representation
        // into our representation on the fly
        // Unfortunatelly, I cannot move these definitions apart keeping
        // class declarations as I did with the other classes above
        // du to later use of them as template arguments
        struct toInitial
        {
            typedef internal_clause_rep argument_type;
            typedef IClause result_type;
            IClause operator() (const internal_clause_rep& clause) const
            {
                return IClause(clause);
            }
        };
        struct toUniversal
        {
            typedef internal_clause_rep argument_type;
            typedef UClause result_type;
            UClause operator() (const internal_clause_rep& clause) const
            {
                return UClause(clause);
            }
        };
        struct toStep
        {
            typedef internal_clause_rep argument_type;
            typedef SClause result_type;
            SClause operator() (const internal_clause_rep& clause) const
            {
                return SClause(clause);
            }
        };
    public:
        // iterators that iterate over specific clauses
        // I am using boost library here as it simplifies A LOT 
        // these issue. transform iterator applies a function 
        // to all object it iterates on; filtering iterator
        // checks whether objects satisfy a specific retrieval
        // condition, if yes, iterates on them, skips else
        typedef Misc::transform_iterator<toInitial, Misc::filter_iterator<IsInitial, internal_clause_collection_rep::const_iterator> >
            const_i_iterator;
        typedef Misc::transform_iterator<toUniversal, Misc::filter_iterator<IsUniversal, internal_clause_collection_rep::const_iterator> >
            const_u_iterator;
        typedef Misc::transform_iterator<toStep, Misc::filter_iterator<IsStep, internal_clause_collection_rep::const_iterator> >
            const_s_iterator;

        typedef Misc::transform_iterator<toUniversal, Misc::filter_iterator<IsStepNextFalse, internal_clause_collection_rep::const_iterator> >
            const_next_false_as_universal_iterator;

        typedef Misc::transform_iterator<toStep, Misc::filter_iterator<IsStepNextFalse, internal_clause_collection_rep::const_iterator> >
            const_next_false_as_step_iterator;

        // Access to specific clauses in the clause set
        //
        // start of initial clauses
        const_i_iterator 
        initial_begin() const;

        // end of initial clauses
        const_i_iterator 
        initial_end() const;

        // etc.
        const_u_iterator 
        universal_begin() const;

        const_u_iterator 
        universal_end() const;

        const_s_iterator 
        step_begin() const;

        const_s_iterator 
        step_end() const;
        
        // tricki part: sometimes (in loop search) I need to find 
        // all "step" clauses with empty next part.
        // The resulting subset can be considered either as a 
        // set of universal clauses
        const_next_false_as_universal_iterator 
        next_false_as_universal_begin() const;

        const_next_false_as_universal_iterator 
        next_false_as_universal_end() const;
        
        // or step clauses
        const_next_false_as_step_iterator 
        next_false_as_step_begin() const;

        const_next_false_as_step_iterator 
        next_false_as_step_end() const;
        
        // saturate the set of clauses by step resolution.
        // returns a pair of booleans (to be replaced with something
        // more verbal): the first is true, if a contradiction was
        // derived, the second one is true, if some new information
        // was obtained.
        std::pair<bool, bool>
        saturate(const TRPPPOptions& options)
        {
            ClauseSet::Flags f;
            f.useFSR = options.isUseFSR();
            if(getMode() == loopSearch)
            {
                TRACE(resolutionModule, {
                std::cerr << "LOOPSEARCH MODE\n";
                });
                f.loopSearch = true;
            }
            else
            {
                TRACE(resolutionModule, {
                std::cerr << "NORMAL MODE\n";
                })
            }
            return ClauseSet::saturate(f);
        }

        // adding new clauses into the clause set
        inline void push_back(const IClause&);
        inline void push_back(const UClause&);
        inline void push_back(const SClause&);

        // to/from internal representation
        // to be used e.g. to copy one clause set into another
        typedef internal_clause_collection_rep::const_iterator const_internal_rep_iterator;

        const_internal_rep_iterator 
        begin() const
        {
            return internal_clause_collection_rep::begin();
        }

        const_internal_rep_iterator 
        end() const
        {
            return internal_clause_collection_rep::end();
        }

        inline void push_back(const internal_clause_rep& p);

        // modes of operation
    public:
        enum OperationMode {normal, loopSearch};
    private:
        static OperationMode ourMode;
    public:
        static void 
        setMode(OperationMode);

        static OperationMode 
        getMode() { return ourMode; }

        // useful operations..
        //  subsumption
        //  helper class
    private:
        template<typename _Clause>
        struct toInternalRepresentation
        {
            typedef _Clause argument_type;
            typedef internal_clause_rep result_type;
            result_type operator() (const argument_type& clause) const
            {
                return clause.getInternalRep();
            }
        };
        // Takes two sets (as ranges) of clauses of THE SAME TYPE
        // if for each clause from the second set there exists a clause
        // from the first subset that subsumes the clause, return true,
        // else return false
    public:
        template<typename _Iterator>
            static inline bool 
            subsumesOrEqual(const _Iterator firstBegin,  const _Iterator firstEnd, 
                            const _Iterator secondBegin, const _Iterator secondEnd);

        void
        getForLoop(const Adaptor& adaptor);

        bool
        notMaximal(clause_const_iterator begin, clause_const_iterator end);

        const InternalStatistics&
        getInternalStatistics() const;

        std::ostream& 
        dump(std::ostream& os) const;

        // C++ requires the folloing classes to be defined INSIDE the
        // class Adaptor. I put them in the end of class Adaptor declaration
        // in order to improve readability. No entities not declared above
        // should occure till the end of class Adaptor declaration.
    private:
        struct IsInitial
        {
            bool
            operator() (const internal_clause_rep& clauseRep) const
            {
                if (clauseRep->size() == 0) 
                    return true; // consider the empty clause as initial
                if(clauseRep->getLeadingLiteral().getAttribute() == PropositionalProver::initial_attr)
                    return true;
                else
                    return false;
            }
        };
        struct IsUniversal
        {
            bool
            operator() (const internal_clause_rep& clauseRep) const
            {
                if (clauseRep->size() == 0) 
                    return false; // it is initial
                PropositionalProver::Attribute tmpAttribute = clauseRep->getLeadingLiteral().getAttribute();
                if(tmpAttribute == PropositionalProver::universal_attr)
                    return true;
                else
                // in the normal mode we consider clauses whose leading 
                // literal is step_now as universal clauses
                // in the loopSearch mode, they are step clauses
                    if((getMode() == normal) && (tmpAttribute == PropositionalProver::step_now_attr))
                        return true;
                    else
                        return false;
            }
        };
        struct IsStep
        {
            bool
            operator() (const internal_clause_rep& clauseRep) const
            {
                if (clauseRep->size() == 0) 
                    return false; // it is initial
                PropositionalProver::Attribute tmpAttribute = clauseRep->getLeadingLiteral().getAttribute();
                if(tmpAttribute == PropositionalProver::step_next_attr)
                    return true;
                else
                // in the normal mode we consider clauses whose leading 
                // literal is step_now as universal clauses
                // in the loopSearch mode, they are step clauses
                    if((getMode() == loopSearch) && (tmpAttribute == PropositionalProver::step_now_attr))
                        return true;
                    else
                        return false;
            }
        };
        struct IsStepNextFalse
        {
            bool
            operator() (const internal_clause_rep& clauseRep) const
            {
                if (clauseRep->size() == 0) 
                    return false; // it is initial
                // as step_next-literals are always greater than
                // step_now-ones, if the leading literal is step_now, then
                // there is no step_next-literals in the clause
                // This does not depend on the mode (although we consider
                // these clauses as step-clauses)
                if(clauseRep->getLeadingLiteral().getAttribute() == PropositionalProver::step_now_attr)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        };
    public:
        class IClause
        {
        private:
            internal_clause_rep myClause;
        public:
            typedef clause_const_iterator const_iterator;
            IClause(internal_clause_rep clause)
            : myClause(clause)
            { }
            const_iterator 
            begin() const
            {
                return (myClause->begin());
            }
            const_iterator 
            end() const
            {
                return (myClause->end());
            }
            const internal_clause_rep& 
            getInternalRep() const
            {
                return myClause;
            }

            const std::string&
            getInfo() const
            {
                return myClause->getInfo();
            }

            const int
            getId() const
            {
                return myClause->getId();
            }
        }; // class IClause
        class UClause
        {
        private:
            internal_clause_rep myClause;
        public:
            typedef clause_const_iterator const_iterator;
            UClause(internal_clause_rep clause)
            : myClause(clause)
            { }
            const_iterator 
            begin() const
            {
                return (myClause->begin());
            }
            const_iterator 
            end() const
            {
                return (myClause->end());
            }
            const internal_clause_rep&
            getInternalRep() const
            {
                return myClause;
            }
            const std::string&
            getInfo() const
            {
                return myClause->getInfo();
            }
            const int
            getId() const
            {
                return myClause->getId();
            }
        }; // class UClause
        class SClause
        {
        private:
            internal_clause_rep myClause;
        public:
            typedef clause_const_iterator const_present_iterator;
            typedef clause_const_iterator const_future_iterator;
            SClause(internal_clause_rep clause)
            : myClause(clause)
            { }
            const_present_iterator 
            present_begin() const;
            const_present_iterator 
            present_end() const;
    
            const_future_iterator 
            future_begin() const;
            const_present_iterator 
            future_end() const;

            const internal_clause_rep&
            getInternalRep() const
            {
                return myClause;
            }

            const std::string&
            getInfo() const
            {
                return myClause->getInfo();
            }

            const int
            getId() const
            {
                return myClause->getId();
            }
        }; // class SClause
    }; // class Adaptor


    // IMPLEMENTATION
    // Since these functions form a time-critical part of the program,
    // they all are inlined.
    //
    inline Adaptor::const_i_iterator 
    Adaptor::initial_begin() const
    {
        return const_i_iterator(Misc::make_filter_iterator<IsInitial>(internal_clause_collection_rep::begin(), internal_clause_collection_rep::end()));
    }

    inline Adaptor::const_i_iterator 
    Adaptor::initial_end() const
    {
        return const_i_iterator(Misc::make_filter_iterator<IsInitial>(internal_clause_collection_rep::end(), internal_clause_collection_rep::end()));
    }

    inline Adaptor::const_u_iterator 
    Adaptor::universal_begin() const
    {
        return const_u_iterator(Misc::make_filter_iterator<IsUniversal>(this->begin(), this->end()));
    }

    inline Adaptor::const_u_iterator 
    Adaptor::universal_end() const
    {
        return const_u_iterator(Misc::make_filter_iterator<IsUniversal>(this->end(), this->end()));
    }

    inline Adaptor::const_s_iterator 
    Adaptor::step_begin() const
    {
        return const_s_iterator(Misc::make_filter_iterator<IsStep>(this->begin(), this->end()));
    }

    inline Adaptor::const_s_iterator 
    Adaptor::step_end() const
    {
        return const_s_iterator(Misc::make_filter_iterator<IsStep>(this->end(), this->end()));
    }
    
    inline Adaptor::const_next_false_as_universal_iterator 
    Adaptor::next_false_as_universal_begin() const
    {
        return const_next_false_as_universal_iterator(Misc::make_filter_iterator<IsStepNextFalse>(this->begin(), this->end()));
    }

    inline Adaptor::const_next_false_as_universal_iterator 
    Adaptor::next_false_as_universal_end() const
    {
        return const_next_false_as_universal_iterator(Misc::make_filter_iterator<IsStepNextFalse>(this->end(), this->end()));
    }
    
    inline Adaptor::const_next_false_as_step_iterator 
    Adaptor::next_false_as_step_begin() const
    {
        return const_next_false_as_step_iterator(Misc::make_filter_iterator<IsStepNextFalse>(this->begin(), this->end()));
    }

    inline Adaptor::const_next_false_as_step_iterator 
    Adaptor::next_false_as_step_end() const
    {
        return const_next_false_as_step_iterator(Misc::make_filter_iterator<IsStepNextFalse>(this->end(), this->end()));
    }
        
    template<typename _Iterator>
        inline bool 
        Adaptor::subsumesOrEqual(const _Iterator firstBegin,  const _Iterator firstEnd, 
                            const _Iterator secondBegin, const _Iterator secondEnd)
        {
            typedef typename Misc::transform_iterator<toInternalRepresentation<typename std::iterator_traits<_Iterator>::value_type>, _Iterator> transformIterator;

            transformIterator transformFirstBegin(firstBegin);
            transformIterator transformFirstEnd(firstEnd);
            for(_Iterator currentSecond = secondBegin;
                    currentSecond != secondEnd;
                    ++currentSecond)
            {
                if (!PropositionalProver::rangeSubsumesOrEqual(
                            transformFirstBegin, transformFirstEnd, 
                            currentSecond->getInternalRep()))
                {
//std::cerr << "Non-subsumed clause: ";
//currentSecond->getInternalRep()->dump(std::cerr);
//std::cerr << std::endl;
                    return false;
                }
            }
            return true;
        }

    inline void
    Adaptor::getForLoop(const Adaptor& adaptor)
    {
        // taking clauses needed for loop search
        LiteralList emptyLiteralList;
        IsUniversal isUniversal;
        IsStep isStep;
        const_internal_rep_iterator iend = adaptor.end();
        for(const_internal_rep_iterator i = adaptor.begin();
                i != iend; ++i)
        {
            if(isUniversal(*i))
            {
                // in this case we have to rewrite those clauses 
                // into next-flase step clauses
                //
                // Do I really need to make an sclause all the time,
                // or UClauses will be also OK?
                LiteralList literalList((*i)->begin(), (*i)->end());
                SClause tmpSClause =
                make_sclause(emptyLiteralList,
                   literalList, (std::string("loop S") + itos((*i)->getId())));
//tmpSClause.getInternalRep()->dump(std::cerr);
//std::cerr << std::endl;
                this->toFirst(tmpSClause.getInternalRep());
            }
            if(isStep(*i))
            {
                this->toFirst(*i);
            }
        }
    }
    inline bool
    Adaptor::notMaximal(clause_const_iterator begin, clause_const_iterator end)
    {
        for(clause_const_iterator i = begin;
                i != end;
                ++i)
        {
            std::pair< PropositionalProver::IndexCarier::const_iterator,
                       PropositionalProver::IndexCarier::const_iterator > candidates = 
                           get_first().getComplementary(*i);
            if(candidates.first == candidates.second)
                return true;
        }
        return false;
    }
    inline const Adaptor::InternalStatistics&
    Adaptor::getInternalStatistics() const
    {
        return internal_clause_collection_rep::getStatistics();
    }

    inline void
    Adaptor::push_back(const Adaptor::IClause& c)
    {
        // get the internal representation
        internal_clause_rep cp(c.getInternalRep());
        // put it into the collection
        this->push_back(cp);
    }

    inline void
    Adaptor::push_back(const UClause& c)
    {
        // get the internal representation
        internal_clause_rep cp(c.getInternalRep());
        // put it into the collection
        this->push_back(cp);
    }

    inline void
    Adaptor::push_back(const SClause& c)
    {
        // get the internal representation
        internal_clause_rep cp(c.getInternalRep());
        // put it into the collection
        this->push_back(cp);
    }

    // This function is a public interface to the internal clause
    // representation. Therefore, it should check whether the data
    // to be inserted is correct befoure actual insert.
    inline void
    Adaptor::push_back(const internal_clause_rep& cp)
    {
        // check whether the inserting clause is correct
        // (not a tautology)
        if(cp->isCorrect())
        {
            this->internal_clause_collection_rep::push_back(cp);
        }
        // else, nothing to add..
    }

    inline Adaptor::IClause
    Adaptor::make_iclause(Adaptor::LiteralList ll, const std::string s)
    {
        typedef Adaptor::LiteralList::iterator llcit;
        llcit pend = ll.end();
        for (llcit p = ll.begin(); p != pend; ++p)
        {
            // assign correct 'initial' attribute to literals..
            p->setAttribute(PropositionalProver::initial_attr);
        }
        return Adaptor::IClause(Adaptor::make_clause(ll,s));
    }

    inline Adaptor::UClause
    Adaptor::make_uclause(Adaptor::LiteralList ll, const std::string s)
    {
        typedef Adaptor::LiteralList::iterator llcit;
        llcit pend = ll.end();
        for (llcit p = ll.begin(); p != pend; ++p)
        {
            // assign correct 'universal' attribute to literals..
            p->setAttribute(PropositionalProver::universal_attr);
        }
        return Adaptor::UClause(Adaptor::make_clause(ll,s));
    }

    inline Adaptor::SClause
    Adaptor::make_sclause(Adaptor::LiteralList ll1,
            Adaptor::LiteralList ll2, const std::string s)
    {
        typedef Adaptor::LiteralList::iterator llcit; 
        llcit pend = ll1.end();
        for (llcit p = ll1.begin(); p != pend; ++p)
        {
            // assign correct 'present' attribute to literals
            p->setAttribute(PropositionalProver::step_now_attr);
        }
        pend = ll2.end();
        for (llcit p = ll2.begin(); p != pend; ++p)
        {
            // assign correct 'future' attribute to literals
            p->setAttribute(PropositionalProver::step_next_attr);
        }
        //ll1.splice(ll1.end(),ll2);
        std::copy(ll2.begin(), ll2.end(), std::back_inserter(ll1));
        return Adaptor::SClause(Adaptor::make_clause(ll1,s));
    }

} // namespace Adaptors
#endif // __STL_TIM_ADAPTOR_H__
