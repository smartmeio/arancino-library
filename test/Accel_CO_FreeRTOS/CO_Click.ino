#define pin A0
void COTask(void *pvPramaters)
{
  while (1)
  {
    double value = analogRead(pin) * 100 / 1023.0;
    Arancino.set("CO", value);
    vTaskDelay(100);
  }
}
