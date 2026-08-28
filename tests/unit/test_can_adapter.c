/**
 * @file test_can_adapter.c
 * @brief Host USB-CAN adapter selection smoke tests (sim path; no HW required).
 */
#include "hal_can.h"
#include "hal_can_adapter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int fail;

static void expect(int cond, const char *name)
{
    if (!cond) {
        printf("FAIL %s\n", name);
        fail = 1;
    } else {
        printf("PASS %s\n", name);
    }
}

int main(void)
{
    ae_can_cfg_t cfg;
    ae_can_frame_t f;
    ae_status_t st;

    fail = 0;
    /* Ensure clean env for deterministic unit run */
#ifdef _WIN32
    _putenv("AE_CAN_BACKEND=");
#else
    unsetenv("AE_CAN_BACKEND");
#endif

    expect(hal_can_cfg_from_env(&cfg) == AE_OK, "cfg_from_env");
    expect(cfg.backend == AE_CAN_BE_SIM, "default_sim");
    expect(cfg.bitrate == 500000u, "default_bitrate");

    st = hal_can_adapter_set(AE_CAN_BE_SIM, "can0", 500000u);
    expect(st == AE_OK, "adapter_set");
    st = hal_can_init_from_env();
    expect(st == AE_OK, "init_from_env_sim");
    expect(!hal_can_adapter_is_live(), "not_live");
    expect(strcmp(hal_can_backend_name(hal_can_backend()), "sim") == 0, "name_sim");

    memset(&f, 0, sizeof(f));
    f.id = 0x1A0;
    f.dlc = 8;
    f.data[0] = 0xA5;
    expect(hal_can_send(&f, 10) == AE_OK, "sim_send");
    expect(hal_can_recv(&f) == AE_OK, "sim_recv");
    expect(f.id == 0x1A0u, "sim_echo_id");

    /* Invalid DLC rejected on sim path */
    f.dlc = 9;
    expect(hal_can_send(&f, 10) != AE_OK, "dlc_reject");

    printf(fail ? "RESULT FAIL\n" : "RESULT PASS\n");
    return fail;
}
