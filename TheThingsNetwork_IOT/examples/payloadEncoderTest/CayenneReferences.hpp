/* This code is free software:
 * you can redistribute it and/or modify it under the terms of a Creative
 * Commons Attribution-NonCommercial 4.0 International License
 * (http://creativecommons.org/licenses/by-nc/4.0/)
 * 
 * Copyright (c) 2024 March by Klaasjan Wagenaar, Tristan Bosveld and Richard Kroesen
 */

#ifndef CAYENNE_REFERENCES_HPP
#define CAYENNE_REFERENCES_HPP

#include <stdint.h>

namespace PAYLOAD_ENCODER
{
    /**
     * @brief Enum class defining data types for Cayenne LPP.
     */
    enum class DATA_TYPES : uint8_t
    {
        DIG_IN      = 0,    /* DIGITAL INPUT */
        DIG_OUT     = 1,    /* DITIAL OUTPUT */
        ANL_IN      = 2,    /* ANALOG INPUT */
        ANL_OUT     = 3,    /* ANALOG OUTPUT */
        ILLUM_SENS  = 101,  /* ILLUMINATION SENSOR */
        PRSNC_SENS  = 102,  /* PRESCENCE SENSOR */
        TEMP_SENS   = 103,  /* TEMPERATURE SENSOR */
        HUM_SENS    = 104,  /* HUMIDITY SENSOR */
        ACCRM_SENS  = 113,  /* ACCELEROMETER */
        BARO_SENS   = 115,  /* BAROMETER */
        GYRO_SENS   = 134,  /* GYROMETER */
        GPS_LOC     = 136   /* GPS LOCATION METER */
    };

    /**
     * @brief Enum class defining sizes of data types for Cayenne LPP.
     */
    enum class DATA_TYPES_SIZES : size_t
    {
        DIG_IN      = 1,    /* 1 bit resolution */
        DIG_OUT     = 1,    /* 1 bit resolution */
        ANL_IN      = 2,    /* 0.01 Signed */
        ANL_OUT     = 2,    /* 0.01 Signed */
        ILLUM_SENS  = 2,    /* 1 Lux Unsigned MSB */
        PRSNC_SENS  = 1,    /* 1 bit resolution */
        TEMP_SENS   = 2,    /* 0.1 °C Signed MSB */
        HUM_SENS    = 2,    /* 0.1 % Unsigned */
        ACCRM_SENS  = 6,    /* 0.001 G Signed MSB per axis */
        BARO_SENS   = 2,    /* 0.1 hPa Unsigned MSB */
        GYRO_SENS   = 6,    /* 0.01 °/s Signed MSB per axis */
        GPS_LOC     = 12    /* Latitude  : 0.0001° Signed MSB
                             * Longitude : 0.0001° Signed MSB
                             * Altitude  : 0.01 meter Signed MSB */
    };
    
    /**
     * @brief Function to get the resolution for floating point data types.
     * @param dataType The data type.
     * @return The resolution of the data type.
     */
    const static inline int16_t FLOATING_DATA_RESOLUTION(DATA_TYPES dataType)
    {
        switch (dataType)
        {
        case DATA_TYPES::DIG_IN:
            break;
        case DATA_TYPES::DIG_OUT:
            break;
        case DATA_TYPES::ANL_IN:    
            return 100;                 // 0.01 Signed Resolution.
        case DATA_TYPES::ANL_OUT:
            return 100;                 // 0.01 Signed Resolution.
        case DATA_TYPES::ILLUM_SENS:
            break;
        case DATA_TYPES::PRSNC_SENS:
            break;
        case DATA_TYPES::TEMP_SENS:
            return 10;                  // 0.1 °C Signed MSB Resolution.
        case DATA_TYPES::HUM_SENS:
            return 10;                  // 0.1 % Unsigned Resolution                       
        case DATA_TYPES::ACCRM_SENS:
            return 1000;                // 0.001 G Signed MSB per axis
        case DATA_TYPES::BARO_SENS:
            return 10;                  // 0.1 hPa Unsigned MSB
        case DATA_TYPES::GYRO_SENS:
            return 100;                 // 0.01 °/s Signed MSB per axis
        case DATA_TYPES::GPS_LOC:
            return 10000;               // 0.0001° Signed MSB for Lat- & Longitude
        default:
            return 0;
        }
        return 0;
    }

    /**
     * @brief Function to create a mapping between data types reference and respective size in bytes.
     * @param dataType The data type.
     * @return The size of the data type in bytes.
     */
    const static inline size_t getDataTypeSize(DATA_TYPES dataType)
    {
        switch (dataType)
        {
        case DATA_TYPES::DIG_IN:
            return static_cast<size_t>(DATA_TYPES_SIZES::DIG_IN);
        case DATA_TYPES::DIG_OUT:
            return static_cast<size_t>(DATA_TYPES_SIZES::DIG_OUT);
        case DATA_TYPES::ANL_IN:
            return static_cast<size_t>(DATA_TYPES_SIZES::ANL_IN);
        case DATA_TYPES::ANL_OUT:
            return static_cast<size_t>(DATA_TYPES_SIZES::ANL_OUT);
        case DATA_TYPES::ILLUM_SENS:
            return static_cast<size_t>(DATA_TYPES_SIZES::ILLUM_SENS);
        case DATA_TYPES::PRSNC_SENS:
            return static_cast<size_t>(DATA_TYPES_SIZES::PRSNC_SENS);
        case DATA_TYPES::TEMP_SENS:
            return static_cast<size_t>(DATA_TYPES_SIZES::TEMP_SENS);
        case DATA_TYPES::HUM_SENS:
            return static_cast<size_t>(DATA_TYPES_SIZES::HUM_SENS);
        case DATA_TYPES::ACCRM_SENS:
            return static_cast<size_t>(DATA_TYPES_SIZES::ACCRM_SENS);
        case DATA_TYPES::BARO_SENS:
            return static_cast<size_t>(DATA_TYPES_SIZES::BARO_SENS);
        case DATA_TYPES::GYRO_SENS:
            return static_cast<size_t>(DATA_TYPES_SIZES::GYRO_SENS);
        case DATA_TYPES::GPS_LOC:
            return static_cast<size_t>(DATA_TYPES_SIZES::GPS_LOC);
        default:
            return 0;
        }
        return 0;
    }

    /**
     * @brief Enum class defining error types for Cayenne LPP.
     */
    enum class ERROR_TYPES : uint8_t
    {
        LPP_ERROR_OVERFLOW      = 0,    /**< No error */
        LPP_ERROR_UNKOWN_TYPE   = 1,    /**< Buffer overflow */
        LPP_ERROR_OK            = 2     /**< Unknown data type */
    };

} // End of PAYLOAD_ENCODER Namespace.

#endif // End of CAYENNE_REFERENCES_HPP