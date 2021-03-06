#include <Arancino.h>
#define LED_PIN 13
#define DELAY 0
TaskHandle_t blinkHandle;
TaskHandle_t task0Handle, task1Handle, task2Handle;


void blink(void *pvPramaters)
{
  while (1)
  {
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(1000);
    digitalWrite(LED_PIN, LOW);
    vTaskDelay(1000);
  }
}


void task0(void *pvPramaters)
{
  long errorCount = 0;
  long rqstCount = 0;
  String str;

  while (1)
  {
    long _sec = millis() / 1000;
    long sec = _sec % 60;
    long minut = (_sec / 60) % 60;
    long ore = ((_sec / 60) / 60);
    Arancino.hset("foo", "baz", "whoo");
    ++rqstCount;

    char* _str = Arancino.get<ArancinoPacket>("asd").response.string;
    String str;

    if (strcmp(_str, "qwe") == 0)
    {
      str = "asd OK\t\terrCount: " + (String)errorCount + "\t\t- rqst: " + (String)++rqstCount + "\t\t- time: " + ore + ":" + minut + ":" + sec + "\t\trate (rqst/sec): " + rqstCount / _sec + "\t\theap: " + (String)xPortGetFreeHeapSize();
    }
    else
    {
      str = "ERRORE asd - heap: " + (String)xPortGetFreeHeapSize() + " error: " + (String)++errorCount +  "\trqst: " + (String)rqstCount + "\t\t- time: " + ore + ":" + minut + ":" + sec;
    }
    vTaskSuspendAll();
    Serial.println(str);
    xTaskResumeAll();

    Arancino.free(_str);

    vTaskDelay(DELAY);
  }
}


void task1(void *pvPramaters)
{
  long errorCount = 0;
  long rqstCount = 0;
  String str;

  while (1)
  {
    long _sec = millis() / 1000;
    long sec = _sec % 60;
    long minut = (_sec / 60) % 60;
    long ore = ((_sec / 60) / 60);
    Arancino.set("pip", "pop");
    ++rqstCount;

    char** values = Arancino.hgetall("foo");
    char* response = values[0];

    String str;

    /*for (int i = 0; i < Arancino.getArraySize(values); i += 1) {
      Serial.print(i);
      Serial.print(" ");
      Serial.print("len: ");
      Serial.print(strlen(values[i]));
      Serial.print(" ");
      for (int k = 0; k < strlen(values[i]) + 1; k++)
      {
        if (values[i][k] < 32)
        {
          Serial.print("|");
          Serial.print(values[i][k], DEC);
          Serial.print("|");
        }
        else
          Serial.print(values[i][k]);
      }
      Serial.println("");
      }*/

    if ((strcmp(values[0], "bar") == 0 && strcmp(values[1], "yeah") == 0 && strcmp(values[2], "baz") == 0 && strcmp(values[3], "whoo") == 0) ||
        (strcmp(values[0], "baz") == 0 && strcmp(values[1], "whoo") == 0 && strcmp(values[2], "bar") == 0 && strcmp(values[3], "yeah") == 0))
    {
      str = "HGET OK\t\terrCount: " + (String)errorCount + "\t\t- rqst: " + (String)++rqstCount + "\t\t- time: " + ore + ":" + minut + ":" + sec + "\t\trate (rqst/sec): " + rqstCount / _sec + "\t\theap: " + (String)xPortGetFreeHeapSize();
    }
    else
    {
      str = "ERRORE HGET - heap: " + (String)xPortGetFreeHeapSize() + " error: " + (String)++errorCount +  "\trqst: " + (String)rqstCount + "\t\t- time: " + ore + ":" + minut + ":" + sec;
    }
    vTaskSuspendAll();
    Serial.println(str);
    xTaskResumeAll();
    Arancino.free(values);


    vTaskDelay(DELAY);
  }
}

void task2(void *pvPramaters)
{
  long errorCount = 0;
  long rqstCount = 0;
  String str;

  while (1)
  {
    long _sec = millis() / 1000;
    long sec = _sec % 60;
    long minut = (_sec / 60) % 60;
    long ore = ((_sec / 60) / 60);
    String str;
    Arancino.set("asd", "qwe");
    ++rqstCount;
    ArancinoPacket temp = Arancino.get<ArancinoPacket>("pip");
    char* _str = temp.response.string;
    char* response = _str;


    /*Serial.print("received get: ");
      for (int i = 0; i < strlen(response); i++)
      {
      if (response[i] < 32)
      {
        Serial.print("|");
        Serial.print(response[i], DEC);
        Serial.print("|");
      }
      else
        Serial.print(response[i]);
      }
      Serial.println("");*/
    if (strcmp(_str, "pop") == 0)
    {
      str = "pip OK\t\terrCount: " + (String)errorCount + "\t\t- rqst: " + (String)++rqstCount + "\t\t- time: " + ore + ":" + minut + ":" + sec + "\t\trate (rqst/sec): " + rqstCount / _sec + "\t\theap: " + (String)xPortGetFreeHeapSize();
    }
    else
    {
      str = "ERRORE pip\t\theap: " + (String)xPortGetFreeHeapSize() + "\t\terror: " + (String)++errorCount + "\trqst: " + (String)rqstCount + "\t\t- time: " + ore + ":" + minut + ":" + sec;
    }
    vTaskSuspendAll();
    Serial.println(str);
    xTaskResumeAll();
    Arancino.free(temp);

    vTaskDelay(DELAY);
  }
}

void setup() {

  Arancino.begin();
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println(sizeof(char));
  delay(1000);
  //Arancino.flush();
  Arancino.hset("foo", "bar", "yeah");
  Arancino.hset("foo", "baz", "whoo");
  Arancino.set("asd", "qwe");
  Arancino.set("pip", "pop");

  //xTaskCreate(blink,     "blink",       128, NULL, tskIDLE_PRIORITY + 1, &blinkHandle);

  xTaskCreate(task0,     "Task 0",       128, NULL, tskIDLE_PRIORITY + 1, &task0Handle);
  xTaskCreate(task1,     "Task 1",       256, NULL, tskIDLE_PRIORITY + 1, &task1Handle);
  xTaskCreate(task2,     "Task 2",       128, NULL, tskIDLE_PRIORITY + 1, &task2Handle);
  Arancino.startScheduler();
}

void loop() {
  //char* _str = Arancino.get("pip");
  //delay(300);
  //free(_str);
  //delay(300);
  String str = "free memory: " + (String)xPortGetFreeHeapSize();
  Serial.println(str);
  delay(1000);
}
