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
#ifndef __TIMSTAT_H__
#define __TIMSTAT_H__

#include <iostream>

namespace PropositionalProver
{
    class Statistics
    {
    private:
        int myNumberOfResolvents;
        int myNumberOfUniversalResolvents;
        int myNumberOfForwardSubsumptions;
        int myNumberOfBackwardSubsumptions;
        int myNumberOfIndividualSubsumptions;
        int myNumberOfFSRs;
    public:

        Statistics()
        : myNumberOfResolvents(0), myNumberOfUniversalResolvents(0), myNumberOfForwardSubsumptions(0), myNumberOfBackwardSubsumptions(0), myNumberOfIndividualSubsumptions(0), myNumberOfFSRs(0)
        { }

        void
        addResolvents(int number)
        {
            myNumberOfResolvents += number;
        }

        void
        addUniversalResolvents(int number)
        {
            myNumberOfUniversalResolvents += number;
        }

        void
        addForwardSubsumptions(int number)
        {
            myNumberOfForwardSubsumptions += number;
        }

        void
        addBackwardSubsumptions(int number)
        {
            myNumberOfBackwardSubsumptions += number;
        }

        void
        addIndividualSubsumptions(int number)
        {
            myNumberOfIndividualSubsumptions += number;
        }

        void
        addFSR(int number)
        {
            myNumberOfFSRs += number;
        }

        int
        getNumberOfResolvents() const
        { 
            return myNumberOfResolvents;
        }

        int
        getNumberOfUniversalResolvents() const
        { 
            return myNumberOfUniversalResolvents;
        }

        int
        getNumberOfForwardSubsumptions() const
        { 
            return myNumberOfForwardSubsumptions;
        }

        int
        getNumberOfBackwardSubsumptions() const
        { 
            return myNumberOfBackwardSubsumptions;
        }

        int
        getNumberOfIndividualSubsumptions() const
        { 
            return myNumberOfIndividualSubsumptions;
        }

        int
        getTotalNumberOfSubsumptions() const
        { 
            return getNumberOfForwardSubsumptions() + 
                getNumberOfBackwardSubsumptions() +
                getNumberOfIndividualSubsumptions();
        }

        int
        getNumberOfFSRs() const
        {
            return myNumberOfFSRs;
        }

        Statistics&
        operator+= (const Statistics& stat)
        {
            myNumberOfResolvents += stat.getNumberOfResolvents();
            myNumberOfUniversalResolvents += stat.getNumberOfUniversalResolvents();
            myNumberOfForwardSubsumptions += stat.getNumberOfForwardSubsumptions();
            myNumberOfBackwardSubsumptions += stat.getNumberOfBackwardSubsumptions();
            myNumberOfIndividualSubsumptions += stat.getNumberOfIndividualSubsumptions();
            myNumberOfFSRs += stat.getNumberOfFSRs();
            return *this;
        }

        Statistics
        operator+ (const Statistics& stat)
        {
            Statistics r = stat;
            return r+=*this;
        }

    };
} // namespace PropositionalProver
std::ostream&
operator<< (std::ostream& os, const PropositionalProver::Statistics& stat);
#endif // __TIMSTAT_H__
