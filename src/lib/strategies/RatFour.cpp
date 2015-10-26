/**
 * @file RatFour.cpp
 *
 */
#include "RatFour.h"
#include "../modules/Modules.h"

namespace smtrat
{

    RatFour::RatFour( bool _externalModuleFactoryAdding ):
        Manager( _externalModuleFactoryAdding )
    {
        //addBackendIntoStrategyGraph( 0, MT_CNFerModule, isCondition );
        //addBackendIntoStrategyGraph( 1, MT_PreprocessingModule, isCondition );
        //addBackendIntoStrategyGraph( 2, MT_SATModule, isCondition );
        //addBackendIntoStrategyGraph( 2, MT_SATModule, isCondition );
        //addBackendIntoStrategyGraph( 4, MT_ICPModule, isCondition );
        //addBackendIntoStrategyGraph( 5, MT_VSModule, isCondition );
        //addBackendIntoStrategyGraph( 6, MT_CADModule, isCondition );
        //addBackendIntoStrategyGraph( 3, MT_LRAModule, isCondition );
        //addBackendIntoStrategyGraph( 8, MT_VSModule, isCondition );
        //addBackendIntoStrategyGraph( 9, MT_CADModule, isCondition );
		
		setStrategy({
			addBackend<PreprocessingModule<PreprocessingSettings1>>({
				addBackend<SATModule<SATSettings1>>(),
				addBackend<SATModule<SATSettings1>>({
					addBackend<ICPModule<ICPSettings1>>({
						addBackend<VSModule<VSSettings1>>({
							addBackend<CADModule<CADSettings1>>()
						}),
					}),
					addBackend<LRAModule<LRASettings1>>(
						addBackend<VSModule<VSSettings1>>({
							addBackend<CADModule<CADSettings1>>()
						})
					)
				})
			})
		});
    }

    RatFour::~RatFour(){}

}    // namespace smtrat
