/*  Bluetooth Mesh */

/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "syscfg/syscfg.h"
#define MESH_LOG_MODULE BLE_MESH_ADV_LOG

#include "mesh/mesh.h"
#include "host/ble_hs_adv.h"
#include "host/ble_gap.h"
#include "mesh/porting.h"
#include "nimble/nimble_port.h"

#include "adv.h"
#include "net.h"
#include "foundation.h"
#include "beacon.h"
#include "prov.h"
#include "proxy.h"

/* Window and Interval are equal for continuous scanning */
#define MESH_SCAN_INTERVAL    BT_MESH_ADV_SCAN_UNIT(BT_MESH_SCAN_INTERVAL_MS)
#define MESH_SCAN_WINDOW      BT_MESH_ADV_SCAN_UNIT(BT_MESH_SCAN_WINDOW_MS)

<<<<<<< HEAD
const uint8_t bt_mesh_adv_type[BT_MESH_ADV_TYPES] = {
	[BT_MESH_ADV_PROV]   = BT_DATA_MESH_PROV,
	[BT_MESH_ADV_DATA]   = BT_DATA_MESH_MESSAGE,
	[BT_MESH_ADV_BEACON] = BT_DATA_MESH_BEACON,
	[BT_MESH_ADV_URI]    = BT_DATA_URI,
};
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
=======
/* Pre-5.0 controllers enforce a minimum interval of 100ms
 * whereas 5.0+ controllers can go down to 20ms.
 */
#define ADV_INT_DEFAULT_MS 100
#define ADV_INT_FAST_MS    20

static s32_t adv_int_min =  ADV_INT_DEFAULT_MS;

/* TinyCrypt PRNG consumes a lot of stack space, so we need to have
 * an increased call stack whenever it's used.
 */
#if MYNEWT
#define ADV_STACK_SIZE 768
OS_TASK_STACK_DEFINE(g_blemesh_stack, ADV_STACK_SIZE);
struct os_task adv_task;
#else
static TaskHandle_t adv_task_h;
>>>>>>> 4ed953346e55320ddcd09e0633fd2c4a8aef6b5b
#endif

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

extern uint8_t g_mesh_addr_type;

struct os_mbuf_pool adv_os_mbuf_pool;
struct ble_npl_eventq bt_mesh_adv_queue;

os_membuf_t adv_buf_mem[OS_MEMPOOL_SIZE(
        MYNEWT_VAL(BLE_MESH_ADV_BUF_COUNT),
        BT_MESH_ADV_DATA_SIZE + BT_MESH_MBUF_HEADER_SIZE)];
struct os_mempool adv_buf_mempool;

static struct bt_mesh_adv adv_pool[CONFIG_BT_MESH_ADV_BUF_COUNT];

static struct bt_mesh_adv *adv_alloc(int id)
{
	return &adv_pool[id];
}

struct os_mbuf *bt_mesh_adv_create_from_pool(struct os_mbuf_pool *pool,
					     bt_mesh_adv_alloc_t get_id,
					     enum bt_mesh_adv_type type,
					     uint8_t xmit, int32_t timeout)
{
	struct bt_mesh_adv *adv;
	struct os_mbuf *buf;

	if (atomic_test_bit(bt_mesh.flags, BT_MESH_SUSPENDED)) {
		BT_WARN("Refusing to allocate buffer while suspended");
		return NULL;
	}

	buf = os_mbuf_get_pkthdr(pool, BT_MESH_ADV_USER_DATA_SIZE);
	if (!buf) {
		return NULL;
	}

	adv = get_id(net_buf_id(buf));
	BT_MESH_ADV(buf) = adv;

	memset(adv, 0, sizeof(*adv));

	adv->type         = type;
	adv->xmit         = xmit;

	adv->ref_cnt = 1;
	ble_npl_event_set_arg(&adv->ev, buf);

	return buf;
}

struct os_mbuf *bt_mesh_adv_create(enum bt_mesh_adv_type type, uint8_t xmit,
				   int32_t timeout)
{
	return bt_mesh_adv_create_from_pool(&adv_os_mbuf_pool, adv_alloc, type,
					    xmit, timeout);
}

void bt_mesh_adv_send(struct os_mbuf *buf, const struct bt_mesh_send_cb *cb,
		      void *cb_data)
{
	BT_DBG("buf %p, type 0x%02x len %u: %s", buf, BT_MESH_ADV(buf)->type, buf->om_len,
	       bt_hex(buf->om_data, buf->om_len));

	BT_MESH_ADV(buf)->cb = cb;
	BT_MESH_ADV(buf)->cb_data = cb_data;
	BT_MESH_ADV(buf)->busy = 1;

	net_buf_put(&bt_mesh_adv_queue, net_buf_ref(buf));
	bt_mesh_adv_buf_ready();
}

static void bt_mesh_scan_cb(const bt_addr_le_t *addr, int8_t rssi,
			    uint8_t adv_type, struct os_mbuf *buf)
{
	if (adv_type != BLE_HCI_ADV_TYPE_ADV_NONCONN_IND) {
		return;
	}

#if BT_MESH_EXTENDED_DEBUG
	BT_DBG("len %u: %s", buf->om_len, bt_hex(buf->om_data, buf->om_len));
#endif

	while (buf->om_len > 1) {
		struct net_buf_simple_state state;
		uint8_t len, type;

		len = net_buf_simple_pull_u8(buf);
		/* Check for early termination */
		if (len == 0) {
			return;
		}

		if (len > buf->om_len) {
			BT_WARN("AD malformed");
			return;
		}

		net_buf_simple_save(buf, &state);

		type = net_buf_simple_pull_u8(buf);

		switch (type) {
		case BLE_HS_ADV_TYPE_MESH_MESSAGE:
			bt_mesh_net_recv(buf, rssi, BT_MESH_NET_IF_ADV);
			break;
#if MYNEWT_VAL(BLE_MESH_PB_ADV)
		case BLE_HS_ADV_TYPE_MESH_PROV:
			bt_mesh_pb_adv_recv(buf);
			break;
#endif
		case BLE_HS_ADV_TYPE_MESH_BEACON:
			bt_mesh_beacon_recv(buf);
			break;
		default:
			break;
		}

		net_buf_simple_restore(buf, &state);
		net_buf_simple_pull_mem(buf, len);
	}
}

<<<<<<< HEAD
=======
void bt_mesh_adv_init(void)
{
	int rc;

	/* Advertising should only be initialized once. Calling
	 * os_task init the second time will result in an assert. */
	if (adv_initialized) {
		return;
	}

	rc = os_mempool_init(&adv_buf_mempool, MYNEWT_VAL(BLE_MESH_ADV_BUF_COUNT),
			     BT_MESH_ADV_DATA_SIZE + BT_MESH_MBUF_HEADER_SIZE,
			     adv_buf_mem, "adv_buf_pool");
	assert(rc == 0);

	rc = os_mbuf_pool_init(&adv_os_mbuf_pool, &adv_buf_mempool,
			       BT_MESH_ADV_DATA_SIZE + BT_MESH_MBUF_HEADER_SIZE,
			       MYNEWT_VAL(BLE_MESH_ADV_BUF_COUNT));
	assert(rc == 0);

	ble_npl_eventq_init(&adv_queue);

#if MYNEWT
	os_task_init(&adv_task, "mesh_adv", mesh_adv_thread, NULL,
	             MYNEWT_VAL(BLE_MESH_ADV_TASK_PRIO), OS_WAIT_FOREVER,
	             g_blemesh_stack, ADV_STACK_SIZE);
#else
    xTaskCreatePinnedToCore(mesh_adv_thread, "mesh_adv", 2768,
            NULL, (configMAX_PRIORITIES - 5), &adv_task_h, NIMBLE_CORE);
#endif

	/* For BT5 controllers we can have fast advertising interval */
	if (ble_hs_hci_get_hci_version() >= BLE_HCI_VER_BCS_5_0) {
	    adv_int_min = ADV_INT_FAST_MS;
	}

	adv_initialized = true;
}

>>>>>>> 4ed953346e55320ddcd09e0633fd2c4a8aef6b5b
int
ble_adv_gap_mesh_cb(struct ble_gap_event *event, void *arg)
{
#if MYNEWT_VAL(BLE_EXT_ADV)
	struct ble_gap_ext_disc_desc *ext_desc;
#endif
	struct ble_gap_disc_desc *desc;
	struct os_mbuf *buf = NULL;

#if BT_MESH_EXTENDED_DEBUG
	BT_DBG("event->type %d", event->type);
#endif

	switch (event->type) {
#if MYNEWT_VAL(BLE_EXT_ADV)
	case BLE_GAP_EVENT_EXT_DISC:
		ext_desc = &event->ext_disc;
		buf = os_mbuf_get_pkthdr(&adv_os_mbuf_pool, 0);
		if (!buf || os_mbuf_append(buf, ext_desc->data, ext_desc->length_data)) {
			BT_ERR("Could not append data");
			goto done;
		}
		bt_mesh_scan_cb(&ext_desc->addr, ext_desc->rssi,
				ext_desc->legacy_event_type, buf);
		break;
#endif
	case BLE_GAP_EVENT_DISC:
		desc = &event->disc;
		buf = os_mbuf_get_pkthdr(&adv_os_mbuf_pool, 0);
		if (!buf || os_mbuf_append(buf, desc->data, desc->length_data)) {
			BT_ERR("Could not append data");
			goto done;
		}

		bt_mesh_scan_cb(&desc->addr, desc->rssi, desc->event_type, buf);
		break;
	default:
		break;
	}

done:
	if (buf) {
		os_mbuf_free_chain(buf);
	}

	return 0;
}

int bt_mesh_scan_enable(void)
{
	int err;

#if MYNEWT_VAL(BLE_EXT_ADV)
	struct ble_gap_ext_disc_params uncoded_params =
		{ .itvl = MESH_SCAN_INTERVAL, .window = MESH_SCAN_WINDOW,
		.passive = 1 };

	BT_DBG("");

	err =  ble_gap_ext_disc(g_mesh_addr_type, 0, 0, 0, 0, 0,
				&uncoded_params, NULL, NULL, NULL);
#else
	struct ble_gap_disc_params scan_param =
		{ .passive = 1, .filter_duplicates = 0, .itvl =
		  MESH_SCAN_INTERVAL, .window = MESH_SCAN_WINDOW };

	BT_DBG("");

	err =  ble_gap_disc(g_mesh_addr_type, BLE_HS_FOREVER, &scan_param,
			    NULL, NULL);
#endif
	if (err && err != BLE_HS_EALREADY) {
		BT_ERR("starting scan failed (err %d)", err);
		return err;
	}

	return 0;
}

int bt_mesh_scan_disable(void)
{
	int err;

	BT_DBG("");

	err = ble_gap_disc_cancel();
	if (err && err != BLE_HS_EALREADY) {
		BT_ERR("stopping scan failed (err %d)", err);
		return err;
	}

	return 0;
}
