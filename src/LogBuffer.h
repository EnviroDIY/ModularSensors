/**
 * @file LogBuffer.cpp
 * @copyright 2023 Thomas Watson
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Thomas Watson <twatson52@icloud.com>
 *
 * @brief Implements the LogBuffer class.
 *
 * This class buffers logged timestamps and variable values for transmission.
 */

// Header Guards
#ifndef SRC_LOGBUFFER_H_
#define SRC_LOGBUFFER_H_

#ifndef MS_LOG_DATA_BUFFER_SIZE
#ifdef ARDUINO_AVR_MEGA2560
#define MS_LOG_DATA_BUFFER_SIZE 512
#elif defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)
#define MS_LOG_DATA_BUFFER_SIZE 256
#elif defined(__AVR_ATmega1284P__)
#define MS_LOG_DATA_BUFFER_SIZE 2048
#else
/**
 * @brief Log Data Buffer
 *
 * This determines how much RAM is reserved to buffer log records before
 * transmission. Each record consumes 4 bytes for the timestamp plus 4 bytes
 * for each logged variable. Increasing this value too far can crash the
 * device! The number of log records buffered is controlled by sendEveryX.
 *
 * This can be changed by setting the build flag MS_LOG_DATA_BUFFER_SIZE when
 * compiling. 8192 bytes is a safe value for the Mayfly 1.1 with six variables.
 */
#define MS_LOG_DATA_BUFFER_SIZE 8192
#endif
#endif

#include <stddef.h>
#include <inttypes.h>

/**
 * @brief This class buffers logged timestamps and variable values for
 * transmission. The log is divided into a number of records. Each record
 * stores the timestamp of the record as a uint32_t, then the value of each
 * variable as a float at that time.
 */
class LogBuffer {
 public:
    /**
     * @brief Constructs a new empty buffer which stores no variables or values.
     */
    LogBuffer();
    /**
     * @brief Destroys the buffer.
     */
    virtual ~LogBuffer();

    /**
     * @brief Sets the number of variables the buffer will store in each record.
     * Clears the buffer as a side effect.
     *
     * @param numVariables_  The number of variables to store.
     */
    void setNumVariables(uint8_t numVariables_);

    /**
     * @brief Gets the number of variables that will be stored in each record.
     *
     * @return The variable count.
     */
    uint8_t getNumVariables(void);

    /**
     * @brief Clears all records from the log.
     */
    void clear(void);

    /**
     * @brief Gets the number of records currently in the log.
     *
     * @return The number of records.
     */
    int getNumRecords(void);

    /**
     * @brief Computes the percentage full of the buffer.
     *
     * @return The current percent full.
     */
    uint8_t getPercentFull(void);

    /**
     * @brief Adds a new record with the given timestamp.
     *
     * @param timestamp  The timestamp
     *
     * @return Index of the new record, or -1 if there was no space.
     */
    int addRecord(uint32_t timestamp);

    /**
     * @brief Sets the value of a particular variable in a particular record.
     *
     * @param record    The record
     * @param variable  The variable
     * @param value     The value
     */
    void setRecordValue(int record, uint8_t variable, float value);

    /**
     * @brief Gets the timestamp of a particular record.
     *
     * @param record  The record
     *
     * @return The record's timestamp.
     */
    uint32_t getRecordTimestamp(int record);

    /**
     * @brief Gets the value of a particular variable in a particular record.
     *
     * @param record    The record
     * @param variable  The variable
     *
     * @return The variable's value.
     */
    float getRecordValue(int record, uint8_t variable);

 protected:
    /**
     * @brief Buffer which stores the log data.
     */
    uint8_t dataBuffer[MS_LOG_DATA_BUFFER_SIZE];

    /**
     * @brief Index of buffer head.
     */
    uint16_t dataBufferTail;
    /**
     * @brief Index of buffer tail.
     */
    uint16_t dataBufferHead;
    /**
     * @brief The buffer overflow status
     */
    bool _bufferOverflow = false;

    /**
     * @brief Number of records currently in the buffer.
     */
    int numRecords;

    /**
     * @brief Size in bytes of each record in the buffer.
     */
    size_t recordSize;

    /**
     * @brief Number of variables stored in each record in the buffer.
     */
    uint8_t numVariables;
};

#endif  // SRC_LOGBUFFER_H_
