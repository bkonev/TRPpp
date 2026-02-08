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
#include <iostream>
#include <iomanip>
#include "adaptor/stl_tim_adaptor.h"
#include "trim/trim.h"
#include "misc/timer.h"  
#include "builder/parse.h"
#include "builder/generic_builder.h"
#include "builder/builder.h"
#include "loop_search/loopsearch.h"
#include "prover/search.h"
#include "loop_search/loopsearch.h"
#include "temporal_resolution/classicalrule.h"
//#include "temporal_resolution/DFKrule.h"
#include "prover/search.h"
#include "prover/temporalstat.h"
#include "interrupthandler.h"
#include "getopt/options.h"
#include "modules.h"
#include "misc/tracer.h"
#include "misc/exception.h"

/*
#ifdef DEBUG
#define BOOST_INCLUDE_MAIN 
#include "boost/test/test_tools.hpp"
#endif // DEBUG
*/


using namespace TRIMs;
using namespace Parsers;

using ClauseSet = TRIM<Adaptors::Adaptor>;
using Proposition = ClauseSet::Proposition;
using Literal = ClauseSet::Literal;
using LiteralList = ClauseSet::LiteralList;
using IClause = ClauseSet::IClause;
using UClause = ClauseSet::UClause;
using SClause = ClauseSet::SClause;
using EClause = ClauseSet::EClause;

using Builder = Builders::Builder<ClauseSet>;

using std::cout;

// I use this function instead of main since I emply boost's mechanism
// to catch "uncought" exceptions. If something bad happens, and I do
// not catch this, hopefully, boost will do. This usually gives more
// information than just "segmentation fault" of "abort"

//#ifdef DEBUG
//int test_main(int argc, char **argv)
//#else
int main(int argc, char **argv)
//#endif
{
    Timer timer;

    extern int trp_yydebug;
    trp_yydebug = 0;

    // Initiating data structures
    // (The reason for this operator is that TRIM<> provides 
    //  with some static functions that might be used befoure
    //  any clause set is constructed (e.g. to construct such a set),
    //  and these function relay on some (static) data structures)
    //
    ClauseSet::InitiateStatic();

    try 
    {
        // parse command line options
        TRPPPOptions cmdOptions(argc, argv);

        // parse the file
        TRPParser<Builder> parser(cmdOptions.getFileName(),
                                     cmdOptions.getOrderFileName());
        if(parser.parseOk())
        {
            //get the set of clauses
        ClauseSet clauseSet = parser.getClauseSet();
        // construct the prover object (for convinience, first, I 
        // give a name to the type
        using Prover = TemporalProver::Prover<ClauseSet,
          TemporalProver::LoopList<ClauseSet>,
          TemporalProver::ClassicalRule<ClauseSet>>;
    
        // then, make an instance)
        Prover prover(clauseSet);
        // dumping ClauseSet (if required)
        if(cmdOptions.isShowInput())
        {
            std::cout << "At the begining.." << std::endl;
            prover.getClauseSet().dump(std::cout);
            std::cout << std::endl;
        }
        // end dumping
        
        // this captures ^C if a user presses it. 
        // allows to see prover statistics on the fly
        HandleSIGINT<Timer, Prover> handleSIGINT(&timer, &prover);
        
        // call the prover
        timer.start();
        bool result = prover.search(cmdOptions);
        timer.stop();
    
        std::cout << (result?"Unsatisfiable":"Satisfiable") << std::endl;     
        if(!cmdOptions.isQuiet())
        {
            std::cout << "Elapsed time " << std::fixed << std::setprecision(3) << timer.elapsedSeconds() << "s" << std::endl;
            // statistics:
#ifdef COLLECT_STAT
            prover.outputStatistics(std::cout);
#endif // COLLECT_STAT
        }
        // dumping ClauseSet (if needed)
        if(cmdOptions.isShowResult())
        {
            std::cout << "At the end.." << std::endl;
            prover.getClauseSet().dump(std::cout);
            std::cout << std::endl;
        }
        // end dumping
        return 0;
        }
        else
        {
            return 1;
        }
    }
    catch(const char * ex)
    {
        std::cerr << ex << std::endl;
        exit(1);
    }
    catch (const std::string& s) 
    {
        std::cerr << s << std::endl;
        exit(1);
    }
    catch (Exceptions::EmptyClauseInInput &e)
    {
        std::cout << ("Unsatisfiable") << std::endl;     
        std::cout << ("Empty clause in the input") << std::endl;     
    }
    catch (Exceptions::Exception &e)
    {
        std::cout << e.module() << ": " << e.what() << '\n';
    }
    // basically, we never reach this point, 
    // put here in order to switch off warnings
    return 0;
}
