#include "dolby-sta310.h"
#include "debug.h"
#include "pins.h"
#include "message.h"

Dolby_STA310::Dolby_STA310( uint8_t addr, I2CBUS *bus ) : mAddr( addr ), mBus( bus ), mInitialized( false ), mRunning( false ), mMuted( true ), mPlaying( false ) {

}

void
Dolby_STA310::init() {

}

void 
Dolby_STA310::tick() {
    if ( mRunning ) {
        uint8_t head3;
        mBus->readRegisterByte( mAddr, Dolby_STA310::HEAD_3, head3 );

        uint8_t dtype = head3 & 0x1f;

        uint8_t freq;
        mBus->readRegisterByte( mAddr, Dolby_STA310::FREQ, freq );

        AMP_DEBUG_I( ".......freq dtype is %d %d",(int)freq, (int)dtype );  
    }
}   

void 
Dolby_STA310::startDolby() {
    softReset();

    if ( mInitialized ) {
        AMP_DEBUG_I( "Starting Dolby Play Routines" );

        configureAudioPLL();
        configureInterrupts( true );
        configureSync();
        configureSPDIF();
        configurePCMOUT();
        configureDecoder();
        configureAC3();

        mute( false );
        run();

        mRunning = true;
    }    
}

void 
Dolby_STA310::stopDolby() {
    if ( mRunning ) {
        mBus->writeRegisterByte( mAddr, Dolby_STA310::SOFT_MUTE, 1 );

        softReset( false );

        vTaskDelay( 100 / portTICK_PERIOD_MS );

        mRunning = false;
        mInitialized = false;
        mMuted = false;
        mPlaying = false;
    }
}

void 
Dolby_STA310::mute( bool enable ) {
    AMP_DEBUG_I( "Setting mute to %d", (int)enable );
	if ( enable ) {
		mBus->writeRegisterByte( mAddr, Dolby_STA310::MUTE, 1 );
	} else {
		mBus->writeRegisterByte( mAddr, Dolby_STA310::MUTE, 0 );
	}

	mMuted = enable;
}

void 
Dolby_STA310::run() {
	if ( !mRunning ) {
        AMP_DEBUG_I( "Attempting to run" );
        mBus->writeRegisterByte( mAddr, Dolby_STA310::RUN, 1 );

        mRunning = true;
    }
}

void 
Dolby_STA310::play( bool enable ) {
    AMP_DEBUG_I( "Setting PLAY to %d", (int)enable );
	if ( enable ) {
		mBus->writeRegisterByte( mAddr, Dolby_STA310::PLAY, 1 );
	} else {
		mBus->writeRegisterByte( mAddr, Dolby_STA310::PLAY, 0 );
	}

	mPlaying = enable;
}

void 
Dolby_STA310::softReset( bool output ) {
    AMP_DEBUG_I( "Performing soft reset" );
    mInitialized = false;
    mRunning = false;
    mMuted = false;
    mPlaying = false;

    mBus->writeRegisterByte( mAddr, Dolby_STA310::SOFT_MUTE, 1 );

    mBus->writeRegisterByte( mAddr, Dolby_STA310::SOFT_RESET, 1 );

    mBus->writeRegisterByte( mAddr, Dolby_STA310::BREAKPOINT, 8 );

    mBus->writeRegisterByte( mAddr, Dolby_STA310::CLOCK_CMD, 0 );

    int attempts = 0;
    	while ( attempts < 20 && !mInitialized ) {
		// We need to check for the device to say it's ready
		uint8_t result = 0;
        
        mBus->readRegisterByte( mAddr, 0xff, result );
		if ( result == 1 ) {
			// Device is ready
			mInitialized = true;

            AMP_DEBUG_I( "Initalized" );

            if ( output ) {
                enableAudioPLL();
                mute( true );
            }
        
            
		} else {
			attempts++;
			// if it's not ready, let's wait 5ms and try again
			vTaskDelay( 100 / portTICK_PERIOD_MS );
		}
	}

    if ( mInitialized ) {
        uint8_t ident = 0;
        uint8_t softVer = 0;
        mBus->readRegisterByte( mAddr, Dolby_STA310::IDENT, ident );
        mBus->readRegisterByte( mAddr, Dolby_STA310::SOFTVER, softVer );

        AMP_DEBUG_I( "Dolby is initialized, IDENT and VER are %d - %d", (int)ident, (int)softVer );
    }
}

void 
Dolby_STA310::setAttenuation( uint8_t db ) {
    // Not working
    mBus->writeRegisterByte( mAddr, 0x4E, db );
    mBus->writeRegisterByte( mAddr, 0x63, db );
    mBus->writeRegisterByte( mAddr, 0x67, 0 );
 
    uint8_t result = 0;
    while ( true ) {
        mBus->readRegisterByte( mAddr, 0x67, result );
        
        if ( result == 4 ) {
            break;
        } else {
            vTaskDelay( 10 / portTICK_PERIOD_MS );
        }
    }
}

void 
Dolby_STA310::enableAudioPLL(){
    AMP_DEBUG_I( "Enabling audio PLL" );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::ENABLE_PLL, 1 );
}

void
Dolby_STA310::configureAudioPLL() {
    AMP_DEBUG_I( "Configuring audio pll" );
    // 22 is only one that works

   // mBus->writeRegisterByte( mAddr, Dolby_STA310::PLL_CTRL, 30 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::PLL_CTRL, 22 );
}

void
Dolby_STA310::configureInterrupts( bool enableHDR  ){
    AMP_DEBUG_I( "Configuring interrupts" );

    uint8_t interrupts = Dolby_STA310::ERR | Dolby_STA310::SFR;
    if ( enableHDR ) {
        interrupts = interrupts | Dolby_STA310::HDR;
    }

    mBus->writeRegisterByte( mAddr, Dolby_STA310::INT1, interrupts );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::INT2, Dolby_STA310::RST | Dolby_STA310::LCK  | Dolby_STA310::FIO | Dolby_STA310::PCM );
}

void
Dolby_STA310::configureSync() {
    AMP_DEBUG_I( "Configuring sync" );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::PACKET_LOCK, 1 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::SYNC_LOCK, 3 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::ID_EN, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::ID, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::ID_EXT, 0 );
    
}

void 
Dolby_STA310::configureSPDIF() {
    AMP_DEBUG_I( "Configuring SPDIF" );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::SIN_SETUP, 11 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::CAN_SETUP, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AUTODETECT_ENA, 1 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AUTODETECT_SENS, 0 );
}

void 
Dolby_STA310::configurePCMOUT() {
    AMP_DEBUG_I( "Configuring PCMOUT" );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::SPDIF_CONF, 1 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::PCM_DIV, 1 );


    // 3 is precision, must be 24 bit
    // 8 is SONY
    // 32 is not right padded   
    // Looks like {d23-d0}{8*0}
    // 3 + 8 + 32 = FORMAT_SONY for DAC
    // 3 = FORMAT_I2S
    mBus->writeRegisterByte( mAddr, Dolby_STA310::PCM_CONF, 3 + 32 );
}

void 
Dolby_STA310::configureAC3() {
    AMP_DEBUG_I( "Configuring AC3" );
    // Turn off subwoofer, turn back on later
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_DECODE_LFE, 1 );

    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_COMP_MOD, 2 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_HDR, 0x0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_LDR, 0x0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_RPC, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_KARAOKE, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_DUALMODE, 0 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::AC3_DOWNMIX, 4 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::OCFG, 2 + 64 );
}

void 
Dolby_STA310::configureDecoder() {
    AMP_DEBUG_I( "Configuring Decoder" );
    // Dolby
    //mBus->writeRegisterByte( mAddr, Dolby_STA310::STREAM_SEL, 5 );
   // mBus->writeRegisterByte( mAddr, Dolby_STA310::DECODE_SEL, 0 );

    mBus->writeRegisterByte( mAddr, Dolby_STA310::STREAM_SEL, 5 );
    mBus->writeRegisterByte( mAddr, Dolby_STA310::DECODE_SEL, 0 );
}

void 
Dolby_STA310::checkForInterrupt() {
    if ( !gpio_get_level( PIN_DECODER_IRQ ) ) {
       // handleInterrupt();
    }
}

void 
Dolby_STA310::handleInterrupt( Queue &queue ) {
    AMP_DEBUG_I( "Attempting to handle interrupt" );
    uint8_t result1 = 0;
    uint8_t result2 = 0;

    mBus->readRegisterByte( mAddr, Dolby_STA310::INT1_RES, result1 );
    mBus->readRegisterByte( mAddr, Dolby_STA310::INT2_RES, result2 );

    AMP_DEBUG_I( "...interrupt registeres are %d %d", (int)result1, (int)result2 );
    if ( result1 & Dolby_STA310::ERR ) {
        // error
        AMP_DEBUG_I( "...error condition" );
        uint8_t error = 0;
        mBus->readRegisterByte( mAddr, Dolby_STA310::ERROR, error );
        AMP_DEBUG_I("..... error was %d", (int)error );
    }

    if ( result1 & Dolby_STA310::HDR ) {
        AMP_DEBUG_I( "...HDR set" );

        uint8_t head3;
        uint8_t head4;

        mBus->readRegisterByte( mAddr, Dolby_STA310::HEAD_3, head3 );
        mBus->readRegisterByte( mAddr, Dolby_STA310::HEAD_4, head4 );

        uint8_t dtype = head3 & 0x1f;

         uint8_t freq;
        mBus->readRegisterByte( mAddr, Dolby_STA310::FREQ, freq );

        AMP_DEBUG_I(".......head was %d %d %d %d", (int)head3, (int)head4, (int)freq, (int)dtype );

        unsigned int samplingRate = 0;
        switch( freq ) {
            case 0:
                samplingRate = 48000;
                break;

        }

        queue.add( Message::MSG_AUDIO_SAMPLING_RATE_CHANGE, samplingRate );
        configureInterrupts( false );
    }

    if ( result1 & Dolby_STA310::SFR ) {
        uint8_t freq;
        mBus->readRegisterByte( mAddr, Dolby_STA310::FREQ, freq );

        AMP_DEBUG_I( "...SFR set %d", (int)freq );
    }

    if ( result2 & LCK ) {
        AMP_DEBUG_I( "...LCK set" );
    }   

    if ( result2 & RST ) {
        AMP_DEBUG_I( "...RST set" );
    }

    if ( result2 & SYN ) {
        AMP_DEBUG_I( "...SYN set" );

        uint8_t status;
        mBus->readRegisterByte( mAddr, Dolby_STA310::SYNC_STATUS, status );

        AMP_DEBUG_I( "..... SYNC status is %d", (int)status );
    }

    bool forceReset = false;
    if ( result2 & Dolby_STA310::FIO  ) {
         AMP_DEBUG_I( "...FIO set" ); 
    }

    if ( result2 & Dolby_STA310::PCM ) {
        AMP_DEBUG_I( "...PCM set" ); 
    }

    if ( ( result2 & RST ) | ( result2 & LCK ) | forceReset ) {
        AMP_DEBUG_I( "Need to reset decoder" ); 

        uint8_t stream;
        uint8_t decode;

        mBus->readRegisterByte( mAddr, Dolby_STA310::STREAM_SEL, stream );
        mBus->readRegisterByte( mAddr, Dolby_STA310::DECODE_SEL, decode );

        softReset();

        configureInterrupts( true );
  
        if ( stream == 3 ) {
            // This means we've crapped out
            // let's force going to 5, 0
           // configureDecoder();
        }  else {
            configureAC3();
        }
       

        AMP_DEBUG_I( "...STream/Decode set to %d %d", (int)stream, (int)decode );

        mute( false );
        run();

        // Maybe defer this until we have a valid header?
        play( true );
    }

    AMP_DEBUG_I( "Interrupt processed" );
}

