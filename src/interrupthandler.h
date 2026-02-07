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
#include <csignal>
#include <cstdlib>
#include <unistd.h>

template <typename Timer, typename Prover>
class HandleSIGINT
{
public:
    HandleSIGINT (Timer* timer, Prover* prover);
    static void 
    handler (int);
    ~HandleSIGINT();
private:
    static Timer* ourPTimer;
    static Prover* ourPProver;
    static void initSIGINTHandler();
};

template<typename Timer, typename Prover>
HandleSIGINT<Timer, Prover>::HandleSIGINT(Timer* ptimer, Prover* pprover)
{
    ourPTimer=ptimer;
    ourPProver=pprover;
    initSIGINTHandler();
}

template<typename Timer, typename Prover>
void
HandleSIGINT<Timer, Prover>::initSIGINTHandler()
{
    std::signal(SIGINT, handler);
    std::signal(SIGALRM, handler);
    //alarm(10);
}

template<typename Timer, typename Prover>
void
HandleSIGINT<Timer, Prover>::handler(int signal)
{
    ourPTimer->stop();
    std::cout << "Elaplsed time so far: " << std::fixed <<
        std::setprecision(3) << ourPTimer->elapsedSeconds() << "s" << std::endl;
#ifdef COLLECT_STAT
    std::cout << "Current statistics:" << std::endl;
    ourPProver->outputStatistics(std::cout);
    std::cout << std::endl;
#endif

    ourPTimer->start();
    initSIGINTHandler();

    if(signal == SIGINT){
        std::cout << "Continue? (y/n)\n";
        char a; std::cin >> a; 
        if ((a == 'n') || (a == 'N'))
        {
            std::cout << "Bye!\n";
            std::exit(0);
        }
        else
        {
            std::cout << "\nContinuing proof search...\n";
            ourPTimer->start();
            initSIGINTHandler();
        };
    };
}

template<typename Timer, typename Prover>
HandleSIGINT<Timer, Prover>::~HandleSIGINT()
{
    std::signal(SIGINT, SIG_DFL);
}

template<typename Timer, typename Prover> Timer* HandleSIGINT<Timer, Prover>::ourPTimer;
template<typename Timer, typename Prover> Prover* HandleSIGINT<Timer, Prover>::ourPProver;

