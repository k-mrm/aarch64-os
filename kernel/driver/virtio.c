#include "driver/virtio.h"
#include "printk.h"
#include "memmap.h"
#include "string.h"

#define VIRTIO_MAGIC  0x74726976

struct virtq disk;

int virtio_blk_rw(char *buf, int op) {
  ;
}

int alloc_desc(struct virtq *virtq) {
  for(int i = 0; i < NQUEUE; i++) {
    if(virtq->desc[i].addr == 0)
      return i;
  }

  return -1;
}

int virtq_init() {
  memset(&disk, 0, sizeof(disk));
}

void virtio_init() {
  printk("magicval %p\n", REG(VIRTIO_REG_MAGICVALUE));
  printk("version %p\n", REG(VIRTIO_REG_VERSION));
  printk("devid %p\n", REG(VIRTIO_REG_DEVICE_ID));

  if(REG(VIRTIO_REG_MAGICVALUE) != VIRTIO_MAGIC ||
     REG(VIRTIO_REG_VERSION) != 2 ||
     REG(VIRTIO_REG_DEVICE_ID) != 2) {
    panic("no block device");
  }

  /* Reset the device. */
  REG(VIRTIO_REG_STATUS) = 0;

  /* Set the ACKNOWLEDGE status bit: the guest OS has noticed the device. */
  REG(VIRTIO_REG_STATUS) |= DEV_STATUS_ACKNOWLEDGE;

  /* Set the DRIVER status bit: the guest OS knows how to drive the device. */
  REG(VIRTIO_REG_STATUS) |= DEV_STATUS_DRIVER;

  /* Read device feature bits, and write the subset of feature bits understood by the OS and driver to the device. */
  u32 features = REG(VIRTIO_REG_DEVICE_FEATURES);
  features &= ~(1 << VIRTIO_BLK_F_RO);
  features &= ~(1 << VIRTIO_BLK_F_GEOMETRY);
  features &= ~(1 << VIRTIO_BLK_F_TOPOLOGY);
  features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
  REG(VIRTIO_REG_DRIVER_FEATURES) = features;

  /* Set the FEATURES_OK status bit. */
  REG(VIRTIO_REG_STATUS) |= DEV_STATUS_FEATURES_OK;


  /* Re-read device status to ensure the FEATURES_OK bit is still set: otherwise, the device does not support our subset of features and the device is unusable. */
  if(!(REG(VIRTIO_REG_STATUS) & DEV_STATUS_FEATURES_OK))
    panic("virtio-blk err");

  /* Perform device-specific setup, including discovery of virtqueues for the device, optional per-bus setup, reading and possibly writing the device's virtio configuration space, and population of virtqueues. */
  virtq_init();

  /* Set the DRIVER_OK status bit. */
  REG(VIRTIO_REG_STATUS) |= DEV_STATUS_DRIVER_OK;
}
