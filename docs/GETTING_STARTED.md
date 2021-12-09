## Getting Started
To get started with Arancino Library you can download the latest version from the repository `arancino-library` within the [smartme.IO Repository Management Site](https://download.smartme.io/artifactory/list/arancino-library/) or directly from Arduino IDE or Visual Studio with Platform.IO.

### Arduino IDE

#### from a zip file
Open the Arduino IDE, unzip the _Arancino Library_ and import the unzipped folder (*Sketch* → *Include Library* → *Add .zip Library...*). The library will appear under *Sketch* → *Include Library* menu at the end, under the *Contributed* section. Examples will be under *File* → *Examples* → *Examples of Custom Libraries*.

#### from Arduino Library Manager
Open the Arduino IDE and go to *Sketch* → *Include Library* → *Manage Libraries*, the Arduino Library Manager window will be shown. Using the text box, type _Arancino_; finally selecte the Arancino Library item within the result list and click install. Be sure to select the latest version available.x

*Important Note:*
> Arancino Library has one dependency when FreeRTOS capabilities are required: [FreeRTOS_SAMD21 Arduino library](https://github.com/BriscoeTech/Arduino-FreeRTOS-SAMD21) by BriscoeTech. Please download it from Arduino Library Manager. In that case you also have to select _Yes_ from _Menu -> Tools -> Using FreeRTOS?_.

### Platform.IO

TODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODO

## Testing the Library
It is possible to run unit tests for the Arancino Library. In order to do so you need to:

1. Flash `test/unit.ino` sketch on the microcontroller.
2. Run `test/unit.sh` on the host where Arancino Module is installed. *
3. Output is written to `Serial1`.

*: It is possible to modify connection variables relative to redis databases inside `unit.sh` file.
