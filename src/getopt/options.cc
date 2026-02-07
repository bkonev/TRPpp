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
#include "options.h"
#include "boost/tokenizer.hpp"
#include "config.h"
#include "modules.h"
#include "misc/tracer.h"

TRPPPOptions::TRPPPOptions(int argc, char** argv)
{
    gengetopt_args_info args_info; 

    if(cmdline_parser(argc, argv, &args_info) != 0)
    {
        reportFailure("Failed to parse the command line\n");
    }

    if(args_info.inputs_num != 1)
    {
        reportFailure("");
    }

    myFileName = args_info.inputs[0];

    myShowInput  = args_info.show_input_flag;
    myShowResult = args_info.show_result_flag;
    myQuiet      = args_info.quiet_flag;

    if(myQuiet)
        if (myShowInput || myShowResult)
        {
            reportFailure("Incompatible options\n");
        }
        else
        {
            myShowInput = false;
            myShowResult = false;
        }

    if(args_info.select_by_given)
    {
        if(std::string("DFS") == args_info.select_by_arg)
        {
            myDFS = true;
        }
        else 
            if(std::string("BFS") == args_info.select_by_arg)
            {
                myDFS = false;
            }
            else
            {
                reportFailure(std::string("Illegal value for the \"select-by\" option: ")+ args_info.select_by_arg);
            }
    }
    else
    {
        myDFS = true; // default value
    }

    myUsePretest = args_info.pretest_flag;

    myUseFSR = args_info.FSR_flag;

    if(args_info.order_given)
    {
        myOrderFile = args_info.order_arg;
    }
    else 
    {
        myOrderFile = "";
    }
#ifdef USE_TRACING
    if(args_info.trace_given)
    {
        std::string argString(args_info.trace_arg);
        boost::tokenizer<> tok(argString);

        for(boost::tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg)
        {
            if(*beg == "index")
            {
                setTrace(indexModule);
            }
            else
                if(*beg == "tempres")
                {
                    setTrace(tempresModule);
                }
                else
                    if(*beg == "loopsearch")
                    {
                        setTrace(loopsearchModule);
                    }
                    else
                        if(*beg == "resolution")
                        {
                            setTrace(resolutionModule);
                        }
                        else
                            if(*beg == "subsumption")
                            {
                                setTrace(subsumptionModule);
                            }
                            else
                                if(*beg == "forward")
                                {
                                    setTrace(forwardsubsumptionModule);
                                }
                                else
                                    if(*beg == "backward")
                                    {
                                        setTrace(backwardsubsumptionModule);
                                    }
                                    else 
                                        if(*beg == "pretest")
                                        {
                                            setTrace(pretestModule);
                                        }
                                        else
                                            if(*beg == "FSR")
                                            {
                                                setTrace(FSRModule);
                                            } 
                                            else 
                                            {
                                                reportFailure(std::string("Tracing of ") + *beg + " unsupported");
                                            }
        }
    }
#endif
}

void
TRPPPOptions::reportFailure(std::string s )
{
    cmdline_parser_print_help();
    throw Exceptions::WrongOption(std::string("\n") + s);
}
