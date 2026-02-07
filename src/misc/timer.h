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
#ifndef __TIMER_H_
#define __TIMER_H_
#define TIMER_H 


// NOTE that this code might be not relly portable (I do not know
// whether the call of ftime exists on every U*ix-like system.
// In addition, it was broken on some versions of libc...
// However, it seems to be a simple and reliable way to get  time 
// with millisecons. 
// Probably, one shoud use a combination of time + clock()

#include <sys/timeb.h>
class Timer
{
public:
    Timer() 
    {
        reset(); 
    };

    void
    reset()
    {
        stopped = true;
        elapsedSec = 0;
    };

    void
    start()
    {
        if (stopped)   
        {
            stopped = false;
            ftime(&startTime);
        };
    };

        void
    stop()
    {
        if (!stopped)
        {
            stopped = true;
            struct timeb currTime;
            ftime(&currTime);
            elapsedSec += getSecondsDifference(startTime, currTime);
        };
    };

    double
    elapsedSeconds()
    {
        if (stopped) return elapsedSec; 
        struct timeb currTime;
        ftime(&currTime);
        return elapsedSec + getSecondsDifference(startTime, currTime);
    };

private:   
    double
    getSecondsDifference(struct timeb& timeFrom, struct timeb& timeTo)
    {
        unsigned int dsec = timeTo.time - timeFrom.time;
        int          dms  = timeTo.millitm - timeFrom.millitm; // can be negative!
        return (static_cast<double>(dsec)*1000. + dms)/1000.; 
    }
private:
    bool          stopped;
    double        elapsedSec; // in seconds 
    struct timeb  startTime; 
}; // class Timer
#endif // __TIMER_H_
