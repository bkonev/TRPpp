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
#ifndef __INFER_H__
#define __INFER_H__

#include <sstream>
#include <list>
#include <utility>
#include <algorithm>
#include "config.h"
#include "modules.h"
#include "misc/scalar.h"
#include "proposition.h"
#include "literal.h"
#include "literallist.h"
#include "attribute.h"
#include "clause.h"
#include "pclause.h"
#include "timstat.h"
#include "misc/tracer.h"
#include "misc/assert.h"

namespace PropositionalProver
{
    // declarations:
    bool
    clauseSubsumes(const PClause& c, const PClause& d);

    bool
    clauseSubsumesOrEqual(const PClause& c, const PClause& d);

    bool
    implementationDependentSubsumesHelper(const PClause& c, const PClause& d);

    template<typename Iterator>
    inline bool
    rangeSubsumes (Iterator __begin, Iterator __end, const PClause& d) 
    {
        while (__begin != __end)
            if (clauseSubsumes(*__begin++, d)) // passing pointers
                return true;
        return false;
    }

    template<typename Iterator>
    inline bool
    rangeSubsumesOrEqual (Iterator __begin, Iterator __end, const PClause& d)
    {
        while (__begin != __end)
            if (clauseSubsumesOrEqual(*__begin++, d)) // passing pointers 
                return true;
        return false;
    }

    Clause*
    resolveMaximals(const PClause& c, const PClause& d);

} //namespace PropositionalProver
#endif // __INFER_H__
