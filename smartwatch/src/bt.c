#ifdef __UNDEFINED__
#include "btstack_config.h"
#include "btstack_run_loop.h"

#define APP_AD_FLAGS 0x06
#include <btstack.h>

// clang-format off
#define APP_AD_FLAGS 0x06
static uint8_t adv_data[] = {
    // Flags general discoverable
    0x02, BLUETOOTH_DATA_TYPE_FLAGS, APP_AD_FLAGS,
    // Name
    0x12, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'P', 'i', 'c', 'o', ' ', 'S', 'm', 'a', 'r', 't', 'w', 'a', 't', 'c', 'h', 'O', 'S', 
    // Class
    0x03, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x1a, 0x18,
};
// clang-format on
static const uint8_t adv_data_len = sizeof(adv_data);

void bt_run(void)
{
    //
}
#endif
