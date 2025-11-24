module FprimeZephyrReference {

  @ Sensor hub component that reads temperature, pressure, gyro, and accelerometer
  active component SensorHub {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Schedules reading at 1Hz
    async input port schedIn: Svc.Sched

    @ Time get port
    time get port timeGetOut

    @ Event port
    event port eventOut

    @ Text event port
    text event port textEventOut

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ Sensor hub initialized
    event SensorHubInitialized \
      severity activity high \
      format "Sensor hub initialized successfully"

    @ Sensor reading taken
    event SensorReading(
      temp: F32 @< Temperature in Celsius
      pressure: F32 @< Pressure in hPa
      gyroX: F32 @< Gyro X (deg/s)
      gyroY: F32 @< Gyro Y (deg/s)
      gyroZ: F32 @< Gyro Z (deg/s)
      accelX: F32 @< Accel X (m/s²)
      accelY: F32 @< Accel Y (m/s²)
      accelZ: F32 @< Accel Z (m/s²)
    ) \
      severity activity low \
      format "Sensors: {f}°C, {f}hPa, Gyro[{f},{f},{f}], Accel[{f},{f},{f}]"

    @ Sensor read error
    event SensorReadError(
      sensor: string size 32 @< Which sensor failed
    ) \
      severity warning high \
      format "Sensor read error: {}"

  }

}
