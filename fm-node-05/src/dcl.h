
#ifndef _DCL_
#define _DCL_

#define ACTIVATE HIGH
#define DEACTIVATE LOW
#define ACTIVATED HIGH

void refillGen02Tank();
void stopGen02Replenishment();
void beginProcessToReplenishGen02FuelTank();

Task StartProcessToReplenishGen02(100, TASK_FOREVER, &beginProcessToReplenishGen02FuelTank);
Task ReplenishGenerator02(1000, TASK_FOREVER, &refillGen02Tank);
Task DactivateGen02Replenishment(1000, TASK_FOREVER, &stopGen02Replenishment);


enum GeneratorFuelTankRefillState {INIT_STATE = 0,
                                   FUEL_REFILL_STARTED = 1,
                                   FUEL_REFILL_IN_PROGRESS = 2, 
                                   FUEL_REFILL_STOPPED = 3,
                                   FUEL_REFILL_COMPLETED = 4,
                                   FUEL_TANK_FULL = 5,
                                   FUEL_REFILL_NOT_IN_PROGRESS = 6
                                  } gen02FuelTankState;

struct FuelManagementControl{
    long currentMillis;
    long previousMillis; 
    int flowInterval;
    float calibrationFactor;
    volatile byte pulseCount;
    byte pulse1Sec;
    float flowRate;
    unsigned int flowMilliLitres;
    unsigned long totalMilliLitres;
    int flowSwitchState;
    int previousFlowSwitchState;
    float flowTemp;
    volatile unsigned long start_time_micros;
    volatile unsigned long delayTime;
    int flowVolume;
    int incomingFuelPublishCount;
    int aSignalCount;
    int dSignalCount;
}generator02FuelTankControl, ps;


void gen02pulseCounter()
{
  generator02FuelTankControl.pulseCount++;
}


void resetGen02ReplenishmentParameter() {
  generator02FuelTankControl.currentMillis = 0;
  generator02FuelTankControl.previousMillis = 0;
  generator02FuelTankControl.flowInterval = 1000;
  generator02FuelTankControl.calibrationFactor = 4.5;
  generator02FuelTankControl.pulse1Sec = 0;
  generator02FuelTankControl.pulseCount = 0;
  generator02FuelTankControl.flowRate = 0.0;
  generator02FuelTankControl.flowMilliLitres = 0;
  generator02FuelTankControl.totalMilliLitres = 0;
  generator02FuelTankControl.previousMillis = 0;
}

#endif //_DCL_