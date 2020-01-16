#!/bin/bash

build_examples () {

  PREFIX="${HOME}/Arduino/libraries/Arancino"
  
  echo -e "\n"
  echo "############################################"
  echo -e "# Building plain Arancino Library examples #"
  echo -e "############################################\n"
  sleep 1

  for sketch in `find ${PREFIX}/examples/Arancino -name '*.ino'`
  do
    echo -e '\n\n' `basename $sketch`
    sleep 1
    arduino-cli compile -vvv --fqbn smartme.IO:samd:$1 $sketch
    
    local rc=$?
    error_check $rc `basename $sketch`
  done
  
  echo -e "\n"
  echo "######################################"
  echo -e "# Building FreeRTOS Library examples #"
  echo -e "######################################\n"
  sleep 1

  RTOS_INO=(
    ${PREFIX}/examples/FreeRTOS/Arancino_FreeRTOS_1/Arancino_FreeRTOS_1.ino
    ${PREFIX}/test/Accel_CO_FreeRTOS/Accel_CO_FreeRTOS.ino
    ${PREFIX}/test/Arancino_heavy_test/Arancino_heavy_test.ino
    )

  for sketch_rtos in "${RTOS_INO[@]}"; do
      echo -e '\n\n' `basename $sketch_rtos`
      sleep 1
      arduino-cli compile --fqbn smartme.IO:samd:$1 \
        -vvv $sketch_rtos \
        --build-properties build.memory_wrapping_flags='-Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=calloc -Wl,--wrap=realloc' \
        --build-properties build.arancino_extra_flags=-DUSEFREERTOS
      
      local rcrtos=$?
      error_check $rcrtos `basename $sketch_rtos`
  done
}

error_check () {
  
  if [ ! $1 -eq 0 ]; then 
    echo "Error building $2"
    exit $1
  fi
  
}

[ -z $1 ] && echo "Set board !" && exit 0

build_examples $1