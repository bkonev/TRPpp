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
#ifndef __PARSE_H__
#define __PARSE_H__

#include <stdio.h>
#include <string>
#include <cstring>
#include <cerrno>
#include "generic_builder.h"

extern int trp_yyparse(void *);
extern FILE *trp_yyin;
extern FILE *trp_yyinsave;
extern bool orderFileGiven;
extern int trp_errlineno;

extern Builders::pbuilding_obj trp_parse_result;

namespace Exceptions
{
    class ParserException : public Exception
    {
    public:
        virtual void
        module(std::ostream& os) const
        {
            os << "Parser";
        };
    }; // class BuilderException
    class FileError : public ParserException
    {
    private:
        std::string name;
    public:
        FileError(std::string n)
        : name(n)
        { }
        virtual void
        what (std::ostream& os) const
        {
            os << name << std::endl ; 
        }
    }; // class ParserFailure

    class ParserFailure : public ParserException
    {
    private:
        std::string name;
    public:
        ParserFailure(std::string n)
        : name(n)
        { }
        virtual void
        what (std::ostream& os) const
        {
            os << "Parser failed with " << name << std::endl ; 
        }
    }; // class ParserFailure
} // namespace Exceptions

namespace Parsers
{
    using namespace Exceptions;

    template<typename _ConcreteBuilder>
    class TRPParser
    {
    public:
        TRPParser(const std::string&, const std::string& orderFileName = "");

        bool
        parseOk() const
        {
            return myParseOk;
        }

        typename _ConcreteBuilder::ClauseSet
        getClauseSet() const;
        
    private:
        bool myParseOk;
        Builders::pbuilding_obj myBuildingObj;
    };

    template<typename _ConcreteBuilder> 
        TRPParser<_ConcreteBuilder>::TRPParser(const std::string& fileName, const std::string& orderFileName)
        {
            _ConcreteBuilder builder;
            trp_yyinsave = fopen(fileName.c_str(),"r");
            if (!trp_yyinsave) 
            {
                std::string error = "Cannot open \"" + fileName + "\"\n" + 
                    std::strerror(errno);
                throw FileError(error);
            }

            if(orderFileName != "")
            {
                orderFileGiven = true;
                trp_yyin = fopen(orderFileName.c_str(),"r");
                if (!trp_yyin)
                {
                    std::string error = "Cannot open order file \"" + orderFileName + "\"\n" + 
                        std::strerror(errno);
                    throw FileError(error);
                }
            }
            else
            {
                orderFileGiven = false;
                trp_yyin = trp_yyinsave;
            }

            myParseOk = !(trp_yyparse(&builder));
            /*
            if (trp_errlineno != -1)
            {
                std::cerr << "Error detected" << std::endl;
            }
            */
            fclose(trp_yyin);
            myBuildingObj = trp_parse_result;
        }

    template<typename _ConcreteBuilder>
        typename _ConcreteBuilder::ClauseSet
        TRPParser<_ConcreteBuilder>::getClauseSet() const
        {
            if(parseOk())
            {
                return _ConcreteBuilder::getClauseSet(myBuildingObj);
            }
            else
            {
                throw ParserFailure("Attempt to get the clause set when parser failed\n");
            }
        }
} // namespace Parsers

#endif // __PARSE_H__
