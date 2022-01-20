## Getting Started
To get started with Arancino Library you can download the latest version from the repository `arancino-library` within the [smartme.IO Repository Management Site](https://download.smartme.io/artifactory/list/arancino-library/) or directly from Arduino IDE or Visual Studio with Platform.IO.

### Arduino IDE

#### from a zip file
Open the Arduino IDE, unzip the _Arancino Library_ and import the unzipped folder (*Sketch* → *Include Library* → *Add .zip Library...*). The library will appear under *Sketch* → *Include Library* menu at the end, under the *Contributed* section. Examples will be under *File* → *Examples* → *Examples of Custom Libraries*.

#### from Arduino Library Manager
Open the Arduino IDE and go to *Sketch* → *Include Library* → *Manage Libraries*, the Arduino Library Manager window will be shown. Using the text box, type _Arancino_; finally selecte the Arancino Library item within the result list and click install. Be sure to select the latest version available.x

### Platform.IO

#### from a zip file
Open your PIO project and unzip the _Arancino Library_ in the lib directory.

#### from PIO Library Manager
Open Visual Studio Code and go to *PIO Home* → *Libraries*. Search for _Arancino_, choose the version and click on *ADD to Project* button.

#### from platformio.ini file
Add the _lib_deps_ option and specify one of the following item:
- the name of the library with semantic version
```cfg
lib_deps = 
            smartme-io/Arancino@^1.4.2  
```
- the library github url followed by *#* to specify the _tag_ or the _branch_ of the repository
```cfg
lib_deps = 
            https://github.com/smartmeio/arancino-library.git#1.4.2 
```

## Testing the Library
It is possible to run unit tests for the Arancino Library. In order to do so you need to:

1. Output is written to the [Debug](DEBUG.md) Serial (`uc UART 1`) and it is possible to read it by serial monitor using 115200 baudrate.
2. Flash `test/unit.ino` sketch on the microcontroller.
3. Run `test/unit.sh` on the host where Arancino Daemon is installed. *

NB: if no output on the serial monitor press `uc reset` button and the output should be refreshed 

*: It is possible to modify connection variables relative to redis databases inside `unit.sh` file.
