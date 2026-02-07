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
#ifndef __PROPOSITIONAL_H
#define __PROPOSITIONAL_H

#include <iostream>
#include "symbol.h"

namespace PropositionalProver
{
   class Proposition: public GenericSymbol
   {
   private:
   public: 
       Proposition(name_t name)
       : GenericSymbol(name)
       {}
       Proposition(const GenericSymbol& s)  //construct from a symbol
       : GenericSymbol(s)
       { }
       bool operator== (const Proposition& p) const
       {
           return this->getId() == p.getId();
       }
       bool operator< (const Proposition &p) const
       {
           return getId() < p.getId();
       }
       std::ostream&  dump(std::ostream& os) const
       {
           os << static_cast<GenericSymbol>(*this);
           return os;
       }
   }; //class Proposition
} //namespace PropositionalProver
#endif // __PROPOSITIONAL_H
