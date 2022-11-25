/*
MIT License

Copyright (c) 2022 jp-rad

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef S3LINK_BLE_SERVICE_H
#define S3LINK_BLE_SERVICE_H

#include "pxt.h"

// S3Link UDK custom Event Bus ID. (base:32768)
#define CUSTOM_EVENT_ID_S3LINK_UDK (32768 + 5)
// Event Value
#define S3LINK_UDK_EVT_DISPLAY_TEXT_BUFFER_FULL 0x01
#define S3LINK_UDK_EVT_DISPLAY_CLEAR 0x02
#define S3LINK_UDK_EVT_DISPLAY_TEXT 0x81
#define S3LINK_UDK_EVT_DISPLAY_SYMBOL 0x82

#define DISPLAY_TEXT_BUFFER_SIZE 5

class S3LinkBLEServiceBase : MicroBitComponent
{
  
public:

    /**
      * Constructor.
      */
    S3LinkBLEServiceBase();

    /**
      * Setters.
      */
    void setTiltX(const int16_t value);
    void setTiltY(const int16_t value);
    
    typedef enum uBit_cIdx
    {
        uBit_cIdxButtonA,
        uBit_cIdxButtonB,
        uBit_cIdxTouchPinP0,
        uBit_cIdxTouchPinP1,
        uBit_cIdxTouchPinP2,
        uBit_cIdxGestureShaken,
        uBit_cIdxGestureJumped,
        uBit_cIdxGestureMoved,
        uBit_cIdxCOUNT
    } uBit_cIdx;

    void setSensor(const uBit_cIdx sensor, const int valueOrCounter);
    
    bool nextDisplayText();

protected:
    
    /**
      * Notify sensors data. 
      */
    void update();

private:

    // next sample timestamp scheduled
    static const uint64_t SENSOR_UPDATE_PERIOD_US = 24000; // 24ms
    uint64_t updateSampleTimestamp = 0;
    
private:

    int16_t tiltX;
    int16_t tiltY;
    
    int uBitSensorValueOrCounter[ uBit_cIdxCOUNT];  // zero, positive, negative. negative is to count up to zero.

    int displayTextBufferHead;
    int displayTextBufferLength;
    ManagedString displayTextBuffers[DISPLAY_TEXT_BUFFER_SIZE];

public:
    ManagedString DisplayText;

protected:
    // Characteristic buffer
    uint8_t rxCharBuffer[20]; // display command (from S3 to device)
    uint8_t txCharBuffer[20]; // sensors data (from device to S3)

    /**
      * Callback. Invoked when any of our attributes are written via BLE, CODAL/DAL.
      */
    void onTxCharValueWritten(const uint8_t *data, const uint16_t length);

private:

    // BLE wrapper methods.
    virtual bool getGapStateConnected() = 0;
    virtual void sendRxCharValue(const uint8_t *data, const uint16_t length) = 0;

};

//================================================================
#if MICROBIT_CODAL
//================================================================

#include "MicroBitBLEManager.h"
#include "MicroBitBLEService.h"

class S3LinkBLEService : public S3LinkBLEServiceBase , public MicroBitBLEService
{
  
public:
    
    /**
      * Constructor.
      */
    S3LinkBLEService();

    /**
      * Periodic callback from MicroBit scheduler.
      * (MicroBitComponent(=CodalComponent))
      */
    virtual void idleCallback();

  private:

    // BLE wrapper methods.
    bool getGapStateConnected();
    void sendRxCharValue(const uint8_t *data, const uint16_t length);

protected:

    /**
      * Callback. Invoked when any of our attributes are written via BLE.
      */
    void onDataWritten(const microbit_ble_evt_write_t *params);

private:
    
    // Index for each charactersitic in arrays of handles and UUIDs
    typedef enum mbbs_cIdx
    {
        mbbs_cIdxRxChar,
        mbbs_cIdxTxChar,
        mbbs_cIdxCOUNT
    } mbbs_cIdx;

    // UUIDs for our service and characteristics
    static const uint8_t  service_base_uuid[ 16];
    static const uint8_t  char_base_uuid[ 16];
    static const uint16_t serviceUUID;
    static const uint16_t charUUID[ mbbs_cIdxCOUNT];
    
    // Data for each characteristic when they are held by Soft Device.
    MicroBitBLEChar      chars[ mbbs_cIdxCOUNT];

public:
    
    int              characteristicCount()          { return mbbs_cIdxCOUNT; };
    MicroBitBLEChar *characteristicPtr( int idx)    { return &chars[ idx]; };

};

//================================================================
#else // MICROBIT_CODAL
//================================================================

#include "ble/BLE.h"

class S3LinkBLEService : public S3LinkBLEServiceBase
{

public:

    /**
      * Constructor.
      */
    S3LinkBLEService();

    /**
      * Periodic callback from MicroBit idle thread.
      * (MicroBitComponent)
      */
    virtual void idleTick();

private:

    // BLE wrapper methods.
    bool getGapStateConnected();
    void sendRxCharValue(const uint8_t *data, const uint16_t length);

private:

    /**
      * Callback. Invoked when any of our attributes are written via BLE.
      */
    void onDataWritten(const GattWriteCallbackParams *params);

    // Bluetooth stack we're running on.
    BLEDevice &ble;
    
    // Handles to access each characteristic when they are held by Soft Device.
    GattAttribute::Handle_t rxCharHandle;
    GattAttribute::Handle_t txCharHandle;

    // UUIDs for our service and characteristics
    static const uint16_t serviceUUID;
    static const uint8_t rxCharUUID[ 16];
    static const uint8_t txCharUUID[ 16];
    
};

//================================================================
#endif // MICROBIT_CODAL
//================================================================

#endif //S3LINK_BLE_SERVICE_H
