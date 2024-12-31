#ifndef __DOLBY_STA310_H__
#define __DOLBY_STA310_H__

#include "i2c-bus.h"
#include "queue.h"

class Dolby_STA310 {
public:
	enum {
		SYN = 1,
		HDR = 2,
		ERR = 4,
		SFR = 8,
		DEM = 16,
		BOF = 32,
		PTR = 64,
		ANC = 128
	} INT1S;

	enum {
		PCM = 1,
		FBF = 2,
		FBE = 4,
		FIO = 8,
		RST = 16,
		LCK = 32,
		USD = 64,
		TBD = 128
	} INT2S;

    enum {
		IDENT = 0x01,
		SOFTVER = 0x71,
		FREQ = 0x05,
		INT1 = 0x07,
		INT2 = 0x08,
		ERROR = 0x0f,
		SOFT_RESET = 0x10,
		INT1_RES = 0x09,
		INT2_RES = 0x0a,
		SIN_SETUP = 0x0c,
		CAN_SETUP = 0x0d,
		PLL_CTRL = 0x12,
		PLAY = 0x13,
		MUTE = 0x14,
		STREAM_SEL = 0x4c,
		DECODE_SEL = 0x4d,
		PCM_DIV = 0x54,
		PCM_CONF = 0x55,
		SOFT_MUTE = 0x73,
		BREAKPOINT = 0x2b,
		CLOCK_CMD = 0x3a,
        SYNC_STATUS = 0x40,
        HEAD_4 = 0x42,
		HEAD_3 = 0x43,
        HEADLEN_1 = 0x44,
        HEADLEN_2 = 0x45,
		PACKET_LOCK = 0x4f,
		ID_EN = 0x50,
		ID = 0x51,
		ID_EXT = 0x52,
		SYNC_LOCK = 0x53,
		OCFG = 0x66,
		SPDIF_CONF = 0x60,
		AC3_DECODE_LFE = 0x68,
		AC3_COMP_MOD = 0x69,
		AC3_HDR = 0x6a,
		AC3_LDR = 0x6b,
		AC3_RPC = 0x6c,
		AC3_KARAOKE = 0x6d,
		AC3_DUALMODE = 0x6e,
		AC3_DOWNMIX = 0x6f,
		RUN = 0x72,
		DOLBY_STATUS_1 = 0x76,
		AC3_STATUS_1 = 0x77,
		ENABLE_PLL = 0xb5,
		AUTODETECT_ENA = 0xe0,
		AUTODETECT_SENS = 0xe1
	} REGISTERS;

    Dolby_STA310( uint8_t addr, I2CBUS *bus );

    void init();
    void mute( bool enable = true );
	void run();
	void play( bool enable = true );
    void handleInterrupt( Queue &queue );
    void checkForInterrupt();
    void setAttenuation( uint8_t db = 0 );

    void startDolby();
    void stopDolby();

    void tick();
protected:
    uint8_t mAddr;
    I2CBUS *mBus;
    bool mInitialized;
    bool mRunning;
    bool mMuted;
    bool mPlaying;
private:
	void softReset( bool output = true );
	void enableAudioPLL();
	void configureAudioPLL();
	void configureInterrupts( bool enableHDR = false );
	void configureSync();
	void configureSPDIF();
	void configurePCMOUT();
	void configureAC3();
	void configureDecoder();
};

#endif