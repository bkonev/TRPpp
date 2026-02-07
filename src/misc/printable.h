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

#ifndef __MISC_PRINTABLE_H__ 
#define __MISC_PRINTABLE_H__ 

#include <iostream>


// Printable is something that can be put to ostream. That means that if class
// T has Printable<Something> as its base class, and T properly defines 
// virtual member "print(ostream&)", you can write "os << t", where 'os' is
// 'ostream' and 't' is T'.
// 
// IMPORTANT: Since "printability" will most probably be used for complex
// oblects, "Printable<T> stores only pointer to T. So this may break if 
// T will be destroyed (e.g. if it is automatic variable) after printable 
// is created but before it is actually "printed". So printables should only 
// be used "locally", or with long-living objects. Actually, this mechanism
// was introduced for exceptions (see misc/exception.h), where Exception is 
// not destroyed until it is printed out.
// An alternative would be for Printable to copy its contents when printable 
// is created. I'm not sure it is worth caring.
//
// Anyway, use it with care.
// 
template<class T>
class Printable {
    protected:
        const T    *p;   // What will be printed 
    public:
        Printable(const T *t) : p(t) {};
        virtual ~Printable() {};
        virtual void print(std::ostream &os) const = 0;  // User-defined printing

        inline friend std::ostream& operator << (std::ostream &os, const Printable<T> &p) 
        {
            p.print(os);
            return os;
        };
};
     
// Specialization of "printable" for pointers (to avoid pointer-to-pointer)
template<class T>
class Printable<T*> {
    protected:
        const T    *p;   // What will be printed 
    public:
        Printable(const T *t) : p(t) {};
        virtual ~Printable() {};
        virtual void print(std::ostream &os) const = 0;  // User-defined printing

        inline friend std::ostream& operator << (std::ostream &os, const Printable<T> &p) {
            p.print(os);
            return os;
        };
};
     

#endif /* __MISC_PRINTABLE_H__ */ 
