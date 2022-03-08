
#ifndef _DECLARATIONS_
#define _DECLARATIONS_

#define ACTIVATE HIGH
#define DEACTIVATE LOW
#define ACTIVATED HIGH


void replenish335KvaGen01();
void abort335kvaGen01Replenishment();
void beginProcessToReplenish335KvaGen01();

Task StartProcessToReplenish335KvaGen01(1000, TASK_FOREVER, &beginProcessToReplenish335KvaGen01);
Task Replenish335KvaGen01(100, TASK_FOREVER, &replenish335KvaGen01);
Task Stop335kvaGen01ReplenishmentProcess(1000, TASK_FOREVER, &abort335kvaGen01Replenishment);



enum GeneratorFuelTankRefillState {INIT_STATE = 0,
                                   FUEL_REFILL_STARTED = 1,
                                   FUEL_REFILL_IN_PROGRESS = 2, 
                                   FUEL_REFILL_STOPPED = 3,
                                   FUEL_REFILL_COMPLETED = 4,
                                   FUEL_TANK_FULL = 5,
                                   FUEL_REFILL_NOT_IN_PROGRESS = 6
                                  } gen01FuelTankState, gen02FuelTankState, gen03FuelTankState, dayTankState;

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
    int outgoingFlowVolume;
    int outgoingFuelPublishCount;
    int incomingFuelPublishCount;
    int deactivateSignalCount;
    int activateSignalCount;
    int aSignalCount;
    int dSignalCount;
    int maxCount = 2;
}ps, generator01335kva, generator335kvaControl;


void gen01335pulseCounter()
{
  generator01335kva.pulseCount++;
}

#endif //_DECLARATIONS_