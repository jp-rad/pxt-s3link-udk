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
#include "lib/S3LinkBLEService.h"

namespace s3linkudk {
    
    S3LinkBLEService* _pService = NULL;

    //%
    void startService() {
        if (NULL != _pService) return;
        _pService = new S3LinkBLEService();
    }

    //%
    void registerDisplayDataReceived(Action handler) {
        registerWithDal(CUSTOM_EVENT_ID_S3LINK_UDK, S3LINK_UDK_EVT_DISPLAY_TEXT, handler);
    }
    
    //%
    bool nextDisplayText() {
        return _pService->nextDisplayText();
    }

    //%
    String getDisplayText() {
        return PSTR(_pService->DisplayText);
    }

    //%
    void setTilt(int kind, int value) {
        if (NULL == _pService) return;
        switch(kind) {
            case 0: // Picth
                _pService->setTiltY((int16_t)value);
                break;
            case 1: // Roll
                _pService->setTiltX((int16_t)value);
                break;
            default:
                break;
        }
    }

    //%
    void setSensorValueOrCounter(int event, int valueOrCounter)
    {
        if (NULL == _pService) return;
        S3LinkBLEService::uBit_cIdx sensor;
        switch (event) {
            case 1: // A
                sensor = S3LinkBLEService::uBit_cIdxButtonA;
                break;
            case 2: // B
                sensor = S3LinkBLEService::uBit_cIdxButtonB;
                break;
            case 3: // P0
                sensor = S3LinkBLEService::uBit_cIdxTouchPinP0;
                break;
            case 4: // P1
                sensor = S3LinkBLEService::uBit_cIdxTouchPinP1;
                break;
            case 5: // P2
                sensor = S3LinkBLEService::uBit_cIdxTouchPinP2;
                break;
            case 6: // Shaken
                sensor = S3LinkBLEService::uBit_cIdxGestureShaken;
                break;
            case 7: // Jumped
                sensor = S3LinkBLEService::uBit_cIdxGestureJumped;
                break;
            case 8: // Moved
                sensor = S3LinkBLEService::uBit_cIdxGestureMoved;
                break;
            default:
                return; // unknown
                break;
        }
        _pService->setSensor(sensor, valueOrCounter);
    }
    
    //%
    void onConnected(Action body) {
        registerWithDal(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, body);
    }

    //%
    void onDisconnected(Action body) {
        registerWithDal(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, body);
    }
    
}
