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
#ifndef __BUILDER_H__
#define __BUILDER_H__
#include <string>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include "misc/exception.h"
#include "generic_builder.h"

namespace Exceptions
{
    class BuilderException : public Exception
    {
        public:
            virtual void 
            module(std::ostream& os) const 
            { 
                os << "Builder";
            }; 
    }; // class BuilderException
    class TypeMissmatch : public BuilderException
    {
    private:
        std::string name;
    public:
        TypeMissmatch(std::string n)
        : name(n)
        { }
        virtual void 
        what (std::ostream& os) const
        {
            os << "Type missmatch with " << name << std::endl ;
        }
    }; // class TypeMissmatch

    class EmptyClauseInInput : public BuilderException
    {
    public:
        EmptyClauseInInput() {}
        virtual void what (std::ostream& os) const
        {
            os << "Empty clause in the input\n";
        }
    };
} // namespace Exceptions

namespace Builders
{
    using namespace Exceptions;
    template <typename _ClauseSet> 
        class Builder : public GenericBuilder
        {
        public:
            typedef _ClauseSet ClauseSet;
            typedef typename ClauseSet::IClause IClause;
            typedef typename ClauseSet::UClause UClause;
            typedef typename ClauseSet::SClause SClause;
            typedef typename ClauseSet::EClause EClause;
            typedef typename ClauseSet::Literal Literal;
            typedef typename ClauseSet::LiteralList LiteralList;
            typedef typename ClauseSet::Proposition Proposition;

        private:
            mutable std::vector<std::unique_ptr<building_obj>> arena_;

            template <typename T, typename... Args>
            pbuilding_obj
            make_obj(Args&&... args) const
            {
                std::unique_ptr<T> ptr = std::make_unique<T>(std::forward<Args>(args)...);
                pbuilding_obj raw = ptr.get();
                arena_.push_back(std::move(ptr));
                return raw;
            }

            // replacement for building_obj
            struct clauselist_obj : public building_obj
            {
                ClauseSet cl;
                virtual void 
                dummy_function() { }
            };
        
            struct initclause_obj : public building_obj
            {
                IClause ic;
                initclause_obj(const IClause& _ic)
                : ic(_ic)
                { }
                virtual void 
                dummy_function() { }
            };
        
            struct univclause_obj : public building_obj
            {
                UClause uc;
                univclause_obj(const UClause& _uc)
                : uc(_uc)
                { }
                virtual void 
                dummy_function() { }
            };
        
            struct stepclause_obj : public building_obj
            {
                SClause sc;
                stepclause_obj(const SClause& _sc)
                : sc(_sc)
                { }
                virtual void 
                dummy_function() { }
            };
        
            struct eventclause_obj : public building_obj
            {
                EClause ec;
                eventclause_obj(const EClause& _ec)
                : ec(_ec)
                { }
                virtual void 
                dummy_function() { }
            };
        
            struct literallist_obj : public building_obj
            {
                LiteralList ll;
                //literallist_obj(const LiteralList& _ll)
                //: ll(_ll)
                //{ }
                virtual void 
                dummy_function() { }
            };
        
            struct literal_obj : public building_obj
            {
                Literal l;
                literal_obj(const Literal& _l)
                : l(_l)
                { }
                virtual void 
                dummy_function() { }
            };
        
            struct atom_obj : public building_obj
            {
                Proposition p;
                atom_obj(const Proposition& _p)
                : p(_p)
                { }
                virtual void 
                dummy_function() { }
            };
        public: 

            static ClauseSet 
            getClauseSet(const pbuilding_obj o) 
            {
                clauselist_obj* pcl = dynamic_cast<clauselist_obj*>(o);
                if(!pcl)
                    throw TypeMissmatch("getClauseSet: o is not a clause set");
                else
                {
                    return pcl->cl;
                }
            }

            virtual pbuilding_obj 
            create_clause_list() const
            {
                return make_obj<clauselist_obj>();
            }
    
            virtual pbuilding_obj
            create_literal_list() const
            {
                return make_obj<literallist_obj>();
            }

            virtual pbuilding_obj
            add_literal_to_list(const pbuilding_obj o1, const pbuilding_obj o2) const
            {
                literallist_obj* pll = dynamic_cast<literallist_obj*>(o1);
                if (!pll)
                    throw TypeMissmatch("Add_literal_to_list: literal list");
                literal_obj* pl = dynamic_cast<literal_obj*>(o2);
                if (!pl)
                    throw TypeMissmatch("Add_literal_to_list: literal");
                pll->ll.push_back(pl->l);
                return pbuilding_obj(pll);
            }
        
            virtual pbuilding_obj
            add_clause_to_list(const pbuilding_obj o1, const pbuilding_obj o2) const
            {
                // This is the most difficult function due to polimorphism
                // Pre-condition: o1 must be a clauselist, o2 a clause
                // RTTI is used for this 
                clauselist_obj* pcl = dynamic_cast<clauselist_obj*>(o1);
                if (!pcl)
                    throw TypeMissmatch(" Add_clause_to_list: Clause list");
                // trying possible combinations...
                initclause_obj* pc = dynamic_cast<initclause_obj*>(o2);
                if (pc)
                {
                    // o2 is an initial clause
                    pcl->cl.push_back(pc->ic);
                    return pbuilding_obj(pcl);
                }
                else
                {
                    univclause_obj* pc = dynamic_cast<univclause_obj*>(o2);
                    if (pc)
                    {
                        // o2 is a universal clause
                        pcl->cl.push_back(pc->uc);
                        return pbuilding_obj(pcl);
                    }
                    else
                    {
                        stepclause_obj* pc = dynamic_cast<stepclause_obj*>(o2); 
                        if (pc)
                        {
                            // o2 is a step clause
                            pcl->cl.push_back(pc->sc);
                            return pbuilding_obj(pcl);
                        }
                        else 
                        {
                            eventclause_obj* pc = dynamic_cast<eventclause_obj*>(o2);
                            if(pc)
                            {
                                // o2 is a eventuality clause
                                pcl->cl.push_back(pc->ec);
                                return pbuilding_obj(pcl);
                            }
                            else 
                                // something goes wrong...
                                throw TypeMissmatch("Add_clause_to_list: Clause");
                        }
                    }
                }
            }
            virtual pbuilding_obj
            build_initial(const pbuilding_obj o) const
            {
                literallist_obj* pll = dynamic_cast<literallist_obj*>(o);
                if (!pll)
                    throw TypeMissmatch("Initial clause: Literal List");
                // a dirty hack to check if the empty clause is in the input
                if (pll->ll.begin() == pll->ll.end()) {throw EmptyClauseInInput();}
                // do the job
                return make_obj<initclause_obj>(ClauseSet::make_iclause(pll->ll, "I"));
            }
        
            virtual pbuilding_obj
            build_universal(const pbuilding_obj o) const
            {
                literallist_obj* pll = dynamic_cast<literallist_obj*>(o);
                if (!pll)
                    throw TypeMissmatch("Universal clause: Literal List");
                // do the job
                return make_obj<univclause_obj>(ClauseSet::make_uclause(pll->ll, "U"));
            }
        
            virtual pbuilding_obj
            build_step(const pbuilding_obj o1, const pbuilding_obj o2) const
            {
                literallist_obj* pll1 = dynamic_cast<literallist_obj*>(o1);
                literallist_obj* pll2 = dynamic_cast<literallist_obj*>(o2);
                if (!pll1)
                    throw TypeMissmatch("Step clause: First Literal List");
                if (!pll2)
                    throw TypeMissmatch("Step clause: Second Literal List");
                // do the job
                return make_obj<stepclause_obj>(ClauseSet::make_sclause(pll1->ll, pll2->ll, "S"));
            }

            virtual pbuilding_obj
            build_sometime(const pbuilding_obj o1, const pbuilding_obj o2) const
            {
                literallist_obj* pll = dynamic_cast<literallist_obj*>(o1);
                literal_obj* pl = dynamic_cast<literal_obj*>(o2);
                if (!pll)
                    throw TypeMissmatch("Eventuality clause: Literal List");
                if (!pl)
                    throw TypeMissmatch("Eventuality clause: Literal");
                // do the job
                return make_obj<eventclause_obj>(ClauseSet::make_eclause(pll->ll, pl->l, "E"));
            }

            virtual pbuilding_obj 
            build_literal(const pbuilding_obj o) const
            {
                atom_obj* pa = dynamic_cast<atom_obj*>(o);
                if(!pa)
                    throw TypeMissmatch("build_literal: atom");
                else
                {
                    return make_obj<literal_obj>(Literal(pa->p));
                }
            }

            virtual pbuilding_obj
            build_atom(const char* s) const
            {
                return make_obj<atom_obj>(Proposition(s));
            }

            virtual pbuilding_obj
            negate(const pbuilding_obj o) const
            {
                literal_obj* pl = dynamic_cast<literal_obj*>(o);
                if (!pl)
                {
                    throw TypeMissmatch("negate: literal");
                }
                pl->l.negate();
                return pbuilding_obj(pl);
            }
        };
} // namespace Builders
#endif // __BUILDER_H__
