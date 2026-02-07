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
#include "resolutionindex.h"

namespace PropositionalProver
{
    void 
    ResolutionIndex::insert(const PClause& __x)
    {
        index[__x->getLeadingLiteral()].push_back(__x);
    }

    void
    ResolutionIndex::remove(const PClause& __x)
    {
/*std::cerr << "removing ";
__x->dump(std::cerr);
std::cerr << std::endl << "from ";
this->dump(std::cerr);
std::cerr << std::endl;*/
        index_t::iterator pos = 
            index.find((__x)->getLeadingLiteral());
        if (pos == index.end())
        {
            return;
            // deactivated
            /*throw (IndexInconsistent((__x)->getLeadingLiteral()));*/
        }
        //bool erased = false;
        //seeking for an occurrence into index...
        for (IndexCarier::iterator p = (pos->second).begin(); p != (pos->second).end(); ++p )
            if (*p == __x)
            {
                (pos->second).erase(p);
         //       erased = true;
                break;
            }
        /*if (!erased)
            throw (IndexInconsistent((__x)->getLeadingLiteral())); 
        else*/
        {
            if ((pos->second).size() == 0)
                index.erase(pos);
        }
    }

    std::pair< IndexCarier::const_iterator, 
               IndexCarier::const_iterator >
    ResolutionIndex::getStartingWith(const Literal& l) const
    {
        IndexCarier list;
        IndexCarier::const_iterator lb = list.begin();
        IndexCarier::const_iterator le = list.end();
        index_t::const_iterator pos = index.find(l);
        return (pos != index.end()) ? std::make_pair((pos->second).begin(),(pos->second).end()) : std::make_pair(lb, le);
    }

    std::pair< IndexCarier::const_iterator, 
               IndexCarier::const_iterator >
    ResolutionIndex::getComplementary(Literal l)  const
    {
        l.negate();
        return getStartingWith(l);
    }

    std::ostream&
    ResolutionIndex::dump(std::ostream& os) const
    {
        for (index_t::const_iterator p = index.begin();
                p != index.end();
                ++p)
        {
            os << "   ";
            (p->first).dump(os);
            os << " ----> ";
            for(IndexCarier::const_iterator it = p->second.begin();
                    it != p->second.end();
                    ++it)
                (*it)->dump(os);
            os << std::endl;
        }
        return os;
    }


} // namespace PropositionalProver


