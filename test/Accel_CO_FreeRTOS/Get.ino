void GetTask(void *pvPramaters)
{
  while (1)
  {
    char* strCO = Arancino.get("CO");
    SERIAL_DEBUG.print("CO percentage = ");
    SERIAL_DEBUG.print(strCO);
    SERIAL_DEBUG.println("%");
    Arancino.free(strCO);

    char** str = Arancino.hgetall("accel");
    int arraySize = Arancino.getArraySize(str);
    for (int i = 0; i < arraySize; i+=2)
    {
      SERIAL_DEBUG.print("accel ");
      SERIAL_DEBUG.print(str[i]);
      SERIAL_DEBUG.print(" = ");
      SERIAL_DEBUG.println(str[i + 1]);
      SERIAL_DEBUG.print("\t\t");
    }
    SERIAL_DEBUG.println("");

    Arancino.free(str);
  }
}
