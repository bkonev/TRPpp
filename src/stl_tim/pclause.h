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
#ifndef __PCLAUSE_H__
#define __PCLAUSE_H__

#include "clause.h"
#include <memory>

// looks stupid, but i may use it in different places..
namespace PropositionalProver
{
    typedef std::shared_ptr<Clause> PClause;

    struct FastCmp
    {
        bool
        operator() (const PClause& pclause1, const PClause& pclause2)
        {
            // whatever order
            return (reinterpret_cast<unsigned long int>(pclause1.get()) < 
                    reinterpret_cast<unsigned long int>(pclause2.get()));
        }
    };
}

#endif // __PCLAUSE_H__
