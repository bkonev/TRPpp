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
#ifndef __DUMMY_ATTRIBUTE_H__
#define __DUMMY_ATTRIBUTE_H__
#include "iostream.h"
namespace PropositionalProver
{
    class Attribute
    {
    public:
        bool operator< (const Attribute& a) const
        {
            return false;
        }
        bool operator== (const Attribute& a) const
        {
            return true;
        }
    };
    const Attribute default_attr = Attribute();
    extern  std::ostream& operator<< (std::ostream& os, const Attribute& attribute);

    inline bool 
    compatible(const Attribute& a1, const Attribute& a2) 
    {
        return true;
    }
} // namespace PropositionalProver
#endif // __DUMMY_ATTRIBUTE_H__
