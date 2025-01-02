#ifndef __DSP_CUSTOM_H
#define __DSP_CUSTOM_H

extern "C" {

esp_err_t dsps_biquad_f32_skip_aes3(const float* input, float* output, int len, float* coef, float* w);
esp_err_t dsps_mulc_f32_skip_ae32(const float *input, float *output, int len, float C, int step_in, int step_out);

}

#endif