#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <mavwrap.h>


LOG_MODULE_REGISTER(mavwrap_sample, LOG_LEVEL_INF);


static void rx_cb(const struct device *dev,
		  const mavlink_message_t *msg,
		  void *user_data)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(user_data);
	LOG_INF("RX msgid=%u sysid=%u compid=%u",
		msg->msgid, msg->sysid, msg->compid);
}


int main(void)
{
	const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(mavlink0));

	if (!device_is_ready(dev)) {
		LOG_ERR("mavlink0 not ready");
		return -ENODEV;
	}

	mavwrap_start(dev, rx_cb, NULL);
	LOG_INF("MAVLink wrapper started");

	mavlink_message_t msg;
	uint32_t count = 0;

	while (1) {
		mavlink_msg_heartbeat_pack(1, MAV_COMP_ID_AUTOPILOT1, &msg,
					   MAV_TYPE_GENERIC,
					   MAV_AUTOPILOT_GENERIC,
					   MAV_MODE_FLAG_MANUAL_INPUT_ENABLED,
					   0,
					   MAV_STATE_ACTIVE);

		int ret = mavwrap_send_message(dev, &msg);

		if (ret < 0) {
			LOG_WRN("TX failed: %d", ret);
		} else {
			LOG_INF("TX heartbeat #%u", count);
		}

		count++;
		k_sleep(K_SECONDS(1));
	}

	return 0;
}
