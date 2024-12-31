/* Automatically generated file; DO NOT EDIT */
/* Espressif IoT Development Framework Linker Script */
/* Generated from: /Users/duane/Code/personal/projects/hifi-amplifier/Code/ESP32/build/esp-idf/esp_system/ld/sections.ld.in */

/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * Automatically generated file. DO NOT EDIT.
 * Espressif IoT Development Framework (ESP-IDF) 5.3.1 Configuration Header
 */
       
/* List of deprecated options */
/* CPU instruction prefetch padding size for flash mmap scenario */
/*
 * PMP region granularity size
 * Software may determine the PMP granularity by writing zero to pmp0cfg, then writing all ones
 * to pmpaddr0, then reading back pmpaddr0. If G is the index of the least-significant bit set,
 * the PMP granularity is 2^G+2 bytes.
 */
/* CPU instruction prefetch padding size for memory protection scenario */
/* Memory alignment size for PMS */
    /* rtc timer data (s_rtc_timer_retain_mem, see esp_clk.c files). For rtc_timer_data_in_rtc_mem section. */
/* Default entry point */
ENTRY(call_start_cpu0);
SECTIONS
{
  /**
   * RTC fast memory holds RTC wake stub code,
   * including from any source file named rtc_wake_stub*.c
   */
  .rtc.text :
  {
    
 . = ALIGN(4); 
 _rtc_text_start = ABSOLUTE(.);
    *(.rtc.literal .rtc.text .rtc.text.*)
    *rtc_wake_stub*.*(.literal .text .literal.* .text.*)
    _rtc_text_end = ABSOLUTE(.);
  } > rtc_iram_seg
  /**
   * This section is required to skip rtc.text area because rtc_iram_seg and
   * rtc_data_seg are reflect the same address space on different buses.
   */
  .rtc.dummy :
  {
    _rtc_dummy_start = ABSOLUTE(.);
    _rtc_fast_start = ABSOLUTE(.);
    . = SIZEOF(.rtc.text);
    _rtc_dummy_end = ABSOLUTE(.);
  } > rtc_data_seg
  /**
   * This section located in RTC FAST Memory area.
   * It holds data marked with RTC_FAST_ATTR attribute.
   * See the file "esp_attr.h" for more information.
   */
  .rtc.force_fast :
  {
    
 . = ALIGN(4); 
 _rtc_force_fast_start = ABSOLUTE(.);
    _coredump_rtc_fast_start = ABSOLUTE(.);
    *(.rtc.fast.coredump .rtc.fast.coredump.*)
    _coredump_rtc_fast_end = ABSOLUTE(.);
    *(.rtc.force_fast .rtc.force_fast.*)
    
 . = ALIGN(4); 
 _rtc_force_fast_end = ABSOLUTE(.);
  } > rtc_data_seg
  /**
   * RTC data section holds RTC wake stub
   * data/rodata, including from any source file
   * named rtc_wake_stub*.c and the data marked with
   * RTC_DATA_ATTR, RTC_RODATA_ATTR attributes.
   * The memory location of the data is dependent on
   * CONFIG_ESP32_RTCDATA_IN_FAST_MEM option.
   */
  .rtc.data :
  {
    _rtc_data_start = ABSOLUTE(.);
    _coredump_rtc_start = ABSOLUTE(.);
    *(.rtc.coredump .rtc.coredump.*)
    _coredump_rtc_end = ABSOLUTE(.);
    *(.rtc.data .rtc.data.*)
    *(.rtc.rodata .rtc.rodata.*)
    *rtc_wake_stub*.*(.data .rodata .data.* .rodata.*)
    _rtc_data_end = ABSOLUTE(.);
  } > rtc_data_location
  /* RTC bss, from any source file named rtc_wake_stub*.c */
  .rtc.bss (NOLOAD) :
  {
    _rtc_bss_start = ABSOLUTE(.);
    *rtc_wake_stub*.*(.bss .bss.*)
    *rtc_wake_stub*.*(COMMON)
    *(.rtc.bss)
    _rtc_bss_end = ABSOLUTE(.);
  } > rtc_data_location
  /**
   * This section holds data that should not be initialized at power up
   * and will be retained during deep sleep.
   * User data marked with RTC_NOINIT_ATTR will be placed
   * into this section. See the file "esp_attr.h" for more information.
   * The memory location of the data is dependent on
   * CONFIG_ESP32_RTCDATA_IN_FAST_MEM option.
   */
  .rtc_noinit (NOLOAD):
  {
    
 . = ALIGN(4); 
 _rtc_noinit_start = ABSOLUTE(.);
    *(.rtc_noinit .rtc_noinit.*)
    
 . = ALIGN(4); 
 _rtc_noinit_end = ABSOLUTE(.);
  } > rtc_data_location
  /**
   * This section located in RTC SLOW Memory area.
   * It holds data marked with RTC_SLOW_ATTR attribute.
   * See the file "esp_attr.h" for more information.
   */
  .rtc.force_slow :
  {
    
 . = ALIGN(4); 
 _rtc_force_slow_start = ABSOLUTE(.);
    *(.rtc.force_slow .rtc.force_slow.*)
    
 . = ALIGN(4); 
 _rtc_force_slow_end = ABSOLUTE(.);
  } > rtc_slow_seg
  /**
   * This section holds RTC FAST data that should have fixed addresses.
   * The data are not initialized at power-up and are retained during deep
   * sleep.
   */
  .rtc_fast_reserved (NOLOAD):
  {
    
 . = ALIGN(4); 
 _rtc_fast_reserved_start = ABSOLUTE(.);
    /**
     * New data can only be added here to ensure existing data are not moved.
     * Because data have adhered to the end of the segment and code is relied
     * on it.
     * >> put new data here <<
     */
    KEEP(*(.bootloader_data_rtc_mem .bootloader_data_rtc_mem.*))
    _rtc_fast_reserved_end = ABSOLUTE(.);
  } > rtc_fast_reserved_seg
  _rtc_fast_reserved_length = _rtc_fast_reserved_end - _rtc_fast_reserved_start;
  ASSERT((_rtc_fast_reserved_length <= LENGTH(rtc_fast_reserved_seg)),
          "RTC FAST reserved segment data does not fit.")
  /**
   * This section holds RTC SLOW data that should have fixed addresses.
   * The data are not initialized at power-up and are retained during deep
   * sleep.
   */
  .rtc_slow_reserved (NOLOAD):
  {
    
 . = ALIGN(4); 
 _rtc_slow_reserved_start = ABSOLUTE(.);
    /**
     * New data can only be added here to ensure existing data are not moved.
     * Because data have adhered to the end of the segment and code is relied
     * on it.
     * >> put new data here <<
     */
    *(.rtc_timer_data_in_rtc_mem .rtc_timer_data_in_rtc_mem.*)
    _rtc_slow_reserved_end = ABSOLUTE(.);
  } > rtc_slow_reserved_seg
  _rtc_slow_reserved_length = _rtc_slow_reserved_end - _rtc_slow_reserved_start;
  _rtc_reserved_length = _rtc_slow_reserved_length;
  ASSERT((_rtc_slow_reserved_length <= LENGTH(rtc_slow_reserved_seg)),
          "RTC SLOW reserved segment data does not fit.")
  /* Get size of rtc slow data based on rtc_data_location alias */
  _rtc_slow_length = (ORIGIN(rtc_slow_seg) == ORIGIN(rtc_data_location))
                        ? (_rtc_force_slow_end - _rtc_data_start)
                        : (_rtc_force_slow_end - _rtc_force_slow_start);
  _rtc_fast_length = (ORIGIN(rtc_slow_seg) == ORIGIN(rtc_data_location))
                        ? (_rtc_force_fast_end - _rtc_fast_start)
                        : (_rtc_noinit_end - _rtc_fast_start);
  ASSERT((_rtc_slow_length <= LENGTH(rtc_slow_seg)),
          "RTC_SLOW segment data does not fit.")
  ASSERT((_rtc_fast_length <= LENGTH(rtc_data_seg)),
          "RTC_FAST segment data does not fit.")
  /* Send .iram0 code to iram */
  .iram0.vectors :
  {
    _iram_start = ABSOLUTE(.);
    /* Vectors go to IRAM */
    _vector_table = ABSOLUTE(.);
    . = 0x0;
    KEEP(*(.WindowVectors.text));
    . = 0x180;
    KEEP(*(.Level2InterruptVector.text));
    . = 0x1c0;
    KEEP(*(.Level3InterruptVector.text));
    . = 0x200;
    KEEP(*(.Level4InterruptVector.text));
    . = 0x240;
    KEEP(*(.Level5InterruptVector.text));
    . = 0x280;
    KEEP(*(.DebugExceptionVector.text));
    . = 0x2c0;
    KEEP(*(.NMIExceptionVector.text));
    . = 0x300;
    KEEP(*(.KernelExceptionVector.text));
    . = 0x340;
    KEEP(*(.UserExceptionVector.text));
    . = 0x3C0;
    KEEP(*(.DoubleExceptionVector.text));
    . = 0x400;
    _invalid_pc_placeholder = ABSOLUTE(.);
    *(.*Vector.literal)
    *(.UserEnter.literal);
    *(.UserEnter.text);
    . = ALIGN (16);
    *(.entry.literal)
    *(.entry.text)
    *(.init.literal)
    *(.init)
    _init_end = ABSOLUTE(.);
  } > iram0_0_seg
  .iram0.text :
  {
    /* Code marked as running out of IRAM */
    _iram_text_start = ABSOLUTE(.);
    *(.iram1 .iram1.*)
    *libapp_trace.a:app_trace.*(.literal .literal.* .text .text.*)
    *libapp_trace.a:app_trace_util.*(.literal .literal.* .text .text.*)
    *libapp_trace.a:port_uart.*(.literal .literal.* .text .text.*)
    *libesp_driver_gptimer.a:gptimer.*(.literal.gptimer_default_isr .text.gptimer_default_isr)
    *libesp_driver_spi.a:spi_dma.*(.literal.spi_dma_reset .text.spi_dma_reset)
    *libesp_driver_spi.a:spi_dma.*(.literal.spi_dma_start .text.spi_dma_start)
    *libesp_event.a:default_event_loop.*(.literal.esp_event_isr_post .text.esp_event_isr_post)
    *libesp_event.a:esp_event.*(.literal.esp_event_isr_post_to .text.esp_event_isr_post_to)
    *libesp_hw_support.a:cpu.*(.literal.esp_cpu_compare_and_set .text.esp_cpu_compare_and_set)
    *libesp_hw_support.a:cpu.*(.literal.esp_cpu_reset .text.esp_cpu_reset)
    *libesp_hw_support.a:cpu.*(.literal.esp_cpu_stall .text.esp_cpu_stall)
    *libesp_hw_support.a:cpu.*(.literal.esp_cpu_unstall .text.esp_cpu_unstall)
    *libesp_hw_support.a:cpu.*(.literal.esp_cpu_wait_for_intr .text.esp_cpu_wait_for_intr)
    *libesp_hw_support.a:esp_memory_utils.*(.literal .literal.* .text .text.*)
    *libesp_hw_support.a:mspi_timing_tuning.*(.literal .literal.* .text .text.*)
    *libesp_hw_support.a:periph_ctrl.*(.literal.periph_module_reset .text.periph_module_reset)
    *libesp_hw_support.a:periph_ctrl.*(.literal.wifi_module_disable .text.wifi_module_disable)
    *libesp_hw_support.a:periph_ctrl.*(.literal.wifi_module_enable .text.wifi_module_enable)
    *libesp_hw_support.a:rtc_clk.*(.literal .literal.* .text .text.*)
    *libesp_hw_support.a:rtc_init.*(.literal.rtc_vddsdio_get_config .text.rtc_vddsdio_get_config)
    *libesp_hw_support.a:rtc_init.*(.literal.rtc_vddsdio_set_config .text.rtc_vddsdio_set_config)
    *libesp_hw_support.a:rtc_sleep.*(.literal .literal.* .text .text.*)
    *libesp_hw_support.a:rtc_time.*(.literal .literal.* .text .text.*)
    *libesp_hw_support.a:rtc_wdt.*(.literal .literal.* .text .text.*)
    *libesp_hw_support.a:sar_periph_ctrl.*(.literal.sar_periph_ctrl_power_enable .text.sar_periph_ctrl_power_enable)
    *libesp_mm.a:cache_esp32.*(.literal .literal.* .text .text.*)
    *libesp_mm.a:esp_cache.*(.literal .literal.* .text .text.*)
    *libesp_netif.a:esp_netif_lwip.*(.literal.esp_netif_free_rx_buffer .text.esp_netif_free_rx_buffer)
    *libesp_netif.a:esp_netif_lwip.*(.literal.esp_netif_receive .text.esp_netif_receive)
    *libesp_netif.a:esp_netif_lwip.*(.literal.esp_netif_transmit_wrap .text.esp_netif_transmit_wrap)
    *libesp_netif.a:esp_pbuf_ref.*(.literal.esp_pbuf_allocate .text.esp_pbuf_allocate)
    *libesp_netif.a:esp_pbuf_ref.*(.literal.esp_pbuf_free .text.esp_pbuf_free)
    *libesp_netif.a:ethernetif.*(.literal.ethernet_low_level_output .text.ethernet_low_level_output)
    *libesp_netif.a:ethernetif.*(.literal.ethernetif_input .text.ethernetif_input)
    *libesp_netif.a:wlanif.*(.literal.low_level_output .text.low_level_output)
    *libesp_netif.a:wlanif.*(.literal.wlanif_input .text.wlanif_input)
    *libesp_ringbuf.a:(EXCLUDE_FILE(*libesp_ringbuf.a:ringbuf.*) .literal EXCLUDE_FILE(*libesp_ringbuf.a:ringbuf.*) .literal.* EXCLUDE_FILE(*libesp_ringbuf.a:ringbuf.*) .text EXCLUDE_FILE(*libesp_ringbuf.a:ringbuf.*) .text.*)
    *libesp_ringbuf.a:ringbuf.*(.literal.vRingbufferDeleteWithCaps .literal.xRingbufferCreateWithCaps .literal.xRingbufferGetStaticBuffer .text .text.vRingbufferDeleteWithCaps .text.xRingbufferCreateWithCaps .text.xRingbufferGetStaticBuffer)
    *libesp_rom.a:esp_rom_spiflash.*(.literal .literal.* .text .text.*)
    *libesp_system.a:esp_err.*(.literal .literal.* .text .text.*)
    *libesp_system.a:esp_system_chip.*(.literal.esp_system_abort .text.esp_system_abort)
    *libesp_system.a:image_process.*(.literal .literal.* .text .text.*)
    *libesp_system.a:ubsan.*(.literal .literal.* .text .text.*)
    *libfreertos.a:(EXCLUDE_FILE(*libfreertos.a:app_startup.* *libfreertos.a:event_groups.* *libfreertos.a:freertos_compatibility.* *libfreertos.a:heap_idf.* *libfreertos.a:idf_additions.* *libfreertos.a:list.* *libfreertos.a:port.* *libfreertos.a:port_common.* *libfreertos.a:port_systick.* *libfreertos.a:queue.* *libfreertos.a:stream_buffer.* *libfreertos.a:tasks.* *libfreertos.a:timers.*) .literal EXCLUDE_FILE(*libfreertos.a:app_startup.* *libfreertos.a:event_groups.* *libfreertos.a:freertos_compatibility.* *libfreertos.a:heap_idf.* *libfreertos.a:idf_additions.* *libfreertos.a:list.* *libfreertos.a:port.* *libfreertos.a:port_common.* *libfreertos.a:port_systick.* *libfreertos.a:queue.* *libfreertos.a:stream_buffer.* *libfreertos.a:tasks.* *libfreertos.a:timers.*) .literal.* EXCLUDE_FILE(*libfreertos.a:app_startup.* *libfreertos.a:event_groups.* *libfreertos.a:freertos_compatibility.* *libfreertos.a:heap_idf.* *libfreertos.a:idf_additions.* *libfreertos.a:list.* *libfreertos.a:port.* *libfreertos.a:port_common.* *libfreertos.a:port_systick.* *libfreertos.a:queue.* *libfreertos.a:stream_buffer.* *libfreertos.a:tasks.* *libfreertos.a:timers.*) .text EXCLUDE_FILE(*libfreertos.a:app_startup.* *libfreertos.a:event_groups.* *libfreertos.a:freertos_compatibility.* *libfreertos.a:heap_idf.* *libfreertos.a:idf_additions.* *libfreertos.a:list.* *libfreertos.a:port.* *libfreertos.a:port_common.* *libfreertos.a:port_systick.* *libfreertos.a:queue.* *libfreertos.a:stream_buffer.* *libfreertos.a:tasks.* *libfreertos.a:timers.*) .text.*)
    *libfreertos.a:event_groups.*(.literal.xEventGroupGetBitsFromISR .literal.xEventGroupGetStaticBuffer .text .text.xEventGroupGetBitsFromISR .text.xEventGroupGetStaticBuffer)
    *libfreertos.a:heap_idf.*(.literal.xPortCheckValidListMem .text .text.xPortCheckValidListMem)
    *libfreertos.a:list.*(.text .text.uxListRemove .text.vListInsert .text.vListInsertEnd)
    *libfreertos.a:port.*(.literal.unlikely.vPortEndScheduler .literal.vApplicationStackOverflowHook .literal.vPortAssertIfInISR .literal.vPortExitCritical .literal.vPortExitCriticalCompliance .literal.vPortSetStackWatchpoint .literal.vPortYieldOtherCore .literal.xPortEnterCriticalTimeout .literal.xPortEnterCriticalTimeoutCompliance .literal.xPortInIsrContext .text .text.unlikely.vPortEndScheduler .text.vApplicationStackOverflowHook .text.vPortAssertIfInISR .text.vPortExitCritical .text.vPortExitCriticalCompliance .text.vPortSetStackWatchpoint .text.vPortYieldOtherCore .text.xPortEnterCriticalTimeout .text.xPortEnterCriticalTimeoutCompliance .text.xPortInIsrContext)
    *libfreertos.a:port_common.*(.text)
    *libfreertos.a:port_systick.*(.literal.xPortSysTickHandler .text .text.xPortSysTickHandler)
    *libfreertos.a:queue.*(.literal.prvCopyDataFromQueue .literal.prvCopyDataToQueue .literal.prvNotifyQueueSetContainer .literal.uxQueueMessagesWaitingFromISR .literal.xQueueGenericGetStaticBuffers .literal.xQueueGenericSendFromISR .literal.xQueueGetMutexHolderFromISR .literal.xQueueGiveFromISR .literal.xQueueIsQueueEmptyFromISR .literal.xQueueIsQueueFullFromISR .literal.xQueuePeekFromISR .literal.xQueueReceiveFromISR .literal.xQueueSelectFromSetFromISR .text .text.prvCopyDataFromQueue .text.prvCopyDataToQueue .text.prvNotifyQueueSetContainer .text.uxQueueMessagesWaitingFromISR .text.xQueueGenericGetStaticBuffers .text.xQueueGenericSendFromISR .text.xQueueGetMutexHolderFromISR .text.xQueueGiveFromISR .text.xQueueIsQueueEmptyFromISR .text.xQueueIsQueueFullFromISR .text.xQueuePeekFromISR .text.xQueueReceiveFromISR .text.xQueueSelectFromSetFromISR)
    *libfreertos.a:stream_buffer.*(.literal.prvInitialiseNewStreamBuffer$isra$0 .literal.prvReadMessageFromBuffer .literal.prvWriteMessageToBuffer .literal.xStreamBufferGetStaticBuffers .literal.xStreamBufferReceiveCompletedFromISR .literal.xStreamBufferReceiveFromISR .literal.xStreamBufferSendCompletedFromISR .literal.xStreamBufferSendFromISR .literal.xStreamBufferSpacesAvailable .text .text.prvBytesInBuffer .text.prvInitialiseNewStreamBuffer$isra$0 .text.prvReadMessageFromBuffer .text.prvWriteMessageToBuffer .text.xStreamBufferGetStaticBuffers .text.xStreamBufferReceiveCompletedFromISR .text.xStreamBufferReceiveFromISR .text.xStreamBufferSendCompletedFromISR .text.xStreamBufferSendFromISR .text.xStreamBufferSpacesAvailable)
    *libfreertos.a:tasks.*(.literal.__getreent .literal.prvCheckTaskCanBeScheduledSMP .literal.prvInitialiseNewTask$constprop$0 .literal.prvIsYieldRequiredSMP .literal.prvResetNextTaskUnblockTime .literal.prvTaskIsTaskSuspended .literal.pvTaskGetCurrentTCBForCore .literal.uxTaskPriorityGetFromISR .literal.vTaskGenericNotifyGiveFromISR .literal.vTaskGetSnapshot .literal.vTaskSwitchContext .literal.xTaskCatchUpTicks .literal.xTaskGenericNotifyFromISR .literal.xTaskGetSchedulerState .literal.xTaskGetStaticBuffers .literal.xTaskGetTickCount .literal.xTaskGetTickCountFromISR .literal.xTaskIncrementTick .literal.xTaskIncrementTickOtherCores .literal.xTaskRemoveFromEventList .literal.xTaskResumeFromISR .text .text.__getreent .text.prvCheckTaskCanBeScheduledSMP .text.prvInitialiseNewTask$constprop$0 .text.prvIsYieldRequiredSMP .text.prvResetNextTaskUnblockTime .text.prvTaskIsTaskSuspended .text.pvTaskGetCurrentTCBForCore .text.uxTaskPriorityGetFromISR .text.vTaskGenericNotifyGiveFromISR .text.vTaskGetSnapshot .text.vTaskSwitchContext .text.xTaskCatchUpTicks .text.xTaskGenericNotifyFromISR .text.xTaskGetSchedulerState .text.xTaskGetStaticBuffers .text.xTaskGetTickCount .text.xTaskGetTickCountFromISR .text.xTaskIncrementTick .text.xTaskIncrementTickOtherCores .text.xTaskRemoveFromEventList .text.xTaskResumeFromISR .text.xTimerCreateTimerTask)
    *libfreertos.a:timers.*(.literal.prvInitialiseNewTimer$part$0 .literal.xTimerGetReloadMode .literal.xTimerGetStaticBuffer .literal.xTimerPendFunctionCallFromISR .text .text.prvInitialiseNewTimer$part$0 .text.xTimerGetReloadMode .text.xTimerGetStaticBuffer .text.xTimerPendFunctionCallFromISR)
    *libgcc.a:lib2funcs.*(.literal .literal.* .text .text.*)
    *libgcov.a:(.literal .literal.* .text .text.*)
    *libhal.a:cache_hal_esp32.*(.literal .literal.* .text .text.*)
    *libhal.a:i2c_hal_iram.*(.literal .literal.* .text .text.*)
    *libhal.a:ledc_hal_iram.*(.literal .literal.* .text .text.*)
    *libhal.a:mmu_hal.*(.literal .literal.* .text .text.*)
    *libhal.a:spi_flash_encrypt_hal_iram.*(.literal .literal.* .text .text.*)
    *libhal.a:spi_flash_hal_iram.*(.literal .literal.* .text .text.*)
    *libhal.a:spi_slave_hal_iram.*(.literal .literal.* .text .text.*)
    *libhal.a:timer_hal.*(.literal.timer_hal_capture_and_get_counter_value .text.timer_hal_capture_and_get_counter_value)
    *libhal.a:wdt_hal_iram.*(.literal .literal.* .text .text.*)
    *liblog.a:log.*(.literal.esp_log_write .text.esp_log_write)
    *liblog.a:log_freertos.*(.literal.esp_log_early_timestamp .text.esp_log_early_timestamp)
    *liblog.a:log_freertos.*(.literal.esp_log_impl_lock .text.esp_log_impl_lock)
    *liblog.a:log_freertos.*(.literal.esp_log_impl_lock_timeout .text.esp_log_impl_lock_timeout)
    *liblog.a:log_freertos.*(.literal.esp_log_impl_unlock .text.esp_log_impl_unlock)
    *liblog.a:log_freertos.*(.literal.esp_log_timestamp .text.esp_log_timestamp)
    *liblwip.a:api_lib.*(.literal.netconn_recv .text.netconn_recv)
    *liblwip.a:api_lib.*(.literal.netconn_recv_data .text.netconn_recv_data)
    *liblwip.a:api_lib.*(.literal.netconn_recv_data_tcp .text.netconn_recv_data_tcp)
    *liblwip.a:api_lib.*(.literal.netconn_recv_tcp_pbuf_flags .text.netconn_recv_tcp_pbuf_flags)
    *liblwip.a:api_lib.*(.literal.netconn_recv_udp_raw_netbuf_flags .text.netconn_recv_udp_raw_netbuf_flags)
    *liblwip.a:api_lib.*(.literal.netconn_send .text.netconn_send)
    *liblwip.a:api_lib.*(.literal.netconn_sendto .text.netconn_sendto)
    *liblwip.a:api_lib.*(.literal.netconn_tcp_recvd .text.netconn_tcp_recvd)
    *liblwip.a:api_lib.*(.literal.netconn_write_partly .text.netconn_write_partly)
    *liblwip.a:api_lib.*(.literal.netconn_write_vectors_partly .text.netconn_write_vectors_partly)
    *liblwip.a:api_msg.*(.literal.lwip_netconn_do_send .text.lwip_netconn_do_send)
    *liblwip.a:api_msg.*(.literal.lwip_netconn_do_write .text.lwip_netconn_do_write)
    *liblwip.a:etharp.*(.literal.etharp_output .text.etharp_output)
    *liblwip.a:etharp.*(.literal.etharp_output_to_arp_index .text.etharp_output_to_arp_index)
    *liblwip.a:ethernet.*(.literal.ethernet_input .text.ethernet_input)
    *liblwip.a:ethernet.*(.literal.ethernet_output .text.ethernet_output)
    *liblwip.a:inet_chksum.*(.literal.inet_chksum_pseudo .text.inet_chksum_pseudo)
    *liblwip.a:inet_chksum.*(.literal.inet_cksum_pseudo_base .text.inet_cksum_pseudo_base)
    *liblwip.a:ip4.*(.literal.ip4_input .text.ip4_input)
    *liblwip.a:ip4.*(.literal.ip4_output .text.ip4_output)
    *liblwip.a:ip4.*(.literal.ip4_output_if .text.ip4_output_if)
    *liblwip.a:ip4.*(.literal.ip4_output_if_opt .text.ip4_output_if_opt)
    *liblwip.a:ip4.*(.literal.ip4_output_if_opt_src .text.ip4_output_if_opt_src)
    *liblwip.a:ip4.*(.literal.ip4_output_if_src .text.ip4_output_if_src)
    *liblwip.a:ip4.*(.literal.ip4_route .text.ip4_route)
    *liblwip.a:ip4.*(.literal.ip4_route_src .text.ip4_route_src)
    *liblwip.a:ip4_addr.*(.literal.ip4_addr_isbroadcast_u32 .text.ip4_addr_isbroadcast_u32)
    *liblwip.a:lwip_default_hooks.*(.literal.ip4_route_src_hook .text.ip4_route_src_hook)
    *liblwip.a:netbuf.*(.literal.netbuf_alloc .text.netbuf_alloc)
    *liblwip.a:netbuf.*(.literal.netbuf_free .text.netbuf_free)
    *liblwip.a:pbuf.*(.literal.pbuf_add_header .text.pbuf_add_header)
    *liblwip.a:pbuf.*(.literal.pbuf_alloc .text.pbuf_alloc)
    *liblwip.a:pbuf.*(.literal.pbuf_alloced_custom .text.pbuf_alloced_custom)
    *liblwip.a:pbuf.*(.literal.pbuf_free .text.pbuf_free)
    *liblwip.a:pbuf.*(.literal.pbuf_header .text.pbuf_header)
    *liblwip.a:pbuf.*(.literal.pbuf_remove_header .text.pbuf_remove_header)
    *liblwip.a:sockets.*(.literal.done_socket .text.done_socket)
    *liblwip.a:sockets.*(.literal.event_callback .text.event_callback)
    *liblwip.a:sockets.*(.literal.get_socket .text.get_socket)
    *liblwip.a:sockets.*(.literal.lwip_recv_tcp .text.lwip_recv_tcp)
    *liblwip.a:sockets.*(.literal.lwip_recvfrom .text.lwip_recvfrom)
    *liblwip.a:sockets.*(.literal.lwip_select .text.lwip_select)
    *liblwip.a:sockets.*(.literal.lwip_send .text.lwip_send)
    *liblwip.a:sockets.*(.literal.lwip_sendto .text.lwip_sendto)
    *liblwip.a:sockets.*(.literal.tryget_socket_unconn_locked .text.tryget_socket_unconn_locked)
    *liblwip.a:sys_arch.*(.literal.sys_arch_mbox_fetch .text.sys_arch_mbox_fetch)
    *liblwip.a:sys_arch.*(.literal.sys_arch_sem_wait .text.sys_arch_sem_wait)
    *liblwip.a:sys_arch.*(.literal.sys_mbox_post .text.sys_mbox_post)
    *liblwip.a:sys_arch.*(.literal.sys_mbox_trypost .text.sys_mbox_trypost)
    *liblwip.a:sys_arch.*(.literal.sys_mutex_lock .text.sys_mutex_lock)
    *liblwip.a:sys_arch.*(.literal.sys_mutex_unlock .text.sys_mutex_unlock)
    *liblwip.a:sys_arch.*(.literal.sys_sem_signal .text.sys_sem_signal)
    *liblwip.a:tcpip.*(.literal.tcpip_callback .text.tcpip_callback)
    *liblwip.a:tcpip.*(.literal.tcpip_inpkt .text.tcpip_inpkt)
    *liblwip.a:tcpip.*(.literal.tcpip_input .text.tcpip_input)
    *liblwip.a:tcpip.*(.literal.tcpip_send_msg_wait_sem .text.tcpip_send_msg_wait_sem)
    *liblwip.a:tcpip.*(.literal.tcpip_thread .text.tcpip_thread)
    *liblwip.a:tcpip.*(.literal.tcpip_try_callback .text.tcpip_try_callback)
    *liblwip.a:udp.*(.literal.udp_input .text.udp_input)
    *liblwip.a:udp.*(.literal.udp_input_local_match .text.udp_input_local_match)
    *liblwip.a:udp.*(.literal.udp_send .text.udp_send)
    *liblwip.a:udp.*(.literal.udp_sendto .text.udp_sendto)
    *liblwip.a:udp.*(.literal.udp_sendto_if .text.udp_sendto_if)
    *liblwip.a:udp.*(.literal.udp_sendto_if_src .text.udp_sendto_if_src)
    *libnewlib.a:abort.*(.literal .literal.* .text .text.*)
    *libnewlib.a:assert.*(.literal .literal.* .text .text.*)
    *libnewlib.a:heap.*(.literal .literal.* .text .text.*)
    *libnewlib.a:stdatomic.*(.literal .literal.* .text .text.*)
    *librtc.a:(.literal .literal.* .text .text.*)
    *libsoc.a:lldesc.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:flash_brownout_hook.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:memspi_host_driver.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_chip_boya.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_chip_gd.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_chip_generic.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_chip_issi.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_chip_mxic.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_chip_th.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_chip_winbond.*(.literal .literal.* .text .text.*)
    *libspi_flash.a:spi_flash_wrap.*(.literal .literal.* .text .text.*)
    *libxt_hal.a:(.literal .literal.* .text .text.*)
    *libxtensa.a:(EXCLUDE_FILE(*libxtensa.a:xt_trax.* *libxtensa.a:xtensa_intr.*) .literal EXCLUDE_FILE(*libxtensa.a:xt_trax.* *libxtensa.a:xtensa_intr.*) .literal.* EXCLUDE_FILE(*libxtensa.a:xt_trax.* *libxtensa.a:xtensa_intr.*) .text EXCLUDE_FILE(*libxtensa.a:xt_trax.* *libxtensa.a:xtensa_intr.*) .text.*)
  } > iram0_0_seg
  .dram0.data :
  {
    _data_start = ABSOLUTE(.);
    *(.gnu.linkonce.d.*)
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.gnu.linkonce.s2.*)
    *(.jcr)
    *(.data .data.*)
    *(.dram1 .dram1.*)
    _coredump_dram_start = ABSOLUTE(.);
    *(.dram2.coredump .dram2.coredump.*)
    _coredump_dram_end = ABSOLUTE(.);
    *libapp_trace.a:app_trace.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libapp_trace.a:app_trace_util.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libapp_trace.a:port_uart.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_hw_support.a:esp_memory_utils.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_hw_support.a:mspi_timing_tuning.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_hw_support.a:rtc_clk.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_mm.a:cache_esp32.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_mm.a:esp_cache.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_rom.a:esp_rom_spiflash.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_system.a:esp_err.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_system.a:image_process.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libesp_system.a:ubsan.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libgcov.a:(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:cache_hal_esp32.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:i2c_hal_iram.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:ledc_hal_iram.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:mmu_hal.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:spi_flash_encrypt_hal_iram.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:spi_flash_hal_iram.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:spi_slave_hal_iram.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libhal.a:wdt_hal_iram.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libnewlib.a:abort.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libnewlib.a:assert.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libnewlib.a:heap.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libnewlib.a:stdatomic.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libphy.a:(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libsoc.a:lldesc.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:flash_brownout_hook.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:memspi_host_driver.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_chip_boya.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_chip_gd.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_chip_generic.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_chip_issi.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_chip_mxic.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_chip_th.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_chip_winbond.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    *libspi_flash.a:spi_flash_wrap.*(.rodata .rodata.* .sdata2 .sdata2.* .srodata .srodata.*)
    _data_end = ABSOLUTE(.);
  } > dram0_0_seg
  /**
   * This section holds data that won't be initialised when startup.
   * This section locates in External RAM region.
   */
  .ext_ram_noinit (NOLOAD) :
  {
    _ext_ram_noinit_start = ABSOLUTE(.);
    *(.ext_ram_noinit*)
    
 . = ALIGN(4); 
 _ext_ram_noinit_end = ABSOLUTE(.);
  } > extern_ram_seg
  /**
   * This section holds data that should not be initialized at power up.
   * The section located in Internal SRAM memory region. The macro _NOINIT
   * can be used as attribute to place data into this section.
   * See the "esp_attr.h" file for more information.
   */
  .noinit (NOLOAD):
  {
    
 . = ALIGN(4); 
 _noinit_start = ABSOLUTE(.);
    *(.noinit .noinit.*)
    
 . = ALIGN(4); 
 _noinit_end = ABSOLUTE(.);
  } > dram0_0_seg
  /* External Memory BSS. (Variables with EXT_RAM_BSS_ATTR attribute). */
  .ext_ram.bss (NOLOAD) :
  {
    
 . = ALIGN(4); 
 _ext_ram_bss_start = ABSOLUTE(.);
    
 . = ALIGN(4); 
 _ext_ram_bss_end = ABSOLUTE(.);
  } > extern_ram_seg
  /* Shared RAM */
  .dram0.bss (NOLOAD) :
  {
    
 . = ALIGN(8); 
 _bss_start = ABSOLUTE(.);
    /**
     * ldgen places all bss-related data to mapping[dram0_bss]
     * (See components/esp_system/app.lf).
     */
    *(.bss .bss.*)
    *(.dynbss .dynsbss .gnu.linkonce.b .gnu.linkonce.b.* .gnu.linkonce.sb .gnu.linkonce.sb.* .gnu.linkonce.sb2 .gnu.linkonce.sb2.* .sbss .sbss.* .sbss2 .sbss2.* .scommon .share.mem)
    *(.ext_ram.bss .ext_ram.bss.*)
    *(COMMON)
    
 . = ALIGN(8); 
 _bss_end = ABSOLUTE(.);
  } > dram0_0_seg
  ASSERT(((_bss_end - ORIGIN(dram0_0_seg)) <= LENGTH(dram0_0_seg)),
          "DRAM segment data does not fit.")
  .flash.appdesc : ALIGN(0x10)
  {
    /**
     * Mark flash.rodata start.
     * This can be used for mmu driver to maintain virtual address
     */
    _rodata_reserved_start = ABSOLUTE(.);
    _rodata_start = ABSOLUTE(.);
    /* !DO NOT PUT ANYTHING BEFORE THIS! */
    /* Should be the first.  App version info. */
    *(.rodata_desc .rodata_desc.*)
    /* Should be the second. Custom app version info. */
    *(.rodata_custom_desc .rodata_custom_desc.*)
    /**
     * Create an empty gap within this section. Thanks to this, the end of this
     * section will match .flah.rodata's begin address. Thus, both sections
     * will be merged when creating the final bin image.
     */
    . = ALIGN(ALIGNOF(.flash.rodata));
  } > default_rodata_seg
  ASSERT((ADDR(.flash.rodata) == ADDR(.flash.appdesc) + SIZEOF(.flash.appdesc)), "The gap between .flash.appdesc and .flash.rodata must not exist to produce the final bin image.")
  .flash.rodata : ALIGN(0x10)
  {
    _flash_rodata_start = ABSOLUTE(.);
    *(EXCLUDE_FILE(*libgcov.a *libphy.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:rtc_clk.* *libesp_mm.a:cache_esp32.* *libesp_mm.a:esp_cache.* *libesp_rom.a:esp_rom_spiflash.* *libesp_system.a:esp_err.* *libesp_system.a:image_process.* *libesp_system.a:ubsan.* *libhal.a:cache_hal_esp32.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:wdt_hal_iram.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_wrap.*) .rodata EXCLUDE_FILE(*libgcov.a *libphy.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:rtc_clk.* *libesp_mm.a:cache_esp32.* *libesp_mm.a:esp_cache.* *libesp_rom.a:esp_rom_spiflash.* *libesp_system.a:esp_err.* *libesp_system.a:image_process.* *libesp_system.a:ubsan.* *libhal.a:cache_hal_esp32.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:wdt_hal_iram.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_wrap.*) .rodata.* EXCLUDE_FILE(*libgcov.a *libphy.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:rtc_clk.* *libesp_mm.a:cache_esp32.* *libesp_mm.a:esp_cache.* *libesp_rom.a:esp_rom_spiflash.* *libesp_system.a:esp_err.* *libesp_system.a:image_process.* *libesp_system.a:ubsan.* *libhal.a:cache_hal_esp32.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:wdt_hal_iram.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_wrap.*) .sdata2 EXCLUDE_FILE(*libgcov.a *libphy.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:rtc_clk.* *libesp_mm.a:cache_esp32.* *libesp_mm.a:esp_cache.* *libesp_rom.a:esp_rom_spiflash.* *libesp_system.a:esp_err.* *libesp_system.a:image_process.* *libesp_system.a:ubsan.* *libhal.a:cache_hal_esp32.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:wdt_hal_iram.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_wrap.*) .sdata2.* EXCLUDE_FILE(*libgcov.a *libphy.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:rtc_clk.* *libesp_mm.a:cache_esp32.* *libesp_mm.a:esp_cache.* *libesp_rom.a:esp_rom_spiflash.* *libesp_system.a:esp_err.* *libesp_system.a:image_process.* *libesp_system.a:ubsan.* *libhal.a:cache_hal_esp32.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:wdt_hal_iram.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_wrap.*) .srodata EXCLUDE_FILE(*libgcov.a *libphy.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:rtc_clk.* *libesp_mm.a:cache_esp32.* *libesp_mm.a:esp_cache.* *libesp_rom.a:esp_rom_spiflash.* *libesp_system.a:esp_err.* *libesp_system.a:image_process.* *libesp_system.a:ubsan.* *libhal.a:cache_hal_esp32.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:wdt_hal_iram.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_wrap.*) .srodata.*)
    *(.rodata_wlog_error .rodata_wlog_error.*)
    *(.rodata_wlog_info .rodata_wlog_info.*)
    *(.rodata_wlog_warning .rodata_wlog_warning.*)
    *(.irom1.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    /* C++ exception handlers table.  */
    
 . = ALIGN(4); 
 __XT_EXCEPTION_TABLE_ = ABSOLUTE(.);
    *(.xt_except_table)
    *(.gcc_except_table .gcc_except_table.*)
    *(.gnu.linkonce.e.*)
    
 . = ALIGN(4); 
 __XT_EXCEPTION_DESCS_ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    /**
     * C++ constructor tables.
     *
     * Excluding crtbegin.o/crtend.o since IDF doesn't use the toolchain crt.
     */
    
 . = ALIGN(4); 
 __init_array_start = ABSOLUTE(.);
    KEEP (*(EXCLUDE_FILE (*crtend.* *crtbegin.*) .ctors SORT(.ctors.*)))
    __init_array_end = ABSOLUTE(.);
    /* Addresses of memory regions reserved via SOC_RESERVE_MEMORY_REGION() */
    
 . = ALIGN(4); 
 soc_reserved_memory_region_start = ABSOLUTE(.);
    KEEP (*(.reserved_memory_address))
    soc_reserved_memory_region_end = ABSOLUTE(.);
    /* System init functions registered via ESP_SYSTEM_INIT_FN */
    
 . = ALIGN(4); 
 _esp_system_init_fn_array_start = ABSOLUTE(.);
    KEEP (*(SORT_BY_INIT_PRIORITY(.esp_system_init_fn.*)))
    _esp_system_init_fn_array_end = ABSOLUTE(.);
    _rodata_end = ABSOLUTE(.);
    /* Literals are also RO data. */
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
    /* TLS data. */
    
 . = ALIGN(4); 
 _thread_local_start = ABSOLUTE(.);
    *(.tdata)
    *(.tdata.*)
    *(.tbss)
    *(.tbss.*)
    _thread_local_end = ABSOLUTE(.);
  } > default_rodata_seg
  _flash_rodata_align = ALIGNOF(.flash.rodata);
  /**
   * This section contains all the rodata that is not used
   * at runtime, helping to avoid an increase in binary size.
   */
  .flash.rodata_noload (NOLOAD) :
  {
    /**
     * This symbol marks the end of flash.rodata. It can be utilized by the MMU
     * driver to maintain the virtual address.
     * NOLOAD rodata may not be included in this section.
     */
    _rodata_reserved_end = ABSOLUTE(.);
    *(.rodata_wlog_debug .rodata_wlog_debug.*)
    *(.rodata_wlog_verbose .rodata_wlog_verbose.*)
  } > default_rodata_seg
  .flash.text :
  {
    _stext = .;
    /**
     * Mark the start of flash.text.
     * This can be used by the MMU driver to maintain the virtual address.
     */
    _instruction_reserved_start = ABSOLUTE(.);
    _text_start = ABSOLUTE(.);
    *(EXCLUDE_FILE(*libesp_ringbuf.a *libfreertos.a *libgcov.a *librtc.a *libxt_hal.a *libxtensa.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libesp_driver_gptimer.a:gptimer.* *libesp_driver_spi.a:spi_dma.* *libesp_event.a:default_event_loop.* *libesp_event.a:esp_event.* *libesp_hw_support.a:cpu.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:periph_ctrl.* *libesp_hw_support.a:rtc_clk.* *libesp_hw_support.a:rtc_init.* *libesp_hw_support.a:rtc_sleep.* *libesp_hw_support.a:rtc_time.* *libesp_hw_support.a:rtc_wdt.* *libesp_hw_support.a:sar_periph_ctrl.* *libesp_mm.a:cache_esp32.* *libesp_mm.a:esp_cache.* *libesp_netif.a:esp_netif_lwip.* *libesp_netif.a:esp_pbuf_ref.* *libesp_netif.a:ethernetif.* *libesp_netif.a:wlanif.* *libesp_rom.a:esp_rom_spiflash.* *libesp_system.a:esp_err.* *libesp_system.a:esp_system_chip.* *libesp_system.a:image_process.* *libesp_system.a:ubsan.* *libgcc.a:lib2funcs.* *libhal.a:cache_hal_esp32.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:timer_hal.* *libhal.a:wdt_hal_iram.* *liblog.a:log.* *liblog.a:log_freertos.* *liblwip.a:api_lib.* *liblwip.a:api_msg.* *liblwip.a:etharp.* *liblwip.a:ethernet.* *liblwip.a:inet_chksum.* *liblwip.a:ip4.* *liblwip.a:ip4_addr.* *liblwip.a:lwip_default_hooks.* *liblwip.a:netbuf.* *liblwip.a:pbuf.* *liblwip.a:sockets.* *liblwip.a:sys_arch.* *liblwip.a:tcpip.* *liblwip.a:udp.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_wrap.*) .literal EXCLUDE_FILE(*libesp_ringbuf.a *libfreertos.a *libgcov.a *librtc.a *libxt_hal.a *libxtensa.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libesp_driver_gptimer.a:gptimer.* *libesp_driver_spi.a:spi_dma.* *libesp_event.a:default_event_loop.* *libesp_event.a:esp_event.* *libesp_hw_support.a:cpu.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:periph_ctrl.* *libesp_hw_support.a:rtc_clk.* *libesp_hw_support.a:rtc_init.* *libesp_hw_support.a:rtc_sleep.* *libesp_hw_support.a:rtc_time.* *libesp_hw_support.a:rtc_wdt.* *libesp_hw_support.a:sar_periph_ctrl.* *libesp_mm.a:cache_esp32.* *libesp_mm.a:esp_cache.* *libesp_netif.a:esp_netif_lwip.* *libesp_netif.a:esp_pbuf_ref.* *libesp_netif.a:ethernetif.* *libesp_netif.a:wlanif.* *libesp_rom.a:esp_rom_spiflash.* *libesp_system.a:esp_err.* *libesp_system.a:esp_system_chip.* *libesp_system.a:image_process.* *libesp_system.a:ubsan.* *libgcc.a:lib2funcs.* *libhal.a:cache_hal_esp32.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:timer_hal.* *libhal.a:wdt_hal_iram.* *liblog.a:log.* *liblog.a:log_freertos.* *liblwip.a:api_lib.* *liblwip.a:api_msg.* *liblwip.a:etharp.* *liblwip.a:ethernet.* *liblwip.a:inet_chksum.* *liblwip.a:ip4.* *liblwip.a:ip4_addr.* *liblwip.a:lwip_default_hooks.* *liblwip.a:netbuf.* *liblwip.a:pbuf.* *liblwip.a:sockets.* *liblwip.a:sys_arch.* *liblwip.a:tcpip.* *liblwip.a:udp.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_wrap.*) .literal.* EXCLUDE_FILE(*libesp_ringbuf.a *libfreertos.a *libgcov.a *librtc.a *libxt_hal.a *libxtensa.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libesp_driver_gptimer.a:gptimer.* *libesp_driver_spi.a:spi_dma.* *libesp_event.a:default_event_loop.* *libesp_event.a:esp_event.* *libesp_hw_support.a:cpu.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:periph_ctrl.* *libesp_hw_support.a:rtc_clk.* *libesp_hw_support.a:rtc_init.* *libesp_hw_support.a:rtc_sleep.* *libesp_hw_support.a:rtc_time.* *libesp_hw_support.a:rtc_wdt.* *libesp_hw_support.a:sar_periph_ctrl.* *libesp_mm.a:cache_esp32.* *libesp_mm.a:esp_cache.* *libesp_netif.a:esp_netif_lwip.* *libesp_netif.a:esp_pbuf_ref.* *libesp_netif.a:ethernetif.* *libesp_netif.a:wlanif.* *libesp_rom.a:esp_rom_spiflash.* *libesp_system.a:esp_err.* *libesp_system.a:esp_system_chip.* *libesp_system.a:image_process.* *libesp_system.a:ubsan.* *libgcc.a:lib2funcs.* *libhal.a:cache_hal_esp32.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:timer_hal.* *libhal.a:wdt_hal_iram.* *liblog.a:log.* *liblog.a:log_freertos.* *liblwip.a:api_lib.* *liblwip.a:api_msg.* *liblwip.a:etharp.* *liblwip.a:ethernet.* *liblwip.a:inet_chksum.* *liblwip.a:ip4.* *liblwip.a:ip4_addr.* *liblwip.a:lwip_default_hooks.* *liblwip.a:netbuf.* *liblwip.a:pbuf.* *liblwip.a:sockets.* *liblwip.a:sys_arch.* *liblwip.a:tcpip.* *liblwip.a:udp.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_wrap.*) .text EXCLUDE_FILE(*libesp_ringbuf.a *libfreertos.a *libgcov.a *librtc.a *libxt_hal.a *libxtensa.a *libapp_trace.a:app_trace.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:port_uart.* *libesp_driver_gptimer.a:gptimer.* *libesp_driver_spi.a:spi_dma.* *libesp_event.a:default_event_loop.* *libesp_event.a:esp_event.* *libesp_hw_support.a:cpu.* *libesp_hw_support.a:esp_memory_utils.* *libesp_hw_support.a:mspi_timing_tuning.* *libesp_hw_support.a:periph_ctrl.* *libesp_hw_support.a:rtc_clk.* *libesp_hw_support.a:rtc_init.* *libesp_hw_support.a:rtc_sleep.* *libesp_hw_support.a:rtc_time.* *libesp_hw_support.a:rtc_wdt.* *libesp_hw_support.a:sar_periph_ctrl.* *libesp_mm.a:cache_esp32.* *libesp_mm.a:esp_cache.* *libesp_netif.a:esp_netif_lwip.* *libesp_netif.a:esp_pbuf_ref.* *libesp_netif.a:ethernetif.* *libesp_netif.a:wlanif.* *libesp_rom.a:esp_rom_spiflash.* *libesp_system.a:esp_err.* *libesp_system.a:esp_system_chip.* *libesp_system.a:image_process.* *libesp_system.a:ubsan.* *libgcc.a:lib2funcs.* *libhal.a:cache_hal_esp32.* *libhal.a:i2c_hal_iram.* *libhal.a:ledc_hal_iram.* *libhal.a:mmu_hal.* *libhal.a:spi_flash_encrypt_hal_iram.* *libhal.a:spi_flash_hal_iram.* *libhal.a:spi_slave_hal_iram.* *libhal.a:timer_hal.* *libhal.a:wdt_hal_iram.* *liblog.a:log.* *liblog.a:log_freertos.* *liblwip.a:api_lib.* *liblwip.a:api_msg.* *liblwip.a:etharp.* *liblwip.a:ethernet.* *liblwip.a:inet_chksum.* *liblwip.a:ip4.* *liblwip.a:ip4_addr.* *liblwip.a:lwip_default_hooks.* *liblwip.a:netbuf.* *liblwip.a:pbuf.* *liblwip.a:sockets.* *liblwip.a:sys_arch.* *liblwip.a:tcpip.* *liblwip.a:udp.* *libnewlib.a:abort.* *libnewlib.a:assert.* *libnewlib.a:heap.* *libnewlib.a:stdatomic.* *libsoc.a:lldesc.* *libspi_flash.a:flash_brownout_hook.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_boya.* *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_th.* *libspi_flash.a:spi_flash_chip_winbond.* *libspi_flash.a:spi_flash_wrap.*) .text.*)
    *(.wifi0iram .wifi0iram.*)
    *(.wifiextrairam .wifiextrairam.*)
    *(.wifiorslpiram .wifiorslpiram.*)
    *(.wifirxiram .wifirxiram.*)
    *(.wifislpiram .wifislpiram.*)
    *(.wifislprxiram .wifislprxiram.*)
    *libesp_driver_gptimer.a:gptimer.*(.literal.gptimer_del_timer .literal.gptimer_destroy .literal.gptimer_disable .literal.gptimer_enable .literal.gptimer_get_captured_count .literal.gptimer_get_raw_count .literal.gptimer_get_resolution .literal.gptimer_new_timer .literal.gptimer_register_event_callbacks .literal.gptimer_set_alarm_action .literal.gptimer_set_raw_count .literal.gptimer_start .literal.gptimer_stop .text .text.gptimer_del_timer .text.gptimer_destroy .text.gptimer_disable .text.gptimer_enable .text.gptimer_get_captured_count .text.gptimer_get_raw_count .text.gptimer_get_resolution .text.gptimer_new_timer .text.gptimer_register_event_callbacks .text.gptimer_set_alarm_action .text.gptimer_set_raw_count .text.gptimer_start .text.gptimer_stop)
    *libesp_driver_spi.a:spi_dma.*(.literal.spi_dma_enable_burst .text .text.spi_dma_enable_burst)
    *libesp_event.a:default_event_loop.*(.literal.esp_event_handler_instance_register .literal.esp_event_handler_instance_unregister .literal.esp_event_handler_register .literal.esp_event_handler_unregister .literal.esp_event_loop_create_default .literal.esp_event_loop_delete_default .literal.esp_event_post .text .text.esp_event_handler_instance_register .text.esp_event_handler_instance_unregister .text.esp_event_handler_register .text.esp_event_handler_unregister .text.esp_event_loop_create_default .text.esp_event_loop_delete_default .text.esp_event_post)
    *libesp_event.a:esp_event.*(.literal.base_node_add_handler .literal.esp_event_handler_instance_register_with .literal.esp_event_handler_instance_unregister_with .literal.esp_event_handler_register_with .literal.esp_event_handler_register_with_internal .literal.esp_event_handler_unregister_with .literal.esp_event_handler_unregister_with_internal .literal.esp_event_loop_create .literal.esp_event_loop_delete .literal.esp_event_loop_run .literal.esp_event_loop_run_task .literal.esp_event_post_to .literal.handler_instances_add .literal.handler_instances_remove .literal.handler_instances_remove_all .literal.loop_node_add_handler .text .text.base_node_add_handler .text.esp_event_dump .text.esp_event_handler_instance_register_with .text.esp_event_handler_instance_unregister_with .text.esp_event_handler_register_with .text.esp_event_handler_register_with_internal .text.esp_event_handler_unregister_with .text.esp_event_handler_unregister_with_internal .text.esp_event_loop_create .text.esp_event_loop_delete .text.esp_event_loop_run .text.esp_event_loop_run_task .text.esp_event_post_to .text.handler_instances_add .text.handler_instances_remove .text.handler_instances_remove_all .text.loop_node_add_handler)
    *libesp_hw_support.a:cpu.*(.literal.esp_cpu_set_watchpoint .text .text.esp_cpu_clear_breakpoint .text.esp_cpu_clear_watchpoint .text.esp_cpu_set_breakpoint .text.esp_cpu_set_watchpoint)
    *libesp_hw_support.a:periph_ctrl.*(.literal.periph_ll_get_rst_en_reg .literal.periph_module_disable .literal.periph_module_enable .literal.periph_rcc_acquire_enter .literal.periph_rcc_acquire_exit .literal.periph_rcc_release_enter .literal.periph_rcc_release_exit .text .text.periph_ll_get_clk_en_mask .text.periph_ll_get_rst_en_mask .text.periph_ll_get_rst_en_reg .text.periph_module_disable .text.periph_module_enable .text.periph_rcc_acquire_enter .text.periph_rcc_acquire_exit .text.periph_rcc_release_enter .text.periph_rcc_release_exit)
    *libesp_hw_support.a:rtc_init.*(.literal.rtc_init .text .text.rtc_init)
    *libesp_hw_support.a:sar_periph_ctrl.*(.literal.s_sar_power_acquire .literal.s_sar_power_release .literal.sar_periph_ctrl_adc_continuous_power_acquire .literal.sar_periph_ctrl_adc_continuous_power_release .literal.sar_periph_ctrl_adc_oneshot_power_acquire .literal.sar_periph_ctrl_adc_oneshot_power_release .literal.sar_periph_ctrl_init .literal.sar_periph_ctrl_power_disable .literal.sar_periph_ctrl_pwdet_power_acquire .literal.sar_periph_ctrl_pwdet_power_release .text .text.s_sar_power_acquire .text.s_sar_power_release .text.sar_periph_ctrl_adc_continuous_power_acquire .text.sar_periph_ctrl_adc_continuous_power_release .text.sar_periph_ctrl_adc_oneshot_power_acquire .text.sar_periph_ctrl_adc_oneshot_power_release .text.sar_periph_ctrl_init .text.sar_periph_ctrl_power_disable .text.sar_periph_ctrl_pwdet_power_acquire .text.sar_periph_ctrl_pwdet_power_release)
    *libesp_netif.a:esp_netif_lwip.*(.literal.dns_clear_servers$constprop$0 .literal.esp_ip4addr_aton .literal.esp_ip4addr_ntoa .literal.esp_netif_add_ip6_address .literal.esp_netif_add_ip6_address_api .literal.esp_netif_api_cb .literal.esp_netif_attach .literal.esp_netif_create_ip6_linklocal .literal.esp_netif_create_ip6_linklocal_api .literal.esp_netif_deinit .literal.esp_netif_destroy .literal.esp_netif_destroy_api .literal.esp_netif_dhcpc_option .literal.esp_netif_dhcpc_start .literal.esp_netif_dhcpc_start_api .literal.esp_netif_dhcpc_stop .literal.esp_netif_dhcpc_stop_api .literal.esp_netif_dhcps_cb .literal.esp_netif_dhcps_get_clients_by_mac .literal.esp_netif_dhcps_get_clients_by_mac_api .literal.esp_netif_dhcps_option .literal.esp_netif_dhcps_option_api .literal.esp_netif_dhcps_start .literal.esp_netif_dhcps_start_api .literal.esp_netif_dhcps_stop .literal.esp_netif_dhcps_stop_api .literal.esp_netif_down .literal.esp_netif_down_api .literal.esp_netif_find_if .literal.esp_netif_find_if_api .literal.esp_netif_get_all_ip6 .literal.esp_netif_get_default_netif .literal.esp_netif_get_dns_info .literal.esp_netif_get_dns_info_api .literal.esp_netif_get_handle_from_ifkey .literal.esp_netif_get_ip6_global .literal.esp_netif_get_ip6_linklocal .literal.esp_netif_get_ip_info .literal.esp_netif_get_mac .literal.esp_netif_get_netif_impl_name .literal.esp_netif_get_netif_impl_name_api .literal.esp_netif_get_old_ip_info .literal.esp_netif_init .literal.esp_netif_ip6_get_addr_type .literal.esp_netif_ip_lost_timer .literal.esp_netif_join_ip6_multicast_group .literal.esp_netif_join_ip6_multicast_group_api .literal.esp_netif_leave_ip6_multicast_group .literal.esp_netif_leave_ip6_multicast_group_api .literal.esp_netif_lwip_ipc_call .literal.esp_netif_lwip_ipc_call_msg .literal.esp_netif_lwip_remove .literal.esp_netif_netstack_buf_free .literal.esp_netif_netstack_buf_ref .literal.esp_netif_new .literal.esp_netif_new_api .literal.esp_netif_ppp_set_auth .literal.esp_netif_ppp_set_auth_api .literal.esp_netif_remove_ip6_address .literal.esp_netif_remove_ip6_address_api .literal.esp_netif_set_default_netif .literal.esp_netif_set_dns_info .literal.esp_netif_set_dns_info_api .literal.esp_netif_set_hostname .literal.esp_netif_set_hostname_api .literal.esp_netif_set_ip4_addr .literal.esp_netif_set_ip_info .literal.esp_netif_set_ip_info_api .literal.esp_netif_set_ip_old_info_api .literal.esp_netif_set_mac .literal.esp_netif_set_mac_api .literal.esp_netif_set_old_ip_info .literal.esp_netif_start .literal.esp_netif_start_api .literal.esp_netif_start_ip_lost_timer$isra$0 .literal.esp_netif_stop .literal.esp_netif_stop_api .literal.esp_netif_str_to_ip4 .literal.esp_netif_str_to_ip6 .literal.esp_netif_tcpip_exec .literal.esp_netif_up .literal.esp_netif_up_api .literal.esp_netif_update_default_netif .literal.esp_netif_update_default_netif_lwip .literal.get_handle_from_ifkey_api .literal.netif_callback_fn .literal.netif_send_garp .literal.netif_send_mldv6 .literal.tcpip_init_done .text .text.dns_clear_servers$constprop$0 .text.esp_ip4addr_aton .text.esp_ip4addr_ntoa .text.esp_netif_add_ip6_address .text.esp_netif_add_ip6_address_api .text.esp_netif_api_cb .text.esp_netif_attach .text.esp_netif_create_ip6_linklocal .text.esp_netif_create_ip6_linklocal_api .text.esp_netif_deinit .text.esp_netif_destroy .text.esp_netif_destroy_api .text.esp_netif_dhcpc_get_status .text.esp_netif_dhcpc_option .text.esp_netif_dhcpc_option_api .text.esp_netif_dhcpc_start .text.esp_netif_dhcpc_start_api .text.esp_netif_dhcpc_stop .text.esp_netif_dhcpc_stop_api .text.esp_netif_dhcps_cb .text.esp_netif_dhcps_get_clients_by_mac .text.esp_netif_dhcps_get_clients_by_mac_api .text.esp_netif_dhcps_get_status .text.esp_netif_dhcps_option .text.esp_netif_dhcps_option_api .text.esp_netif_dhcps_start .text.esp_netif_dhcps_start_api .text.esp_netif_dhcps_stop .text.esp_netif_dhcps_stop_api .text.esp_netif_down .text.esp_netif_down_api .text.esp_netif_find_if .text.esp_netif_find_if_api .text.esp_netif_get_all_ip6 .text.esp_netif_get_default_netif .text.esp_netif_get_desc .text.esp_netif_get_dns_info .text.esp_netif_get_dns_info_api .text.esp_netif_get_event_id .text.esp_netif_get_flags .text.esp_netif_get_handle_from_ifkey .text.esp_netif_get_handle_from_netif_impl .text.esp_netif_get_hostname .text.esp_netif_get_ifkey .text.esp_netif_get_io_driver .text.esp_netif_get_ip6_global .text.esp_netif_get_ip6_linklocal .text.esp_netif_get_ip_info .text.esp_netif_get_mac .text.esp_netif_get_netif_impl .text.esp_netif_get_netif_impl_index .text.esp_netif_get_netif_impl_name .text.esp_netif_get_netif_impl_name_api .text.esp_netif_get_old_ip_info .text.esp_netif_get_route_prio .text.esp_netif_init .text.esp_netif_ip6_get_addr_type .text.esp_netif_ip_lost_timer .text.esp_netif_is_netif_up .text.esp_netif_is_valid_static_ip .text.esp_netif_join_ip6_multicast_group .text.esp_netif_join_ip6_multicast_group_api .text.esp_netif_leave_ip6_multicast_group .text.esp_netif_leave_ip6_multicast_group_api .text.esp_netif_lwip_ipc_call .text.esp_netif_lwip_ipc_call_msg .text.esp_netif_lwip_remove .text.esp_netif_napt_disable .text.esp_netif_napt_enable .text.esp_netif_netstack_buf_free .text.esp_netif_netstack_buf_ref .text.esp_netif_new .text.esp_netif_new_api .text.esp_netif_ppp_set_auth .text.esp_netif_ppp_set_auth_api .text.esp_netif_remove_ip6_address .text.esp_netif_remove_ip6_address_api .text.esp_netif_reset_ip_info$isra$0 .text.esp_netif_set_default_netif .text.esp_netif_set_dns_info .text.esp_netif_set_dns_info_api .text.esp_netif_set_driver_config .text.esp_netif_set_hostname .text.esp_netif_set_hostname_api .text.esp_netif_set_ip4_addr .text.esp_netif_set_ip_info .text.esp_netif_set_ip_info_api .text.esp_netif_set_ip_old_info_api .text.esp_netif_set_link_speed .text.esp_netif_set_mac .text.esp_netif_set_mac_api .text.esp_netif_set_old_ip_info .text.esp_netif_start .text.esp_netif_start_api .text.esp_netif_start_ip_lost_timer$isra$0 .text.esp_netif_stop .text.esp_netif_stop_api .text.esp_netif_str_to_ip4 .text.esp_netif_str_to_ip6 .text.esp_netif_tcpip_exec .text.esp_netif_transmit .text.esp_netif_up .text.esp_netif_up_api .text.esp_netif_update_default_netif .text.esp_netif_update_default_netif_lwip .text.get_handle_from_ifkey_api .text.netif_callback_fn .text.netif_send_garp .text.netif_send_mldv6 .text.tcpip_exec_api .text.tcpip_init_done)
    *libesp_netif.a:esp_pbuf_ref.*(.text)
    *libesp_netif.a:ethernetif.*(.literal.ethernetif_init .text .text.ethernetif_init)
    *libesp_netif.a:wlanif.*(.literal.wlanif_init .literal.wlanif_init_ap .literal.wlanif_init_nan .literal.wlanif_init_sta .text .text.wlanif_init .text.wlanif_init_ap .text.wlanif_init_nan .text.wlanif_init_sta)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvAcquireItemNoSplit .text.prvAcquireItemNoSplit)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvCheckItemAvail .text.prvCheckItemAvail)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvCheckItemFitsByteBuffer .text.prvCheckItemFitsByteBuffer)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvCheckItemFitsDefault .text.prvCheckItemFitsDefault)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvCopyItemAllowSplit .text.prvCopyItemAllowSplit)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvCopyItemByteBuf .text.prvCopyItemByteBuf)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvCopyItemNoSplit .text.prvCopyItemNoSplit)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvGetCurMaxSizeAllowSplit .text.prvGetCurMaxSizeAllowSplit)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvGetCurMaxSizeByteBuf .text.prvGetCurMaxSizeByteBuf)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvGetCurMaxSizeNoSplit .text.prvGetCurMaxSizeNoSplit)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvGetItemByteBuf .text.prvGetItemByteBuf)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvGetItemDefault .text.prvGetItemDefault)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvInitializeNewRingbuffer .text.prvInitializeNewRingbuffer)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvReceiveGeneric .text.prvReceiveGeneric)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvReceiveGenericFromISR .text.prvReceiveGenericFromISR)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvReturnItemByteBuf .text.prvReturnItemByteBuf)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvReturnItemDefault .text.prvReturnItemDefault)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvSendAcquireGeneric .text.prvSendAcquireGeneric)
    *libesp_ringbuf.a:ringbuf.*(.literal.prvSendItemDoneNoSplit .text.prvSendItemDoneNoSplit)
    *libesp_ringbuf.a:ringbuf.*(.literal.vRingbufferDelete .text.vRingbufferDelete)
    *libesp_ringbuf.a:ringbuf.*(.literal.vRingbufferGetInfo .text.vRingbufferGetInfo)
    *libesp_ringbuf.a:ringbuf.*(.literal.vRingbufferReturnItem .text.vRingbufferReturnItem)
    *libesp_ringbuf.a:ringbuf.*(.literal.vRingbufferReturnItemFromISR .text.vRingbufferReturnItemFromISR)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferAddToQueueSetRead .text.xRingbufferAddToQueueSetRead)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferCreate .text.xRingbufferCreate)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferCreateNoSplit .text.xRingbufferCreateNoSplit)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferCreateStatic .text.xRingbufferCreateStatic)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferGetCurFreeSize .text.xRingbufferGetCurFreeSize)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferGetMaxItemSize .text.xRingbufferGetMaxItemSize)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferPrintInfo .text.xRingbufferPrintInfo)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferReceive .text.xRingbufferReceive)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferReceiveFromISR .text.xRingbufferReceiveFromISR)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferReceiveSplit .text.xRingbufferReceiveSplit)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferReceiveSplitFromISR .text.xRingbufferReceiveSplitFromISR)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferReceiveUpTo .text.xRingbufferReceiveUpTo)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferReceiveUpToFromISR .text.xRingbufferReceiveUpToFromISR)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferRemoveFromQueueSetRead .text.xRingbufferRemoveFromQueueSetRead)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferSend .text.xRingbufferSend)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferSendAcquire .text.xRingbufferSendAcquire)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferSendComplete .text.xRingbufferSendComplete)
    *libesp_ringbuf.a:ringbuf.*(.literal.xRingbufferSendFromISR .text.xRingbufferSendFromISR)
    *libesp_system.a:esp_system_chip.*(.literal.esp_get_free_heap_size .literal.esp_get_free_internal_heap_size .literal.esp_get_idf_version .literal.esp_get_minimum_free_heap_size .text .text.esp_get_free_heap_size .text.esp_get_free_internal_heap_size .text.esp_get_idf_version .text.esp_get_minimum_free_heap_size)
    *libfreertos.a:app_startup.*(.literal .literal.* .text .text.*)
    *libfreertos.a:event_groups.*(.literal.vEventGroupClearBitsCallback .text.vEventGroupClearBitsCallback)
    *libfreertos.a:event_groups.*(.literal.vEventGroupDelete .text.vEventGroupDelete)
    *libfreertos.a:event_groups.*(.literal.vEventGroupSetBitsCallback .text.vEventGroupSetBitsCallback)
    *libfreertos.a:event_groups.*(.literal.xEventGroupClearBits .text.xEventGroupClearBits)
    *libfreertos.a:event_groups.*(.literal.xEventGroupCreate .text.xEventGroupCreate)
    *libfreertos.a:event_groups.*(.literal.xEventGroupCreateStatic .text.xEventGroupCreateStatic)
    *libfreertos.a:event_groups.*(.literal.xEventGroupSetBits .text.xEventGroupSetBits)
    *libfreertos.a:event_groups.*(.literal.xEventGroupSync .text.xEventGroupSync)
    *libfreertos.a:event_groups.*(.literal.xEventGroupWaitBits .text.xEventGroupWaitBits)
    *libfreertos.a:freertos_compatibility.*(.literal .literal.* .text .text.*)
    *libfreertos.a:heap_idf.*(.literal.pvPortMalloc .text.pvPortMalloc)
    *libfreertos.a:heap_idf.*(.literal.vPortFree .text.vPortFree)
    *libfreertos.a:heap_idf.*(.literal.xPortCheckValidTCBMem .text.xPortCheckValidTCBMem)
    *libfreertos.a:heap_idf.*(.literal.xPortGetFreeHeapSize .text.xPortGetFreeHeapSize)
    *libfreertos.a:heap_idf.*(.literal.xPortGetMinimumEverFreeHeapSize .text.xPortGetMinimumEverFreeHeapSize)
    *libfreertos.a:heap_idf.*(.literal.xPortcheckValidStackMem .text.xPortcheckValidStackMem)
    *libfreertos.a:idf_additions.*(.literal .literal.* .text .text.*)
    *libfreertos.a:list.*(.literal.vListInitialise .text.vListInitialise)
    *libfreertos.a:list.*(.literal.vListInitialiseItem .text.vListInitialiseItem)
    *libfreertos.a:port.*(.literal.pxPortInitialiseStack .text.pxPortInitialiseStack)
    *libfreertos.a:port.*(.literal.vPortTCBPreDeleteHook .text.vPortTCBPreDeleteHook)
    *libfreertos.a:port.*(.literal.xPortGetTickRateHz .text.xPortGetTickRateHz)
    *libfreertos.a:port.*(.literal.xPortStartScheduler .text.xPortStartScheduler)
    *libfreertos.a:port_common.*(.literal.vApplicationGetIdleTaskMemory .text.vApplicationGetIdleTaskMemory)
    *libfreertos.a:port_common.*(.literal.vApplicationGetTimerTaskMemory .text.vApplicationGetTimerTaskMemory)
    *libfreertos.a:port_systick.*(.literal.vPortSetupTimer .text.vPortSetupTimer)
    *libfreertos.a:queue.*(.literal.uxQueueMessagesWaiting .text.uxQueueMessagesWaiting)
    *libfreertos.a:queue.*(.literal.uxQueueSpacesAvailable .text.uxQueueSpacesAvailable)
    *libfreertos.a:queue.*(.literal.vQueueDelete .text.vQueueDelete)
    *libfreertos.a:queue.*(.literal.vQueueWaitForMessageRestricted .text.vQueueWaitForMessageRestricted)
    *libfreertos.a:queue.*(.literal.xQueueAddToSet .text.xQueueAddToSet)
    *libfreertos.a:queue.*(.literal.xQueueCreateCountingSemaphore .text.xQueueCreateCountingSemaphore)
    *libfreertos.a:queue.*(.literal.xQueueCreateCountingSemaphoreStatic .text.xQueueCreateCountingSemaphoreStatic)
    *libfreertos.a:queue.*(.literal.xQueueCreateMutex .text.xQueueCreateMutex)
    *libfreertos.a:queue.*(.literal.xQueueCreateMutexStatic .text.xQueueCreateMutexStatic)
    *libfreertos.a:queue.*(.literal.xQueueCreateSet .text.xQueueCreateSet)
    *libfreertos.a:queue.*(.literal.xQueueGenericCreate .text.xQueueGenericCreate)
    *libfreertos.a:queue.*(.literal.xQueueGenericCreateStatic .text.xQueueGenericCreateStatic)
    *libfreertos.a:queue.*(.literal.xQueueGenericReset .text.xQueueGenericReset)
    *libfreertos.a:queue.*(.literal.xQueueGenericSend .text.xQueueGenericSend)
    *libfreertos.a:queue.*(.literal.xQueueGetMutexHolder .text.xQueueGetMutexHolder)
    *libfreertos.a:queue.*(.literal.xQueueGiveMutexRecursive .text.xQueueGiveMutexRecursive)
    *libfreertos.a:queue.*(.literal.xQueuePeek .text.xQueuePeek)
    *libfreertos.a:queue.*(.literal.xQueueReceive .text.xQueueReceive)
    *libfreertos.a:queue.*(.literal.xQueueRemoveFromSet .text.xQueueRemoveFromSet)
    *libfreertos.a:queue.*(.literal.xQueueSelectFromSet .text.xQueueSelectFromSet)
    *libfreertos.a:queue.*(.literal.xQueueSemaphoreTake .text.xQueueSemaphoreTake)
    *libfreertos.a:queue.*(.literal.xQueueTakeMutexRecursive .text.xQueueTakeMutexRecursive)
    *libfreertos.a:stream_buffer.*(.literal.prvReadBytesFromBuffer .text.prvReadBytesFromBuffer)
    *libfreertos.a:stream_buffer.*(.literal.prvWriteBytesToBuffer .text.prvWriteBytesToBuffer)
    *libfreertos.a:stream_buffer.*(.literal.vStreamBufferDelete .text.vStreamBufferDelete)
    *libfreertos.a:stream_buffer.*(.literal.xStreamBufferBytesAvailable .text.xStreamBufferBytesAvailable)
    *libfreertos.a:stream_buffer.*(.literal.xStreamBufferGenericCreate .text.xStreamBufferGenericCreate)
    *libfreertos.a:stream_buffer.*(.literal.xStreamBufferGenericCreateStatic .text.xStreamBufferGenericCreateStatic)
    *libfreertos.a:stream_buffer.*(.literal.xStreamBufferIsEmpty .text.xStreamBufferIsEmpty)
    *libfreertos.a:stream_buffer.*(.literal.xStreamBufferIsFull .text.xStreamBufferIsFull)
    *libfreertos.a:stream_buffer.*(.literal.xStreamBufferNextMessageLengthBytes .text.xStreamBufferNextMessageLengthBytes)
    *libfreertos.a:stream_buffer.*(.literal.xStreamBufferReceive .text.xStreamBufferReceive)
    *libfreertos.a:stream_buffer.*(.literal.xStreamBufferReset .text.xStreamBufferReset)
    *libfreertos.a:stream_buffer.*(.literal.xStreamBufferSend .text.xStreamBufferSend)
    *libfreertos.a:stream_buffer.*(.literal.xStreamBufferSetTriggerLevel .text.xStreamBufferSetTriggerLevel)
    *libfreertos.a:tasks.*(.literal.eTaskGetState .text.eTaskGetState)
    *libfreertos.a:tasks.*(.literal.pcTaskGetName .text.pcTaskGetName)
    *libfreertos.a:tasks.*(.literal.prvAddCurrentTaskToDelayedList .text.prvAddCurrentTaskToDelayedList)
    *libfreertos.a:tasks.*(.literal.prvAddNewTaskToReadyList .text.prvAddNewTaskToReadyList)
    *libfreertos.a:tasks.*(.literal.prvDeleteTCB .text.prvDeleteTCB)
    *libfreertos.a:tasks.*(.literal.prvIdleTask .text.prvIdleTask)
    *libfreertos.a:tasks.*(.literal.prvReleaseKernelLock .text.prvReleaseKernelLock)
    *libfreertos.a:tasks.*(.literal.prvSearchForNameWithinSingleList .text.prvSearchForNameWithinSingleList)
    *libfreertos.a:tasks.*(.literal.prvTakeKernelLock .text.prvTakeKernelLock)
    *libfreertos.a:tasks.*(.literal.prvTaskPriorityRaise .text.prvTaskPriorityRaise)
    *libfreertos.a:tasks.*(.literal.prvTaskPriorityRestore .text.prvTaskPriorityRestore)
    *libfreertos.a:tasks.*(.literal.pvTaskGetThreadLocalStoragePointer .text.pvTaskGetThreadLocalStoragePointer)
    *libfreertos.a:tasks.*(.literal.pvTaskIncrementMutexHeldCount .text.pvTaskIncrementMutexHeldCount)
    *libfreertos.a:tasks.*(.literal.pxTaskGetStackStart .text.pxTaskGetStackStart)
    *libfreertos.a:tasks.*(.literal.ulTaskGenericNotifyTake .text.ulTaskGenericNotifyTake)
    *libfreertos.a:tasks.*(.literal.ulTaskGenericNotifyValueClear .text.ulTaskGenericNotifyValueClear)
    *libfreertos.a:tasks.*(.literal.uxTaskGetNumberOfTasks .text.uxTaskGetNumberOfTasks)
    *libfreertos.a:tasks.*(.literal.uxTaskGetSnapshotAll .text.uxTaskGetSnapshotAll)
    *libfreertos.a:tasks.*(.literal.uxTaskGetStackHighWaterMark .text.uxTaskGetStackHighWaterMark)
    *libfreertos.a:tasks.*(.literal.uxTaskGetStackHighWaterMark2 .text.uxTaskGetStackHighWaterMark2)
    *libfreertos.a:tasks.*(.literal.uxTaskPriorityGet .text.uxTaskPriorityGet)
    *libfreertos.a:tasks.*(.literal.uxTaskResetEventItemValue .text.uxTaskResetEventItemValue)
    *libfreertos.a:tasks.*(.literal.vTaskDelay .text.vTaskDelay)
    *libfreertos.a:tasks.*(.literal.vTaskDelete .text.vTaskDelete)
    *libfreertos.a:tasks.*(.literal.vTaskEndScheduler .text.vTaskEndScheduler)
    *libfreertos.a:tasks.*(.literal.vTaskInternalSetTimeOutState .text.vTaskInternalSetTimeOutState)
    *libfreertos.a:tasks.*(.literal.vTaskMissedYield .text.vTaskMissedYield)
    *libfreertos.a:tasks.*(.literal.vTaskPlaceOnEventList .text.vTaskPlaceOnEventList)
    *libfreertos.a:tasks.*(.literal.vTaskPlaceOnEventListRestricted .text.vTaskPlaceOnEventListRestricted)
    *libfreertos.a:tasks.*(.literal.vTaskPlaceOnUnorderedEventList .text.vTaskPlaceOnUnorderedEventList)
    *libfreertos.a:tasks.*(.literal.vTaskPriorityDisinheritAfterTimeout .text.vTaskPriorityDisinheritAfterTimeout)
    *libfreertos.a:tasks.*(.literal.vTaskPrioritySet .text.vTaskPrioritySet)
    *libfreertos.a:tasks.*(.literal.vTaskRemoveFromUnorderedEventList .text.vTaskRemoveFromUnorderedEventList)
    *libfreertos.a:tasks.*(.literal.vTaskResume .text.vTaskResume)
    *libfreertos.a:tasks.*(.literal.vTaskSetThreadLocalStoragePointer .text.vTaskSetThreadLocalStoragePointer)
    *libfreertos.a:tasks.*(.literal.vTaskSetThreadLocalStoragePointerAndDelCallback .text.vTaskSetThreadLocalStoragePointerAndDelCallback)
    *libfreertos.a:tasks.*(.literal.vTaskSetTimeOutState .text.vTaskSetTimeOutState)
    *libfreertos.a:tasks.*(.literal.vTaskStartScheduler .text.vTaskStartScheduler)
    *libfreertos.a:tasks.*(.literal.vTaskSuspend .text.vTaskSuspend)
    *libfreertos.a:tasks.*(.literal.vTaskSuspendAll .text.vTaskSuspendAll)
    *libfreertos.a:tasks.*(.literal.xTaskAbortDelay .text.xTaskAbortDelay)
    *libfreertos.a:tasks.*(.literal.xTaskCheckForTimeOut .text.xTaskCheckForTimeOut)
    *libfreertos.a:tasks.*(.literal.xTaskCreatePinnedToCore .text.xTaskCreatePinnedToCore)
    *libfreertos.a:tasks.*(.literal.xTaskCreateStaticPinnedToCore .text.xTaskCreateStaticPinnedToCore)
    *libfreertos.a:tasks.*(.literal.xTaskDelayUntil .text.xTaskDelayUntil)
    *libfreertos.a:tasks.*(.literal.xTaskGenericNotify .text.xTaskGenericNotify)
    *libfreertos.a:tasks.*(.literal.xTaskGenericNotifyStateClear .text.xTaskGenericNotifyStateClear)
    *libfreertos.a:tasks.*(.literal.xTaskGenericNotifyWait .text.xTaskGenericNotifyWait)
    *libfreertos.a:tasks.*(.literal.xTaskGetCoreID .text.xTaskGetCoreID)
    *libfreertos.a:tasks.*(.literal.xTaskGetCurrentTaskHandle .text.xTaskGetCurrentTaskHandle)
    *libfreertos.a:tasks.*(.literal.xTaskGetCurrentTaskHandleForCore .text.xTaskGetCurrentTaskHandleForCore)
    *libfreertos.a:tasks.*(.literal.xTaskGetHandle .text.xTaskGetHandle)
    *libfreertos.a:tasks.*(.literal.xTaskGetIdleTaskHandle .text.xTaskGetIdleTaskHandle)
    *libfreertos.a:tasks.*(.literal.xTaskGetIdleTaskHandleForCore .text.xTaskGetIdleTaskHandleForCore)
    *libfreertos.a:tasks.*(.literal.xTaskGetNext .text.xTaskGetNext)
    *libfreertos.a:tasks.*(.literal.xTaskPriorityDisinherit .text.xTaskPriorityDisinherit)
    *libfreertos.a:tasks.*(.literal.xTaskPriorityInherit .text.xTaskPriorityInherit)
    *libfreertos.a:tasks.*(.literal.xTaskResumeAll .text.xTaskResumeAll)
    *libfreertos.a:timers.*(.literal.pcTimerGetName .text.pcTimerGetName)
    *libfreertos.a:timers.*(.literal.prvCheckForValidListAndQueue .text.prvCheckForValidListAndQueue)
    *libfreertos.a:timers.*(.literal.prvInsertTimerInActiveList .text.prvInsertTimerInActiveList)
    *libfreertos.a:timers.*(.literal.prvProcessExpiredTimer .text.prvProcessExpiredTimer)
    *libfreertos.a:timers.*(.literal.prvReloadTimer .text.prvReloadTimer)
    *libfreertos.a:timers.*(.literal.prvSampleTimeNow .text.prvSampleTimeNow)
    *libfreertos.a:timers.*(.literal.prvTimerTask .text.prvTimerTask)
    *libfreertos.a:timers.*(.literal.pvTimerGetTimerID .text.pvTimerGetTimerID)
    *libfreertos.a:timers.*(.literal.uxTimerGetReloadMode .text.uxTimerGetReloadMode)
    *libfreertos.a:timers.*(.literal.vTimerSetReloadMode .text.vTimerSetReloadMode)
    *libfreertos.a:timers.*(.literal.vTimerSetTimerID .text.vTimerSetTimerID)
    *libfreertos.a:timers.*(.literal.xTimerCreate .text.xTimerCreate)
    *libfreertos.a:timers.*(.literal.xTimerCreateStatic .text.xTimerCreateStatic)
    *libfreertos.a:timers.*(.literal.xTimerCreateTimerTask .text.xTimerCreateTimerTask)
    *libfreertos.a:timers.*(.literal.xTimerGenericCommand .text.xTimerGenericCommand)
    *libfreertos.a:timers.*(.literal.xTimerGetExpiryTime .text.xTimerGetExpiryTime)
    *libfreertos.a:timers.*(.literal.xTimerGetPeriod .text.xTimerGetPeriod)
    *libfreertos.a:timers.*(.literal.xTimerGetTimerDaemonTaskHandle .text.xTimerGetTimerDaemonTaskHandle)
    *libfreertos.a:timers.*(.literal.xTimerIsTimerActive .text.xTimerIsTimerActive)
    *libfreertos.a:timers.*(.literal.xTimerPendFunctionCall .text.xTimerPendFunctionCall)
    *libhal.a:timer_hal.*(.literal.timer_hal_deinit .literal.timer_hal_init .text .text.timer_hal_deinit .text.timer_hal_init .text.timer_hal_set_counter_value)
    *liblog.a:log.*(.literal.esp_log_level_get .literal.esp_log_level_set .literal.esp_log_set_vprintf .literal.esp_log_writev .literal.fix_cache_generation_overflow .literal.heap_bubble_down .literal.s_log_level_get_and_unlock .text .text.esp_log_level_get .text.esp_log_level_set .text.esp_log_set_vprintf .text.esp_log_writev .text.fix_cache_generation_overflow .text.heap_bubble_down .text.s_log_level_get_and_unlock)
    *liblog.a:log_freertos.*(.literal.esp_log_system_timestamp .text .text.esp_log_system_timestamp)
    *liblwip.a:api_lib.*(.literal.netconn_accept .literal.netconn_apimsg .literal.netconn_bind .literal.netconn_bind_if .literal.netconn_close .literal.netconn_close_shutdown .literal.netconn_connect .literal.netconn_delete .literal.netconn_disconnect .literal.netconn_err .literal.netconn_getaddr .literal.netconn_gethostbyname_addrtype .literal.netconn_join_leave_group .literal.netconn_join_leave_group_netif .literal.netconn_listen_with_backlog .literal.netconn_new_with_proto_and_callback .literal.netconn_prepare_delete .literal.netconn_recv_tcp_pbuf .literal.netconn_recv_udp_raw_netbuf .literal.netconn_shutdown .literal.netconn_thread_cleanup .literal.netconn_thread_init .text .text.netconn_accept .text.netconn_apimsg .text.netconn_bind .text.netconn_bind_if .text.netconn_close .text.netconn_close_shutdown .text.netconn_connect .text.netconn_delete .text.netconn_disconnect .text.netconn_err .text.netconn_getaddr .text.netconn_gethostbyname_addrtype .text.netconn_join_leave_group .text.netconn_join_leave_group_netif .text.netconn_listen_with_backlog .text.netconn_new_with_proto_and_callback .text.netconn_prepare_delete .text.netconn_recv_tcp_pbuf .text.netconn_recv_udp_raw_netbuf .text.netconn_shutdown .text.netconn_thread_cleanup .text.netconn_thread_init)
    *liblwip.a:api_msg.*(.literal.accept_function .literal.err_tcp .literal.lwip_netconn_do_accepted .literal.lwip_netconn_do_bind .literal.lwip_netconn_do_bind_if .literal.lwip_netconn_do_close .literal.lwip_netconn_do_close_internal$isra$0 .literal.lwip_netconn_do_connect .literal.lwip_netconn_do_connected .literal.lwip_netconn_do_delconn .literal.lwip_netconn_do_disconnect .literal.lwip_netconn_do_dns_found .literal.lwip_netconn_do_getaddr .literal.lwip_netconn_do_gethostbyname .literal.lwip_netconn_do_join_leave_group .literal.lwip_netconn_do_join_leave_group_netif .literal.lwip_netconn_do_listen .literal.lwip_netconn_do_newconn .literal.lwip_netconn_do_recv .literal.lwip_netconn_do_writemore$isra$0 .literal.lwip_netconn_is_deallocated_msg .literal.lwip_netconn_is_err_msg .literal.netconn_alloc .literal.netconn_drain .literal.netconn_free .literal.netconn_mark_mbox_invalid .literal.poll_tcp .literal.recv_raw .literal.recv_tcp .literal.recv_udp .literal.sent_tcp .literal.setup_tcp .literal.tcp_do_abort .text .text.accept_function .text.err_tcp .text.lwip_netconn_do_accepted .text.lwip_netconn_do_bind .text.lwip_netconn_do_bind_if .text.lwip_netconn_do_close .text.lwip_netconn_do_close_internal$isra$0 .text.lwip_netconn_do_connect .text.lwip_netconn_do_connected .text.lwip_netconn_do_delconn .text.lwip_netconn_do_disconnect .text.lwip_netconn_do_dns_found .text.lwip_netconn_do_getaddr .text.lwip_netconn_do_gethostbyname .text.lwip_netconn_do_join_leave_group .text.lwip_netconn_do_join_leave_group_netif .text.lwip_netconn_do_listen .text.lwip_netconn_do_newconn .text.lwip_netconn_do_recv .text.lwip_netconn_do_writemore$isra$0 .text.lwip_netconn_is_deallocated_msg .text.lwip_netconn_is_err_msg .text.netconn_alloc .text.netconn_drain .text.netconn_free .text.netconn_mark_mbox_invalid .text.poll_tcp .text.recv_raw .text.recv_tcp .text.recv_udp .text.sent_tcp .text.setup_tcp .text.tcp_do_abort)
    *liblwip.a:etharp.*(.literal.etharp_add_static_entry .literal.etharp_cleanup_netif .literal.etharp_find_addr .literal.etharp_find_entry .literal.etharp_free_entry .literal.etharp_get_entry .literal.etharp_input .literal.etharp_query .literal.etharp_raw .literal.etharp_remove_static_entry .literal.etharp_request .literal.etharp_tmr .literal.etharp_update_arp_entry .text .text.etharp_add_static_entry .text.etharp_cleanup_netif .text.etharp_find_addr .text.etharp_find_entry .text.etharp_free_entry .text.etharp_get_entry .text.etharp_input .text.etharp_query .text.etharp_raw .text.etharp_remove_static_entry .text.etharp_request .text.etharp_tmr .text.etharp_update_arp_entry)
    *liblwip.a:ethernet.*(.text)
    *liblwip.a:inet_chksum.*(.literal.inet_chksum .literal.inet_chksum_pbuf .literal.inet_chksum_pseudo_partial .literal.inet_cksum_pseudo_partial_base .literal.ip6_chksum_pseudo .literal.ip6_chksum_pseudo_partial .literal.ip_chksum_pseudo .literal.ip_chksum_pseudo_partial .text .text.inet_chksum .text.inet_chksum_pbuf .text.inet_chksum_pseudo_partial .text.inet_cksum_pseudo_partial_base .text.ip6_chksum_pseudo .text.ip6_chksum_pseudo_partial .text.ip_chksum_pseudo .text.ip_chksum_pseudo_partial .text.lwip_standard_chksum)
    *liblwip.a:ip4.*(.literal.ip4_input_accept .literal.ip4_set_default_multicast_netif .text .text.ip4_input_accept .text.ip4_set_default_multicast_netif)
    *liblwip.a:ip4_addr.*(.literal.ip4_addr_netmask_valid .literal.ip4addr_aton .literal.ip4addr_ntoa .literal.ip4addr_ntoa_r .literal.ipaddr_addr .text .text.ip4_addr_netmask_valid .text.ip4addr_aton .text.ip4addr_ntoa .text.ip4addr_ntoa_r .text.ipaddr_addr)
    *liblwip.a:lwip_default_hooks.*(.literal.dhcp_parse_extra_opts .literal.ip4_netif_exist .text .text.dhcp_append_extra_opts .text.dhcp_parse_extra_opts .text.ip4_netif_exist)
    *liblwip.a:netbuf.*(.literal.netbuf_chain .literal.netbuf_delete .literal.netbuf_new .literal.netbuf_ref .text .text.netbuf_chain .text.netbuf_data .text.netbuf_delete .text.netbuf_first .text.netbuf_new .text.netbuf_next .text.netbuf_ref)
    *liblwip.a:pbuf.*(.literal.pbuf_add_header_force .literal.pbuf_add_header_impl .literal.pbuf_alloc_reference .literal.pbuf_cat .literal.pbuf_chain .literal.pbuf_clone .literal.pbuf_coalesce .literal.pbuf_copy .literal.pbuf_copy_partial .literal.pbuf_copy_partial_pbuf .literal.pbuf_dechain .literal.pbuf_free_header .literal.pbuf_free_ooseq_callback .literal.pbuf_get_at .literal.pbuf_get_contiguous .literal.pbuf_header_force .literal.pbuf_memcmp .literal.pbuf_memfind .literal.pbuf_put_at .literal.pbuf_realloc .literal.pbuf_ref .literal.pbuf_skip .literal.pbuf_strstr .literal.pbuf_take .literal.pbuf_take_at .literal.pbuf_try_get_at .text .text.pbuf_add_header_force .text.pbuf_add_header_impl .text.pbuf_alloc_reference .text.pbuf_cat .text.pbuf_chain .text.pbuf_clen .text.pbuf_clone .text.pbuf_coalesce .text.pbuf_copy .text.pbuf_copy_partial .text.pbuf_copy_partial_pbuf .text.pbuf_dechain .text.pbuf_free_header .text.pbuf_free_ooseq_callback .text.pbuf_get_at .text.pbuf_get_contiguous .text.pbuf_header_force .text.pbuf_memcmp .text.pbuf_memfind .text.pbuf_put_at .text.pbuf_realloc .text.pbuf_ref .text.pbuf_skip .text.pbuf_skip_const .text.pbuf_strstr .text.pbuf_take .text.pbuf_take_at .text.pbuf_try_get_at)
    *liblwip.a:sockets.*(.literal.alloc_socket .literal.free_socket .literal.free_socket_free_elements$isra$0 .literal.lwip_accept .literal.lwip_bind .literal.lwip_close .literal.lwip_connect .literal.lwip_fcntl .literal.lwip_getaddrname .literal.lwip_getpeername .literal.lwip_getsockname .literal.lwip_getsockopt .literal.lwip_getsockopt_callback .literal.lwip_inet_ntop .literal.lwip_inet_pton .literal.lwip_ioctl .literal.lwip_link_select_cb .literal.lwip_listen .literal.lwip_poll .literal.lwip_poll_dec_sockets_used .literal.lwip_pollscan .literal.lwip_read .literal.lwip_readv .literal.lwip_recv .literal.lwip_recvfrom_udp_raw$constprop$0 .literal.lwip_recvmsg .literal.lwip_select_dec_sockets_used .literal.lwip_select_inc_sockets_used_set .literal.lwip_selscan .literal.lwip_sendmsg .literal.lwip_setsockopt .literal.lwip_setsockopt_callback .literal.lwip_shutdown .literal.lwip_sock_make_addr$isra$0 .literal.lwip_socket .literal.lwip_socket_dbg_get_socket .literal.lwip_socket_thread_cleanup .literal.lwip_socket_thread_init .literal.lwip_sockopt_to_ipopt .literal.lwip_unlink_select_cb .literal.lwip_write .literal.lwip_writev .literal.sockaddr_to_ipaddr_port .literal.tryget_socket .literal.tryget_socket_unconn .text .text.alloc_socket .text.free_socket .text.free_socket_free_elements$isra$0 .text.lwip_accept .text.lwip_bind .text.lwip_close .text.lwip_connect .text.lwip_fcntl .text.lwip_getaddrname .text.lwip_getpeername .text.lwip_getsockname .text.lwip_getsockopt .text.lwip_getsockopt_callback .text.lwip_inet_ntop .text.lwip_inet_pton .text.lwip_ioctl .text.lwip_link_select_cb .text.lwip_listen .text.lwip_poll .text.lwip_poll_dec_sockets_used .text.lwip_pollscan .text.lwip_read .text.lwip_readv .text.lwip_recv .text.lwip_recvfrom_udp_raw$constprop$0 .text.lwip_recvmsg .text.lwip_select_dec_sockets_used .text.lwip_select_inc_sockets_used_set .text.lwip_selscan .text.lwip_sendmsg .text.lwip_setsockopt .text.lwip_setsockopt_callback .text.lwip_shutdown .text.lwip_sock_make_addr$isra$0 .text.lwip_socket .text.lwip_socket_dbg_get_socket .text.lwip_socket_thread_cleanup .text.lwip_socket_thread_init .text.lwip_sockopt_to_ipopt .text.lwip_unlink_select_cb .text.lwip_write .text.lwip_writev .text.sockaddr_to_ipaddr_port .text.tryget_socket .text.tryget_socket_unconn)
    *liblwip.a:sys_arch.*(.literal.sys_arch_mbox_tryfetch .literal.sys_arch_protect .literal.sys_arch_unprotect .literal.sys_delay_ms .literal.sys_init .literal.sys_jiffies .literal.sys_mbox_free .literal.sys_mbox_new .literal.sys_mbox_trypost_fromisr .literal.sys_mutex_free .literal.sys_mutex_new .literal.sys_now .literal.sys_sem_free .literal.sys_sem_new .literal.sys_sem_signal_isr .literal.sys_thread_new .literal.sys_thread_sem_deinit .literal.sys_thread_sem_free .literal.sys_thread_sem_get .literal.sys_thread_sem_init .literal.sys_thread_tcpip .text .text.sys_arch_mbox_tryfetch .text.sys_arch_protect .text.sys_arch_unprotect .text.sys_delay_ms .text.sys_init .text.sys_jiffies .text.sys_mbox_free .text.sys_mbox_new .text.sys_mbox_set_owner .text.sys_mbox_trypost_fromisr .text.sys_mutex_free .text.sys_mutex_new .text.sys_now .text.sys_sem_free .text.sys_sem_new .text.sys_sem_signal_isr .text.sys_thread_new .text.sys_thread_sem_deinit .text.sys_thread_sem_free .text.sys_thread_sem_get .text.sys_thread_sem_init .text.sys_thread_tcpip)
    *liblwip.a:tcpip.*(.literal.mem_free_callback .literal.pbuf_free_callback .literal.pbuf_free_int .literal.tcpip_api_call .literal.tcpip_callbackmsg_delete .literal.tcpip_callbackmsg_new .literal.tcpip_callbackmsg_trycallback .literal.tcpip_callbackmsg_trycallback_fromisr .literal.tcpip_init .text .text.mem_free_callback .text.pbuf_free_callback .text.pbuf_free_int .text.tcpip_api_call .text.tcpip_callbackmsg_delete .text.tcpip_callbackmsg_new .text.tcpip_callbackmsg_trycallback .text.tcpip_callbackmsg_trycallback_fromisr .text.tcpip_init)
    *liblwip.a:udp.*(.literal.udp_bind .literal.udp_connect .literal.udp_disconnect .literal.udp_init .literal.udp_netif_ip_addr_changed .literal.udp_new .literal.udp_new_ip_type .literal.udp_remove .text .text.udp_bind .text.udp_bind_netif .text.udp_connect .text.udp_disconnect .text.udp_init .text.udp_netif_ip_addr_changed .text.udp_new .text.udp_new_ip_type .text.udp_recv .text.udp_remove)
    *libphy.a:(.phyiram .phyiram.*)
    *libxtensa.a:xt_trax.*(.literal .literal.* .text .text.*)
    *libxtensa.a:xtensa_intr.*(.literal .literal.* .text .text.*)
    *(.stub)
    *(.gnu.warning)
    *(.gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.irom0.text) /* catch stray ICACHE_RODATA_ATTR */
    /**
     * CPU will try to prefetch up to 16 bytes of of instructions.
     * This means that any configuration (e.g. MMU, PMS) must allow
     * safe access to up to 16 bytes after the last real instruction,
     * add dummy bytes to ensure this.
     */
    . += 16;
    _text_end = ABSOLUTE(.);
    /**
     * Mark the flash.text end.
     * This can be used for MMU driver to maintain virtual address.
     */
    _instruction_reserved_end = ABSOLUTE(.);
    _etext = .;
    /**
     * Similar to _iram_start, this symbol goes here so it is
     * resolved by addr2line in preference to the first symbol in
     * the flash.text segment.
     */
    _flash_cache_start = ABSOLUTE(0);
  } > default_code_seg
  /* Marks the end of IRAM code segment */
  .iram0.text_end (NOLOAD) :
  {
    
 . = ALIGN(4); 
 _iram_text_end = ABSOLUTE(.);
  } > iram0_0_seg
  .iram0.data :
  {
    
 . = ALIGN(4); 
 _iram_data_start = ABSOLUTE(.);
    *(.iram.data .iram.data.*)
    _coredump_iram_start = ABSOLUTE(.);
    *(.iram2.coredump .iram2.coredump.*)
    _coredump_iram_end = ABSOLUTE(.);
    _iram_data_end = ABSOLUTE(.);
  } > iram0_0_seg
  .iram0.bss (NOLOAD) :
  {
    
 . = ALIGN(4); 
 _iram_bss_start = ABSOLUTE(.);
    *(.iram.bss .iram.bss.*)
    _iram_bss_end = ABSOLUTE(.);
    
 . = ALIGN(4); 
 _iram_end = ABSOLUTE(.);
  } > iram0_0_seg
  /* Marks the end of data, bss and possibly rodata  */
  .dram0.heap_start (NOLOAD) :
  {
    /* Lowest possible start address for the heap */
    
 . = ALIGN(8); 
 _heap_low_start = ABSOLUTE(.);
  } > dram0_0_seg
  /* DWARF 1 */
  .debug 0 : { *(.debug) }
  .line 0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo 0 : { *(.debug_srcinfo) }
  .debug_sfnames 0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges 0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info 0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev 0 : { *(.debug_abbrev) }
  .debug_line 0 : { *(.debug_line) }
  .debug_frame 0 : { *(.debug_frame) }
  .debug_str 0 : { *(.debug_str) }
  .debug_loc 0 : { *(.debug_loc) }
  .debug_macinfo 0 : { *(.debug_macinfo) }
  .debug_pubtypes 0 : { *(.debug_pubtypes) }
  /* DWARF 3 */
  .debug_ranges 0 : { *(.debug_ranges) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames 0 : { *(.debug_varnames) }
  /* GNU DWARF 2 extensions */
  .debug_gnu_pubnames 0 : { *(.debug_gnu_pubnames) }
  .debug_gnu_pubtypes 0 : { *(.debug_gnu_pubtypes) }
  /* DWARF 4 */
  .debug_types 0 : { *(.debug_types) }
  /* DWARF 5 */
  .debug_addr 0 : { *(.debug_addr) }
  .debug_line_str 0 : { *(.debug_line_str) }
  .debug_loclists 0 : { *(.debug_loclists) }
  .debug_macro 0 : { *(.debug_macro) }
  .debug_names 0 : { *(.debug_names) }
  .debug_rnglists 0 : { *(.debug_rnglists) }
  .debug_str_offsets 0 : { *(.debug_str_offsets) }
  .comment 0 : { *(.comment) }
  .note.GNU-stack 0: { *(.note.GNU-stack) }
/**
 * .xt.prop and .xt.lit sections will be used by the debugger and disassembler
 * to get more information about raw data present in the code.
 * Indeed, it may be required to add some padding at some points in the code
 * in order to align a branch/jump destination on a particular bound.
 * Padding these instructions will generate null bytes that shall be
 * interpreted as data, and not code by the debugger or disassembler.
 * This section will only be present in the ELF file, not in the final binary
 * For more details, check GCC-212
 */
  .xtensa.info 0: { *(.xtensa.info) }
  .xt.prop 0 : { *(.xt.prop .xt.prop.* .gnu.linkonce.prop.*) }
  .xt.lit 0 : { *(.xt.lit .xt.lit.* .gnu.linkonce.p.*) }
  /DISCARD/ :
  {
   *(.fini)
   *(.eh_frame_hdr)
   *(.eh_frame)
  }
}
ASSERT(((_iram_end - ORIGIN(iram0_0_seg)) <= LENGTH(iram0_0_seg)),
          "IRAM0 segment data does not fit.")
ASSERT(((_heap_low_start - ORIGIN(dram0_0_seg)) <= LENGTH(dram0_0_seg)),
          "DRAM segment data does not fit.")
