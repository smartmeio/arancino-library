#include <SparkFun_ADXL345.h>         // SparkFun ADXL345 Library
ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION - change I2C addr to 0x1D

void AccelTask(void *pvPramaters)
{
  adxl.powerOn();                     // Power on the ADXL345
  adxl.setRangeSetting(8);           // Give the range settings
  // Accepted values are 2g, 4g, 8g or 16g
  // Higher Values = Wider Measurement Range
  // Lower Values = Greater Sensitivity
  while (1)
  {
    int x, y, z;
    adxl.readAccel(&x, &y, &z);

    Arancino.hset("accel", "x", x);
    Arancino.hset("accel", "y", y);
    Arancino.hset("accel", "z", z);

    vTaskDelay(300);
  }
}
