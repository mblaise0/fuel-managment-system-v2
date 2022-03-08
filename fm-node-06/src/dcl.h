
#ifndef _DCL_
#define _DCL_

#define ACTIVATE HIGH
#define DEACTIVATE LOW
#define ACTIVATED HIGH

void refillGen03Tank();
void stopGen03Replenishment();
void beginProcessToReplenishGen03FuelTank();

Task StartProcessToReplenishGen03(100, TASK_FOREVER, &beginProcessToReplenishGen03FuelTank);
Task ReplenishGenerator03(1000, TASK_FOREVER, &refillGen03Tank);
Task DactivateGen03Replenishment(1000, TASK_FOREVER, &stopGen03Replenishment);


enum GeneratorFuelTankRefillState {INIT_STATE = 0,
                                   FUEL_REFILL_STARTED = 1,
                                   FUEL_REFILL_IN_PROGRESS = 2, 
                                   FUEL_REFILL_STOPPED = 3,
                                   FUEL_REFILL_COMPLETED = 4,
                                   FUEL_TANK_FULL = 5,
                                   FUEL_REFILL_NOT_IN_PROGRESS = 6
                                  } gen03FuelTankState;

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
}generator03FuelTankControl, ps;


void gen03pulseCounter()
{
  generator03FuelTankControl.pulseCount++;
}


void resetGen03ReplenishmentParameter() {
  generator03FuelTankControl.currentMillis = 0;
  generator03FuelTankControl.previousMillis = 0;
  generator03FuelTankControl.flowInterval = 1000;
  generator03FuelTankControl.calibrationFactor = 4.5;
  generator03FuelTankControl.pulse1Sec = 0;
  generator03FuelTankControl.pulseCount = 0;
  generator03FuelTankControl.flowRate = 0.0;
  generator03FuelTankControl.flowMilliLitres = 0;
  generator03FuelTankControl.totalMilliLitres = 0;
  generator03FuelTankControl.previousMillis = 0;
}

#endif //_DCL_