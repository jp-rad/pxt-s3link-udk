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

/**
 * S3LinkEvent
 */
enum S3LinkEvent {
    //% block="A"
    ButtonA = 1,
    //% block="B"
    ButtonB = 2,
    //% block="P0"
    TouchPinP0 = 3,
    //% block="P1"
    TouchPinP1 = 4,
    //% block="P2"
    TouchPinP2 = 5,
    //% block="shaken"
    GestureShaken = 6,
    //% block="jumped"
    GestureJumped = 7,
    //% block="moved"
    GestureMoved = 8
}

/**
 * S3Link UDK blocks.
 */
//% weight=101 color=#696969 icon="\uf0c1"
//% block="S3Link UDK"
namespace s3linkudk {
    // icon="\uf0c1" https://fontawesome.com/v5/icons/link
    
    let initialized: boolean = false;
    let cbDisplayTextReceived: (text: string) => void;
    
    function init() {
        if (initialized) return;
        initialized = true;
        registerDisplayDataReceived(handlerDisplayDataReceived);
    }

    export let lastText: string = "";
    function handlerDisplayDataReceived() {
        while (nextDisplayText()) {
            lastText = getDisplayText();
            if (cbDisplayTextReceived) {
                cbDisplayTextReceived(lastText);
            }
        }
    }

    /**
     * Register display data received.
     */
    //% weight=0
    //% blockId=s3linkudk_register_display_data_received
    //% block="UDK register display data received"
    //% deprecated=true blockHidden=1
    //% shim=s3linkudk::registerDisplayDataReceived
    export function registerDisplayDataReceived(handler: () => void) {
        // Block definition only.
        return;
    }

    /**
     * Next display text
     */
    //% weight=0
    //% blockid=s3linkudk_next_display_text
    //% block="UDK next display text"
    //% deprecated=true blockHidden=1
    //% shim=s3linkudk::nextDisplayText
    export function nextDisplayText(): boolean {
        // Block definition only.
        return false;
    }

    /**
     * Get display text
     */
    //% weight=0
    //% blockid=s3linkudk_get_display_text
    //% block="UDK get display text"
    //% deprecated=true blockHidden=1
    //% shim=s3linkudk::getDisplayText
    export function getDisplayText(): string {
        // Block definition only.
        return "";
    }

    /**
     * Start and advertise the S3Link BLE Service.
     */
    //% weight=40
    //% blockId=s3linkudk_start_service
    //% block="UDK start service"
    //% shim=s3linkudk::startService
    export function startService(): void {
        // Block definition only.
        return;
    }

    /**
     * Registers code to run when the device receives a display text.
     */
    //% weight=30
    //% blockId=s3linkudk_on_display_text_received_drag
    //% block="UDK on display text received"
    //% draggableParameters=reporter
    export function onDisplayTextReceived(cb: (text: string) => void): void {
        init();
        cbDisplayTextReceived = cb;
    }

    /**
     * Set tilt.
     * @param kind pitch or roll
     * @param value tilt
     */
    //% weight=23
    //% blockid=s3linkudk_set_tilt
    //% block="UDK set tilt %kind to %value"
    //% value.min=-32768 value.max=32767 value.defl=0
    //% shim=s3linkudk::setTilt
    export function setTilt(kind: Rotation, value: number): void {
        // Block definition only.
        return;
    }
    
    /**
     * Set sensor value or counter
     * @param event Scratch event for micro:bit
     * @param valueOrCounter value or counter
     */
    //% weight=22
    //% blockid=s3linkudk_set_sensor
    //% block="UDK set sensor %event to %valueOrCounter"
    //% deprecated=true blockHidden=1
    //% shim=s3linkudk::setSensorValueOrCounter
    export function setSensorValueOrCounter(event: S3LinkEvent, valueOrCounter: number): void {
        // Block definition only.
        return;
    }

    /**
     * Raise event.
     * @param event Scratch event for micro:bit
     */
    //% weight=21
    //% blockid=s3linkudk_raise_event
    //% block="UDK raise event %event"
    export function raiseEvent(event: S3LinkEvent): void {
        // 3 periods
        setSensorValueOrCounter(event, -3);
    }
    
    /**
     * Set state.
     * @param event Scratch event for micro:bit
     * @param on pressed, touched, occurred
     */
    //% weight=20
    //% blockid=s3linkudk_set_state
    //% block="UDK set state %event to %state"
    export function setState(event: S3LinkEvent, on: boolean): void {
        setSensorValueOrCounter(event, on ? 1 : 0);
    }
    
    /**
     * Register code to run when the micro:bit is connected to over Bluetooth
     * @param body Code to run when a Bluetooth connection is established
     */
    //% weight=11
    //% blockId=s3linkudk_on_connected
    //% block="UDK on connected"
    //% shim=s3linkudk::onConnected
    export function onConnected(body: () => void): void {
        // Block definition only.
        return;
    }

    /**
     * Register code to run when a bluetooth connection to the micro:bit is lost
     * @param body Code to run when a Bluetooth connection is lost
     */
    //% weight=10
    //% blockId=s3linkudk_on_disconnected
    //% block="UDK on disconnected"
    //% shim=s3linkudk::onDisconnected
    export function onDisconnected(body: () => void): void {
        // Block definition only.
        return;
    }

}
