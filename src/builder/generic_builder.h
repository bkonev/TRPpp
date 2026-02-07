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
#ifndef __GENERIC_BUILDER_H
#define __GENERIC_BUILDER_H
// the mission of this file is to make an envelop of every 
// created entity. The primer goal is to separate creation
// of entities from the place where these entities are
// created. In this way, it is possible to re-use the
// same creation procedures for different input syntaxes.

namespace Builders
{
    struct building_obj
    {
        virtual void dummy_function() = 0;
        virtual ~building_obj()
        { /* in order to avoid warnings */ }
    };
    typedef building_obj* pbuilding_obj;

    class GenericBuilder
    {
    public:
        virtual pbuilding_obj create_clause_list() const = 0;
        virtual pbuilding_obj add_clause_to_list(const pbuilding_obj, const pbuilding_obj) const = 0;
        virtual pbuilding_obj build_initial(const pbuilding_obj) const = 0;
        virtual pbuilding_obj build_universal(const pbuilding_obj) const = 0;
        virtual pbuilding_obj build_step(const pbuilding_obj, const pbuilding_obj) const = 0;
        virtual pbuilding_obj build_sometime(const pbuilding_obj, const pbuilding_obj) const = 0;
        virtual pbuilding_obj create_literal_list() const = 0;
        virtual pbuilding_obj add_literal_to_list(const pbuilding_obj, const pbuilding_obj) const = 0;
        virtual pbuilding_obj build_literal(const pbuilding_obj) const = 0;
        virtual pbuilding_obj negate(const pbuilding_obj) const = 0;
        virtual pbuilding_obj build_atom(const char*) const = 0;
    };
} //namespace builders
#endif // __GENERIC_BUILDER_H
