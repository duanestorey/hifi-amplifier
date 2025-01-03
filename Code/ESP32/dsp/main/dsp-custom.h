#ifndef __DSP_CUSTOM_H
#define __DSP_CUSTOM_H

extern "C" {

esp_err_t dsps_biquad_f32_skip_aes3(const float* input, float* output, int len, float* coef, float* w);
esp_err_t dsps_biquad_f32_skip_dft2_aes3(const float* input, float* output, int len, float* coef, float* w);

float dsps_to_float( int32_t i );

}

#endif