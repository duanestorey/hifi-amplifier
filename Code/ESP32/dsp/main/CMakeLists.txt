

idf_component_register(SRCS 
    "main.cpp" 
    "queue.cpp" 
    "mutex.cpp" 
    "debug.cpp" 
    "timer.cpp" 
    "dsp.cpp" 
    "i2c-bus.cpp" 
    "pipeline.cpp" 
    "biquad.cpp" 
    "i2s.cpp" 
    "circ-buffer.cpp" 
    "profile.cpp"
    "asm/dsps_biquad_f32_skip_aes3.S"
    "asm/dsps_mulc_f32_skip_ae32.S"
    INCLUDE_DIRS "."
)
