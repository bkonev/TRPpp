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
#ifndef __TEMPORALSTAT_H__
#define __TEMPORALSTAT_H__

#include <iostream>

namespace TemporalProver
{
    class TemporalStatistics
    {
    private:
        int myNumberOfLoopSearches;
        int myNumberOfSuccessfulLoopSearches;
        int myNumberOfNonredundandLoopSearches;
    public:

        TemporalStatistics()
        : myNumberOfLoopSearches(0), myNumberOfSuccessfulLoopSearches(0),
          myNumberOfNonredundandLoopSearches(0)
        { }

        void
        addToLoopSearches(int number)
        {
            myNumberOfLoopSearches += number;
        }

        void
        addToSuccessfulLoopSearches(int number)
        {
            myNumberOfSuccessfulLoopSearches += number;
        }

        void
        addToNonredundandLoopSearches(int number)
        {
            myNumberOfNonredundandLoopSearches += number;
        }

        int
        getNumberOfLoopSearches() const
        { 
            return myNumberOfLoopSearches;
        }

        int
        getNumberOfSuccessfulLoopSearches() const
        { 
            return myNumberOfSuccessfulLoopSearches;
        }

        int
        getNumberOfNonredundandLoopSearches() const
        { 
            return myNumberOfNonredundandLoopSearches;
        }

        TemporalStatistics&
        operator+= (const TemporalStatistics& stat)
        {
            myNumberOfLoopSearches += stat.getNumberOfLoopSearches();
            myNumberOfSuccessfulLoopSearches += stat.getNumberOfSuccessfulLoopSearches();
            myNumberOfNonredundandLoopSearches += stat.getNumberOfNonredundandLoopSearches();
            return *this;
        }

        TemporalStatistics
        operator+ (const TemporalStatistics& stat)
        {
            TemporalStatistics r = stat;
            return r+=*this;
        }

    };
} // namespace TemporalProver
std::ostream&
operator<< (std::ostream& os, const TemporalProver::TemporalStatistics& stat);
#endif // __TEMPORALSTAT_H__
