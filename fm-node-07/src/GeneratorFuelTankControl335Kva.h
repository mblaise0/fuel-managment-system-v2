
#ifndef _GENERATOR_FUEL_TANK_CONTROL
#define _GENERATOR_FUEL_TANK_CONTROL

#define ACTIVATE HIGH
#define DEACTIVATE LOW
#define FLOW_DETECTED HIGH


class GeneratorFuelTankControl335KVA
{
    private:
        int FlowSwitchSensor;
        int SolenoidValveSensor;

    public:
        void activateSolenoidValve(int sensor) {
            digitalWrite(sensor, ACTIVATE);
        }

        void deactivateSolenoidValve(int sensor) {
            digitalWrite(sensor, DEACTIVATE);
        }

        void start335KvaGenerator01FuelHysteresis() {
            char data1[750];
            StaticJsonDocument<1024> doc;
            generator01335kva.incomingFuelPublishCount++;
            generator01335kva.currentMillis = millis();
            if (generator01335kva.currentMillis - generator01335kva.previousMillis > generator01335kva.flowInterval) {
                
                generator01335kva.pulse1Sec = generator01335kva.pulseCount;
                generator01335kva.pulseCount = 0;
                generator01335kva.flowRate = ((1000.0 / (millis() - generator01335kva.previousMillis)) * generator01335kva.pulse1Sec) / generator01335kva.calibrationFactor;
                generator01335kva.previousMillis = millis();
                generator01335kva.flowMilliLitres = (generator01335kva.flowRate / 60) * 1000;
                generator01335kva.totalMilliLitres += generator01335kva.flowMilliLitres;
                generator01335kva.outgoingFlowVolume = generator01335kva.totalMilliLitres / 1000;

                if (generator01335kva.incomingFuelPublishCount == 10) {
                String payload = "335G01"+(String)generator01335kva.outgoingFlowVolume;
                payload.toCharArray(data1, (payload.length() + 1));
                client.publish("FMPIN", data1);
                generator01335kva.incomingFuelPublishCount = 0;
                }
                
                Serial.print("Input To Gen01335Tank====>>> Flow rate: ");
                Serial.print(int(generator01335kva.flowRate));  
                Serial.print("L/min");
                Serial.print("\t");       
                Serial.print("Liquid Quantity: ");
                Serial.print(generator01335kva.totalMilliLitres);
                Serial.print("mL / ");
                Serial.print(generator01335kva.totalMilliLitres / 1000);
                Serial.println("L");
            }
        }

};


void beginProcessToReplenish335KvaGen01() {
    GeneratorFuelTankControl335KVA generatorFuelTankControl335KVA;
    generatorFuelTankControl335KVA.activateSolenoidValve(GEN_01_335KVA_VALVE);
    Replenish335KvaGen01.enable();
    StartProcessToReplenish335KvaGen01.disable();
}

void replenish335KvaGen01() {
    GeneratorFuelTankControl335KVA generatorFuelTankControl335KVA;
    generatorFuelTankControl335KVA.start335KvaGenerator01FuelHysteresis();
    //send signal to enable outputFromDayTankHysteresis
}

void abort335kvaGen01Replenishment() {
    GeneratorFuelTankControl335KVA generatorFuelTankControl335KVA;
    generatorFuelTankControl335KVA.deactivateSolenoidValve(GEN_01_335KVA_VALVE);
    //send signal to disable outputFromDayTankHysteresis
    Replenish335KvaGen01.disable();
    Stop335kvaGen01ReplenishmentProcess.disable();
}


char parseCommand(String Value, String inTopic) {
    if (Value == "E.335G01R") {
      generator01335kva.aSignalCount++;
      if (generator01335kva.aSignalCount == 2) {
        StartProcessToReplenish335KvaGen01.enable();
        generator01335kva.aSignalCount = 0;
      }
    }else if (Value == "D.335G01R") {
      generator01335kva.dSignalCount++;
      if (generator01335kva.dSignalCount == 2) {
        Stop335kvaGen01ReplenishmentProcess.enable();
        generator01335kva.dSignalCount = 0;
      }
    }
}

#endif   //_GENERATOR_FUEL_TANK_CONTROL
