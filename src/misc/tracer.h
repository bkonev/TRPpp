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
#ifndef __TRACER_H__
#define __TRACER_H__
#include <iostream>
#include <string>
#include <set>
#include "config.h"

#ifdef USE_TRACING
extern std::set<std::string> * traceSet();

inline bool
checkTrace(const std::string& str)
{
    if (traceSet()->count(str))
        return true;
    else 
        return false;
}
inline void
setTrace(const std::string& str)
{
    if (!checkTrace(str))
        traceSet()->insert(str);
}
inline void
clearTrace(const std::string& str)
{
    if (checkTrace(str))
        traceSet()->erase(str);
}

#define TRACE(n,s) if(checkTrace(n)) { \
        s; \
     }
#else
inline void
setTrace(const std::string&)
{ }
inline void
clearTrace(const std::string&)
{ }
#define TRACE(n,s)
#endif
// Sample usage:
// 
//int
//main()
//{
//    const std::string module = "This module";
//    setTrace(module);
//    TRACE(module,{
//            std::cout << "Here ";
//            std::cout << " I am\n";
//    });
//    clearTrace(module);
//    TRACE(module,{
//            std::cout << "And here \n";
//    });
//    return 0;
#endif // __TRACER_H__
//}
