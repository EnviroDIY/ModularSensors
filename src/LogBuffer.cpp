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
#include "LogBuffer.h"

#include <string.h>

// Constructor
LogBuffer::LogBuffer() {}
// Destructor
LogBuffer::~LogBuffer() {}

void LogBuffer::setNumVariables(uint8_t numVariables_) {
    // each record is one uint32_t to hold the timestamp, plus N floats to hold
    // each variable's value
    recordSize   = sizeof(uint32_t) + sizeof(float) * numVariables_;
    numVariables = numVariables_;

    // this scrambles all the data in the buffer so clear it out
    numRecords = 0;
}

void LogBuffer::clear(void) {
    // clear out the buffer
    numRecords = 0;
}

uint8_t LogBuffer::getNumVariables(void) {
    return numVariables;
}

int LogBuffer::getNumRecords(void) {
    return numRecords;
}

uint8_t LogBuffer::getPercentFull(void) {
    uint32_t bytesFull = (uint32_t)numRecords * (uint32_t)recordSize;
    uint32_t bytesTotal = MS_LOG_DATA_BUFFER_SIZE;

    return (uint8_t)((bytesFull*(uint32_t)100)/bytesTotal);
}

int LogBuffer::addRecord(uint32_t timestamp) {
    int record = numRecords;
    // compute position of the new record's timestamp in the buffer
    // (the timestamp is the first data in the record)
    size_t pos = record * recordSize;
    // verify we have sufficient space for the record and bail if not
    if (MS_LOG_DATA_BUFFER_SIZE - pos < recordSize) { return -1; }

    // write the timestamp to the record
    memcpy(static_cast<void*>(&dataBuffer[pos]), static_cast<void*>(&timestamp),
           sizeof(uint32_t));
    numRecords += 1;  // just added another record

    return record;
}

void LogBuffer::setRecordValue(int record, uint8_t variable, float value) {
    // compute position of this value in the buffer
    size_t pos = record * recordSize + sizeof(uint32_t) +
        variable * sizeof(float);

    // write the value to the record
    memcpy(static_cast<void*>(&dataBuffer[pos]), static_cast<void*>(&value),
           sizeof(float));
}

uint32_t LogBuffer::getRecordTimestamp(int record) {
    // read the timestamp from the record (which is the first data in it)
    uint32_t timestamp;
    memcpy(static_cast<void*>(&timestamp),
           static_cast<void*>(&dataBuffer[record * recordSize]),
           sizeof(uint32_t));

    return timestamp;
}

float LogBuffer::getRecordValue(int record, uint8_t variable) {
    // compute position of this value in the buffer
    size_t pos = record * recordSize + sizeof(uint32_t) +
        variable * sizeof(float);

    // read the value from the record
    float value;
    memcpy(static_cast<void*>(&value), static_cast<void*>(&dataBuffer[pos]),
           sizeof(float));

    return value;
}
