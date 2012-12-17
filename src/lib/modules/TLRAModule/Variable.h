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
 * @file Variable.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 *
 * @version 2012-04-05
 * Created on November 14th, 2012
 */

#ifndef TLRA_VARIABLE_H
#define TLRA_VARIABLE_H

#include "Bound.h"
#include <sstream>

namespace tlra
{
    template<class T>
    class Variable
    {
        private:

            /**
             * Members.
             */
            bool                     mBasic;
            unsigned                 mPosition;
            class Bound<T>::BoundSet mUpperbounds;
            class Bound<T>::BoundSet mLowerbounds;
            const Bound<T>*          mpSupremum;
            const Bound<T>*          mpInfimum;
            const GiNaC::ex*         mExpression;
            Value<T>                 mAssignment;

        public:
            Variable( unsigned, bool, const GiNaC::ex*, smtrat::Formula::iterator );
            virtual ~Variable();

            const Value<T>& assignment() const
            {
                return mAssignment;
            }

            Value<T>& rAssignment()
            {
                return mAssignment;
            }

            void setBasic( bool _basic )
            {
                mBasic = _basic;
            }

            bool isBasic() const
            {
                return mBasic;
            }

            void setSupremum( const Bound<T>* _supremum )
            {
                assert( _supremum->isActive() );
                assert( mpSupremum->isActive() );

                if( !mpSupremum->isInfinite() )
                {
                    --mpSupremum->pInfo()->updated;
                }
                ++_supremum->pInfo()->updated;
                mpSupremum = _supremum;

            }

            const Bound<T>* pSupremum() const
            {
                assert( !mpSupremum->origins().empty() );
                return mpSupremum;
            }

            const Bound<T>& supremum() const
            {
                assert( !mpSupremum->origins().empty() );
                return *mpSupremum;
            }

            void setInfimum( const Bound<T>* _infimum )
            {
                assert( _infimum->isActive() );
                assert( mpInfimum->isActive() );

                if( !mpInfimum->isInfinite() )
                {
                    --mpInfimum->pInfo()->updated;
                }
                ++_infimum->pInfo()->updated;
                mpInfimum = _infimum;
            }

            const Bound<T>* pInfimum() const
            {
                assert( !mpInfimum->origins().empty() );
                return mpInfimum;
            }

            const Bound<T>& infimum() const
            {
                assert( !mpInfimum->origins().empty() );
                return *mpInfimum;
            }

            const unsigned position() const
            {
                return mPosition;
            }

            unsigned rLowerBoundsSize()
            {
                return mLowerbounds.size();
            }

            unsigned rUpperBoundsSize()
            {
                return mUpperbounds.size();
            }

            const class Bound<T>::BoundSet& upperbounds() const
            {
                return mUpperbounds;
            }

            const class Bound<T>::BoundSet& lowerbounds() const
            {
                return mLowerbounds;
            }

            class Bound<T>::BoundSet& rUpperbounds()
            {
                return mUpperbounds;
            }

            class Bound<T>::BoundSet& rLowerbounds()
            {
                return mLowerbounds;
            }

            unsigned& rPosition()
            {
                return mPosition;
            }

            const GiNaC::ex* pExpression() const
            {
                return mExpression;
            }

            const GiNaC::ex expression() const
            {
                return *mExpression;
            }

            std::pair<const Bound<T>*, std::pair<const Bound<T>*, const Bound<T>*> > addUpperBound( Value<T>* const, smtrat::Formula::iterator, const smtrat::Constraint* = NULL, bool = false );
            std::pair<const Bound<T>*, std::pair<const Bound<T>*, const Bound<T>*> > addLowerBound( Value<T>* const, smtrat::Formula::iterator, const smtrat::Constraint* = NULL, bool = false );
            std::pair<const Bound<T>*, std::pair<const Bound<T>*, const Bound<T>*> > addEqualBound( Value<T>* const, smtrat::Formula::iterator, const smtrat::Constraint* = NULL );
            void deactivateBound( const Bound<T>*, smtrat::Formula::iterator );

            void print( std::ostream& = std::cout ) const;
            void printAllBounds( std::ostream& = std::cout, const std::string = "" ) const;
    };

    template<class T>
    Variable<T>::Variable( unsigned _position, bool _basic, const GiNaC::ex* _expression, smtrat::Formula::iterator _defaultBoundPosition ):
        mBasic( _basic ),
        mPosition( _position ),
        mUpperbounds(),
        mLowerbounds(),
        mExpression( _expression),
        mAssignment()
    {
        mpSupremum = addUpperBound( NULL, _defaultBoundPosition ).first;
        mpInfimum  = addLowerBound( NULL, _defaultBoundPosition ).first;
    }

    template<class T>
    Variable<T>::~Variable()
    {
        while( !mLowerbounds.empty() )
        {
            const Bound<T>* b = *mLowerbounds.begin();
            mLowerbounds.erase( mLowerbounds.begin() );
            if( !b->type() == Bound<T>::EQUAL ) delete b;
        }
        while( !mUpperbounds.empty() )
        {
            const Bound<T>* b = *mUpperbounds.begin();
            mUpperbounds.erase( mUpperbounds.begin() );
            delete b;
        }
    }

    /**
     *
     * @param _val
     * @return
     */
    template<class T>
    std::pair<const Bound<T>*, std::pair<const Bound<T>*, const Bound<T>*> > Variable<T>::addUpperBound( Value<T>* const _val, smtrat::Formula::iterator _position, const smtrat::Constraint* _constraint, bool _deduced )
    {
        class Bound<T>::Info* boundInfo = new class Bound<T>::Info();
        boundInfo->updated = 0;
        boundInfo->position = _position;
        const Bound<T>* newBound = new Bound<T>( _val, this, Bound<T>::UPPER, _constraint, boundInfo, _deduced );
        std::pair<class Bound<T>::BoundSet::iterator, bool> result = mUpperbounds.insert( newBound );
        if( !result.second )
        {
            delete newBound;
            return std::pair<const Bound<T>*,std::pair<const Bound<T>*, const Bound<T>*> >( *result.first, std::pair<const Bound<T>*, const Bound<T>*>( NULL, NULL ) );
        }
        else
        {
            const Bound<T>* nextStrongerBound = NULL;
            const Bound<T>* nextWeakerBound = NULL;
            if( result.first != mUpperbounds.begin() )
            {
                nextStrongerBound = *(--result.first);
                ++result.first;
            }
            if( result.first != mUpperbounds.end() )
            {
                ++result.first;
                while( result.first != mUpperbounds.end() )
                {
                    if( (*result.first)->type() != Bound<T>::EQUAL )
                    {
                        nextWeakerBound = *result.first;
                        break;
                    }
                    ++result.first;
                }
            }
            return std::pair<const Bound<T>*,std::pair<const Bound<T>*, const Bound<T>*> >( newBound, std::pair<const Bound<T>*, const Bound<T>*>( nextStrongerBound, nextWeakerBound ) );
        }
    }

    /**
     *
     * @param _val
     * @return
     */
    template<class T>
    std::pair<const Bound<T>*,std::pair<const Bound<T>*, const Bound<T>*> > Variable<T>::addLowerBound( Value<T>* const _val, smtrat::Formula::iterator _position, const smtrat::Constraint* _constraint, bool _deduced )
    {
        class Bound<T>::Info* boundInfo = new class Bound<T>::Info();
        boundInfo->updated = 0;
        boundInfo->position = _position;
        const Bound<T>* newBound = new Bound<T>( _val, this, Bound<T>::LOWER, _constraint, boundInfo, _deduced );
        std::pair<class Bound<T>::BoundSet::iterator, bool> result = mLowerbounds.insert( newBound );
        if( !result.second )
        {
            delete newBound;
            return std::pair<const Bound<T>*,std::pair<const Bound<T>*, const Bound<T>*> >( *result.first, std::pair<const Bound<T>*, const Bound<T>*>( NULL, NULL ) );
        }
        else
        {
            const Bound<T>* nextStrongerBound = NULL;
            const Bound<T>* nextWeakerBound = NULL;
            ++result.first;
            if( result.first != mLowerbounds.end() )
            {
                nextStrongerBound = *result.first;
            }
            --result.first;
            while( result.first != mLowerbounds.begin() )
            {
                --result.first;
                if( (*result.first)->type() != Bound<T>::EQUAL )
                {
                    nextWeakerBound = *result.first;
                    break;
                }
            }
            return std::pair<const Bound<T>*,std::pair<const Bound<T>*, const Bound<T>*> >( newBound, std::pair<const Bound<T>*, const Bound<T>*>( nextStrongerBound, nextWeakerBound ) );
        }
    }

    /**
     *
     * @param _val
     * @return
     */
    template<class T>
    std::pair<const Bound<T>*,std::pair<const Bound<T>*, const Bound<T>*> > Variable<T>::addEqualBound( Value<T>* const _val, smtrat::Formula::iterator _position, const smtrat::Constraint* _constraint )
    {
        class Bound<T>::Info* boundInfo = new class Bound<T>::Info();
        boundInfo->updated = 0;
        boundInfo->position = _position;
        const Bound<T>* newBound = new Bound<T>( _val, this, Bound<T>::EQUAL, _constraint, boundInfo );
        std::pair<class Bound<T>::BoundSet::iterator, bool> result = mLowerbounds.insert( newBound );
        if( !result.second )
        {
            delete newBound;
            return std::pair<const Bound<T>*,std::pair<const Bound<T>*, const Bound<T>*> >( *result.first, std::pair<const Bound<T>*, const Bound<T>*>( NULL, NULL ) );
        }
        else
        {
            const Bound<T>* nextWeakerLowerBound = NULL;
            while( result.first != mLowerbounds.begin() )
            {
                --result.first;
                if( (*result.first)->type() != Bound<T>::EQUAL )
                {
                    nextWeakerLowerBound = *result.first;
                    break;
                }
            }
            std::pair<class Bound<T>::BoundSet::iterator, bool> result = mUpperbounds.insert( newBound );
            ++result.first;
            const Bound<T>* nextWeakerUpperBound = NULL;
            while( result.first != mUpperbounds.end() )
            {
                if( (*result.first)->type() != Bound<T>::EQUAL )
                {
                    nextWeakerUpperBound = *result.first;
                    break;
                }
                ++result.first;
            }
            return std::pair<const Bound<T>*,std::pair<const Bound<T>*, const Bound<T>*> >( newBound, std::pair<const Bound<T>*, const Bound<T>*>( nextWeakerLowerBound, nextWeakerUpperBound ) );
        }
    }

    /**
     *
     * @param bound
     */
    template<class T>
    void Variable<T>::deactivateBound( const Bound<T>* bound, smtrat::Formula::iterator _position )
    {
        assert( !bound->isInfinite() );
        assert( !bound->isActive() );
        bound->pInfo()->updated = 0;
        bound->pInfo()->position = _position;
        if( bound->isUpperBound() )
        {
            //check if it is the supremum
            if( mpSupremum == bound )
            {
                //find the supremum
                class Bound<T>::BoundSet::iterator newBound = mUpperbounds.begin();
                while( newBound != --mUpperbounds.end() )
                {
                    if( (*newBound)->isActive() )
                    {
                        ++(*newBound)->pInfo()->updated;
                        mpSupremum = *newBound;
                        goto LowerBounds;
                    }
                    ++newBound;
                }
                mpSupremum = *newBound;
            }
        }
LowerBounds:
        if( bound->isLowerBound() )
        {
            //check if it is the infimum
            if( mpInfimum == bound )
            {
                //find the infimum
                class Bound<T>::BoundSet::reverse_iterator newBound = mLowerbounds.rbegin();
                while( newBound != --mLowerbounds.rend() )
                {
                    if( (*newBound)->isActive() )
                    {
                        ++(*newBound)->pInfo()->updated;
                        mpInfimum = *newBound;
                        return;
                    }
                    ++newBound;
                }
                mpInfimum = *newBound;
            }
        }
    }

    /**
     *
     * @param _out
     */
    template<class T>
    void Variable<T>::print( std::ostream& _out ) const
    {
        std::stringstream out;
        out << *mExpression;
        _out << std::setw( 15 ) << out.str();
        _out << std::setw( 6 ) << "  ->  ";
        _out << std::setw( 35 ) << mAssignment.toString();
        _out << std::setw( 6 ) << "  in [";
        _out << std::setw( 12 ) << mpInfimum->toString();
        _out << std::setw( 2 ) << ", ";
        _out << std::setw( 12 ) << mpSupremum->toString();
        _out << std::setw( 1 ) << "]";
    }

    template<class T>
    void Variable<T>::printAllBounds( std::ostream& _out, const std::string _init ) const
    {
        _out << _init << " Upper bounds: " << std::endl;
        for( class Bound<T>::BoundSet::const_iterator bIter = mUpperbounds.begin(); bIter != mUpperbounds.end(); ++bIter )
        {
            _out << _init << "     ";
            (*bIter)->print( true, _out, true );
            _out << " [" << (*bIter)->pInfo()->updated << "]" << std::endl;
        }
        _out << _init << " Lower bounds: " << std::endl;
        for( class Bound<T>::BoundSet::const_reverse_iterator bIter = mLowerbounds.rbegin(); bIter != mLowerbounds.rend(); ++bIter )
        {
            _out << _init << "     ";
            (*bIter)->print( true, _out, true );
            _out << " [" << (*bIter)->pInfo()->updated << "]" << std::endl;
        }
    }
}    // end namspace tlra
#endif   /* TLRA_VARIABLE_H */