/**
 * Class to create a settings object for the VSModule.
 * @author Florian Corzilius
 * @since 2013-07-02
 * @version 2014-09-18
 */

#pragma once

#include "../../config.h"
    
namespace smtrat
{   
    struct VSSettings1
    {
#ifdef __VS
		static const std::string getModuleName() { return "VSModule<VSSettings1>"; }
#else
		static constexpr auto moduleName = "VSModule<VSSettings1>";
#endif
        static const bool elimination_with_factorization                        = false;
        static const bool local_conflict_search                                 = false;
        static const bool use_strict_inequalities_for_test_candidate_generation = true;
        #ifdef SMTRAT_VS_VARIABLEBOUNDS
        static const bool use_variable_bounds                                   = true;
        #else
        static const bool use_variable_bounds                                   = false;
        #endif
        static const bool sturm_sequence_for_root_check                         = use_variable_bounds && false;
        static const bool check_conflict_for_side_conditions                    = false;
        static const bool incremental_solving                                   = true;
        static const bool infeasible_subset_generation                          = true;
        static const bool virtual_substitution_according_paper                  = false;
        static const bool prefer_equation_over_all                              = false;
        static const bool mixed_int_real_constraints_allowed                    = false;
        static const bool split_neq_constraints                                 = false;
        static const size_t int_max_range                                       = 1;
        static const size_t lazy_check_threshold                                = 1;
        static const bool try_first_lazy                                        = false;
    };
    
    struct VSSettings2 : VSSettings1
    {
        static const bool elimination_with_factorization                        = true;
    };
    
    struct VSSettings23 : VSSettings2
    {
        static const bool local_conflict_search                                 = true;
    };
    
    struct VSSettings234 : VSSettings23
    {
        static const bool check_conflict_for_side_conditions                    = true;
        static const bool prefer_equation_over_all                              = true;
    };
    
    struct VSSettings2345 : VSSettings234
    {
        static const bool sturm_sequence_for_root_check                         = use_variable_bounds && true;
    };
    
    struct VSSettings235 : VSSettings23
    {
        static const bool sturm_sequence_for_root_check                         = use_variable_bounds && true;
    };
    
    struct VSSettings24 : VSSettings2
    {
        static const bool check_conflict_for_side_conditions                    = true;
        static const bool prefer_equation_over_all                              = true;
    };
    
    struct VSSettings245 : VSSettings24
    {
        static const bool sturm_sequence_for_root_check                         = use_variable_bounds && true;
    };
    
    struct VSSettings25 : VSSettings2
    {
        static const bool sturm_sequence_for_root_check                         = use_variable_bounds && true;
    };
    
    struct VSSettings3 : VSSettings1
    {
        static const bool local_conflict_search                                 = true;
    };
    
    struct VSSettings34 : VSSettings3
    {
        static const bool check_conflict_for_side_conditions                    = true;
        static const bool prefer_equation_over_all                              = true;
    };
    
    struct VSSettings345 : VSSettings34
    {
        static const bool sturm_sequence_for_root_check                         = use_variable_bounds && true;
    };
    
    struct VSSettings35 : VSSettings3
    {
        static const bool sturm_sequence_for_root_check                         = use_variable_bounds && true;
    };
    
    struct VSSettings4 : VSSettings1
    {
        static const bool check_conflict_for_side_conditions                    = true;
        static const bool prefer_equation_over_all                              = true;
    };
    
    struct VSSettings45 : VSSettings4
    {
        static const bool sturm_sequence_for_root_check                         = use_variable_bounds && true;
    };
    
    struct VSSettings5 : VSSettings1
    {
        static const bool sturm_sequence_for_root_check                         = use_variable_bounds && true;
    };
}
