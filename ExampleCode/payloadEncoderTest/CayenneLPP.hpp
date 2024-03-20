/* This code is free software:
 * you can redistribute it and/or modify it under the terms of a Creative
 * Commons Attribution-NonCommercial 4.0 International License
 * (http://creativecommons.org/licenses/by-nc/4.0/)
 *
 * Copyright (c) 2024 March by Klaasjan Wagenaar, Tristan Bosveld and Richard Kroesen
 */

#ifndef CAYENNE_LPP_HPP
#define CAYENNE_LPP_HPP

#include <stdint.h>
#include "CayenneReferences.hpp"

namespace PAYLOAD_ENCODER
{
    /**
     * @brief Template class for CayenneLPP payload encoder.
     *
     * @tparam MaxSize Maximum size of the buffer.
     */
    template <size_t MaxSize>
    class CayenneLPP
    {
    public:
        /**
         * @brief Constructor for CayenneLPP.
         *
         * @param size Size of the buffer.
         */
        explicit CayenneLPP(const uint8_t size) : operationalSize(size > MaxSize ? MaxSize : size), currentIndex(0) 
        {
            for(size_t i = 0; i < MaxSize; i++) {
                buffer[i] = 0;
            }
        }

        /**
         * @brief Copy constructor for the CayenneLPP class.
         *
         * Initializes a new instance of the CayenneLPP class by deep copying the contents
         * from another instance. This includes copying the operational size, current index,
         * and the contents of the data buffer up to the current index. The purpose of this
         * constructor is to create a new object with the same state as the object passed as a parameter.
         *
         * @param other The CayenneLPP instance from which to copy.
         */
        CayenneLPP(const CayenneLPP& other) : operationalSize(other.operationalSize), currentIndex(other.currentIndex)
        {
            for(size_t i = 0; i < currentIndex; ++i)
            {
                buffer[i] = other.buffer[i];
            }
        }

        /**
         * @brief Copy assignment operator for the CayenneLPP class.
         *
         * Replaces the contents of this instance with a copy of the contents of another
         * instance.
         *
         * @param other The CayenneLPP instance to assign from.
         * @return CayenneLPP& A reference to this instance after copying.
         */
        CayenneLPP& operator=(const CayenneLPP& other) {
            if (this != &other) {
                currentIndex = other.currentIndex;
                memcpyAVR(buffer, other.getBuffer(), currentIndex);
            }
            return *this;
        }

        /**
         * @brief Destructor for CayenneLPP.
         */
        ~CayenneLPP() {}

        /* REQUIRED FUNCTIONS by ASSIGNMENT #1 */
        /**
         * @brief Resets the buffer.
         */
        void reset()
        {
            currentIndex = 0; // Reset currentIndex when buffer is reset
        }

        /**
         * @brief Gets the size of the buffer.
         *
         * @return size_t Size of the buffer.
         */
        size_t getSize(void) const
        {
            return currentIndex; // Returns the count of used bytes
        }

        /**
         * @brief Returns the buffer.
         *
         * @return const uint8_t* Pointer to the buffer.
         */
        const uint8_t *getBuffer(void) const
        {
            return buffer;
        }

        /**
         * @brief Copies the buffer to the destination buffer.
         *
         * @param destBuffer Destination buffer.
         * @return uint8_t Number of bytes copied.
         */
        const uint8_t copy(uint8_t *destBuffer) const
        {
            if (!destBuffer)
            {
                return 0; // Safety check to ensure the destination buffer is valid
            }
            memcpyAVR(destBuffer, buffer, currentIndex);
            return static_cast<uint8_t>(currentIndex);
        }

        /* END of REQUIRED FUNCTIONS by ASSIGNMENT #1 */

        /**
         * @brief Adds a digital input field to the payload.
         *
         * @param sensorChannel The channel number of the digital input sensor.
         * @param value The value of the digital input byte or bool (0 or 1).
         * @return uint8_t Returns the new size of the payload after adding the digital input.
         */
        const uint8_t addDigitalInput(const uint8_t sensorChannel, const uint8_t value)
        {
            return addField(DATA_TYPES::DIG_IN, sensorChannel, value);
        }

        /**
         * @brief Adds a digital output field to the payload.
         *
         * @param sensorChannel The channel number of the digital output sensor.
         * @param value The value of the digital output byte or bool (0 or 1).
         * @return uint8_t Returns the new size of the payload after adding the digital output.
         */
        const uint8_t addDigitalOutput(const uint8_t sensorChannel, const uint8_t value)
        {
            return addField(DATA_TYPES::DIG_OUT, sensorChannel, value);
        }

        /**
         * @brief Adds an analog input field to the payload.
         *
         * @param sensorChannel The channel number of the analog input sensor.
         * @param value The analog value to be added. This value is scaled appropriately before adding to the payload.
         * Precision of 0.01 Signed.
         * @return uint8_t Returns the new size of the payload after adding the analog input.
         */
        const uint8_t addAnalogInput(const uint8_t sensorChannel, const float value)
        {
            return addField(DATA_TYPES::ANL_IN, sensorChannel, value);
        }

        /**
         * @brief Adds an analog output field to the payload.
         *
         * @param sensorChannel The channel number of the analog output sensor.
         * @param value The analog value to be added. This value is scaled appropriately before adding to the payload.
         * Precision of 0.01 Signed.
         * @return uint8_t Returns the new size of the payload after adding the analog output.
         */
        const uint8_t addAnalogOutput(const uint8_t sensorChannel, const float value)
        {
            return addField(DATA_TYPES::ANL_OUT, sensorChannel, value);
        }

        /**
         * @brief Adds an illumination field to the payload.
         *
         * This method appends an illumination value (two-byte value) to the payload.
         * The illumination is identified by a unique sensor channel and its value, representing light intensity in lux.
         *
         * @param sensorChannel The channel number of the illumination sensor.
         * @param value The illumination value in lux to be added. This value is encoded directly into the payload.
         * @return uint8_t Returns the new size of the payload after adding the illumination value.
         */
        const uint8_t addIllumination(const uint8_t sensorChannel, const uint16_t value)
        {
            return addField(DATA_TYPES::ILLUM_SENS, sensorChannel, value);
        }

        /**
         * @brief Adds a presence sensor field to the payload.
         *
         * This method appends a presence sensor value (single byte value) to the payload.
         * The presence sensor is identified by a unique sensor channel and its value (0 or 1), indicating absence or presence.
         *
         * @param sensorChannel The channel number of the presence sensor.
         * @param value The value of the presence sensor (0 for absence, 1 for presence).
         * @return uint8_t Returns the new size of the payload after adding the presence sensor value.
         */
        const uint8_t addPresence(const uint8_t sensorChannel, const uint8_t value)
        {
            return addField(DATA_TYPES::PRSNC_SENS, sensorChannel, value);
        }

        /**
         * @brief Adds a temperature field to the payload.
         *
         * This method appends a temperature value (two-byte value) to the payload.
         * The temperature is identified by a unique sensor channel and its value, representing the temperature in degrees Celsius.
         *
         * @param sensorChannel The channel number of the temperature sensor.
         * @param value The temperature value in degrees Celsius to be added. This value is scaled appropriately before adding to the payload.
         * @return uint8_t Returns the new size of the payload after adding the temperature value.
         */
        const uint8_t addTemperature(const uint8_t sensorChannel, const float value)
        {
            return addField(DATA_TYPES::TEMP_SENS, sensorChannel, value);
        }

        /**
         * @brief Adds a humidity field to the payload.
         *
         * This method appends a humidity value to the payload, represented as a percentage of relative humidity.
         * The humidity sensor is identified by a unique sensor channel, and the value must be between 0 and 100,
         * representing 0% to 100% relative humidity.
         *
         * @param sensorChannel The channel number of the humidity sensor.
         * @param value The relative humidity value to be added, specified as a percentage from 0 to 100.
         *              This value is scaled appropriately before adding to the payload.
         * @return uint8_t Returns the new size of the payload after adding the humidity value. Returns 0 if
         *                 there was an error or if the payload could not be appended.
         */
        const uint8_t addHumidity(const uint8_t sensorChannel, const float value)
        {
            return addField(DATA_TYPES::HUM_SENS, sensorChannel, value);
        }

        /**
         * @brief Adds an accelerometer field to the payload.
         *
         * This method appends accelerometer data to the payload, including x, y, and z axis measurements.
         * The accelerometer data is identified by a unique sensor channel, with each axis value representing
         * acceleration in Gs. This method allows for the capture of three-dimensional motion or orientation data.
         *
         * @param sensorChannel The channel number of the accelerometer sensor.
         * @param x The acceleration value along the x-axis in Gs.
         * @param y The acceleration value along the y-axis in Gs.
         * @param z The acceleration value along the z-axis in Gs.
         * @return uint8_t Returns the new size of the payload after adding the accelerometer data. Returns 0 if
         *                 there was an error or if the payload could not be appended.
         */
        const uint8_t addAccelerometer(const uint8_t sensorChannel, const float x, const float y, const float z)
        {
            return addField(DATA_TYPES::ACCRM_SENS, sensorChannel, x, y, z);
        }

        /**
         * @brief Adds a barometric pressure field to the payload.
         *
         * This method appends a barometric pressure value to the payload. The barometric pressure
         * is identified by a unique sensor channel, with the value representing atmospheric pressure
         * in hPa (hectopascals).
         *
         * @param sensorChannel The channel number of the barometer sensor.
         * @param value The barometric pressure in hPa to be added. This value is scaled appropriately
         *              before adding to the payload.
         * @return uint8_t Returns the new size of the payload after adding the barometric pressure value.
         *                 Returns 0 if there was an error or if the payload could not be appended.
         */
        const uint8_t addBarometer(const uint8_t sensorChannel, const float value)
        {
            return addField(DATA_TYPES::BARO_SENS, sensorChannel, value);
        }

        /**
         * @brief Adds a gyroscope field to the payload.
         *
         * This method appends gyroscope data to the payload, including angular velocity around the x, y,
         * and z axes. The gyroscope data is identified by a unique sensor channel, with each axis value
         * representing angular velocity in degrees per second.
         *
         * @param sensorChannel The channel number of the gyroscope sensor.
         * @param x The angular velocity around the x-axis in degrees per second.
         * @param y The angular velocity around the y-axis in degrees per second.
         * @param z The angular velocity around the z-axis in degrees per second.
         * @return uint8_t Returns the new size of the payload after adding the gyroscope data. Returns 0 if
         *                 there was an error or if the payload could not be appended.
         */
        const uint8_t addGyroscope(const uint8_t sensorChannel, const float x, const float y, const float z)
        {
            return addField(DATA_TYPES::GYRO_SENS, sensorChannel, x, y, z);
        }

        /**
         * @brief Adds a GPS location field to the payload.
         *
         * This method appends GPS location data to the payload, including latitude, longitude,
         * and altitude. The GPS location data is identified by a unique sensor channel, with the latitude
         * and longitude values in degrees and the altitude in meters.
         *
         * @param sensorChannel The channel number of the GPS sensor.
         * @param lat The latitude in degrees.
         * @param lon The longitude in degrees.
         * @param alt The altitude in meters above sea level.
         * @return uint8_t Returns the new size of the payload after adding the GPS location data. Returns 0 if
         *                 there was an error or if the payload could not be appended.
         */
        const uint8_t addGPSLocation(uint8_t sensorChannel, const float lat, const float lon, const float alt)
        {
            return addField(DATA_TYPES::GPS_LOC, sensorChannel, lat, lon, alt);
        }

    private:
        uint8_t buffer[MaxSize];
        size_t operationalSize;
        size_t currentIndex;

        /**
         * @brief Rounds a floating-point value and casts it to an int32_t.
         * 
         * This function rounds the given floating-point value to the nearest integer,
         * following the half-away-from-zero rule (also known as commercial rounding).
         * It is designed to avoid precision issues commonly associated with floating-point
         * arithmetic when converting to an integer type.
         * 
         * @param value The floating-point value to be rounded and cast.
         * @return int32_t The rounded value, cast to an int32_t.
         */
        const static inline int32_t round_and_cast(const float value)
        {
            if (value > 0)
            {
                return static_cast<int32_t>(value + 0.5f);
            }
            else
            {
                return static_cast<int32_t>(value - 0.5f);
            }
        }

        /**
         * @brief Rounds a floating-point value and casts it to an int16_t.
         * 
         * Similar to round_and_cast, this function rounds the given floating-point value
         * to the nearest integer, following the half-away-from-zero rule. The result is then
         * cast to an int16_t. This function is particularly useful for values expected to be
         * within the int16_t range and when working with data types that require lower precision
         * and less storage.
         * 
         * @param value The floating-point value to be rounded and cast.
         * @return int16_t The rounded value, cast to an int16_t.
         */
        const static inline int16_t round_and_cast_int16(const float value)
        {
            if (value > 0)
            {
                return static_cast<int16_t>(value + 0.5f);
            }
            else
            {
                return static_cast<int16_t>(value - 0.5f);
            }
        }

        /**
         * @brief Checks if there is enough space in the buffer to append new data.
         * 
         * This function determines if the buffer has sufficient capacity to accommodate
         * additional data of a specified size. It is useful for preventing buffer overflows
         * by ensuring that operations do not exceed the buffer's operational size.
         * 
         * @param dataSize The size of the data (in bytes) that needs to be appended to the buffer.
         * @return bool Returns true if there is enough space to append the data; otherwise, false.
         */
        const bool checkCapacity(const size_t dataSize)
        {
            return currentIndex + dataSize <= operationalSize;
        }

        /**
         * @brief Appends a header consisting of data type and sensor channel to the buffer.
         * 
         * This function appends a two-byte header to the payload buffer. The first byte
         * represents the data type (encoded as a uint8_t), and the second byte represents
         * the sensor channel. This header precedes the actual sensor data in the payload.
         * 
         * @param dataType The data type identifier for the sensor data being appended.
         * @param sensorChannel The channel number associated with the sensor data.
         */
        void appendHeader(const DATA_TYPES dataType, const uint8_t sensorChannel)
        {
            buffer[currentIndex++] = static_cast<uint8_t>(dataType);
            buffer[currentIndex++] = sensorChannel;
        }

        /**
         * @brief Appends data of any type to the buffer.
         * 
         * Utilizes template specialization to append data of a specified type to the payload buffer.
         * The function leverages the memcpyAVR function to copy the data into the buffer, handling
         * various data types seamlessly. This method automatically adjusts the currentIndex to reflect
         * the new size of the buffer after the data append operation.
         * 
         * @param data The data to be appended to the buffer. Can be of any type (e.g., int, float).
         * @tparam T The type of the data being appended.
         */
        template <typename T>
        void appendData(const T data)
        {
            memcpyAVR(&buffer[currentIndex], &data, sizeof(T));
            currentIndex += sizeof(T);
        }

        /**
         * @brief Adds a field with a single-byte value to the payload.
         * 
         * Appends a sensor data field to the payload, consisting of a header (data type and sensor channel)
         * followed by a single-byte value. This function is typically used for data types that are represented
         * by a single byte, such as digital inputs or outputs.
         * 
         * @param dataType The data type identifier for the sensor data being appended.
         * @param sensorChannel The channel number associated with the sensor data.
         * @param value The single-byte sensor data value to be appended.
         * @return uint8_t Returns the new current index in the buffer after appending the data.
         *                 Returns 0 if there was insufficient capacity to append the data.
         */
        const uint8_t addFieldImpl(const DATA_TYPES dataType, const uint8_t sensorChannel, const uint8_t value)
        {
            if (!checkCapacity(3)) {
                return 0;
            }
            appendHeader(dataType, sensorChannel);
            buffer[currentIndex++] = value;
            return currentIndex;
        }

        /**
         * @brief Adds a field with a two-byte value to the payload.
         * 
         * Appends a sensor data field to the payload, including a header (data type and sensor channel)
         * followed by a two-byte value. This function is suitable for data types like analog values,
         * which are represented by two bytes.
         * 
         * @param dataType The data type identifier for the sensor data being appended.
         * @param sensorChannel The channel number associated with the sensor data.
         * @param value The two-byte sensor data value to be appended. It's directly appended using appendData function.
         * @return uint8_t Returns the new current index in the buffer after appending the data.
         *                 Returns 0 if there was insufficient capacity to append the data.
         */
        const uint8_t addFieldImpl(const DATA_TYPES dataType, const uint8_t sensorChannel, const uint16_t value)
        {
            if (!checkCapacity(4)) {
                return 0;
            }
            appendHeader(dataType, sensorChannel);
            appendData(value);
            return currentIndex;
        }

        /**
         * @brief Adds a field with a scaled float value to the payload.
         * 
         * Appends a sensor data field to the payload, including a header (data type and sensor channel)
         * followed by a float value that is scaled and cast to a two-byte integer before appending.
         * This is typically used for sensor data like temperature, humidity, etc., that need scaling.
         * 
         * @param dataType The data type identifier for the sensor data being appended.
         * @param sensorChannel The channel number associated with the sensor data.
         * @param value The float sensor data value to be scaled, cast, and appended.
         * @return uint8_t Returns the new current index in the buffer after appending the data.
         *                 Returns 0 if there was insufficient capacity to append the data.
         */
        const uint8_t addFieldImpl(const DATA_TYPES dataType, const uint8_t sensorChannel, const float value)
        {
            const uint16_t resolution = FLOATING_DATA_RESOLUTION(dataType);
            int16_t scaledValue = round_and_cast_int16(value * resolution);
            if (!checkCapacity(4)) {
                return 0;
            }
            appendHeader(dataType, sensorChannel);
            appendData(scaledValue);
            return currentIndex;
        }

        /**
         * @brief Adds a field with three float values to the payload, with special handling for GPS data.
         * 
         * This method appends data consisting of three float values to the payload, including a header
         * (data type and sensor channel). It provides special handling for GPS location data, applying
         * appropriate scaling and precision adjustments. For other data types requiring three floats,
         * it scales and casts each float to a two-byte integer before appending.
         * 
         * @param dataType The data type identifier for the sensor data being appended.
         * @param sensorChannel The channel number associated with the sensor data.
         * @param first The first float value (e.g., latitude or x-axis acceleration).
         * @param second The second float value (e.g., longitude or y-axis acceleration).
         * @param third The third float value (e.g., altitude or z-axis acceleration).
         * @return uint8_t Returns the new current index in the buffer after appending the data.
         *                 Returns 0 if there was insufficient capacity to append the data.
         */
        const uint8_t addFieldImpl(const DATA_TYPES dataType, const uint8_t sensorChannel, 
            const float first, const float second, const float third)
        {
            const size_t totalBytes = getDataTypeSize(dataType) + 2;
            if (!checkCapacity(totalBytes))
                return 0;

            appendHeader(dataType, sensorChannel);

            if (dataType == DATA_TYPES::GPS_LOC)
            {
                // Special handling for GPS data
                appendData(round_and_cast(first * FLOATING_DATA_RESOLUTION(dataType)));
                appendData(round_and_cast(second * FLOATING_DATA_RESOLUTION(dataType)));
                appendData(round_and_cast(third * (FLOATING_DATA_RESOLUTION(dataType) / 100)));
            }
            else
            {
                // Generic handling for other types requiring three floats
                appendData(round_and_cast_int16(first * FLOATING_DATA_RESOLUTION(dataType)));
                appendData(round_and_cast_int16(second * FLOATING_DATA_RESOLUTION(dataType)));
                appendData(round_and_cast_int16(third * FLOATING_DATA_RESOLUTION(dataType)));
            }
            return currentIndex;
        }

        /**
         * @brief Dispatches the call to the appropriate addFieldImpl overload based on the argument types.
         * 
         * This variadic template function allows for flexible addition of various types of sensor data
         * to the payload. It routes the call to the correct implementation of addFieldImpl based on the
         * count and types of the arguments provided.
         * 
         * @param dataType The data type identifier for the sensor data being appended.
         * @param sensorChannel The channel number associated with the sensor data.
         * @param args The sensor data values to be appended. This could be a single value, two values,
         *             or three values depending on the data type.
         * @return uint8_t Returns the new current index in the buffer after appending the data.
         *                 The return value is determined by the specific addFieldImpl overload called.
         */
        template <typename... Args>
        const uint8_t addField(const DATA_TYPES dataType, const uint8_t sensorChannel, Args... args)
        {
            return addFieldImpl(dataType, sensorChannel, args...);
        }

        /**
         * @brief Custom implementation of memcpy for AVR or similar architectures.
         * 
         * This function copies 'n' bytes from memory area 'src' to memory area 'dest'.
         * It is designed for environments where the standard library's memcpy may not be
         * available or optimal. This function performs a byte-by-byte copy.
         * 
         * @param dest Pointer to the destination array where the content is to be copied.
         * @param src Pointer to the source of data to be copied.
         * @param n Number of bytes to be copied.
         */
        static inline void memcpyAVR(void *dest, const void *src, const size_t n)
        {
            uint8_t *cdest = static_cast<uint8_t*>(dest);
            const uint8_t *csrc = static_cast<const uint8_t*>(src);
            for (size_t i = 0; i < n; i++)
            {
                cdest[i] = csrc[i];
            }
        }
    }; // End of class CayenneLPP.
} // End of Namespace PAYLOAD_ENCODER.
#endif // CAYENNE_LPP_HPP