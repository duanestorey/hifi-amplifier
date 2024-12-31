#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "esp_log.h"
#include <sstream>

#define AMP_DEBUG_I(FORMAT, ...) ESP_LOGI( __PRETTY_FUNCTION__, FORMAT, ##__VA_ARGS__ )
#define AMP_DEBUG_INT_I(FORMAT, ...) ESP_DRAM_LOGI( __PRETTY_FUNCTION__, FORMAT, ##__VA_ARGS__ )
#define AMP_DEBUG_W(FORMAT, ...) ESP_LOGW( __PRETTY_FUNCTION__, FORMAT, ##__VA_ARGS__ )
#define AMP_DEBUG_E(FORMAT, ...) ESP_LOGE( __PRETTY_FUNCTION__, FORMAT, ##__VA_ARGS__ )

#endif 