####
# SensorHub Component
# 
# Component for reading multiple sensors on Raspberry Pi Pico
# with Zephyr RTOS. Reads temperature, pressure, gyroscope,
# and accelerometer data.
####

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SensorHub.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/SensorHub.cpp"
)
