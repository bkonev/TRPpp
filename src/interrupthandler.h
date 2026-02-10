#ifndef __INTERRUPTHANDLER_H__
#define __INTERRUPTHANDLER_H__
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
#include <csignal>
#include <cstdlib>
#include <unistd.h>

namespace SignalHandling
{
    using ProcessFn = void (*)(int);

    inline volatile sig_atomic_t& pendingSignal()
    {
        static volatile sig_atomic_t signal = 0;
        return signal;
    }

    inline ProcessFn& processFn()
    {
        static ProcessFn fn = 0;
        return fn;
    }

    inline void setProcessFn(ProcessFn fn)
    {
        processFn() = fn;
    }

    inline int takeSignal()
    {
        sig_atomic_t s = pendingSignal();
        pendingSignal() = 0;
        return static_cast<int>(s);
    }

    inline void processPending()
    {
        int s = takeSignal();
        if (s == 0)
        {
            return;
        }
        ProcessFn fn = processFn();
        if (fn)
        {
            fn(s);
        }
    }
}

template <typename Timer, typename Prover>
class HandleSIGINT
{
public:
    HandleSIGINT (Timer* timer, Prover* prover);
    static void 
    handler (int);
    static void
    processSignal(int);
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
    SignalHandling::setProcessFn(&HandleSIGINT<Timer, Prover>::processSignal);
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
    // Async-signal-safe: record the signal and emit a short message.
    SignalHandling::pendingSignal() = signal;
    const char msg[] = "Signal received. Processing will continue.\n";
    ssize_t ignored = write(STDERR_FILENO, msg, sizeof(msg) - 1);
    (void)ignored;
}

template<typename Timer, typename Prover>
void
HandleSIGINT<Timer, Prover>::processSignal(int signal)
{
    if (!ourPTimer || !ourPProver)
    {
        return;
    }
    ourPTimer->stop();
    std::cout << "Elaplsed time so far: " << std::fixed <<
        std::setprecision(3) << ourPTimer->elapsedSeconds() << "s" << std::endl;
#ifdef COLLECT_STAT
    std::cout << "Current statistics:" << std::endl;
    ourPProver->outputStatistics(std::cout);
    std::cout << std::endl;
#endif

    ourPTimer->start();

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

#endif // __INTERRUPTHANDLER_H__
