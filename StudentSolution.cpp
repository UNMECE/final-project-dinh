#include "acequia_manager.h"
#include <iostream>
#include <algorithm> 


//Note: Please ensure the RandomValues.dat file is generated before passing the 'run' command to avoid a stoi error. Thanks :)

void solveProblems(AcequiaManager& manager){
    auto canals = manager.getCanals();

    while(!manager.isSolved && manager.hour != manager.SimulationMax){
        for (auto canal : canals){
            double srcLevel = canal->sourceRegion->waterLevel;
            double srcNeed  = canal->sourceRegion->waterNeed;
            double srcCap   = canal->sourceRegion->waterCapacity;
            
            double dstLevel = canal->destinationRegion->waterLevel;
            double dstNeed  = canal->destinationRegion->waterNeed;
            double dstCap   = canal->destinationRegion->waterCapacity;

            double srcSurplus = srcLevel - srcNeed;
            double dstSurplus = dstLevel - dstNeed;

            if(srcSurplus > dstSurplus + 0.05){
                double rate = (srcSurplus - dstSurplus) * 1.5; 
                if(rate > 1.0) { rate = 1.0; } 


                //source drought protection (dont let it drop below 20%)
                double droughtThreshold = 0.2 * srcCap;
                double maxSafeDrop = srcLevel - droughtThreshold - 0.1; // -0.1 is tiny safety cushion
                
                if(maxSafeDrop <= 0){
                    rate = 0.0;
                }else {
                    rate = std::min(rate, (maxSafeDrop / 2.0) / 0.36);
                }

                //destination flood protection (dont let it hit 100%)
                double maxSafeIncrease = dstCap - dstLevel - 0.1;
                
                if(maxSafeIncrease <= 0){
                    rate = 0.0;
                }else {
                    rate = std::min(rate, (maxSafeIncrease / 2.0) / 0.36);
                }

                //apply the final flow rate
                if(rate > 0){
                    canal->toggleOpen(true);
                    canal->setFlowRate(rate);
                }else {
                    canal->toggleOpen(false);
                    canal->setFlowRate(0.0);
                }
            } 
            else {
                //not enough water to share =>closed
                canal->toggleOpen(false);
                canal->setFlowRate(0.0);
            }
        }
        
        //move simulation forward 1hr to apply the flow rates
        manager.nexthour();
    }
}