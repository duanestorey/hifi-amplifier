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
	.text
	.align  4
	.global dsps_to_float
	.type   dsps_to_float,@function


dsps_to_float: 
        entry   sp, 48
        FLOAT.S f0, a2, 1       

        ssi     f0, sp, 0
        rfr     a2, f0

	retw.n

#endif // dsps_biquad_f32_aes3_enabled
