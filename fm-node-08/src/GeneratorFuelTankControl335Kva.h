
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

        void start335KvaGenerator02FuelHysteresis() {
            char data1[750];
            StaticJsonDocument<1024> doc;
            generator02335kva.incomingFuelPublishCount++;
            generator02335kva.currentMillis = millis();
            if (generator02335kva.currentMillis - generator02335kva.previousMillis > generator02335kva.flowInterval) {
                
                generator02335kva.pulse1Sec = generator02335kva.pulseCount;
                generator02335kva.pulseCount = 0;
                generator02335kva.flowRate = ((1000.0 / (millis() - generator02335kva.previousMillis)) * generator02335kva.pulse1Sec) / generator02335kva.calibrationFactor;
                generator02335kva.previousMillis = millis();
                generator02335kva.flowMilliLitres = (generator02335kva.flowRate / 60) * 1000;
                generator02335kva.totalMilliLitres += generator02335kva.flowMilliLitres;
                generator02335kva.outgoingFlowVolume = generator02335kva.totalMilliLitres / 1000;

                if (generator02335kva.incomingFuelPublishCount == 10) {
                String payload = "335G02"+(String)generator02335kva.outgoingFlowVolume;
                payload.toCharArray(data1, (payload.length() + 1));
                client.publish("FMPIN", data1);
                generator02335kva.incomingFuelPublishCount = 0;
                }
                
                Serial.print("Input To Gen02335Tank====>>> Flow rate: ");
                Serial.print(int(generator02335kva.flowRate));  
                Serial.print("L/min");
                Serial.print("\t");       
                Serial.print("Liquid Quantity: ");
                Serial.print(generator02335kva.totalMilliLitres);
                Serial.print("mL / ");
                Serial.print(generator02335kva.totalMilliLitres / 1000);
                Serial.println("L");
            }
        }

};


void beginProcessToReplenish335KvaGen02() {
    GeneratorFuelTankControl335KVA generatorFuelTankControl335KVA;
    generatorFuelTankControl335KVA.activateSolenoidValve(GEN_02_335KVA_VALVE);
    Replenish335KvaGen02.enable();
    StartProcessToReplenish335KvaGen02.disable();
}

void replenish335KvaGen02() {
    GeneratorFuelTankControl335KVA generatorFuelTankControl335KVA;
    generatorFuelTankControl335KVA.start335KvaGenerator02FuelHysteresis();
    //send signal to enable outputFromDayTankHysteresis
}

void abort335kvaGen02Replenishment() {
    GeneratorFuelTankControl335KVA generatorFuelTankControl335KVA;
    generatorFuelTankControl335KVA.deactivateSolenoidValve(GEN_02_335KVA_VALVE);
    //send signal to disable outputFromDayTankHysteresis
    Replenish335KvaGen02.disable();
    Stop335kvaGen02ReplenishmentProcess.disable();
}


char parseCommand(String Value, String inTopic) {
     if (Value == "E.335G02R") {
      generator02335kva.aSignalCount++;
      if (generator02335kva.aSignalCount == 2) {
        StartProcessToReplenish335KvaGen02.enable();
        generator02335kva.aSignalCount = 0;
      }
    }else if (Value == "D.335G02R") {
      generator02335kva.dSignalCount++;
      if (generator02335kva.dSignalCount == 2) {
        Stop335kvaGen02ReplenishmentProcess.enable();
        generator02335kva.dSignalCount = 0;
      }
    }
}

#endif   //_GENERATOR_FUEL_TANK_CONTROL
