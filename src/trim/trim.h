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
#ifndef __TRIM_H__
#define __TRIM_H__

#include <list>
#include <string>
#include <iostream>
#include "adaptor/adaptor_concept.h"
#include "adaptor/stl_tim_adaptor.h"

namespace TRIMs
{
    using namespace Adaptors;
    template <typename _Adaptor>
        // this class extends functionality of _Adaptor with 
        // Representation-independent data structures and methods
        // It also checks that the template parameter supplies it
        // with all needed functionality
        class TRIM : public _Adaptor
        {
            //BOOST_CLASS_REQUIRES(_Adaptor, Adaptor_concept);
            // directly goes from _Adaptor
        public:
            TRIM()
            : _Adaptor(), elist()
            { 
                InitiateStatic();
            }

            TRIM(const TRIM<_Adaptor>& adaptor)
            : _Adaptor(adaptor), elist(adaptor.elist)
            { }

            static void
            InitiateStatic()
            {
                _Adaptor::setMode(_Adaptor::normal);
            }

        public:
            typedef typename _Adaptor::Proposition Proposition;
            typedef typename _Adaptor::Literal Literal;
            typedef typename _Adaptor::LiteralList LiteralList;
            typedef typename _Adaptor::IClause IClause;
            typedef typename _Adaptor::UClause UClause;
            typedef typename _Adaptor::SClause SClause;
            typedef typename _Adaptor::internal_clause_rep internal_clause_rep;
            typedef typename _Adaptor::const_i_iterator const_i_iterator;
            typedef typename _Adaptor::const_u_iterator const_u_iterator;
            typedef typename _Adaptor::const_s_iterator const_s_iterator;
            typedef typename _Adaptor::const_next_false_as_universal_iterator const_next_false_as_universal_iterator;
            typedef typename _Adaptor::const_next_false_as_step_iterator const_next_false_as_step_iterator;
            typedef typename _Adaptor::OperationMode OperationMode;
            // added value
            // EClause: E:(literal, literal,..., sometime(literal))
            // (Note that for now (forever?) there is only one literal under
            // sometime)
            class EClause
            {
                // we represent an eventuality clause as a pairs of
                // a literal and a universal clause (why not? ;-) )
            private:
                Literal l;
                UClause c;
            public:
                typedef typename UClause::const_iterator const_present_iterator;
                typedef const Literal* const_sometime_iterator;
                EClause()
                : l(), c()
                { }
                EClause(const EClause& _ec)
                : l(_ec.l), c(_ec.c)
                {}
                EClause(const UClause& _uc, const Literal &_l)
                : l(_l), c(_uc)
                { }
            public:
                // members access
                const_present_iterator 
                present_begin() const
                {
                    return c.begin();
                }
                const_present_iterator
                present_end() const
                {
                    return c.end();
                }
                const Literal&
                getEventuality() const
                {
                    return l;
                }
                const std::string&
                getInfo() const
                {
                    return c.getInfo();
                }

                // simple output
                std::ostream& 
                dump(std::ostream& os) const
                {
                    os << "E:(";
                    for (const_present_iterator p = this->present_begin();
                            p != this->present_end();
                            ++p)
                    {
                        p->dump(os);
                        os << " ";
                    }
                    os << ", Sometime(";
                    l.dump(os);
                    os << ") )" << std::endl;
                    return os;
                }
            };
        private:
            typedef std::vector<EClause> internal_eclause_collection_rep;
            internal_eclause_collection_rep elist;
        public:
            typedef typename internal_eclause_collection_rep::const_iterator const_e_iterator;
            typedef typename internal_eclause_collection_rep::iterator e_iterator;
            e_iterator
            eventuality_begin() 
            {
                return elist.begin();
            }
            e_iterator
            eventuality_end() 
            {
                return elist.end();
            }
            const_e_iterator
            eventuality_begin() const
            {
                return elist.begin();
            }
            const_e_iterator
            eventuality_end() const
            {
                return elist.end();
            }
            static EClause make_eclause(const LiteralList& _ll, 
                    const Literal& _l, const std::string _s="")
            {
                return EClause(_Adaptor::make_uclause(_ll,_s),_l);     
            }
            e_iterator
            erase(e_iterator pos)
            {
                return elist.erase(pos);
            }
            void push_back(const EClause& _ec)
            {
                elist.push_back(_ec);   
            }
            void push_back(const IClause& _ic)
            {
                _Adaptor::push_back(_ic);
            }
            void push_back(const UClause& _uc)
            {
                _Adaptor::push_back(_uc);
            }
            void push_back(const SClause& _sc)
            {
                _Adaptor::push_back(_sc);
            }
            void push_back(const internal_clause_rep& _cr)
            {
                _Adaptor::push_back(_cr);
            }
            std::ostream& dump(std::ostream& os) const
            {
                // other part
                os << "Initial, Universal and Step parts" << std::endl;
                _Adaptor::dump(os);
                // eventuality part
                os << "Eventuality part" << std::endl;
                for(const_e_iterator p = eventuality_begin();
                        p != eventuality_end();
                        ++p)
                {
                    p->dump(os);
                }
                return os;
            }
        }; // class TRIM
} // namespace TRIMs
#endif // __TRIM_H__
