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
#ifndef __SYMBOL_H__
#define __SYMBOL_H__ 1

#include "misc/scalar.h"
#include "misc/assert.h"
#include <map>
#include <iostream>
#include <string>
#include "config.h"


namespace PropositionalProver
{
    class GenericSymbol 
    {
    private:
        //typedef std::string name_t;
        typedef std::map< name_t, symid_t > name2id_t;
        typedef std::map<symid_t, name_t> id2name_t;
        static name2id_t name2id; //Map name to id
        static id2name_t id2name;  // Map id to name 
        static const symid_t id_none = 0;    // Non-existing symbol  
        static const symid_t init_unused = 1;// First not used id on startup  
        static symid_t id_unused;            // First currently not used id 
        static const symid_t id_last = static_cast<symid_t>(-1); // Max possible id 

        // Allocate new symbol id 
        static symid_t get_unused_id() 
        {
            Assert(id_unused != id_last, "maximum symbol id reached");
            return id_unused++;
        }
    private:
        symid_t       myId;    // Symbol id  
    public:
        //symbol from name
        typedef id2name_t::const_iterator const_iterator;

        GenericSymbol (const name_t& name)
        {
            if (name2id.count(name)) 
                myId = name2id[name];
            else
            {
                myId = get_unused_id();
                name2id[name] = myId;
                id2name[myId] = name;
            }
        }
        // symbol with the specified id
        GenericSymbol (const symid_t id = id_none) 
        : myId (id)
        { }
        const name_t& getName() const
        {
            return id2name[myId];
        }

        symid_t getId() const 
        {
            return myId;
        }
        static const_iterator 
        begin() 
        {
            return id2name.begin();
        }
        static const_iterator 
        end()
        {
            return id2name.end();
        }

        static std::ostream& 
        dump(std::ostream& os)
        {
            typedef const_iterator CI;
            using std::endl;
            os << "Dumping all known symbols symbols:" << endl;
            for (CI p=begin(); p!=end(); ++p)
                os << "     " << p->first << '\t' << p->second << endl;
            os << "Finished" << endl;
            return os;
        }
    }; //class GenericSymbol
    extern std::ostream& operator<< (std::ostream& os, GenericSymbol s);
} //namespace PropositionalProver

#endif //__SYMBOL_H__
