#if defined (_VARIANT_ARANCINO_VOLANTE_)

#include <Adafruit_TinyUSB.h>
#include <Arancino.h>
#include <ArancinoBleSetup.h>
ArancinoMetadata amdata = {
  .fwname = "Bluetooth Example",
  .fwversion = "1.0.0",
  .tzoffset = "+1000"
};
TaskHandle_t loopTaskHandle;
void loopTask(void *pvParameters);


void setup() {
  ArancinoBleSetup();
  BluetoothIface iface;
  iface.setBLESerial(getBleSerialAddr());
  Arancino.attachInterface(&iface);
  Arancino.begin(amdata);  
  xTaskCreate(loopTask, "loopTask", 256, NULL, 1, &loopTaskHandle);
  Arancino.startScheduler();


}

void loopTask(void *pvParameters)
{
  while (1) {
    Arancino.set("Test_key", "my_value");
    vTaskDelay(1000); //wait 1 second
  }
}

void loop(){ 
}

#endif
