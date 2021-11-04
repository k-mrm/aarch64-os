#include "driver/virtio.h"
#include "printk.h"
#include "memmap.h"

#define VIRTIO_MAGIC  0x74726976

void virtio_init() {
  printk("magicval %p\n", REG(VIRTIO_REG_MAGICVALUE));
  printk("version %p\n", REG(VIRTIO_REG_VERSION));
  printk("devid %p\n", REG(VIRTIO_REG_DEVICE_ID));

  if(REG(VIRTIO_REG_MAGICVALUE) != VIRTIO_MAGIC ||
     REG(VIRTIO_REG_VERSION) != 2 ||
     REG(VIRTIO_REG_DEVICE_ID) != 2) {
    panic("no block device");
  }

  REG(VIRTIO_REG_STATUS) |= DEV_STATUS_ACKNOWLEDGE;
  REG(VIRTIO_REG_STATUS) |= DEV_STATUS_DRIVER;

  u32 features = REG(VIRTIO_REG_DEVICE_FEATURES);

  REG(VIRTIO_REG_STATUS) |= DEV_STATUS_FEATURES_OK;
}
