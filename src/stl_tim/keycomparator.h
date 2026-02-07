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
#ifndef __KEYCOMPARATOR_H__
#define __KEYCOMPARATOR_H__

#include "literal.h"

namespace PropositionalProver
{
    struct index_key_comparator
    {
        bool 
        operator() (const Literal& l, const Literal& m) const
        {
            // only proposition and its sign contribute but not
            // attribute!
            return ((l.getProposition() < m.getProposition()) || 
                    ( (l.getProposition() == m.getProposition()) && 
                      l.isPositive() && 
                      m.isNegative()));
        }
    };
}
#endif // __KEYCOMPARATOR_H__
