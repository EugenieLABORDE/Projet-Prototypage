#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Float64.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <VL53L0X.h>

#define SERVICE_UUID        "3ab0e826-2a67-11eb-adc1-0242ac120002"
#define CHARACTERISTIC_UUID "45d9bdfe-2a67-11eb-adc1-0242ac120002"

#define SERVICE_UUID2        "feeaff5e-2820-11eb-adc1-0242ac120002"
#define CHARACTERISTIC_UUID2 "0f97be82-2821-11eb-adc1-0242ac120002"

#define P_SHARP 13

ros::NodeHandle  nh;

std_msgs::Float64 servoposition;
ros::Publisher chatter("/pan_controller/command", &servoposition);

float pos=1.57; // pi/2

VL53L0X sensor;
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;


class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      // Feed button pressed
      pos=-pos;
      servoposition.data = pos;
      chatter.publish( &servoposition );
      nh.spinOnce();
    }
};


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};



void setup()
{
  Serial.begin(9600);

  // Capteur distance
  pinMode(P_SHARP,INPUT_PULLUP);
  digitalWrite(P_SHARP,HIGH);
  
  Wire.begin();

  sensor.init();
  sensor.setTimeout(5000);

  // Start continuous back-to-back mode (take readings as
  // fast as possible).  To use continuous timed mode
  // instead, provide a desired inter-measurement period in
  // ms (e.g. sensor.startContinuous(100)).
  sensor.startContinuous();


  // Create the BLE Device
  BLEDevice::init("esp32_chat");
  
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Services
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLEService *pService2 = pServer->createService(SERVICE_UUID2);

  // Create a BLE Characteristics
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  BLECharacteristic *pCharacteristic = pService2->createCharacteristic(
                                         CHARACTERISTIC_UUID2,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();
  pService2->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  pAdvertising->start();

  nh.initNode();
  nh.advertise(chatter);
}

void loop()
{
    // notify changed value
    if (deviceConnected) { //deviceConnected
        int distance = sensor.readRangeContinuousMillimeters();//170-0
        //Serial.println(distance);
        float pourcentage;
        if (distance<=30)
        {
          pourcentage=100;
        }
        if (distance>=150)
        {
          pourcentage=0;
        }
        if (distance>30 && distance<150)
        {
          pourcentage=100-100*(distance-30)/120;
        }
        String distance_string = (String) pourcentage;
        distance_string=" Qt= "+distance_string+"%";
    
        int sensorValue = digitalRead(P_SHARP);
        int n=0;
        String ret = "";
        if (sensorValue==1)
        {
          ret = "empty|";
        }
          else
        {
          ret = "full |";
        }
        String sensor_values=ret+distance_string;
        pCharacteristic->setValue(sensor_values.c_str()); // Pone el numero aleatorio
        pCharacteristic->notify();
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
