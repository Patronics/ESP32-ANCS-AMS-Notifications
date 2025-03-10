#ifndef ANCS_BLE_CLIENT_H_
#define ANCS_BLE_CLIENT_H_

#include "ble_notification.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class BLEAddress;
class BLERemoteCharacteristic;
class ANCSNotificationQueue;
class BLEClient;

#include "BLEUUID.h"

/**
 * Internal module for creating and managing an ANCS client connection singleton.
 * This needs to run asynchronously as a FreeRTOS task.
 */
class ANCSBLEClient {
public:
   /**
    * Become a BLE client to a remote BLE server. Pass in address of the server.
    */
	ANCSBLEClient();
	virtual ~ANCSBLEClient();
	void setNotificationArrivedCallback(ble_notification_arrived_t cbNotification, const void *userData = nullptr);
	void setNotificationRemovedCallback(ble_notification_removed_t cbNotification, const void *userData = nullptr);

	void performAction(uint32_t notifyUUID, uint8_t actionID);

public:
	static BLEUUID getAncsServiceUUID(); // To be able to advertise it
	
	/**
	 * Start the FreeRTOS task that handles the client connection.
	 */
	static void startClientTask(void *data);
	
public:
	TaskHandle_t clientTaskHandle;
	
	void onDataSourceNotify(BLERemoteCharacteristic*, uint8_t*, size_t, bool);
	void onNotificationSourceNotify(BLERemoteCharacteristic*, uint8_t*, size_t, bool);
	
	void setup(BLEClient*);
private:
	
	/**
	 * A notification event only contains minimal information. We need to request the extra info
	 * in a second BLE request, then fill in the data in our notification queue as it arrives.
	 */
	void retrieveExtraNotificationData(Notification &);
	
	/**
	 * Check if this notification is a call event, by checking the triggering app type.
	 * @return true if this is a an incoming call.
	 */
	bool isIncomingCall(const Notification & notification) const;
	
	ANCSNotificationQueue * notificationQueue;
	
	ble_notification_arrived_t notificationCB;
	const void *notificationCBUserData;
	ble_notification_removed_t removedCB;
	const void *removedCBUserData;
	
	class BLERemoteCharacteristic* pControlPointCharacteristic;
};

#endif // ANCS_BLE_CLIENT_H_