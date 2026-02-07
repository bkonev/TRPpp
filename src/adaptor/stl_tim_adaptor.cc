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
#include <algorithm>
#include "stl_tim_adaptor.h"
#include "stl_tim/attribute.h"
namespace Exceptions
{
    class AdaptorException : public Exception
    {
    public:
        virtual void 
        module(std::ostream& os) const
        {
            os << "Adaptor";
        }
    }; // class ClauseSetException
    class IllegalMode : public AdaptorException
    {
    public:
        virtual void 
        what (std::ostream& os) const
        {
            os << "Illegal operation mode aquired" << std::endl;
        }
    }; //class IndexInconsistent
} // namespace Exceptions

namespace Adaptors
{
    using namespace Exceptions;
    void
    Adaptor::setMode(Adaptor::OperationMode mode = Adaptor::normal)
    {
        ourMode = mode;
        //ourMode = Adaptor::normal;
        switch(ourMode)
        {
            case normal:
                // PropositionalProver::Attribute::setComparator(&PropositionalProver::normalComparator);
                PropositionalProver::Attribute::setCompatibilitiser(&PropositionalProver::normalCompatibilitiser);
                PropositionalProver::Attribute::setEqualizer(&PropositionalProver::normalEqualizer);
                break;
            case loopSearch:
                // PropositionalProver::Attribute::setComparator(&PropositionalProver::loopSearchComparator);
                PropositionalProver::Attribute::setCompatibilitiser(&PropositionalProver::loopSearchCompatibilitiser);
                PropositionalProver::Attribute::setEqualizer(&PropositionalProver::loopSearchEqualizer);
                break;
            default:
                throw IllegalMode() ;
                break;
        }
    }

    Adaptor::OperationMode Adaptor::ourMode;

    std::ostream& 
    Adaptor::dump(std::ostream& os) const
    {
        return this->internal_clause_collection_rep::dump(os);
    }
} // namespace Adaptors
