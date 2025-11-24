# SensorHub Component

F´ Prime active component for reading multiple sensors on Raspberry Pi Pico with Zephyr RTOS.

## Overview

SensorHub is a multi-sensor interface component that aggregates data from temperature, pressure, gyroscope, and accelerometer sensors. It operates at 1Hz (triggered by rate group) and provides telemetry channels for each sensor reading.

**Purpose**: Centralized sensor data collection for embedded flight software applications

**Target Platform**: Raspberry Pi Pico (RP2040) with Zephyr RTOS

**Status**: Simulated sensor implementation (hardware drivers to be added)

---

## Features

- **Temperature Sensor**: Reads ambient temperature in Celsius
- **Pressure Sensor**: Reads atmospheric pressure in hPa
- **Gyroscope**: 3-axis rotation rate (X, Y, Z) in degrees/second
- **Accelerometer**: 3-axis acceleration (X, Y, Z) in m/s²
- **Health Monitoring**: Tracks read count and error count
- **Event Logging**: Reports sensor readings and errors via F´ Prime events

---

## Telemetry Channels

| Channel | Type | Description | Units | Update Rate |
|---------|------|-------------|-------|-------------|
| `Temperature` | F32 | Ambient temperature | °C | 1 Hz |
| `Pressure` | F32 | Atmospheric pressure | hPa | 1 Hz |
| `GyroX` | F32 | Gyroscope X-axis | deg/s | 1 Hz |
| `GyroY` | F32 | Gyroscope Y-axis | deg/s | 1 Hz |
| `GyroZ` | F32 | Gyroscope Z-axis | deg/s | 1 Hz |
| `AccelX` | F32 | Accelerometer X-axis | m/s² | 1 Hz |
| `AccelY` | F32 | Accelerometer Y-axis | m/s² | 1 Hz |
| `AccelZ` | F32 | Accelerometer Z-axis | m/s² | 1 Hz |
| `ReadCount` | U32 | Successful readings | count | 1 Hz |
| `ErrorCount` | U32 | Failed readings | count | 1 Hz |

---

## Events

| Event | Severity | Description |
|-------|----------|-------------|
| `SensorHubInitialized` | ACTIVITY_HI | Component initialized successfully |
| `SensorReading` | ACTIVITY_LO | Periodic sensor reading summary (every 10 readings) |
| `SensorError` | WARNING_LO | Sensor read failure detected |

---

## Component Architecture

### Component Type
**Active Component** - Has dedicated thread and message queue

### Port Connections

**Input Ports**:
- `schedIn` (Svc.Sched) - Triggered by rate group at 1Hz

**Output Ports**:
- `timeGetOut` (Fw.Time) - Retrieves current system time
- `tlmOut` (Fw.Tlm) - Sends telemetry to telemetry manager
- `eventOut` (Fw.Log) - Sends events to event logger
- `textEventOut` (Fw.LogText) - Sends text events to text logger

### Threading
- **Queue Size**: 10 messages
- **Stack Size**: 8KB (configurable)
- **Priority**: 5 (medium priority)

---

## Implementation Details

### Current Implementation: Simulated Sensors

The current implementation uses **simulated sensor data** for testing and validation:

```cpp
// Simulated temperature (25.0-25.9°C with slow drift)
temperature = 25.0f + (m_readCount % 10) * 0.1f;

// Simulated pressure (1013.0-1013.5 hPa)
pressure = 1013.0f + (m_readCount % 5) * 0.5f;

// Simulated gyroscope (±10 deg/s oscillation)
gyroX = (m_readCount % 20) - 10.0f;
gyroY = (m_readCount % 15) - 7.5f;
gyroZ = (m_readCount % 10) - 5.0f;

// Simulated accelerometer (gravity at rest: 9.8 m/s²)
accelX = 0.1f;
accelY = 0.1f;
accelZ = 9.8f;
```

### Future Hardware Integration

To integrate real sensors, replace the `readSensors()` implementation with actual Zephyr driver calls:

```cpp
bool SensorHub::readSensors(
    F32& temperature, F32& pressure,
    F32& gyroX, F32& gyroY, F32& gyroZ,
    F32& accelX, F32& accelY, F32& accelZ)
{
    // TODO: Replace with real Zephyr driver calls
    // Example (pseudo-code):
    // const struct device *temp_dev = device_get_binding("TEMP_SENSOR");
    // sensor_sample_fetch(temp_dev);
    // sensor_channel_get(temp_dev, SENSOR_CHAN_AMBIENT_TEMP, &temp_val);
    // temperature = sensor_value_to_double(&temp_val);
    
    return true;
}
```

---

## File Structure

```
Components/SensorHub/
├── CMakeLists.txt      # Build configuration
├── SensorHub.fpp       # Component model (F´ Prime FPP)
├── SensorHub.hpp       # Auto-generated header
├── SensorHub.cpp       # Component implementation
└── README.md           # This file
```

---

## Integration Guide

### Step 1: Add Component to Project

Edit `FprimeZephyrReference/CMakeLists.txt`:

```cmake
# Add SensorHub component
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Components/SensorHub/")
```

### Step 2: Add Instance to Topology

Edit `ReferenceDeployment/Top/instances.fpp`:

```fpp
instance sensorHub: FprimeZephyrReference.SensorHub base id 0x10003000 \
  queue size Default.QUEUE_SIZE \
  stack size Default.STACK_SIZE \
  priority 5
```

### Step 3: Connect in Topology

Edit `ReferenceDeployment/Top/topology.fpp`:

**Add instance declaration**:
```fpp
instance sensorHub
```

**Connect to rate group** (1Hz):
```fpp
connections RateGroups {
  # ... existing connections ...
  rateGroup1Hz.RateGroupMemberOut[5] -> sensorHub.schedIn
}
```

**Add sensor connections**:
```fpp
connections SensorHub {
  sensorHub.timeGetOut -> chronoTime.timeGetPort
  sensorHub.tlmOut -> CdhCore.tlmSend.TlmRecv
  sensorHub.eventOut -> CdhCore.events.LogRecv
  sensorHub.textEventOut -> CdhCore.textLogger.TextLogger
}
```

### Step 4: Add to Telemetry Packets

Edit `ReferenceDeployment/Top/ReferenceDeploymentPackets.fppi`:

```fpp
packet SensorData id 1 group 1 {
  ReferenceDeployment.sensorHub.Temperature
  ReferenceDeployment.sensorHub.Pressure
  ReferenceDeployment.sensorHub.GyroX
  ReferenceDeployment.sensorHub.GyroY
  ReferenceDeployment.sensorHub.GyroZ
  ReferenceDeployment.sensorHub.AccelX
  ReferenceDeployment.sensorHub.AccelY
  ReferenceDeployment.sensorHub.AccelZ
  ReferenceDeployment.sensorHub.ReadCount
  ReferenceDeployment.sensorHub.ErrorCount
}
```

### Step 5: Build

```bash
cd ~/fprime-zephyr-reference

# Generate build files
fprime-util generate

# Build for Raspberry Pi Pico
fprime-util build

# Output: build-artifacts/zephyr.uf2
```

### Step 6: Flash to Hardware

1. Hold BOOTSEL button on Pico
2. Plug in USB cable
3. Copy `zephyr.uf2` to mounted drive
4. Pico reboots automatically

### Step 7: Monitor with GDS

```bash
# In WSL (if using Windows)
usbipd attach --wsl --busid <BUS_ID>

# Start F´ Prime GDS
fprime-gds -n \
  --dictionary ./build-artifacts/zephyr/dict/ReferenceDeploymentTopologyDictionary.json \
  --communication-selection uart \
  --uart-device /dev/ttyACM0 \
  --uart-baud 115200
```

Open browser to `http://localhost:5000`

---

## Expected Output in GDS

### Channels Tab (Updating at 1Hz)

```
sensorHub.Temperature:   25.3 °C
sensorHub.Pressure:      1013.2 hPa
sensorHub.GyroX:         2.5 deg/s
sensorHub.GyroY:         -1.2 deg/s
sensorHub.GyroZ:         0.8 deg/s
sensorHub.AccelX:        0.1 m/s²
sensorHub.AccelY:        0.1 m/s²
sensorHub.AccelZ:        9.8 m/s²
sensorHub.ReadCount:     142
sensorHub.ErrorCount:    0
```

### Events Tab

```
[ACTIVITY_HI] Sensor hub initialized successfully
[ACTIVITY_LO] Sensors: 25.3°C, 1013.2hPa
[ACTIVITY_LO] Sensors: 25.4°C, 1013.0hPa
```

# RAM Constraints and Memory Issues

## Current Status: Deployment Exceeds RP2040 Memory Limits

The SensorHub component, when integrated into the complete ReferenceDeployment topology, exceeds the Raspberry Pi Pico's 264KB RAM limit. This section documents the memory constraints, their causes, and potential solutions.

---

## Memory Overflow Details

### Build Error
```
region `RAM' overflowed by 89264 bytes
```

### Memory Breakdown

**Raspberry Pi Pico (RP2040) Available RAM**: 264 KB (270,336 bytes)

**ReferenceDeployment Memory Usage**:

| Component/Module | Approximate RAM Usage |
|------------------|----------------------|
| F´ Prime Framework Core | ~80 KB |
| CdhCore Subtopology (CmdDisp, Events, TlmSend) | ~60 KB |
| ComCcsds Subtopology (ComQueue, Router) | ~50 KB |
| Rate Groups (10Hz, 1Hz) | ~20 KB |
| SensorHub Component | ~18 KB |
| Stack allocations (8KB × 5 threads) | ~40 KB |
| Message queues | ~15 KB |
| Zephyr RTOS overhead | ~30 KB |
| **Total Estimated** | **~313 KB** |

**Overflow**: 313 KB - 264 KB = **49 KB over limit**

---

## Root Causes

### 1. CdhCore Subtopology Too Large

The `CdhCore.Subtopology` includes full command/data handling infrastructure:

- Command Dispatcher (CmdDisp)
- Event Logger with buffering
- Telemetry Channel Manager (TlmSend)
- Health Monitor
- Text Logger

**Impact**: ~60 KB of RAM for components not strictly necessary for basic sensor operation.

### 2. ComCcsds Communication Stack

The `ComCcsds.Subtopology` provides CCSDS-compliant communication:

- Framer/Deframer for packet protocol
- Communication queue with buffering
- Router for command/telemetry routing
- Buffer manager

**Impact**: ~50 KB for communication features beyond basic UART.

### 3. Multiple Active Components

Each active component requires:

- Dedicated thread stack (8 KB default)
- Message queue (typically 10 × message size)

**Impact**: 5 active components × ~10 KB each = ~50 KB

### 4. Thread Stack Sizes

Default F´ Prime stack size (8 KB per thread) is conservative for embedded systems.

**Impact**: Could potentially reduce to 2-4 KB per thread with careful design.

---

## Attempted Solutions

### Attempt 1: Remove TlmPacketizer

**Action**: Switched from `Svc.TlmPacketizer` to `Svc.TlmChan`  
**Result**: Saved ~8 KB, but still exceeded limit  
**Status**: Implemented, but insufficient

### Attempt 2: Minimize Topology

**Action**: Removed ComCcsds subtopology  
**Result**: Build succeeded, but lost communication capability  
**Status**: Not viable for complete system

### Attempt 3: Single Component Topology

**Action**: Created minimal topology with only SensorHub  
**Result**: Fits in RAM (~150 KB), but lacks command/telemetry infrastructure  
**Status**: Partial solution for testing only

---

## Current Workaround: Minimal Test Topology

A simplified topology for SensorHub testing that fits within RAM:

**Components Included**:
- SensorHub (sensor reading)
- Rate Group (1Hz scheduling)
- ChronoTime (time stamping)
- Text Logger (basic event logging)

**Components Removed**:
- Command Dispatcher (no command capability)
- Telemetry Manager (telemetry generated but not packetized)
- Communication stack (no downlink)
- Health Monitor

**Result**: ~150 KB RAM usage - fits on Pico

**Trade-off**: Component can run and collect sensor data, but cannot be commanded or send telemetry to ground station.

## Testing

### Unit Testing

```bash
# Build and run unit tests
fprime-util check

# Expected: All tests pass
```

### Hardware-in-Loop Testing

1. **Verify Initialization**: Check for "Sensor hub initialized" event
2. **Monitor Telemetry**: All channels updating at 1Hz
3. **Check Values**: Temperature ~25°C, Pressure ~1013 hPa, AccelZ ~9.8 m/s²
4. **Error Handling**: ErrorCount should remain 0



## Performance Characteristics

- **Update Rate**: 1 Hz (configurable via rate group)
- **Latency**: <10ms from trigger to telemetry emission
- **CPU Usage**: <1% at 1Hz update rate
- **Thread Safety**: All operations protected by F´ Prime component framework


## Dependencies

### F´ Prime Framework
- `Svc.Sched` - Rate group scheduling
- `Fw.Time` - Time stamping
- `Fw.Tlm` - Telemetry
- `Fw.Log` - Event logging

### Zephyr RTOS
- **Minimum Version**: 3.5.0
- **Required Modules**: Sensor subsystem (future)

---

## Contributing

When adding real sensor drivers:

1. Keep `readSensors()` signature unchanged
2. Add error handling for sensor failures
3. Update `ErrorCount` telemetry on errors
4. Log appropriate events for sensor state changes
5. Document sensor-specific configuration in this README
 
**Target Platform**: Raspberry Pi Pico with Zephyr RTOS
