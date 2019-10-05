void GetTask(void *pvPramaters)
{
  while (1)
  {
    char* strCO = Arancino.get("CO");
    Serial.print("CO percentage = ");
    Serial.print(strCO);
    Serial.println("%");
    Arancino.free(strCO);
  
    char** str = Arancino.hgetall("accel");
    int arraySize = Arancino.getArraySize(str);
    for (int i = 0; i < arraySize; i+=2)
    {
      Serial.print("accel ");
      Serial.print(str[i]);
      Serial.print(" = ");
      Serial.println(str[i + 1]);
      Serial.print("\t\t");
    }
    Serial.println("");

    Arancino.free(str);
  }
}
