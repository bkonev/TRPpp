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
#include "attribute.h"
namespace PropositionalProver
{
    // i/o
    std::ostream& operator<< (std::ostream& os, const Attribute& attribute)
    {
        switch(attribute.getType())
        {
            case Attribute::initial:
                os << "initial";
                break;
            case Attribute::universal:
                os << "universal";
                break;
            case Attribute::step_now:
                os << "step_now";
                break;
            case Attribute::step_next:
                os << "step_next";
                break;
            default:
                throw AttributeException("");
                break;
        }
        return os;
    }
    // static members
    //const Attribute::AttributeComparator*       Attribute::ourAttributeComparator;
    const Attribute::AttributeCompatibilitiser* Attribute::ourCompatibilitiser;
    const Attribute::AttributeEqualizer*        Attribute::ourEqualizer;
    // pre-defined attributes
    const Attribute initial_attr   = Attribute(Attribute::initial);
    const Attribute universal_attr = Attribute(Attribute::universal);
    const Attribute step_now_attr  = Attribute(Attribute::step_now);
    const Attribute step_next_attr = Attribute(Attribute::step_next);
    // default attribute
    const Attribute& default_attr  = initial_attr;

    // Re: that attributes encode First-Order terms, namely the correspondents
    // is:
    // initial       0
    // universal     X
    // step_now      X
    // step_next     s(X)
    //
    // normal operation
    // In normal operation universal and step_now are not distinguishable
    //
    /*
    const bool normalComparisionTable[] = 
    {
    //   initial universal step_now  step_next 2
    //                                           /1 arguments
        false  , false   , false   , false   , // initial
        false  , false   , false   , false   , // universal
        false  , false   , false   , true    , // step_now
        false  , false   , false   , false     // step_next
    };
    const Attribute::AttributeComparator normalComparator(normalComparisionTable); */

    const bool normalCompatibilityTable[]=
    {
    //   initial universal step_now  step_next
        true   , true    , true    , false   , // initial
        true   , true    , true    , true    , // universal
        true   , true    , true    , true    , // step_now
        false  , true    , true    , true      // step_next
    };
    const Attribute::AttributeCompatibilitiser normalCompatibilitiser(normalCompatibilityTable);

    const bool normalEqualityTable[]=
    {
    //   initial universal step_now  step_next
        true   , false   , false   , false   , // initial
        false  , true    , true    , false   , // universal
        false  , true    , true    , false   , // step_now
        false  , false   , false   , true      // step_next
    };
    const Attribute::AttributeEqualizer normalEqualizer(normalEqualityTable);

    // loop-search operation
    // in loop-search mode, there IS destinction between universal and
    // step_now
    //
    // comparision is not affected
    /*const Attribute::AttributeComparator loopSearchComparator(normalComparisionTable); */

    const bool loopSearchCompatibilityTable[]=
    {
    //   initial universal step_now  step_next
        false  , false   , false   , false   , // initial
        false  , true    , false   , true    , // universal
        false  , false   , false   , false   , // step_now
        false  , true    , false   , true      // step_next
    };
    const Attribute::AttributeCompatibilitiser loopSearchCompatibilitiser(loopSearchCompatibilityTable);

    const bool loopSearchEqualityTable[]=
    {
    //   initial universal step_now  step_next
        true   , false   , false   , false   , // initial
        false  , true    , false   , false   , // universal
        false  , false   , true    , false   , // step_now
        false  , false   , false   , true      // step_next
    };
    const Attribute::AttributeEqualizer loopSearchEqualizer(loopSearchEqualityTable);


    ///////////////////////////////////////////////////////////////////
    // 
    // AttributeTransformer staff
    //
    ///////////////////////////////////////////////////////////////////
    class CommutativeInitInitState : public AttributeTransformer::TransformerState
    {
    public:
        virtual const Attribute& 
        transform(const Attribute&)
        {
            return initial_attr;
            /*
            switch(attribute.getType())
            {
                case Attribute::initial:
                    return initial_attr;
                    break;
                default:
                    throw AttributeException("");
                    break;
            }
            */
        }
    };
    CommutativeInitInitState commutativeInitInitState;

    class CommutativeInitUniversalState : public AttributeTransformer::TransformerState
    {
    public:
        virtual const Attribute& 
        transform(const Attribute&)
        {
            return initial_attr;
            /*
            switch(attribute.getType())
            {
                case Attribute::initial:
                case Attribute::universal:
                    return initial_attr;
                    break;
                default:
                    throw AttributeException("");
                    break;
            }
            */
        }
    };
    CommutativeInitUniversalState commutativeInitUniversalState;

    class CommutativeInitStepNowState : public AttributeTransformer::TransformerState
    {
    public:
        virtual const Attribute& 
        transform(const Attribute&)
        {
            return initial_attr;
            /*
            switch(attribute.getType())
            {
                case Attribute::initial:
                case Attribute::step_now:
                    return initial_attr;
                    break;
                default:
                    throw AttributeException("");
                    break;
            }
            */
        }
    };
    CommutativeInitStepNowState commutativeInitStepNowState;

    class CommutativeUniversalUniversalState : public AttributeTransformer::TransformerState
    {
    public:
        virtual const Attribute& 
        transform(const Attribute&)
        {
            return universal_attr;
            /*
            switch(attribute.getType())
            {
                case Attribute::universal:
                    return universal_attr;
                    break;
                default:
                    throw AttributeException("");
                    break;
            }
            */
        }
    };
    CommutativeUniversalUniversalState commutativeUniversalUniversalState;

    class CommutativeUniversalStepNowState : public AttributeTransformer::TransformerState
    {
    public:
        virtual const Attribute& 
        transform(const Attribute&)
        {
            return universal_attr;
            /*
            switch(attribute.getType())
            {
                case Attribute::universal:
                case Attribute::step_now:
                    return universal_attr;
                    break;
                default:
                    throw AttributeException("");
                    break;
            }
            */
        }
    };
    CommutativeUniversalStepNowState commutativeUniversalStepNowState;

    class CommutativeUniversalStepNextState : public AttributeTransformer::TransformerState
    {
    public:
        virtual const Attribute& 
        transform(const Attribute& attribute)
        {
            switch(attribute.getType())
            {
                case Attribute::step_now:
                    return step_now_attr;
                    break;
                case Attribute::universal:
                case Attribute::step_next:
                    return step_next_attr;
                    break;
                default:
                    throw AttributeException("");
                    break;
            }
        }
    };
    CommutativeUniversalStepNextState commutativeUniversalStepNextState;

    class CommutativeStepNowStepNowState : public AttributeTransformer::TransformerState
    {
    public:
        virtual const Attribute& 
        transform(const Attribute&)
        {
            return universal_attr;
            /*
            switch(attribute.getType())
            {
                case Attribute::step_now:
                    return universal_attr;
                    break;
                default:
                    throw AttributeException("");
                    break;
            }
            */
        }
    };
    CommutativeStepNowStepNowState commutativeStepNowStepNowState;

    // the only place where it is now commutative:
    // rewriting StepNow (wich is essentially Universal) into StepNext
    // by StepNext
    class NonCommutativeStepNowStepNextState : public AttributeTransformer::TransformerState
    {
    public:
        virtual const Attribute& 
        transform(const Attribute& attribute)
        {
            switch(attribute.getType())
            {
                case Attribute::step_now:
                    return step_next_attr;
                    break;
                case Attribute::step_next:
                    return step_next_attr;
                    break;
                default:
                    throw AttributeException("");
                    break;
            }
        }
    };
    NonCommutativeStepNowStepNextState nonCommutativeStepNowStepNextState;

    // the only place where it is now commutative:
    // rewriting StepNext by StepNow
    class NonCommutativeStepNextStepNowState : public AttributeTransformer::TransformerState
    {
    public:
        virtual const Attribute& 
        transform(const Attribute& attribute)
        {
            // I have to enumerate possibilities explicitely
            // since otherwise I would probabli return a 
            // reference to a temporary object
            switch(attribute.getType())
            {
                case Attribute::step_now:
                    return step_now_attr;
                    break;
                case Attribute::step_next:
                    return step_next_attr;
                    break;
                default:
                    throw AttributeException("");
                    break;
            }
        }
    };
    NonCommutativeStepNextStepNowState nonCommutativeStepNextStepNowState;

    class CommutativeStepNextStepNextState : public AttributeTransformer::TransformerState
    {
    public:
        virtual const Attribute& 
        transform(const Attribute& attribute)
        {
            // I have to enumerate possibilities explicitely
            // since otherwise I would probabli return a 
            // reference to a temporary object
            switch(attribute.getType())
            {
                case Attribute::step_now:
                    return step_now_attr;
                    break;
                case Attribute::step_next:
                    return step_next_attr;
                    break;
                default:
                    throw AttributeException("");
                    break;
            }
        }
    };
    CommutativeStepNextStepNextState commutativeStepNextStepNextState;

    // Recall: transform the first argument by the second
    AttributeTransformer::AttributeTransformer
                  (const Attribute& attribute1, const Attribute& attribute2)
    {
        switch(attribute1.getType())
        {
            case Attribute::initial:
                switch(attribute2.getType())
                {
                    case Attribute::initial:
                        myState = &commutativeInitInitState;
                        break;
                    case Attribute::universal:
                        myState = &commutativeInitUniversalState;
                        break;
                    case Attribute::step_now:
                        myState = &commutativeInitStepNowState;
                        break;
                    case Attribute::step_next:
                        throw AttributeException("");
                        break;
                }
                ;
                break;
            case Attribute::universal:
                switch(attribute2.getType())
                {
                    case Attribute::initial:
                        myState = &commutativeInitUniversalState;
                        break;
                    case Attribute::universal:
                        myState = &commutativeUniversalUniversalState;
                        break;
                    case Attribute::step_now:
                        myState = &commutativeUniversalStepNowState;
                        break;
                    case Attribute::step_next:
                        myState = &commutativeUniversalStepNextState;
                        break;
                }
                break;
            case Attribute::step_now:
                switch(attribute2.getType())
                {
                    case Attribute::initial:
                        myState = &commutativeInitStepNowState;
                        break;
                    case Attribute::universal:
                        myState = &commutativeUniversalStepNowState;
                        break;
                    case Attribute::step_now:
                        myState = &commutativeStepNowStepNowState;
                        break;
                    case Attribute::step_next:
                        myState = &nonCommutativeStepNowStepNextState;
                        break;
                }
                break;
            case Attribute::step_next:
                switch(attribute2.getType())
                {
                    case Attribute::initial:
                        throw AttributeException("");
                        break;
                    case Attribute::universal:
                        myState = &commutativeUniversalStepNextState;
                        break;
                    case Attribute::step_now:
                        myState = &nonCommutativeStepNextStepNowState;
                        break;
                    case Attribute::step_next:
                        myState = &commutativeStepNextStepNextState;
                        break;
                }
                break;
            default:
                throw AttributeException("");
                break;
        }
    } // AttributeTransformer constructor

} //namespace PropositionalProver
