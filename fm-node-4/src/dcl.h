
#ifndef _DCL_
#define _DCL_

#define ACTIVATE HIGH
#define DEACTIVATE LOW
#define ACTIVATED HIGH

void refillGen01Tank();
void stopGen01Replenishment();
void beginProcessToReplenishGen01FuelTank();

Task StartProcessToReplenishGen01(100, TASK_FOREVER, &beginProcessToReplenishGen01FuelTank);
Task ReplenishGenerator01(1000, TASK_FOREVER, &refillGen01Tank);
Task DactivateGen01Replenishment(1000, TASK_FOREVER, &stopGen01Replenishment);


enum GeneratorFuelTankRefillState {INIT_STATE = 0,
                                   FUEL_REFILL_STARTED = 1,
                                   FUEL_REFILL_IN_PROGRESS = 2, 
                                   FUEL_REFILL_STOPPED = 3,
                                   FUEL_REFILL_COMPLETED = 4,
                                   FUEL_TANK_FULL = 5,
                                   FUEL_REFILL_NOT_IN_PROGRESS = 6
                                  } gen01FuelTankState;

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
}generator01FuelTankControl, ps;


void gen01pulseCounter()
{
  generator01FuelTankControl.pulseCount++;
}


void resetGen01ReplenishmentParameter() {
  generator01FuelTankControl.currentMillis = 0;
  generator01FuelTankControl.previousMillis = 0;
  generator01FuelTankControl.flowInterval = 1000;
  generator01FuelTankControl.calibrationFactor = 4.5;
  generator01FuelTankControl.pulse1Sec = 0;
  generator01FuelTankControl.pulseCount = 0;
  generator01FuelTankControl.flowRate = 0.0;
  generator01FuelTankControl.flowMilliLitres = 0;
  generator01FuelTankControl.totalMilliLitres = 0;
  generator01FuelTankControl.previousMillis = 0;
}

#endif //_DCL_