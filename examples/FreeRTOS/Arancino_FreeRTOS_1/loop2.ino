void loop2(void *pvPramaters)
{
  while (1)
  {
    Arancino.set("exrtos_foo1", "baz");

    //gets the value from the 'foo1' key
    char* value = Arancino.get("exrtos_foo1");
    Serial.print("exrtos_foo1 -> ");
    Serial.println(value);
    //foo1 -> baz
    Arancino.free(value);

    vTaskDelay(2000); //wait 2 seconds (non-blocking delay)
  }
}
