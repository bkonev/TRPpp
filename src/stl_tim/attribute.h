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
#ifndef __ATTRIBUTE_H__
#define __ATTRIBUTE_H__
#include <iostream>
#include "misc/exception.h"
//#include <valgrind/memcheck.h>

namespace Exceptions
{
    class AttributeException : public Exception
    {
    private:
        const std::string myWhat;
    public:
        virtual void 
        module(std::ostream& os) const
        {
            os << "Attribute: ";
        }
        virtual void 
        what (std::ostream& os) const
        {
            os << "wrong attribute occured" << myWhat << std::endl;
        }
        AttributeException(const std::string& what)
        : myWhat(what)
        { }
    }; // class AttributeException
}

namespace PropositionalProver
{
    using namespace Exceptions;
    // Attribute models  First-Order terms.
    class Attribute
    {
    public:
        enum AttributeType {initial, universal, step_now, step_next};
        static const int attributeTypeSize = 4;
        //
        // disactivated as it is not needed for now
        /*class AttributeComparator
        {
        private: 
            bool myComparisionTable[attributeTypeSize*attributeTypeSize];
        public:
            // constructor from an array I know that it is not qute safe but
            // this is a low-level data, so I do not want to spend time on
            // object-orientation.. I wonder if this works..
            //
            AttributeComparator(const bool* comparisionTable)
            {
                for (unsigned int i = 0; i < attributeTypeSize*attributeTypeSize; i++)
                    myComparisionTable[i] = comparisionTable[i];
            }

            bool
            compare(const AttributeType aType1, const AttributeType aType2) const
            {
                return myComparisionTable[aType1 * attributeTypeSize + aType2];
            }
        }; */
        class AttributeCompatibilitiser
        {
        private: 
            bool myCompatibilityTable[attributeTypeSize*attributeTypeSize];
        public:
            // constructor from an array I know that it is not qute safe but
            // this is a low-level data, so I do not want to spend time on
            // object-orientation.. I wonder if this works..
            AttributeCompatibilitiser(const bool* compatibilityTable)
            {
                for (unsigned int i = 0; i < attributeTypeSize*attributeTypeSize; i++)
                    myCompatibilityTable[i] = compatibilityTable[i];
            }

            bool 
            areCompatibleTypes(const AttributeType aType1, const AttributeType aType2) const
            {
                return myCompatibilityTable[aType1 * attributeTypeSize + aType2];
            }
        };

        class AttributeEqualizer
        {
        private: 
            bool myEqualityTable[attributeTypeSize*attributeTypeSize];
        public:
            // constructor from an array I know that it is not qute safe but
            // this is a low-level data, so I do not want to spend time on
            // object-orientation.. I wonder if this works..
            AttributeEqualizer(const bool* equalityTable)
            {
                for (unsigned int i = 0; i < attributeTypeSize*attributeTypeSize; i++)
                    myEqualityTable[i] = equalityTable[i];
            }

            bool 
            areEqual(const AttributeType aType1, const AttributeType aType2) const
            {
                return myEqualityTable[aType1 * attributeTypeSize + aType2];
            }
        };
    private:
        AttributeType myType;
        //static const AttributeComparator*       ourAttributeComparator; 
        static const AttributeCompatibilitiser* ourCompatibilitiser;
        static const AttributeEqualizer*        ourEqualizer;

    public:
        Attribute(const AttributeType& type)
        : myType(type)
        { }

        /*inline static void 
        setComparator(const AttributeComparator* comparator); */

        inline static void 
        setCompatibilitiser(const AttributeCompatibilitiser* compatibilitiser);

        inline static void 
        setEqualizer(const AttributeEqualizer* equalizer);

        bool
        operator< (const Attribute& attribute) const
        {
            // In fact, for now I do not need to change comparision
            // criteria. Therefore, I cut off this thing
            //return ourAttributeComparator->compare(myType, attribute.myType); 
            //VALGRIND_CHECK_VALUE_IS_DEFINED(myType);
            //VALGRIND_CHECK_VALUE_IS_DEFINED(attribute.myType);
            
            if ((myType == step_now) && (attribute.myType == step_next))
                return true;
            else 
                return false;
        } 

        bool 
        operator== (const Attribute& attribute) const
        {
            return ourEqualizer->areEqual(attribute.myType, myType);
        } 

        bool 
        operator!= (const Attribute& attribute) const
        {
            return !operator==(attribute);
        } 

        const AttributeType&
        getType() const
        {
            return myType;
        }

        friend inline bool
                areCompatible(const Attribute& attribute1, const Attribute& attribute2);
    };

    //input/output:
    extern std::ostream& 
    operator<< (std::ostream&, const Attribute&);

    inline bool
    areCompatible(const Attribute& attribute1, const Attribute& attribute2) 
    {
        return Attribute::ourCompatibilitiser->areCompatibleTypes(attribute1.getType(), attribute2.getType());
    }

    // when we resolve clauses, we recompute attributes of the result..
    // AttributeTransformer models unification
    class AttributeTransformer
    {
    public:
        class TransformerState
        {
        public:
            virtual const Attribute&
            transform(const Attribute&) = 0;
            virtual ~TransformerState() {} // suppress warnings
        };
    private: 
        TransformerState* myState;
    public:
        // Constructor.
        // The first argument is the Attribute of a clause to 
        // be transformed, the second is the Attribute that determines
        // how to transform. Thus, it may be not commutative!
        AttributeTransformer(const Attribute&, const Attribute&);
        const Attribute& operator() (const Attribute& attribute)
        {
            return myState->transform(attribute.getType());        
        }
    };

    // constants declarations
    extern const Attribute initial_attr   ;
    extern const Attribute universal_attr ;
    extern const Attribute step_now_attr  ;
    extern const Attribute step_next_attr ;
    extern const Attribute& default_attr  ;

    //extern const Attribute::AttributeComparator normalComparator;
    extern const Attribute::AttributeCompatibilitiser normalCompatibilitiser;
    extern const Attribute::AttributeEqualizer normalEqualizer;

    //extern const Attribute::AttributeComparator loopSearchComparator;
    extern const Attribute::AttributeCompatibilitiser loopSearchCompatibilitiser;
    extern const Attribute::AttributeEqualizer loopSearchEqualizer;

    /*inline void 
    Attribute::setComparator(const Attribute::AttributeComparator 
            *comparator = &normalComparator)
    {
        ourAttributeComparator = comparator;
    }*/

    inline void 
    Attribute::setCompatibilitiser(
            const Attribute::AttributeCompatibilitiser 
            *compatibilitiser = &normalCompatibilitiser)
    {
        ourCompatibilitiser=compatibilitiser;
    }

    inline void 
    Attribute::setEqualizer(
            const Attribute::AttributeEqualizer 
            *equalizer = &normalEqualizer)
    {
        ourEqualizer=equalizer;
    }
} // namespace PropositionalProver
#endif // __ATTRIBUTE_H__
