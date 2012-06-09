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
 * @file TSManager.h
 *
 * @author Florian Corzilius
 * @author Ulrich Loup
 * @author Sebastian Junges
 * @since 2012-01-18
 * @version 2012-06-08
 */

#ifndef SMTRAT_TSMANAGER_H
#define SMTRAT_TSMANAGER_H

#include <vector>

#include "Constraint.h"
#include "Answer.h"
#include "ModuleFactory.h"
#include "Strategy.h"
#include "ModuleType.h"
#include "Module.h"
#include "config.h"
#include "modules/StandardModuleFactory.h"

namespace smtrat
{
    /**
     * Base class for solvers. This is the interface to the user.
     */
    class Manager
    {
        private:

            /// the constraints so far passed to the primary backend
            Formula* mpPassedFormula;
            /// all generated instances of modules
            std::vector<Module*> mGeneratedModules;
            /// a mapping of each module to its backends
            std::map<const Module* const, std::vector<Module*> > mBackendsOfModules;
            /// the primary backends
            Module* mpPrimaryBackend;
            /// the backtrack points
            std::vector<unsigned> mBackTrackPoints;
            /// a Boolean showing whether the manager has received new constraint after the last consistency check
            bool mBackendsUptodate;
            /// modules we can use
            std::map<const ModuleType, ModuleFactory*>* mpModulFactories;
            /// primary strategy
            Strategy* mpStrategy;

        public:
            Manager( Formula* = new Formula( AND ) );
            virtual ~Manager();

            Answer isConsistent()
            {
                return mpPrimaryBackend->isConsistent();
            }

            void pushBacktrackPoint()
            {
                mBackTrackPoints.push_back( mpPassedFormula->size() );
                mpPrimaryBackend->pushBacktrackPoint();
            }

            const std::map<const ModuleType, ModuleFactory*>& rModulFactories() const
            {
                return *mpModulFactories;
            }

            void addModuleType( const ModuleType _moduleType, ModuleFactory* _factory )
            {
                mpModulFactories->insert( std::pair<const ModuleType, ModuleFactory*>( _moduleType, _factory ));
            }

            Strategy& strategy()
            {
                return *mpStrategy;
            }

            std::vector<Module*> getAllBackends( Module* _module )
            {
            	return mBackendsOfModules[_module];
            }

            unsigned uniqueModuleNumber( const Module* const _module )
            {
                unsigned result = 0;
                std::vector< Module* >::const_iterator moduleIter = mGeneratedModules.begin();
                while( moduleIter != mGeneratedModules.end() )
                {
                    if( *moduleIter == _module )
                    {
                        return result;
                    }
                    ++moduleIter;
                    ++result;
                }
                assert( false );
                return 0;
            }

            bool inform( const std::string&, bool );
            void popBacktrackPoint();
            bool addConstraint( const std::string&, const bool, const bool );
            std::vector<std::vector<unsigned> > getReasons() const;
            std::vector<Module*> getBackends( Formula*, Module* );
            std::vector<ModuleFactory*> getBackendsFactories( Formula* const , Module* ) const;
    };

}    // namespace smtrat

#endif   /** SMTRAT_TSMANAGER_H */
