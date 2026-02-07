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
#include "timstat.h"
std::ostream&
operator<< (std::ostream& os, const PropositionalProver::Statistics& stat)
{
    os << "Number of generated resolvents: " << stat.getNumberOfResolvents() << std::endl;
    os << "Number of generated universal resolvents: " << stat.getNumberOfUniversalResolvents() << std::endl;
    os << "Number of forward subsumed clauses: " << stat.getNumberOfForwardSubsumptions() << std::endl;
    os << "Number of backward subsumed clauses: " << stat.getNumberOfBackwardSubsumptions() << std::endl;
    os << "Number of individual subsumed clauses: " << stat.getNumberOfIndividualSubsumptions() << std::endl;
    os << "Total number of subsumed clauses: " << stat.getTotalNumberOfSubsumptions() << std::endl;
    os << "Simplified by FSR: " << stat.getNumberOfFSRs() << std::endl;
    return os;
}
