/**
 * Class to create a condition object.
 * @author Florian Corzilius
 * @since 2010-07-26
 * @version 2011-06-20
 */

#include "Condition.h"

using namespace std;

namespace vs
{
    Condition::Condition( const smtrat::ConstraintT& _cons, size_t _id, size_t _val, bool _flag, const carl::PointerSet<Condition>& _oConds, bool _rAdded ):
        mFlag( _flag ),
        mRecentlyAdded( _rAdded ),
        mValuation( _val ),
        mId( _id ),
        mConstraint( _cons ),
        mpOriginalConditions( new carl::PointerSet<Condition>( _oConds ) )
    {}

    Condition::Condition( const Condition& _cond, size_t _id ):
        mFlag( _cond.flag() ),
        mRecentlyAdded( false ),
        mValuation( _cond.valuation() ),
        mId( _id ),
        mConstraint( _cond.constraint() ),
        mpOriginalConditions( new carl::PointerSet<Condition>( _cond.originalConditions() ) )
    {}

    Condition::~Condition()
    {
        (*mpOriginalConditions).clear();
        delete mpOriginalConditions;
    }

    const double Condition::INFINITLY_MANY_SOLUTIONS_WEIGHT = 2;
    
    /**
     * Valuates the constraint according to a variable (it possibly not contains).
     *
     * @param _consideredVariable The variable which is considered in this valuation.
     * @param _maxNumberOfVars
     *
     * @return A valuation of the constraint according to an heuristic.
     */
    double Condition::valuate( const carl::Variable& _consideredVariable, size_t _maxNumberOfVars, bool _preferEquation ) const
    {
        if( !constraint().hasVariable( _consideredVariable ) )
            return 0;
        const smtrat::VarPolyInfo& varInfo = constraint().varInfo( _consideredVariable );
        double maximum = 0;
        if( _maxNumberOfVars < 4 )
            maximum = 16;
        else
            maximum = (double)_maxNumberOfVars * (double)_maxNumberOfVars;
        // Check the relation symbol.
        double relationSymbolWeight = 0;
        switch( constraint().relation() )
        {
            case carl::Relation::EQ:
                relationSymbolWeight += 1;
                break;
            case carl::Relation::GEQ:
                relationSymbolWeight += 2;
                break;
            case carl::Relation::LEQ:
                relationSymbolWeight += 2;
                break;
            case carl::Relation::LESS:
                relationSymbolWeight += 4;
                break;
            case carl::Relation::GREATER:
                relationSymbolWeight += 4;
                break;
            case carl::Relation::NEQ:
                relationSymbolWeight += 3;
                break;
            default:
                return 0;
        }
        //Check the degree of the variable.
        double degreeWeight = (double)varInfo.maxDegree();
        if( maximum <= degreeWeight )
            degreeWeight = maximum - 1;
        //Check the leading coefficient of the given variable.
        unsigned lCoeffWeight = 0;
        unsigned lCoeffWeightB = 2;
        if( degreeWeight <= 1 )
        {
            smtrat::Poly coeff = constraint().coefficient( _consideredVariable, varInfo.maxDegree() );
            if( coeff.isConstant() )
            {
                if( _consideredVariable.type() == carl::VariableType::VT_INT && (coeff == smtrat::ONE_POLYNOMIAL || coeff == smtrat::MINUS_ONE_POLYNOMIAL) )
                {
                    lCoeffWeightB = 1;
                }
                lCoeffWeight = 1;
            }
            else
                lCoeffWeight = 3;
        }
        else if( degreeWeight == 2 )
        {
            bool hasRationalLeadingCoefficient = constraint().coefficient( _consideredVariable, varInfo.maxDegree() ).isConstant();
            if( hasRationalLeadingCoefficient && constraint().coefficient( _consideredVariable, varInfo.maxDegree() - 1 ).isConstant() )
                lCoeffWeight = 1;
            else if( hasRationalLeadingCoefficient )
                lCoeffWeight = 2;
            else
                lCoeffWeight = 3;
        }
        // Check the number of variables.
        double numberOfVariableWeight = (double) constraint().variables().size();
        // Check how in how many monomials the variable occurs.
        double numberOfVariableOccurencesWeight = (double)varInfo.occurence();
        if( maximum <= numberOfVariableOccurencesWeight )
            numberOfVariableOccurencesWeight = maximum - 1;
        // If variable occurs only in one monomial, give a bonus if all other monomials are either positive or negative.
        double otherMonomialsPositiveWeight = 2;
        double finitlyManySolutionsWeight = INFINITLY_MANY_SOLUTIONS_WEIGHT;
        // TODO: avoid expanding the polynomial somehow
#ifdef __VS
        smtrat::Poly::PolyType polyExpanded = (smtrat::Poly::PolyType)constraint().lhs();
#else
	typename smtrat::Poly::PolyType polyExpanded = (typename smtrat::Poly::PolyType)constraint().lhs();
#endif
	if( numberOfVariableOccurencesWeight == 1 && ( polyExpanded.nrTerms() == 1 || (constraint().constantPart() != smtrat::ZERO_RATIONAL && polyExpanded.nrTerms() > 1) ) )
        {
            bool allOtherMonomialsPos = true;
            bool allOtherMonomialsNeg = true;
            for( auto term = polyExpanded.begin(); term != polyExpanded.end(); ++term )
            {
                if( term->has( _consideredVariable ) )
                {
                    if( term->getNrVariables() > 1 )
                    {
                        allOtherMonomialsPos = false;
                        allOtherMonomialsNeg = false;
                        break;
                    }
                }
                else
                {
                    carl::Definiteness defin = term->definiteness();
                    if( defin == carl::Definiteness::NON )
                    {
                        allOtherMonomialsPos = false;
                        allOtherMonomialsNeg = false;
                        break;
                    }
                    else if( defin > carl::Definiteness::NON )
                    {
                        if( !allOtherMonomialsNeg )
                            allOtherMonomialsNeg = false;
                        if( !allOtherMonomialsPos )
                            break;
                    }
                    else
                    {
                        if( !allOtherMonomialsPos )
                            allOtherMonomialsPos = false;
                        if( !allOtherMonomialsNeg )
                            break;
                    }
                }
            }
            if( allOtherMonomialsPos || allOtherMonomialsNeg )
            {
                otherMonomialsPositiveWeight = 1;
                if( constraint().relation() == carl::Relation::EQ )
                    finitlyManySolutionsWeight = 1;
            }
        }
        double weightFactorTmp = maximum;
//        cout << "valuate " << mConstraint << " for " << _consideredVariable << endl;
//        cout << "finitlyManySolutionsWeight = " << finitlyManySolutionsWeight << endl;
//        cout << "wtfweight = " << (_preferEquation ? ((constraint().relation() == carl::Relation::EQ || degreeWeight <= 2) ? 1 : 2) : (degreeWeight <= 2 ? 1 : 2)) << endl;
//        cout << "relationSymbolWeight = " << relationSymbolWeight << endl;
//        cout << "univariateWeight = " << (degreeWeight <= 2 && numberOfVariableWeight == 1 ? 1 : 2) << endl;
//        cout << "degreeWeight = " << degreeWeight << endl;
//        cout << "lCoeffWeight = " << lCoeffWeight << endl;
//        if( _consideredVariable.getType() == carl::VariableType::VT_INT )
//        {
//            cout << "lCoeffWeightB = " << lCoeffWeightB << endl;
//        }
//        cout << "numberOfVariableWeight = " << numberOfVariableWeight << endl;
//        cout << "numberOfVariableOccurencesWeight = " << numberOfVariableOccurencesWeight << endl;
//        cout << "otherMonomialsPositiveWeight = " << otherMonomialsPositiveWeight << endl;
        double result = finitlyManySolutionsWeight;
        if( _preferEquation )
            result += ((constraint().relation() == carl::Relation::EQ || degreeWeight <= 2) ? 1 : 2)/weightFactorTmp;
        else
            result += (degreeWeight <= 2 ? 1 : 2)/weightFactorTmp;
        weightFactorTmp *= maximum;
        result += relationSymbolWeight/weightFactorTmp;
        weightFactorTmp *= maximum;
        //result += (degreeWeight <= 2 && numberOfVariableWeight == 1 ? 1 : 2) /weightFactorTmp;
        //weightFactorTmp *= maximum;
        result += degreeWeight/weightFactorTmp;
        weightFactorTmp *= maximum;
        result += lCoeffWeight/weightFactorTmp;
        weightFactorTmp *= maximum;
        if( _consideredVariable.type() == carl::VariableType::VT_INT )
        {
            result += lCoeffWeightB/weightFactorTmp;
            weightFactorTmp *= maximum;
        }
        //result += numberOfVariableWeight/weightFactorTmp;
        //weightFactorTmp *= maximum;
        result += numberOfVariableOccurencesWeight/weightFactorTmp;
        weightFactorTmp *= maximum;
        result += otherMonomialsPositiveWeight/weightFactorTmp;
        return result;
            
    }

    /**
     * Checks the equality of a given condition (right hand side) with this condition (left hand side).
     *
     * @param _condition The condition to compare with (rhs).
     *
     * @return  true    ,if the given condition is equal to this condition;
     *          false   ,otherwise.
     */
    bool Condition::operator==( const Condition& _condition ) const
    {
        return mId == _condition.getId();
    }

    /**
     * Checks if the given condition (right hand side) is greater than this condition (left hand side).
     *
     * @param _condition The condition to compare with (rhs).
     *
     * @return  true    ,if the given substitution is greater than this substitution;
     *          false   ,otherwise.
     */
    bool Condition::operator<( const Condition& _condition ) const
    {
        return mId < _condition.getId();
    }
    
    std::ostream& operator<<( std::ostream& _out, const Condition& _condition )
    {
        _condition.print( _out );
        return _out;
    }

    /**
     * Prints the condition to an output stream.
     *
     * @param _out The output stream, where it should print.
     */
    void Condition::print( std::ostream& _out ) const
    {
        _out << constraint().toString( 0, true, true );
        _out << " [" << mId << "]";
        _out << "   ";
        if( flag() )
            _out << "(true, ";
        else
            _out << "(false, ";
        _out << "valuation=" << valuation();
        if( recentlyAdded() )
            _out << ", recently added) {";
        else
            _out << ") {";
        if( originalConditions().empty() )
            _out << "no original condition}";
        else
        {
            for( auto oCond = originalConditions().begin(); oCond != originalConditions().end(); ++oCond )
            {
                if( oCond != originalConditions().begin() )
                    _out << ", ";
                _out << "[" << (*oCond)->getId() << "]";
            }
            _out << " }";
        }
    }
}    // end namspace vs
