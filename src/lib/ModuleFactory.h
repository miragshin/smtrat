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
 * @file ModuleFactory.h
 *
 * @author Ulrich Loup
 * @author Sebastian Junges
 * @since: 2012-01-18
 * @version: 2012-02-04
 */

#include "Module.h"

#ifndef SMTRAT_MODULEFACTORY_H
#define SMTRAT_MODULEFACTORY_H

namespace smtrat
{
    class Manager;
    class Module;

    /**
     * An abstract base class for Module factories
     * @author Ulrich Loup
     * @author Sebastian Junges
     * @since: 2012-01-18
     * @version: 2012-02-10
     */
    class ModuleFactory
    {
        protected:
            ModuleType mModuleType;

        public:
            ModuleFactory():
                mModuleType( MT_NoModule )
            {}
            ~ModuleFactory(){}

            virtual Module* create( Manager* const _tsManager, const Formula* const _formula ) = 0;

            const ModuleType type() const
            {
                return this->mModuleType;
            }
    };

}    // namespace smtrat
#endif   /** SMTRAT_MODULEFACTORY_H */