#include "bluetooth.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "esp_nimble_hci.h"
#include "debug.h"


Bluetooth* Bluetooth::mInstance = 0;


static const ble_uuid128_t gatt_svr_svc_uuid =
    BLE_UUID128_INIT(0xdf, 0x9e, 0xb9, 0x20, 0x64, 0x48, 0xe2, 0x99,
                     0x7b, 0x4e, 0x72, 0xec, 0x12, 0x9a, 0xfb, 0xa5);

static const ble_uuid128_t gatt_svr_chr_uuid =
    BLE_UUID128_INIT(0xc, 0x8d, 0xf9, 0x12, 0xa5, 0xd5, 0xfe, 0xa7,
                     0x81, 0x46, 0x09, 0xf3, 0x34, 0x09, 0x9f, 0x03);   

static const char *tag = "NimBLE_BLE_CENT";
static int blecent_gap_event(struct ble_gap_event *event, void *arg);
static uint8_t peer_addr[6];

void ble_store_config_init(void);   


Bluetooth &
Bluetooth::instance() {
    if ( mInstance == 0 ) {
        mInstance = new Bluetooth;
    }

    return *mInstance;
}

Bluetooth::Bluetooth() : mAdvDone( false ) {

}
void nimble_host_task(void *param)
{
	nimble_port_run();
}

void ble_host_task(void *param)
{
	nimble_host_task(param);
}
static void 
on_reset(int reason) {
}

static void
on_sync(void)
{
    /* Begin advertising, scanning for peripherals, etc. */
}

void
Bluetooth::init() {
  nimble_port_init();
 // gatt_svr_init();

  /* Initialize the NimBLE host configuration. */
  ble_hs_cfg.reset_cb = on_reset;
  ble_hs_cfg.sync_cb = on_sync;
  //ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
  //ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
  ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_DISP_ONLY;

  ble_hs_cfg.sm_bonding = 1;
  ble_hs_cfg.sm_mitm = 1;
  ble_hs_cfg.sm_sc = 1;

  ble_hs_cfg.sm_our_key_dist = 1;
  ble_hs_cfg.sm_their_key_dist = 1;

  /* Register custom service */
  int rc;
  // = gatt_svr_register();
  //assert(rc == 0);

  /* Set the default device name. */
  char name[] = "TestBLEspp";
  rc = ble_svc_gap_device_name_set(name);
  assert(rc == 0);

  /* XXX Need to have template for store */
  //ble_store_config_init();

  nimble_port_freertos_init(ble_host_task);
}