#!/bin/bash

RED='\033[0;31m'
BLUE='\033[0;34m'
ORANGE='\033[0;33m'
NC='\033[0m' # No Color

build_examples () {

  PREFIX="${HOME}/Arduino/libraries/Arancino"
  
  echo -e "\n"
  echo -e "${RED}############################################"
  echo -e "${RED}# Building plain Arancino Library examples #"
  echo -e "${RED}############################################\n"
  sleep 1

  for sketch in `find ${PREFIX}/examples/Arancino -name '*.ino'`
  do
    echo -e "${BLUE}"
    echo -e '\n\n' `basename $sketch`
    sleep 1
    echo -e "${ORANGE}"
    arduino-cli compile -v --fqbn smartme.IO:samd:$1 $sketch
    
    local rc=$?
    error_check $rc `basename $sketch`
  done
  
  echo -e "\n"
  echo -e "${RED}######################################"
  echo -e "${RED}# Building FreeRTOS Library examples #"
  echo -e "${RED}######################################\n"
  sleep 1

  for sketch_rtos in `find ${PREFIX}/examples/Arancino_FreeRTOS -name '*.ino'`
  do
    echo -e "${BLUE}"
    echo -e '\n\n' `basename $sketch_rtos`
    sleep 1
    echo -e "${ORANGE}"
    arduino-cli compile -v --fqbn smartme.IO:samd:$1 $sketch_rtos \
      --build-property build.arancino_extra_flags=-DUSEFREERTOS

    local rcrtos=$?
    error_check $rcrtos `basename $sketch_rtos`
  done

  echo -e "${NC}"
}

error_check () {
  
  if [ ! $1 -eq 0 ]; then 
    echo "Error building $2"
    exit $1
  fi
  
}

[ -z $1 ] && echo "Set board !" && exit 0

build_examples $1