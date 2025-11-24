// ======================================================================
// \title  SensorHub.cpp
// \author Madison Wright
// \brief  cpp file for SensorHub component implementation class
//
// Sensor hub component for reading temperature, pressure, gyro, and
// accelerometer data on Raspberry Pi Pico with Zephyr RTOS.
// ======================================================================

#include <FprimeZephyrReference/Components/SensorHub/SensorHub.hpp>
#include <Fw/Types/Assert.hpp>

namespace FprimeZephyrReference {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  SensorHub::SensorHub(const char* const compName) :
      SensorHubComponentBase(compName),
      m_readCount(0),
      m_errorCount(0)
  {
  }

  SensorHub::~SensorHub()
  {
  }

  void SensorHub::init(
      FwSizeType queueDepth,
      FwSizeType instance
  )
  {
    SensorHubComponentBase::init(queueDepth, instance);
    
    // Log initialization
    this->log_ACTIVITY_HI_SensorHubInitialized();
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void SensorHub::schedIn_handler(
      FwIndexType portNum,
      U32 context
  )
  {
    F32 temp, pressure;
    F32 gyroX, gyroY, gyroZ;
    F32 accelX, accelY, accelZ;

    // Read all sensors
    if (this->readSensors(temp, pressure, gyroX, gyroY, gyroZ, accelX, accelY, accelZ)) {
      // Success - increment counter
      this->m_readCount++;

      // Log all sensor readings
      this->log_ACTIVITY_LO_SensorReading(
        temp, pressure,
        gyroX, gyroY, gyroZ,
        accelX, accelY, accelZ
      );

    } else {
      // Error reading sensors
      this->m_errorCount++;
      this->log_WARNING_HI_SensorReadError("Multiple sensors");
    }
  }

  // ----------------------------------------------------------------------
  // Private helper methods
  // ----------------------------------------------------------------------

  bool SensorHub::readSensors(
      F32& temp,
      F32& pressure,
      F32& gyroX, F32& gyroY, F32& gyroZ,
      F32& accelX, F32& accelY, F32& accelZ
  )
  {
    // TODO: Replace with real sensor reads using I2C/SPI via Zephyr drivers
    // For now, return simulated data to test the system
    
    // Simulated temperature (25°C with small variation)
    temp = 25.0f + (m_readCount % 10) * 0.1f;
    
    // Simulated pressure (1013 hPa with small variation)
    pressure = 1013.0f + (m_readCount % 5) * 0.5f;
    
    // Simulated gyro (degrees per second)
    gyroX = (m_readCount % 20) - 10.0f;
    gyroY = (m_readCount % 15) - 7.5f;
    gyroZ = (m_readCount % 10) - 5.0f;
    
    // Simulated accelerometer (m/s²)
    // Should be close to 9.8 when stationary (gravity on Z-axis)
    accelX = 0.1f;
    accelY = 0.1f;
    accelZ = 9.8f;
    
    return true;  // Always succeeds with simulated data
    
    // REAL IMPLEMENTATION EXAMPLE (commented out):
    /*
    #ifdef CONFIG_I2C
    
    // Temperature/Pressure sensor (e.g., BMP280)
    const struct device *bmp280 = device_get_binding("BMP280");
    if (bmp280 && device_is_ready(bmp280)) {
      struct sensor_value temp_val, press_val;
      
      sensor_sample_fetch(bmp280);
      sensor_channel_get(bmp280, SENSOR_CHAN_AMBIENT_TEMP, &temp_val);
      sensor_channel_get(bmp280, SENSOR_CHAN_PRESS, &press_val);
      
      temp = sensor_value_to_float(&temp_val);
      pressure = sensor_value_to_float(&press_val);
    } else {
      this->log_WARNING_HI_SensorReadError("BMP280");
      return false;
    }
    
    // IMU sensor (e.g., MPU6050)
    const struct device *mpu6050 = device_get_binding("MPU6050");
    if (mpu6050 && device_is_ready(mpu6050)) {
      struct sensor_value gyro_vals[3], accel_vals[3];
      
      sensor_sample_fetch(mpu6050);
      sensor_channel_get(mpu6050, SENSOR_CHAN_GYRO_XYZ, gyro_vals);
      sensor_channel_get(mpu6050, SENSOR_CHAN_ACCEL_XYZ, accel_vals);
      
      gyroX = sensor_value_to_float(&gyro_vals[0]);
      gyroY = sensor_value_to_float(&gyro_vals[1]);
      gyroZ = sensor_value_to_float(&gyro_vals[2]);
      
      accelX = sensor_value_to_float(&accel_vals[0]);
      accelY = sensor_value_to_float(&accel_vals[1]);
      accelZ = sensor_value_to_float(&accel_vals[2]);
    } else {
      this->log_WARNING_HI_SensorReadError("MPU6050");
      return false;
    }
    
    return true;
    
    #else
    this->log_WARNING_HI_SensorReadError("I2C not enabled");
    return false;
    #endif
    */
  }

}
