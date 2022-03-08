
#ifndef _DECLARATIONS_
#define _DECLARATIONS_

#define ACTIVATE HIGH
#define DEACTIVATE LOW
#define ACTIVATED HIGH

#define MAXIMUM_FUEL_LEVEL 92
#define MINIMUM_FUEL_LEVEL 50

void ReportFlowDetected();
void computeFuelInputToDayTank();
void computeFuelOutputFromDayTank();
void measureFuelFlowFromPump01();
void measureFuelFlowFromPump02();
void beginFlowComputationToDayTank(); 
void abortFlowComputationToDayTank();

Task DetectFuelFlowFromDayTank(100, TASK_FOREVER, &ReportFlowDetected);
Task ProcessInputToDayTank(100, TASK_FOREVER, &computeFuelInputToDayTank);
Task ProcessOutputFromDayTank(100, TASK_FOREVER, &computeFuelOutputFromDayTank);
Task ProcessPump01FlowRate(100, TASK_FOREVER, &measureFuelFlowFromPump01);
Task ProcessPump02FlowRate(100, TASK_FOREVER, &measureFuelFlowFromPump02);
Task StartInputFlowComputationToDayTank(100, TASK_FOREVER, &beginFlowComputationToDayTank);
Task StopInputFlowComputationToDayTank(100, TASK_FOREVER, &abortFlowComputationToDayTank);


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
}dayTankFuelTank, inputFlowMeterControl, outputFlowMeterControl, pump01FlowMeterControl, pump02FlowMeterControl, ps;

void inputFlowMeterPulseCounter()
{
  inputFlowMeterControl.pulseCount++;
}

void outputFlowMeterPulseCounter()
{
  outputFlowMeterControl.pulseCount++;
}

void pump01FlowMeterPulseCounter()
{
  pump01FlowMeterControl.pulseCount++;
}

void pump02FlowMeterPulseCounter()
{
  pump02FlowMeterControl.pulseCount++;
}


void resetInputParameters() {
  inputFlowMeterControl.currentMillis = 0;
  inputFlowMeterControl.previousMillis = 0;
  inputFlowMeterControl.flowInterval = 1000;
  inputFlowMeterControl.calibrationFactor = 4.5;
  inputFlowMeterControl.pulse1Sec = 0;
  inputFlowMeterControl.pulseCount = 0;
  inputFlowMeterControl.flowRate = 0.0;
  inputFlowMeterControl.flowMilliLitres = 0;
  inputFlowMeterControl.totalMilliLitres = 0;
  inputFlowMeterControl.previousMillis = 0;

  pump01FlowMeterControl.currentMillis = 0;
  pump01FlowMeterControl.previousMillis = 0;
  pump01FlowMeterControl.flowInterval = 1000;
  pump01FlowMeterControl.calibrationFactor = 4.5;
  pump01FlowMeterControl.pulse1Sec = 0;
  pump01FlowMeterControl.pulseCount = 0;
  pump01FlowMeterControl.flowRate = 0.0;
  pump01FlowMeterControl.flowMilliLitres = 0;
  pump01FlowMeterControl.totalMilliLitres = 0;
  pump01FlowMeterControl.previousMillis = 0;

  pump02FlowMeterControl.currentMillis = 0;
  pump02FlowMeterControl.previousMillis = 0;
  pump02FlowMeterControl.flowInterval = 1000;
  pump02FlowMeterControl.calibrationFactor = 4.5;
  pump02FlowMeterControl.pulse1Sec = 0;
  pump02FlowMeterControl.pulseCount = 0;
  pump02FlowMeterControl.flowRate = 0.0;
  pump02FlowMeterControl.flowMilliLitres = 0;
  pump02FlowMeterControl.totalMilliLitres = 0;
  pump02FlowMeterControl.previousMillis = 0;
}


void resetOutputParameters() {
  outputFlowMeterControl.currentMillis = 0;
  outputFlowMeterControl.previousMillis = 0;
  outputFlowMeterControl.flowInterval = 1000;
  outputFlowMeterControl.calibrationFactor = 4.5;
  outputFlowMeterControl.pulse1Sec = 0;
  outputFlowMeterControl.pulseCount = 0;
  outputFlowMeterControl.flowRate = 0.0;
  outputFlowMeterControl.flowMilliLitres = 0;
  outputFlowMeterControl.totalMilliLitres = 0;
  outputFlowMeterControl.previousMillis = 0;
}

#endif //_DECLARATIONS_