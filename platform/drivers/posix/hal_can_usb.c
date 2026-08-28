/**
 * @file hal_can_usb.c
 * @brief Host Classical CAN: sim + CANable/SocketCAN + PCAN-USB + SLCAN.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 * @note Link this instead of (or with) hal_host CAN symbols removed from host.
 *       Products stay on hal_can.h only.
 */

#include "hal_can.h"
#include "hal_can_adapter.h"

#include "ae_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__linux__)
#include <errno.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <termios.h>
#endif

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#if defined(AE_HAVE_PCANBASIC)
#include "PCANBasic.h"
#endif

#define AE_BUS_DEPTH 64u

static ae_can_frame_t s_bus[AE_BUS_DEPTH];
static uint32_t s_bus_head;
static uint32_t s_bus_tail;
static uint32_t s_bus_count;
static ae_can_rx_isr_cb_t s_rx_cb;
static void *s_rx_ctx;

static uint8_t s_backend = AE_CAN_BE_SIM;
static char s_iface[32] = "can0";
static uint32_t s_bitrate = 500000u;
#if defined(__linux__)
static int s_live_fd = -1; /* SocketCAN socket or SLCAN fd */
#endif

#if defined(_WIN32)
static HANDLE s_slcan_win = INVALID_HANDLE_VALUE;
#if defined(AE_HAVE_PCANBASIC)
static TPCANHandle s_pcan = PCAN_NONEBUS;
#endif
#endif

static void sim_reset(void)
{
    s_bus_head = 0u;
    s_bus_tail = 0u;
    s_bus_count = 0u;
}

static ae_status_t sim_send(const ae_can_frame_t *frame)
{
    uint32_t next;

    if (frame == NULL) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    if (frame->dlc > AE_CAN_DATA_MAX) {
        return ae_err_make(AE_MOD_HAL, 4u);
    }
    next = (s_bus_head + 1u) % AE_BUS_DEPTH;
    if (next == s_bus_tail) {
        return ae_err_make(AE_MOD_HAL, 2u);
    }
    s_bus[s_bus_head] = *frame;
    s_bus_head = next;
    s_bus_count++;
    if (s_rx_cb != NULL) {
        s_rx_cb(frame, s_rx_ctx);
    }
    return AE_OK;
}

static ae_status_t sim_recv(ae_can_frame_t *frame)
{
    if (frame == NULL) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    if (s_bus_head == s_bus_tail) {
        return ae_err_make(AE_MOD_HAL, 3u);
    }
    *frame = s_bus[s_bus_tail];
    s_bus_tail = (s_bus_tail + 1u) % AE_BUS_DEPTH;
    return AE_OK;
}

const char *hal_can_backend_name(uint8_t backend)
{
    switch (backend) {
    case AE_CAN_BE_SIM:
        return "sim";
    case AE_CAN_BE_SOCKETCAN:
        return "socketcan";
    case AE_CAN_BE_PCAN:
        return "pcan";
    case AE_CAN_BE_SLCAN:
        return "slcan";
    default:
        return "unknown";
    }
}

uint8_t hal_can_backend(void)
{
    return s_backend;
}

int hal_can_adapter_is_live(void)
{
    return (s_backend != AE_CAN_BE_SIM) ? 1 : 0;
}

ae_status_t hal_can_adapter_set(uint8_t backend, const char *iface, uint32_t bitrate)
{
    s_backend = backend;
    if ((iface != NULL) && (iface[0] != '\0')) {
        strncpy(s_iface, iface, sizeof(s_iface) - 1u);
        s_iface[sizeof(s_iface) - 1u] = '\0';
    }
    if (bitrate > 0u) {
        s_bitrate = bitrate;
    }
    return AE_OK;
}

ae_status_t hal_can_cfg_from_env(ae_can_cfg_t *cfg)
{
    const char *be;
    const char *ifc;
    const char *br;

    if (cfg == NULL) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    memset(cfg, 0, sizeof(*cfg));
    cfg->bitrate = 500000u;
    cfg->backend = AE_CAN_BE_SIM;
    strncpy(cfg->iface, "can0", sizeof(cfg->iface) - 1u);

    be = getenv("AE_CAN_BACKEND");
    ifc = getenv("AE_CAN_IFACE");
    br = getenv("AE_CAN_BITRATE");
    if (br != NULL) {
        unsigned long v = strtoul(br, NULL, 10);
        if (v > 0u) {
            cfg->bitrate = (uint32_t)v;
        }
    }
    if (ifc != NULL && ifc[0] != '\0') {
        strncpy(cfg->iface, ifc, sizeof(cfg->iface) - 1u);
    }
    if (be != NULL && be[0] != '\0') {
        if (strcmp(be, "socketcan") == 0 || strcmp(be, "canable") == 0) {
            cfg->backend = AE_CAN_BE_SOCKETCAN;
        } else if (strcmp(be, "pcan") == 0 || strcmp(be, "pcan-usb") == 0) {
            cfg->backend = AE_CAN_BE_PCAN;
        } else if (strcmp(be, "slcan") == 0) {
            cfg->backend = AE_CAN_BE_SLCAN;
        } else {
            cfg->backend = AE_CAN_BE_SIM;
        }
    }
    return AE_OK;
}

ae_status_t hal_can_init_from_env(void)
{
    ae_can_cfg_t cfg;
    (void)hal_can_cfg_from_env(&cfg);
    return hal_can_init(&cfg);
}

static void close_live(void)
{
#if defined(__linux__)
    if (s_live_fd >= 0) {
        close(s_live_fd);
        s_live_fd = -1;
    }
#endif
#if defined(_WIN32)
    if (s_slcan_win != INVALID_HANDLE_VALUE) {
        CloseHandle(s_slcan_win);
        s_slcan_win = INVALID_HANDLE_VALUE;
    }
#if defined(AE_HAVE_PCANBASIC)
    if (s_pcan != PCAN_NONEBUS) {
        CAN_Uninitialize(s_pcan);
        s_pcan = PCAN_NONEBUS;
    }
#endif
#endif
}

#if defined(__linux__)
static ae_status_t socketcan_open(const char *iface)
{
    struct ifreq ifr;
    struct sockaddr_can addr;
    int s;
    int flags;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        return ae_err_make(AE_MOD_DRV, 1u);
    }
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        close(s);
        return ae_err_make(AE_MOD_DRV, 2u);
    }
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(s);
        return ae_err_make(AE_MOD_DRV, 3u);
    }
    flags = fcntl(s, F_GETFL, 0);
    if (flags >= 0) {
        (void)fcntl(s, F_SETFL, flags | O_NONBLOCK);
    }
    s_live_fd = s;
    return AE_OK;
}

static ae_status_t socketcan_send(const ae_can_frame_t *frame)
{
    struct can_frame cf;
    ssize_t n;

    if (s_live_fd < 0 || frame == NULL) {
        return ae_err_make(AE_MOD_DRV, 4u);
    }
    if (frame->dlc > 8u) {
        return ae_err_make(AE_MOD_HAL, 4u);
    }
    memset(&cf, 0, sizeof(cf));
    cf.can_id = frame->id & CAN_SFF_MASK;
    if (frame->ide) {
        cf.can_id = (frame->id & CAN_EFF_MASK) | CAN_EFF_FLAG;
    }
    if (frame->rtr) {
        cf.can_id |= CAN_RTR_FLAG;
    }
    cf.can_dlc = frame->dlc;
    memcpy(cf.data, frame->data, frame->dlc);
    n = write(s_live_fd, &cf, sizeof(cf));
    if (n != (ssize_t)sizeof(cf)) {
        return ae_err_make(AE_MOD_DRV, 5u);
    }
    /* Local echo so host use cases can drain via hal_can_recv. */
    return sim_send(frame);
}

static ae_status_t socketcan_recv(ae_can_frame_t *frame)
{
    struct can_frame cf;
    ssize_t n;

    if (s_live_fd < 0 || frame == NULL) {
        return ae_err_make(AE_MOD_DRV, 4u);
    }
    n = read(s_live_fd, &cf, sizeof(cf));
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return ae_err_make(AE_MOD_HAL, 3u);
        }
        return ae_err_make(AE_MOD_DRV, 6u);
    }
    memset(frame, 0, sizeof(*frame));
    if (cf.can_id & CAN_EFF_FLAG) {
        frame->ide = 1u;
        frame->id = cf.can_id & CAN_EFF_MASK;
    } else {
        frame->id = cf.can_id & CAN_SFF_MASK;
    }
    frame->rtr = (cf.can_id & CAN_RTR_FLAG) ? 1u : 0u;
    frame->dlc = cf.can_dlc;
    memcpy(frame->data, cf.data, frame->dlc);
    if (s_rx_cb != NULL) {
        s_rx_cb(frame, s_rx_ctx);
    }
    return AE_OK;
}

static int slcan_bitrate_code(uint32_t br)
{
    if (br == 1000000u) {
        return 8;
    }
    if (br == 800000u) {
        return 7;
    }
    if (br == 500000u) {
        return 6;
    }
    if (br == 250000u) {
        return 5;
    }
    if (br == 125000u) {
        return 4;
    }
    if (br == 100000u) {
        return 3;
    }
    if (br == 50000u) {
        return 2;
    }
    if (br == 20000u) {
        return 1;
    }
    if (br == 10000u) {
        return 0;
    }
    return 6; /* default 500k */
}

static ae_status_t slcan_open_linux(const char *path, uint32_t bitrate)
{
    int fd;
    struct termios tio;
    char cmd[16];
    int code;

    fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        return ae_err_make(AE_MOD_DRV, 10u);
    }
    if (tcgetattr(fd, &tio) == 0) {
        cfmakeraw(&tio);
        cfsetispeed(&tio, B115200);
        cfsetospeed(&tio, B115200);
        tio.c_cflag |= (CLOCAL | CREAD);
        (void)tcsetattr(fd, TCSANOW, &tio);
    }
    code = slcan_bitrate_code(bitrate);
    (void)snprintf(cmd, sizeof(cmd), "C\r");
    (void)write(fd, cmd, strlen(cmd));
    (void)snprintf(cmd, sizeof(cmd), "S%d\r", code);
    (void)write(fd, cmd, strlen(cmd));
    (void)snprintf(cmd, sizeof(cmd), "O\r");
    if (write(fd, cmd, strlen(cmd)) < 0) {
        close(fd);
        return ae_err_make(AE_MOD_DRV, 11u);
    }
    s_live_fd = fd;
    return AE_OK;
}

static ae_status_t slcan_send_linux(const ae_can_frame_t *frame)
{
    char buf[64];
    int n;
    uint8_t i;

    if (s_live_fd < 0 || frame == NULL || frame->dlc > 8u) {
        return ae_err_make(AE_MOD_DRV, 12u);
    }
    if (frame->ide) {
        n = snprintf(buf, sizeof(buf), "T%08X%u", frame->id & 0x1FFFFFFFu, frame->dlc);
    } else {
        n = snprintf(buf, sizeof(buf), "t%03X%u", frame->id & 0x7FFu, frame->dlc);
    }
    if (n < 0 || (size_t)n >= sizeof(buf)) {
        return ae_err_make(AE_MOD_DRV, 12u);
    }
    for (i = 0u; i < frame->dlc; i++) {
        n += snprintf(buf + n, sizeof(buf) - (size_t)n, "%02X", frame->data[i]);
    }
    n += snprintf(buf + n, sizeof(buf) - (size_t)n, "\r");
    if (write(s_live_fd, buf, (size_t)n) != n) {
        return ae_err_make(AE_MOD_DRV, 13u);
    }
    return sim_send(frame);
}
#endif /* __linux__ */

#if defined(_WIN32)
static int slcan_bitrate_code_w(uint32_t br)
{
    if (br == 1000000u) {
        return 8;
    }
    if (br == 500000u) {
        return 6;
    }
    if (br == 250000u) {
        return 5;
    }
    if (br == 125000u) {
        return 4;
    }
    return 6;
}

static ae_status_t slcan_open_win(const char *port, uint32_t bitrate)
{
    char path[64];
    DCB dcb;
    char cmd[16];
    DWORD wr;
    int code;

    if (strncmp(port, "\\\\.\\", 4) == 0) {
        strncpy(path, port, sizeof(path) - 1u);
    } else {
        (void)snprintf(path, sizeof(path), "\\\\.\\%s", port);
    }
    path[sizeof(path) - 1u] = '\0';
    s_slcan_win = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (s_slcan_win == INVALID_HANDLE_VALUE) {
        return ae_err_make(AE_MOD_DRV, 10u);
    }
    memset(&dcb, 0, sizeof(dcb));
    dcb.DCBlength = sizeof(dcb);
    if (GetCommState(s_slcan_win, &dcb)) {
        dcb.BaudRate = CBR_115200;
        dcb.ByteSize = 8;
        dcb.Parity = NOPARITY;
        dcb.StopBits = ONESTOPBIT;
        (void)SetCommState(s_slcan_win, &dcb);
    }
    code = slcan_bitrate_code_w(bitrate);
    (void)snprintf(cmd, sizeof(cmd), "C\r");
    (void)WriteFile(s_slcan_win, cmd, (DWORD)strlen(cmd), &wr, NULL);
    (void)snprintf(cmd, sizeof(cmd), "S%d\r", code);
    (void)WriteFile(s_slcan_win, cmd, (DWORD)strlen(cmd), &wr, NULL);
    (void)snprintf(cmd, sizeof(cmd), "O\r");
    if (!WriteFile(s_slcan_win, cmd, (DWORD)strlen(cmd), &wr, NULL)) {
        CloseHandle(s_slcan_win);
        s_slcan_win = INVALID_HANDLE_VALUE;
        return ae_err_make(AE_MOD_DRV, 11u);
    }
    return AE_OK;
}

static ae_status_t slcan_send_win(const ae_can_frame_t *frame)
{
    char buf[64];
    int n;
    DWORD wr;
    uint8_t i;

    if (s_slcan_win == INVALID_HANDLE_VALUE || frame == NULL || frame->dlc > 8u) {
        return ae_err_make(AE_MOD_DRV, 12u);
    }
    if (frame->ide) {
        n = snprintf(buf, sizeof(buf), "T%08X%u", frame->id & 0x1FFFFFFFu, frame->dlc);
    } else {
        n = snprintf(buf, sizeof(buf), "t%03X%u", frame->id & 0x7FFu, frame->dlc);
    }
    for (i = 0u; i < frame->dlc; i++) {
        n += snprintf(buf + n, sizeof(buf) - (size_t)n, "%02X", frame->data[i]);
    }
    n += snprintf(buf + n, sizeof(buf) - (size_t)n, "\r");
    if (!WriteFile(s_slcan_win, buf, (DWORD)n, &wr, NULL) || (int)wr != n) {
        return ae_err_make(AE_MOD_DRV, 13u);
    }
    return sim_send(frame);
}

#if defined(AE_HAVE_PCANBASIC)
static TPCANHandle pcan_resolve(const char *iface)
{
    if (iface == NULL) {
        return PCAN_USBBUS1;
    }
    if (strcmp(iface, "PCAN_USBBUS1") == 0 || strcmp(iface, "usb1") == 0) {
        return PCAN_USBBUS1;
    }
    if (strcmp(iface, "PCAN_USBBUS2") == 0 || strcmp(iface, "usb2") == 0) {
        return PCAN_USBBUS2;
    }
    return PCAN_USBBUS1;
}

static TPCANBaudrate pcan_baud(uint32_t br)
{
    if (br == 1000000u) {
        return PCAN_BAUD_1M;
    }
    if (br == 500000u) {
        return PCAN_BAUD_500K;
    }
    if (br == 250000u) {
        return PCAN_BAUD_250K;
    }
    if (br == 125000u) {
        return PCAN_BAUD_125K;
    }
    return PCAN_BAUD_500K;
}

static ae_status_t pcan_open(const char *iface, uint32_t bitrate)
{
    TPCANStatus st;
    s_pcan = pcan_resolve(iface);
    st = CAN_Initialize(s_pcan, pcan_baud(bitrate), 0, 0, 0);
    if (st != PCAN_ERROR_OK) {
        s_pcan = PCAN_NONEBUS;
        return ae_err_make(AE_MOD_DRV, 20u);
    }
    return AE_OK;
}

static ae_status_t pcan_send(const ae_can_frame_t *frame)
{
    TPCANMsg msg;
    TPCANStatus st;

    if (s_pcan == PCAN_NONEBUS || frame == NULL || frame->dlc > 8u) {
        return ae_err_make(AE_MOD_DRV, 21u);
    }
    memset(&msg, 0, sizeof(msg));
    msg.ID = frame->id;
    msg.MSGTYPE = frame->ide ? PCAN_MESSAGE_EXTENDED : PCAN_MESSAGE_STANDARD;
    if (frame->rtr) {
        msg.MSGTYPE |= PCAN_MESSAGE_RTR;
    }
    msg.LEN = frame->dlc;
    memcpy(msg.DATA, frame->data, frame->dlc);
    st = CAN_Write(s_pcan, &msg);
    if (st != PCAN_ERROR_OK) {
        return ae_err_make(AE_MOD_DRV, 22u);
    }
    return sim_send(frame);
}

static ae_status_t pcan_recv(ae_can_frame_t *frame)
{
    TPCANMsg msg;
    TPCANTimestamp ts;
    TPCANStatus st;

    if (s_pcan == PCAN_NONEBUS || frame == NULL) {
        return ae_err_make(AE_MOD_DRV, 21u);
    }
    st = CAN_Read(s_pcan, &msg, &ts);
    if (st == PCAN_ERROR_QRCVEMPTY) {
        return ae_err_make(AE_MOD_HAL, 3u);
    }
    if (st != PCAN_ERROR_OK) {
        return ae_err_make(AE_MOD_DRV, 23u);
    }
    memset(frame, 0, sizeof(*frame));
    frame->id = msg.ID;
    frame->ide = (msg.MSGTYPE & PCAN_MESSAGE_EXTENDED) ? 1u : 0u;
    frame->rtr = (msg.MSGTYPE & PCAN_MESSAGE_RTR) ? 1u : 0u;
    frame->dlc = msg.LEN;
    memcpy(frame->data, msg.DATA, frame->dlc);
    if (s_rx_cb != NULL) {
        s_rx_cb(frame, s_rx_ctx);
    }
    return AE_OK;
}
#endif /* AE_HAVE_PCANBASIC */
#endif /* _WIN32 */

ae_status_t hal_can_init(const ae_can_cfg_t *cfg)
{
    ae_status_t st = AE_OK;

    close_live();
    sim_reset();
    if (cfg != NULL) {
        s_backend = cfg->backend;
        s_bitrate = (cfg->bitrate > 0u) ? cfg->bitrate : 500000u;
        if (cfg->iface[0] != '\0') {
            strncpy(s_iface, cfg->iface, sizeof(s_iface) - 1u);
            s_iface[sizeof(s_iface) - 1u] = '\0';
        }
    }

    if (s_backend == AE_CAN_BE_SIM) {
        return AE_OK;
    }

#if defined(__linux__)
    if (s_backend == AE_CAN_BE_SOCKETCAN) {
        st = socketcan_open(s_iface);
        if (st != AE_OK) {
            s_backend = AE_CAN_BE_SIM;
        }
        return st;
    }
    if (s_backend == AE_CAN_BE_SLCAN) {
        st = slcan_open_linux(s_iface, s_bitrate);
        if (st != AE_OK) {
            s_backend = AE_CAN_BE_SIM;
        }
        return st;
    }
    if (s_backend == AE_CAN_BE_PCAN) {
        /* On Linux, prefer SocketCAN peak_usb after ip link — same API. */
        st = socketcan_open(s_iface);
        if (st != AE_OK) {
            s_backend = AE_CAN_BE_SIM;
        } else {
            s_backend = AE_CAN_BE_SOCKETCAN;
        }
        return st;
    }
#elif defined(_WIN32)
    if (s_backend == AE_CAN_BE_SLCAN) {
        st = slcan_open_win(s_iface, s_bitrate);
        if (st != AE_OK) {
            s_backend = AE_CAN_BE_SIM;
        }
        return st;
    }
    if (s_backend == AE_CAN_BE_PCAN) {
#if defined(AE_HAVE_PCANBASIC)
        st = pcan_open(s_iface, s_bitrate);
        if (st != AE_OK) {
            s_backend = AE_CAN_BE_SIM;
        }
        return st;
#else
        (void)st;
        s_backend = AE_CAN_BE_SIM;
        return ae_err_make(AE_MOD_DRV, 24u); /* rebuild with AE_HAVE_PCANBASIC + PCANBasic */
#endif
    }
    if (s_backend == AE_CAN_BE_SOCKETCAN) {
        /* No native SocketCAN on Windows — try SLCAN path as CANable fallback. */
        st = slcan_open_win(s_iface, s_bitrate);
        if (st != AE_OK) {
            s_backend = AE_CAN_BE_SIM;
            return ae_err_make(AE_MOD_DRV, 25u);
        }
        s_backend = AE_CAN_BE_SLCAN;
        return AE_OK;
    }
#else
    (void)st;
    s_backend = AE_CAN_BE_SIM;
    return ae_err_make(AE_MOD_DRV, 26u);
#endif
    return AE_OK;
}

void hal_can_bus_reset(void)
{
    close_live();
    sim_reset();
    /* Keep backend selection; re-open on next init. */
}

uint32_t hal_can_bus_count(void)
{
    return s_bus_count;
}

ae_status_t hal_can_attach_rx(ae_can_rx_isr_cb_t cb, void *ctx)
{
    s_rx_cb = cb;
    s_rx_ctx = ctx;
    return AE_OK;
}

ae_status_t hal_can_set_filter(const ae_can_filter_t *filters, uint8_t count)
{
    (void)filters;
    (void)count;
    /* SocketCAN/PCAN hardware filters can be added later; accept for now. */
    return AE_OK;
}

ae_status_t hal_can_send(const ae_can_frame_t *frame, uint32_t timeout_ms)
{
    (void)timeout_ms;
#if defined(__linux__)
    if (s_backend == AE_CAN_BE_SOCKETCAN) {
        return socketcan_send(frame);
    }
    if (s_backend == AE_CAN_BE_SLCAN) {
        return slcan_send_linux(frame);
    }
#endif
#if defined(_WIN32)
    if (s_backend == AE_CAN_BE_SLCAN) {
        return slcan_send_win(frame);
    }
#if defined(AE_HAVE_PCANBASIC)
    if (s_backend == AE_CAN_BE_PCAN) {
        return pcan_send(frame);
    }
#endif
#endif
    return sim_send(frame);
}

ae_status_t hal_can_recv(ae_can_frame_t *frame)
{
#if defined(__linux__)
    if (s_backend == AE_CAN_BE_SOCKETCAN) {
        return socketcan_recv(frame);
    }
    if (s_backend == AE_CAN_BE_SLCAN) {
        /* RX parse can be extended; drain not implemented → empty. */
        return ae_err_make(AE_MOD_HAL, 3u);
    }
#endif
#if defined(_WIN32) && defined(AE_HAVE_PCANBASIC)
    if (s_backend == AE_CAN_BE_PCAN) {
        return pcan_recv(frame);
    }
#endif
    return sim_recv(frame);
}
