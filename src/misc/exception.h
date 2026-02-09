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

#ifndef __MISC_EXCEPTION_H__ 
#define __MISC_EXCEPTION_H__ 

#include <string>
#include "misc/printable.h"

// Exception is surely what you can throw to identify problems. It is also 
// possible to print it out after catching: printable things are 
// "exception.what()" identifying what really happened, and "expection.module()"
// showing the module (logical part of code) where it's happened.
// So, this would be the correct code:
//
//    try 
//    {
//        ...
//    }
//    catch (Exception &e)
//    {
//        cout << "Exception in " << e.module() << ": " << e.what() << '\n';
//    }
// 
// NB: you cannot say "Exception e" above, since Exception is abstract type.

namespace Exceptions {

    
    class Exception {
        
        class PrintableModule : public Printable<Exception> {
            public:
                PrintableModule(const Exception *e) : Printable<Exception>(e) {}    
                virtual void print(std::ostream &os) const { p->module(os); }
        };
                
        class PrintableWhat : public Printable<Exception> {
            public:
                PrintableWhat(const Exception *e) : Printable<Exception>(e) {}    
                virtual void print(std::ostream &os) const { p->what(os); }
        };
        
        public:
            PrintableModule module() const {return PrintableModule(this);}
            PrintableWhat   what() const {return PrintableWhat(this);}
            Exception() {}
            virtual ~Exception() {}
            virtual void module(std::ostream &os) const = 0;
            virtual void what(std::ostream &os) const = 0;
    };
    
    // system-wide exceptions go here...
    //
    class SystemException : public Exception {
	    public:
		    virtual void module (std::ostream& os) const {os << "system"; }
    };
    
} // namespace Exceptions

#endif /* __MISC_EXCEPTION_H__ */ 
