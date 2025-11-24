// ======================================================================
// \title  SensorHub.hpp
// \author Madison Wright
// \brief  hpp file for SensorHub component implementation class
//
// Sensor hub component for reading temperature, pressure, gyro, and
// accelerometer data on Raspberry Pi Pico with Zephyr RTOS.
// ======================================================================

#ifndef FprimeZephyrReference_SensorHub_HPP
#define FprimeZephyrReference_SensorHub_HPP

#include "FprimeZephyrReference/Components/SensorHub/SensorHubComponentAc.hpp"

namespace FprimeZephyrReference {

  class SensorHub :
    public SensorHubComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct SensorHub object
      SensorHub(
          const char* const compName //!< The component name
      );

      //! Destroy SensorHub object
      ~SensorHub();

      //! Initialize the component
      void init(
          FwSizeType queueDepth, //!< The queue depth
          FwSizeType instance = 0 //!< The instance number
      );

    private:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for schedIn
      void schedIn_handler(
          FwIndexType portNum, //!< The port number
          U32 context //!< The call context
      ) override;

      // ----------------------------------------------------------------------
      // Private helper methods
      // ----------------------------------------------------------------------

      //! Read all sensors
      //! \return true if successful, false on error
      bool readSensors(
          F32& temp,      //!< Temperature output (Celsius)
          F32& pressure,  //!< Pressure output (hPa)
          F32& gyroX,     //!< Gyro X output (deg/s)
          F32& gyroY,     //!< Gyro Y output (deg/s)
          F32& gyroZ,     //!< Gyro Z output (deg/s)
          F32& accelX,    //!< Accel X output (m/s²)
          F32& accelY,    //!< Accel Y output (m/s²)
          F32& accelZ     //!< Accel Z output (m/s²)
      );

      // ----------------------------------------------------------------------
      // Private member variables
      // ----------------------------------------------------------------------

      //! Counter for successful reads
      U32 m_readCount;

      //! Counter for failed reads
      U32 m_errorCount;

  };

}

#endif
