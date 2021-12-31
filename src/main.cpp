#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "Lpf2Hub.h"
#include "SerialLogger.h"

#define HUBS 1
Lpf2Hub* myHubs[HUBS];
Lpf2Hub* myHub;
bool hubStatus[4] = {false, false, false, false};

Color hubColors[4] = {GREEN, RED, PURPLE, ORANGE};

TFT_eSPI tft = TFT_eSPI();

SerialLogger logger = SerialLogger(Trace);

// create a hub instance
byte portD = (byte)ControlPlusHubPort::D;

void keepAllHubsConnected( void * pvParameters ){
  logger.debug("keepAllHubsConnected started");

  for ( ; ; ) {

  logger.debug("Hi from connectiong loop");
    for (int hub=0; hub<HUBS; hub++) {

    vTaskDelay(1000/portTICK_PERIOD_MS);
    myHub = myHubs[hub];

    if (!myHub->isConnected() && !myHub->isConnecting()) 
      {
        myHub->init(); 
      }

      // connect flow. Search for BLE services and try to connect if the uuid of the hub is found
      if (myHub->isConnecting()) {
        myHub->connectHub();
        if (myHub->isConnected()) {
          logger.debug("Connected to HUB");
          hubStatus[hub] = true;
        } else {
          logger.debug("Failed to connect to HUB");
          hubStatus[hub] = false;
        }
        continue;
      }

      // if connected, you can set the name of the hub, the led color and shut it down
      if (myHub->isConnected()) {
        // Do some stuff here
        //char hubName[] = "Hubb Namee";
        //myHub->setHubName(hubName);
        myHub->setLedColor(hubColors[hub]);

      } else {
        logger.debug("ControlPlus hub is disconnected");
        hubStatus[hub] = false;
      }
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void processAllHubs( void * pvParameters ){

  logger.debug("processAllHubs started");

  for (;;) {
    for (int hub=0; hub<HUBS; hub++) {
      myHub = myHubs[hub];
      // if connected, you can set the name of the hub, the led color and shut it down
      if (myHub->isConnected()) {
     
        vTaskDelay(1000/portTICK_PERIOD_MS);
        myHub->setTachoMotorSpeed(portD, 35);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        myHub->stopTachoMotor(portD);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        myHub->setTachoMotorSpeed(portD, -35);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        myHub->stopTachoMotor(portD);
        vTaskDelay(1000/portTICK_PERIOD_MS);
      } else {
        logger.debug("ControlPlus hub is disconnected");
      }
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

void showStatusTask ( void * pvParameters ){
  for (;;) {
    tft.fillScreen(TFT_BLACK);  
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(0, 30);
    for (int hub=0; hub<HUBS; hub++) {

        tft.printf("Hub %d:", hub);
        if (hubStatus[hub]) {
          tft.println("Connected");

        } else {
          tft.println("Disconnected");
        }
    }
  vTaskDelay(500/portTICK_PERIOD_MS);
  }

}



void setup() {

  for (int x=0; x<HUBS; x++) {
    myHubs[x] = new Lpf2Hub;
    myHubs[x]->init();
  }

  tft.begin();
  tft.setRotation(1);


  BaseType_t xReturned;
  TaskHandle_t xHandle1 = NULL;
  TaskHandle_t xHandle2 = NULL;

   xTaskCreatePinnedToCore(
                    keepAllHubsConnected,           /* Function that implements the task. */
                    "Keep All Hubs Connected",      /* Text name for the task. */
                    8000,                           /* Stack size in words, not bytes. */
                    NULL,                   /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,               /* Priority at which the task is created. */
                    &xHandle1,
                    CONFIG_ARDUINO_RUNNING_CORE);        /* Used to pass out the created task's handle. */

  xReturned = xTaskCreatePinnedToCore(
                    processAllHubs,    /* Function that implements the task. */
                    "Send Moves to All Hubs",  /* Text name for the task. */
                    8000,              /* Stack size in words, not bytes. */
                    NULL,      /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,  /* Priority at which the task is created. */
                    &xHandle2, /* Used to pass out the created task's handle. */
                    CONFIG_ARDUINO_RUNNING_CORE);        

xReturned = xTaskCreate(
                    showStatusTask, 
                    "Show status", 
                    8000, 
                    NULL, 
                    tskIDLE_PRIORITY, 
                    NULL
                    );   
  //vTaskStartScheduler();
}


// main loop
void loop() {
    logger.debug("Hi from main thread");
    delay(1000);
 } // End of loop
