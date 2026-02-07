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
#ifndef __MISC_ASSERT_H__
#define __MISC_ASSERT_H__

#include <string>
#include <iostream>
#include "config.h"
#include "misc/exception.h"

namespace Exceptions {

    class AssertionFailed : public SystemException {
            std::string comment;
            std::string file;
            int    line;
        public:
            AssertionFailed(std::string c, std::string f, int l) :
                comment(c), file(f), line(l) { }
            virtual void what(std::ostream &os) const
            {
                os << "assertion \"" << comment << "\" failed in " <<
                      file << ":" << line;
            }
    };
    
} // namespace Exceptions

/*
inline void func_Assert(std::string file, int line, 
                        bool cond, std::string comment = "...")
{
    if (!(cond)) throw Exceptions::AssertionFailed(comment, file, line);
}
*/

// Assert --
//      assert condition. 
#ifdef DEBUG
//#define Assert(cond,comment) func_Assert(__FILE__, __LINE__, cond, comment)
#define Assert(cond,comment) \
   (static_cast<void> (cond ? 0 : throw Exceptions::AssertionFailed(comment, __FILE__, __LINE__)))
#else
#define Assert(cond,comment) 
#endif

#endif // __MISC_ASSERT_H__

