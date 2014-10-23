/*
 *  SMT-RAT - Satisfiability-Modulo-Theories Real Algebra Toolbox
 * Copyright (C) 2012 Florian Corzilius, Ulrich Loup, Erika Abraham, Sebastian Junges
 *
 * This file is part of SMT-RAT.
 *
 * SMT-RAT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SMT-RAT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SMT-RAT.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
/**
 * @file ModuleInput.h
 * @author Florian Corzilius
 * @version 2014-05-16
 */

#pragma once


#include <algorithm>
#include <list>
#include <vector>
#include <set>
#include <iterator>
#include "../ConstraintPool.h"
#include "../FormulaPool.h"
#include "../datastructures/Assignment.h"
#include "../../cli/parser/ParserTypes.h"
#include "../config.h"

#define MODULE_INPUT_USE_HASHING_FOR_FIND

namespace smtrat
{
    /// Stores a formula along with its origins.
    class FormulaWithOrigins
    {
        // Member
        
        /// The formula.
        const Formula* mpFormula;
        /// The formulas origins.
        std::vector<PointerSet<Formula>> mOrigins;
        
    public:
        
        FormulaWithOrigins(); // Default constructor disabled.
        
        /**
         * Constructs a formula with empty origins.
         * @param _formula The formula of the formula with origins to construct.
         */
        FormulaWithOrigins( const Formula* _formula ):
            mpFormula( _formula ),
            mOrigins()
        {}
        
        /**
         * Constructs a formula with the given origins.
         * @param _formula The formula of the formula with origins to construct.
         * @param _origins The origins of the formula with origins to construct.
         */
        FormulaWithOrigins( const Formula* _formula, const std::vector<PointerSet<Formula>>& _origins ):
            mpFormula( _formula ),
            mOrigins( _origins )
        {}
        
        /**
         * Constructs a formula with the given origins.
         * @param _formula The formula of the formula with origins to construct.
         * @param _origins The origins of the formula with origins to construct.
         */
        FormulaWithOrigins( const Formula* _formula, std::vector<PointerSet<Formula>>&& _origins ):
            mpFormula( _formula ),
            mOrigins( std::move( _origins ) )
        {}
        
        FormulaWithOrigins( const FormulaWithOrigins& ); // Copy constructor disabled.
        
        /**
         * @param _fwoA The first formula with origins to compare.
         * @param _fwoB The second formula with origins to compare.
         * @return true, if the formula of the first formula with origins is less than the formula of the second formula with origins;
         *         false, otherwise.
         */
        friend bool operator<( const FormulaWithOrigins& _fwoA, const FormulaWithOrigins& _fwoB )
        {
            return _fwoA.formula() < _fwoB.formula();
        }
        
        /**
         * @param _fwoA The first formula with origins to compare.
         * @param _fwoB The second formula with origins to compare.
         * @return true, if the formula of the first formula with origins and the formula of the second formula with origins are equal;
         *         false, otherwise.
         */
        friend bool operator==( const FormulaWithOrigins& _fwoA, const FormulaWithOrigins& _fwoB )
        {
            return _fwoA.formula() == _fwoB.formula();
        }
        
        /**
         * @return A pointer to the formula.
         */
        const Formula* pFormula() const
        {
            return mpFormula;
        }
        
        /**
         * @return A constant reference to the formula.
         */
        const Formula& formula() const
        {
            return *mpFormula;
        }
        
        /**
         * @return A constant reference to the origins.
         */
        const std::vector<PointerSet<Formula>>& origins() const
        {
            return mOrigins;
        }
        
        /**
         * @return A reference to the origins.
         */
        std::vector<PointerSet<Formula>>& rOrigins()
        {
            return mOrigins;
        }
    };
    
    class Manager; // Forward declaration.
    
    class Module; // Forward declaration.
    
    /**
     * The input formula a module has to consider for it's satisfiability check. It is a list of formulas
     * and semantically considered as their conjunction.
     */
    class ModuleInput : private std::list<FormulaWithOrigins>
    {
        friend class Module;
        
        friend class Manager;
        
    private:
        
        typedef std::list<FormulaWithOrigins> super;
        
        
    public:
            
        /// Passing through the list::iterator.
        typedef super::iterator iterator;
        /// Passing through the list::const_iterator.
        typedef super::const_iterator const_iterator;
        
    private:
        
        // Member.
        /// Store some properties about the conjunction of the stored formulas.
        mutable Condition mProperties;
        #ifdef MODULE_INPUT_USE_HASHING_FOR_FIND
        FastPointerMap<Formula,iterator> mFormulaPositionMap;
        #endif

    public:
            
        /**
         * Constructs a module input.
         */
        ModuleInput(): 
            std::list<FormulaWithOrigins>(),
            mProperties()
            #ifdef MODULE_INPUT_USE_HASHING_FOR_FIND
            ,
            mFormulaPositionMap()
            #endif
        {}
        
        // Methods.
        
        /**
         * @return All known properties of the underlying formula of this module input.
         */
        const Condition& properties() const
        {
            updateProperties();
            return mProperties;
        }
        
        /**
         * @return true, if this formula is a conjunction of constraints;
         *          false, otherwise.
         */
        bool isConstraintConjunction() const
        {
            if( PROP_IS_PURE_CONJUNCTION <= mProperties )
                return !(PROP_CONTAINS_BOOLEAN <= mProperties);
            else
                return false;
        }

        /**
         * @return true, if this formula is a conjunction of real constraints;
         *          false, otherwise.
         */
        bool isRealConstraintConjunction() const
        {
            if( PROP_IS_PURE_CONJUNCTION <= mProperties )
                return (!(PROP_CONTAINS_INTEGER_VALUED_VARS <= mProperties) && !(PROP_CONTAINS_BOOLEAN <= mProperties));
            else
                return false;
        }

        /**
         * @return true, if this formula is a conjunction of integer constraints;
         *         false, otherwise.
         */
        bool isIntegerConstraintConjunction() const
        {
            if( PROP_IS_PURE_CONJUNCTION <= mProperties )
                return (!(PROP_CONTAINS_REAL_VALUED_VARS <= mProperties) && !(PROP_CONTAINS_BOOLEAN <= mProperties));
            else
                return false;
        }
        
        /**
         * @param _assignment The model to check conjunction of the stored formulas against.
         * @return 1, if the conjunction of the stored formulas is satisfied by the given model;
         *         0, if the given model conflicts the conjunction of the stored formulas;
         *         2, if it cannot be determined cheaply, whether the given model conflicts or satisfies 
         *            the conjunction of the stored formulas.
         */
        unsigned satisfiedBy( const Model& _assignment ) const
        {
            EvalRationalMap rationalAssigns;
            if( getRationalAssignmentsFromModel( _assignment, rationalAssigns ) )
                return satisfiedBy( rationalAssigns );
            else
                return 2; // TODO: Check also models having square roots as value.
        }
        
        /**
         * @param _assignment The assignment to check conjunction of the stored formulas against.
         * @return 1, if the conjunction of the stored formulas is satisfied by the given assignment;
         *         0, if the given assignment conflicts the conjunction of the stored formulas;
         *         2, if it cannot be determined cheaply, whether the given assignment conflicts or satisfies 
         *            the conjunction of the stored formulas.
         */
        unsigned satisfiedBy( const EvalRationalMap& _assignment ) const;
        
        iterator begin()
        {
            return super::begin();
        }
        
        iterator end()
        {
            return super::end();
        }
        
        const_iterator begin() const
        {
            return super::begin();
        }
        
        const_iterator end() const
        {
            return super::end();
        }
        
        bool empty() const
        {
            return super::empty();
        }
        
        size_t size() const
        {
            return super::size();
        }
        
        iterator find( const Formula* _formula );
        
        const_iterator find( const Formula* _formula ) const;
        
        iterator find( const_iterator _hint, const Formula* _formula );
        
        const_iterator find( const_iterator _hint, const Formula* _formula ) const;
        
        /**
         * @param _subformula The formula for which to check whether it is one of the stored formulas.
         * @return true, if the given formula is one of the stored formulas;
         *         false, otherwise.
         */
        bool contains( const Formula* _subformula ) const
        {
            #ifdef MODULE_INPUT_USE_HASHING_FOR_FIND
            return mFormulaPositionMap.find( _subformula ) != mFormulaPositionMap.end();
            #else
            return std::find( begin(), end(), _subformula ) != end();
            #endif
        }
        
        /**
         * Updates all properties of the formula underlying this module input.
         */
        void updateProperties() const;
        
        /**
         * Collects all real valued variables occurring in this formula.
         * @param _realVars The container to collect the real valued variables in.
         */
        void realValuedVars( Variables& _realVars ) const
        {
            for( const FormulaWithOrigins& fwo : *this )
                fwo.formula().realValuedVars( _realVars );
        }

        /**
         * Collects all integer valued variables occurring in this formula.
         * @param _intVars The container to collect the integer valued variables in.
         */
        void integerValuedVars( Variables& _intVars ) const
        {
            for( const FormulaWithOrigins& fwo : *this )
                fwo.formula().integerValuedVars( _intVars );
        }

        /**
         * Collects all arithmetic variables occurring in this formula.
         * @param _arithmeticVars The container to collect the arithmetic variables in.
         */
        void arithmeticVars( Variables& _arithmeticVars ) const
        {
            for( const FormulaWithOrigins& fwo : *this )
                fwo.formula().arithmeticVars( _arithmeticVars );
        }

        /**
         * Collects all Boolean variables occurring in this formula.
         * @param _booleanVars The container to collect the Boolean variables in.
         */
        void booleanVars( Variables& _booleanVars ) const
        {
            for( const FormulaWithOrigins& fwo : *this )
                fwo.formula().booleanVars( _booleanVars );
        }
        
        struct IteratorCompare
        {
            bool operator() ( const_iterator i1, const_iterator i2 ) const
            {
                return (*i1) < (*i2);
            }
        };
        
        /**
         * @return The string representation of this module input.
         */
        std::string toString() const
        {
            std::string result = "(and";
            for( auto& fwo : *this )
                result += " " + fwo.formula().toString();
            result += ")";
            return result;
        }
        
//        friend std::ostream& operator<<( std::ostream& _out, const ModuleInput& _mi )
//        {
//            return _out << _mi.toString()
//        }
        
        iterator erase( iterator _formula );
        
        bool removeOrigin( iterator _formula, const Formula* _origin );
        
        bool removeOrigins( iterator _formula, const std::vector<PointerSet<Formula>>& _origins );
        
        bool removeOrigins( iterator _formula, const PointerSet<Formula>& _origins );
        
        std::pair<iterator,bool> add( const Formula* _formula )
        {
            PointerSet<Formula> origins;
            return add( _formula, std::move( origins ) );
        }
        
        std::pair<iterator,bool> add( const Formula* _formula, const Formula* _origin )
        {
            PointerSet<Formula> origins;
            origins.insert( _origin );
            return add( _formula, std::move( origins ) );
        }
        
        std::pair<iterator,bool> add( const Formula* _formula, const PointerSet<Formula>& _origins )
        {
            PointerSet<Formula> originsCopy( _origins );
            return add( _formula, std::move( originsCopy ) );
        }
        
        std::pair<iterator,bool> add( const Formula* _formula, const std::vector<PointerSet<Formula>>& _origins )
        {
            std::vector<PointerSet<Formula>> originsCopy( _origins );
            return add( _formula, std::move( originsCopy ) );
        }
        
        std::pair<iterator,bool> add( const Formula* _formula, PointerSet<Formula>&& _origins );
        
        std::pair<iterator,bool> add( const Formula* _formula, std::vector<PointerSet<Formula>>&& _origins );
    };
    
    void annotateFormula( const Formula*, const std::vector<parser::Attribute>& );
}    // namespace smtrat