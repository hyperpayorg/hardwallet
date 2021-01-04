#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#include <thread>
#include <iostream>
#include "../include/JUB_SDK.h"
#include <atomic>

using namespace std;
static std::atomic_bool isInsert{false};

void get_device_info_test()
{

	JUB_UINT16 deviceIDs[MAX_DEVICE] = { 0xffff };
	JUB_ListDeviceHid(deviceIDs);

  //cout << "deviceID :" << deviceIDs[0] <<std::endl;

	JUB_RV rv = JUB_ConnetDeviceHid(deviceIDs[0]);
	if (rv != JUBR_OK)
	{
		cout << "cannot find JubtierWallet" << std::endl;
    return;
	}

	JUB_UINT16 deviceID = deviceIDs[0];

	char* cert;
	rv = JUB_GetDeviceCert(deviceID, &cert);
	if (rv != JUBR_OK)
	{
		cout << " JUB_GetDeviceCert error " << endl;
		return;
	}

	cout << "device cert is :" << cert << endl;
	JUB_FreeMemory(cert);

  JUB_DisconnetDeviceHid(deviceIDs[0]);
}


int hotplug_callback(struct libusb_context *ctx, struct libusb_device *dev,
                     libusb_hotplug_event event, void *user_data) {

  std::cout << "callback in :" << std::this_thread::get_id() <<std::endl;
  static libusb_device_handle *handle = NULL;
  struct libusb_device_descriptor desc;
  int rc;
  (void)libusb_get_device_descriptor(dev, &desc);
  if (LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED == event) {
    /*rc = libusb_open(dev, &handle);
    if (LIBUSB_SUCCESS != rc) {
      printf("Could not open USB device\n");
    }*/
    //get_device_info_test();
    isInsert = true;
  } else if (LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT == event) {
    /*if (handle) {
      libusb_close(handle);
      handle = NULL;
    }*/
    isInsert = false;
  } else {
    printf("Unhandled event %d\n", event);
  }
  return 0;
}



void worker_thread(bool wait_event)
{
  while (1) {
    if(wait_event)
      libusb_handle_events_completed(NULL, NULL);

    if(isInsert)
    {
      libusb_lock_events(NULL);
      try{
        //sometime exception
        std::cout << "Thread id :" << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        get_device_info_test();
        

      }
      catch(...)
      {
        std::cout << "Exception catched!!!"<<std::endl;
      }

      libusb_unlock_events(NULL);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}


int main (void) {
  libusb_hotplug_callback_handle handle;
  int rc;
  libusb_init(NULL);
  rc = libusb_hotplug_register_callback(NULL, (libusb_hotplug_event)(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED |
                                        LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT), LIBUSB_HOTPLUG_NO_FLAGS, 0x096e, 0x0891,
                                        LIBUSB_HOTPLUG_MATCH_ANY, hotplug_callback, NULL,
                                        &handle);
  if (LIBUSB_SUCCESS != rc) {
    printf("Error creating a hotplug callback\n");
    libusb_exit(NULL);
    return EXIT_FAILURE;
  }
  std::thread worker(worker_thread,true);
  std::thread worker2(worker_thread,false);
  while (1) {
    //libusb_handle_events_completed(NULL, NULL);
    auto start = std::chrono::system_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "main thread elapsed time: " << elapsed_seconds.count() << "s\n";
  }
  //worker.join();
  libusb_hotplug_deregister_callback(NULL, handle);
  libusb_exit(NULL);
  return 0;
}
