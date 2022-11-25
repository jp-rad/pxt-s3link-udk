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

#include "pxt.h"
#include "S3LinkBLEService.h"

S3LinkBLEServiceBase::S3LinkBLEServiceBase()
{
    // MicroBitComponent/CodalComponent
    id = CUSTOM_EVENT_ID_S3LINK_UDK;
    status = 0;

    // Sensors
    tiltX = 0;
    tiltY = 0;
    memset(&uBitSensorValueOrCounter, 0, sizeof(uBitSensorValueOrCounter));

    // Display
    DisplayText = "";
    
    displayTextBufferHead = 0;
    displayTextBufferLength = 0;

    // Initialise our characteristic values.
    memset(&rxCharBuffer, 0, sizeof(rxCharBuffer));
    memset(&txCharBuffer, 0, sizeof(txCharBuffer));

    // Enable callback, idleTick()/idleCallback().
    fiber_add_idle_component(this);

}

void S3LinkBLEServiceBase::setTiltX(const int16_t value)
{
    tiltX = value;
}

void S3LinkBLEServiceBase::setTiltY(const int16_t value)
{
    tiltY = value;
}

void S3LinkBLEServiceBase::setSensor(const uBit_cIdx sensor, const int valueOrCounter)
{
    if (sensor < uBit_cIdxCOUNT) {
        uBitSensorValueOrCounter[sensor] = valueOrCounter;
    }
}

bool S3LinkBLEServiceBase::nextDisplayText()
{
    if (displayTextBufferLength > 0)
    {
        DisplayText = displayTextBuffers[displayTextBufferHead];
        displayTextBufferLength -= 1;
        displayTextBufferHead = (displayTextBufferHead + 1) % DISPLAY_TEXT_BUFFER_SIZE;
        return true;
    } else {
        // empty
        return false;
    }
}

void S3LinkBLEServiceBase::update(void)
{
    uint64_t currentTime = system_timer_current_time_us();
    if (currentTime < updateSampleTimestamp)
    {
        return;
    }

    // Schedule our next sample.
    updateSampleTimestamp = currentTime + SENSOR_UPDATE_PERIOD_US;
    
    // RxChar - notify 
    if (!getGapStateConnected())
    {
        return;
    }
    rxCharBuffer[0] = (tiltX>>8) & 0xFF;
    rxCharBuffer[1] = tiltX & 0xFF;
    rxCharBuffer[2] = (tiltY>>8) & 0xFF;
    rxCharBuffer[3] = tiltY & 0xFF;
    rxCharBuffer[4] = 0 == uBitSensorValueOrCounter[uBit_cIdxButtonA] ? 0 : 1;
    rxCharBuffer[5] = 0 == uBitSensorValueOrCounter[uBit_cIdxButtonB] ? 0 : 1;
    rxCharBuffer[6] = 0 == uBitSensorValueOrCounter[uBit_cIdxTouchPinP0] ? 0 : 1;
    rxCharBuffer[7] = 0 == uBitSensorValueOrCounter[uBit_cIdxTouchPinP1] ? 0 : 1;
    rxCharBuffer[8] = 0 == uBitSensorValueOrCounter[uBit_cIdxTouchPinP2] ? 0 : 1;
    rxCharBuffer[9] = (0 == uBitSensorValueOrCounter[uBit_cIdxGestureShaken] ? 0 : 1)   // bit0(1):shaken
                    | (0 == uBitSensorValueOrCounter[uBit_cIdxGestureJumped] ? 0 : 2)   // bit1(2):jumped
                    | (0 == uBitSensorValueOrCounter[uBit_cIdxGestureMoved ] ? 0 : 4);  // bit2(4):moved
    //rxCharBuffer[19] = rxCharBuffer[19] + 1; // update counter
    sendRxCharValue((const uint8_t *)&rxCharBuffer, (const uint16_t)sizeof(rxCharBuffer));
    
    // one shot counter, count up to zero.
    for(int i=0; i<uBit_cIdxCOUNT; i++)
    {
        if (uBitSensorValueOrCounter[i]<0)
        {
            uBitSensorValueOrCounter[i] += 1;
        }
    }
}

void S3LinkBLEServiceBase::onTxCharValueWritten(const uint8_t *data, const uint16_t length)
{
    if (data[0] == 0x81)
    {
        if (displayTextBufferLength<DISPLAY_TEXT_BUFFER_SIZE)
        {
            char text[length];
            memcpy(text, &(data[1]), length - 1);
            text[length - 1] = '\0';
            ManagedString message(text);
            int pos = (displayTextBufferHead + displayTextBufferLength) % DISPLAY_TEXT_BUFFER_SIZE;
            displayTextBuffers[pos] = message;
            displayTextBufferLength += 1;
            MicroBitEvent(id, S3LINK_UDK_EVT_DISPLAY_TEXT);
        } else {
            // full
            MicroBitEvent(id, S3LINK_UDK_EVT_DISPLAY_TEXT_BUFFER_FULL);
        }
    }
    else if ((data[0] == 0x82) && (length == 6))
    {
        uint8_t matrix[25];
        bool ledClear = true;
        for (int row = 0; row < 5; row++)
        {
            uint8_t rowData = data[row+1];
            if (rowData>0) {
                ledClear = false;
            }
            for (int col = 0; col < 5; col++)
            {
                matrix[col + row * 5] = (rowData & (0x01 << col)) ? 255 : 0;
            }
        }
        MicroBitImage image(5, 5, matrix);
        uBit.display.stopAnimation();
        uBit.display.print(image);
        MicroBitEvent(id, ledClear ? S3LINK_UDK_EVT_DISPLAY_CLEAR : S3LINK_UDK_EVT_DISPLAY_SYMBOL);
    }

}


//================================================================
#if MICROBIT_CODAL
//================================================================

S3LinkBLEService::S3LinkBLEService()
{
    
    // Register the base UUID and create the service.
    RegisterBaseUUID( service_base_uuid);
    CreateService( serviceUUID);

    // Register the base UUID and create the characteristics.
    RegisterBaseUUID( char_base_uuid);
    CreateCharacteristic( mbbs_cIdxRxChar, charUUID[ mbbs_cIdxRxChar],
                         (uint8_t *)&rxCharBuffer,
                         sizeof(rxCharBuffer), sizeof(rxCharBuffer),
                         microbit_propREAD | microbit_propNOTIFY);
    CreateCharacteristic( mbbs_cIdxTxChar, charUUID[ mbbs_cIdxTxChar],
                         (uint8_t *)&txCharBuffer,
                         sizeof(txCharBuffer), sizeof(txCharBuffer),
                         microbit_propWRITE);
    
    // Default values.
    writeChrValue(mbbs_cIdxRxChar
         ,(const uint8_t *)&rxCharBuffer, sizeof(rxCharBuffer));

}

void S3LinkBLEService::idleCallback()
{
    update();
}

bool S3LinkBLEService::getGapStateConnected()
{
    return getConnected();
}

void S3LinkBLEService::sendRxCharValue(const uint8_t *data, const uint16_t length)
{
    notifyChrValue(mbbs_cIdxRxChar, data, length);
}

void S3LinkBLEService::onDataWritten(const microbit_ble_evt_write_t *params)
{
    microbit_charattr_t type;
    int index = charHandleToIdx(params->handle, &type);

    if (index == mbbs_cIdxTxChar && params->len >= 1)
    {
        onTxCharValueWritten((const uint8_t *)params->data, (const uint16_t)params->len);
    }
}

// base uuid : {00000000-0000-1000-8000-00805F9B34FB}
const uint8_t  S3LinkBLEService::service_base_uuid[ 16] =
{ 0x00,0x00,0x00,0x00, 0x00,0x00, 0x10,0x00, 0x80,0x00, 0x00,0x80,0x5f,0x9b,0x34,0xfb };

// base uuid : {52610000-FA7E-42AB-850B-7C80220097CC}
const uint8_t  S3LinkBLEService::char_base_uuid[ 16] =
{ 0x52,0x61,0x00,0x00, 0xfa,0x7e, 0x42,0xab, 0x85,0x0b, 0x7c,0x80,0x22,0x00,0x97,0xcc };

// uuid
const uint16_t S3LinkBLEService::serviceUUID = 0xf005;
const uint16_t S3LinkBLEService::charUUID[ mbbs_cIdxCOUNT] = 
{
    0xda01, /* mbbs_cIdxRxChar */
    0xda02  /* mbbs_cIdxTxChar */
};

//================================================================
#else // MICROBIT_CODAL
//================================================================

S3LinkBLEService::S3LinkBLEService() :
    ble(*uBit.ble)
{
    
    // Caractieristic
    GattCharacteristic  rxChar(
        rxCharUUID
        , (uint8_t *)&rxCharBuffer, 0, sizeof(rxCharBuffer)
        , GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
    );
    GattCharacteristic  txChar(
        txCharUUID
        , (uint8_t *)&txCharBuffer, 0, sizeof(txCharBuffer)
        , GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE
    );

    // Set default security requirements
    rxChar.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
    txChar.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);

    // Service
    GattCharacteristic *characteristics[] = {
        &rxChar,
        &txChar,
    };
    GattService service(
        serviceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *)
    );
    ble.addService(service);

    // Characteristic Handle
    rxCharHandle = rxChar.getValueHandle();
    txCharHandle = txChar.getValueHandle();
    
    // Default values.
    ble.gattServer().write(rxCharHandle
        ,(const uint8_t *)&rxCharBuffer, sizeof(rxCharBuffer));
    
    // onDataWritten
    ble.onDataWritten(this, &S3LinkBLEService::onDataWritten);
    
}

void S3LinkBLEService::idleTick()
{
    update();
}

bool S3LinkBLEService::getGapStateConnected()
{
    return ble.getGapState().connected;
}

void S3LinkBLEService::sendRxCharValue(const uint8_t *data, const uint16_t length)
{
    ble.gattServer().notify(rxCharHandle, data, length);
}

void S3LinkBLEService::onDataWritten(const GattWriteCallbackParams *params)
{   
    if (params->handle == txCharHandle && params->len >= 1)
    {
        onTxCharValueWritten((const uint8_t *)params->data, (const uint16_t)params->len);
    }
}

// uuid
const uint16_t S3LinkBLEService::serviceUUID = 0xf005;  // short: 0xF005
// base uuid : {52610000-FA7E-42AB-850B-7C80220097CC}
const uint8_t  S3LinkBLEService::rxCharUUID[ 16] =
{ 0x52,0x61,0xda,0x01, 0xfa,0x7e, 0x42,0xab, 0x85,0x0b, 0x7c,0x80,0x22,0x00,0x97,0xcc }; // 0xDA01
const uint8_t  S3LinkBLEService::txCharUUID[ 16] =
{ 0x52,0x61,0xda,0x02, 0xfa,0x7e, 0x42,0xab, 0x85,0x0b, 0x7c,0x80,0x22,0x00,0x97,0xcc }; // 0xDA02

//================================================================
#endif // MICROBIT_CODAL
//================================================================
