#ifndef __AMPLIFIER_H__
#define __AMPLIFIER_H__

#include "pins.h"
#include "timer.h"
#include "queue.h"

#include "state.h"
#include "mutex.h"
#include "i2c-bus.h"
#include "lcd.h"
#include "dac.h"
#include "channelsel.h"

#include "tmp100.h"
#include "encoder.h"
#include "button.h"
#include "http-server.h"
#include "cs8416.h"
#include "driver/rmt_rx.h"

class Amplifier {
public:
    Amplifier();
    void init();
    void handleDisplayThread();
    void handleAudioThread();
    void handleRadioThread();
    void handleAmplifierThread();

    void handleTimerThread();

    void _handleWifiCallback( int32_t event_id );
    void _handleDecoderISR();
    void _handlePowerButtonISR();
    void _handleVolumeButtonISR();
    void _handleVolumeButtonEncoderISR();
    void _handleInputButtonISR();
    void _handleInputButtonEncoderISR();
    void _handleNecRemoteCommand( uint8_t address, uint8_t command );

    void _handleIRInterrupt( const rmt_rx_done_event_data_t *edata );

    AmplifierState getCurrentState();
protected:
    void asyncUpdateDisplay();

    bool mWifiEnabled;   
    bool mWifiConnectionAttempts;
    bool mUpdatingFromNTP;
    bool mPoweredOn;
    bool mDigitalAudioStarted;

    Timer mTimer;

    Queue mDisplayQueue;
    Queue mAudioQueue;
    Queue mRadioQueue;
    Queue mAmplifierQueue;

    uint32_t mTimerID;
    uint32_t mButtonTimerID;
    uint32_t mReconnectTimerID;

    AmplifierState mState;
    I2CBUS mI2C;
    LCD *mLCD;

    DAC *mDAC;
    ChannelSel *mChannelSel;
    HTTP_Server *mWebServer;
    CS8416 *mSPDIF;

    TMP100 *mMicroprocessorTemp;

    Encoder mVolumeEncoder;
    Encoder mInputEncoder;

    uint32_t mAudioTimerID;
    uint32_t mSpdifTimerID;
    
    bool mPendingVolumeChange;
    uint32_t mPendingVolume;

    Button *mPowerButton;
    Button *mVolumeButton;
    Button *mInputButton;

    uint8_t mIRBuffer[ 218 ];
    rmt_channel_handle_t mIRChannel;

private:
    void configurePins();
    void configureOnePin( PIN pin, gpio_int_type_t interrupts, gpio_mode_t mode, gpio_pulldown_t pulldown, gpio_pullup_t pullup );

    void setupWifi();
    void attemptWifiConnect();
    void taskDelayInMs( uint32_t ms );

    void updateTimeFromNTP();
    void handleWifiCallback( int32_t event_id );

    void updateInput( uint8_t newInput );
    void updateConnectedStatus( bool connected, bool doActualUpdate = true );

    void updateDisplay();

    void setupPWM();
    void handlePowerButtonPress();
    void handleInputButtonPress();
    void handleVolumeButtonPress();
    void handleDecoderIRQ();

    void changeAmplifierState( uint8_t newState );
    void startDigitalAudio();
    void stopDigitalAudio();

    void activateButtonLight( bool activate = true );
    void setupRemoteReceiver();
    void doActualRemoteReceive();

    Mutex mStateMutex;

};

#endif