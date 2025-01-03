// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License. 

#include "dsps_biquad_platform.h"
#if (dsps_biquad_f32_aes3_enabled == 1)

// This is bi quad filter form II for ESP32 processor.
	.text
	.align  4
	.global dsps_biquad_f32_skip_dft2_aes3
	.type   dsps_biquad_f32_skip_dft2_aes3,@function
// The function implements the following C code:
//esp_err_t dsps_biquad_f32_skip_aes3(const float* input, float* output, int len, float* coef, float* w)
//  {
//    for (int i=0 ; i< len ; i++)
//    {
//        float d0 = input[i] - coef[3]*w[0] - coef[4]*w[1]; (input[i] - a[1]*w[0] - a[2]*w[1];)
//        output[i] = coef[0]*d0 +  coef[1]*w[0] + coef[2]*w[1];
//        w[1] = w[0];
//        w[0] = d0;
//    }
//    return ESP_OK;
//  }

dsps_biquad_f32_skip_dft2_aes3: 
// input    - a2
// output   - a3
// len  - a4
// coeffs  - a5
// w- a6

// f0 - b0
// f1 - b1
// f2 - b2
// f3 - a1
// f4 - a2

// f6 - a1
// f7 - a2
	entry	a1, 16

	lsi   f0, a5, 0
	lsi   f1, a5, 4
	lsi   f2, a5, 8
	lsi   f3, a5, 12
	lsi   f4, a5, 16

	neg.s  f5, f3   // -a[1]
	neg.s  f6, f4   // -a[2]

	// lsi   f7, a6, 0 // w[0]
	lsi  f10, a6, 0		// f10 = w[0];
	lsi   f8, a6, 4 	// f8 = w[1]

	addi    a3, a3, -8       // i-- // preset a3
	lsi     f9, a2, 0        // f9 = x[i]

	loopnez a4, .loop_bq_end_m_aes3
		madd.s 	f10, f0, f9  	// f10 = w[0] +b0*x[i]
		madd.s 	f8, f1, f9  	// f8 = w[1] + b1*x[i]
		addi    a3, a3, 8    	// out++;
		addi    a2, a2, 8    	// in++;
		mul.s 	f11, f2, f9	 	// f11 = b2*x[i];
		madd.s 	f8, f5, f10  	// f8 = f8 - a1*y[i]
		ssi	    f10, a3, 0   	// y[i] = result
		madd.s 	f11, f6, f10	// f11 = f11 - a2*y[i]
		mov.s   f10, f8       	// w0 = w1
		lsi     f9, a2, 0       // f9 = x[i]
		mov.s   f8, f11       	// w1 = w2
.loop_bq_end_m_aes3:
		ssi     f10, a6, 0
		ssi     f8, a6, 4
	movi.n	a2, 0 // return status ESP_OK
	retw.n

#endif // dsps_biquad_f32_aes3_enabled
