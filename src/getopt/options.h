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
#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <iostream>
#include <string>

#include "genoptions.h"
#include "misc/exception.h"

namespace Exceptions
{
    class OptionException : public Exception
    {
    public:
        virtual void 
        module(std::ostream& os) const
        {
            os << "Program options: ";
        }
    }; // class ClauseSetException
    class WrongOption : public OptionException
    {
    private:
        const std::string myOption;
    public:
        WrongOption(const std::string& option)
        : myOption (option)
        { }
        virtual void 
        what (std::ostream& os) const
        {
            os << myOption << std::endl;
        }
    }; //class IndexInconsistent
}

class
TRPPPOptions
{
public:
    TRPPPOptions(int argc, char** argv);

    const std::string&
    getFileName() const  { return myFileName; }

    const std::string&
    getOrderFileName() const  { return myOrderFile; }

    bool
    isShowInput() const  { return myShowInput; }

    bool
    isShowResult() const { return myShowResult; }

    bool 
    isQuiet() const      { return myQuiet; }

    bool
    isUsePretest() const { return myUsePretest; }

    bool
    isDFS() const        { return myDFS; }

    bool
    isUseFSR() const { return myUseFSR; }

private:
    void
    reportFailure(std::string s = "");

    std::string
    myFileName;

    std::string 
    myOrderFile;

    bool 
    myShowInput;

    bool
    myShowResult;

    bool 
    myQuiet;

    bool 
    myDFS;

    bool
    myUsePretest;

    bool
    myUseFSR;
};
#endif //  __OPTIONS_H__
