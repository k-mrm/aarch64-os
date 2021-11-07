#include "driver/virtio.h"
#include "printk.h"
#include "memmap.h"

#define VIRTIO_MAGIC  0x74726976

struct virtq *disk;

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
  ;
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

  REG(VIRTIO_REG_STATUS) = 0;
  REG(VIRTIO_REG_STATUS) |= DEV_STATUS_ACKNOWLEDGE;
  REG(VIRTIO_REG_STATUS) |= DEV_STATUS_DRIVER;

  u32 features = REG(VIRTIO_REG_DEVICE_FEATURES);
  features &= ~(1 << VIRTIO_BLK_F_RO);
  features &= ~(1 << VIRTIO_BLK_F_GEOMETRY);
  features &= ~(1 << VIRTIO_BLK_F_TOPOLOGY);
  features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
  REG(VIRTIO_REG_DRIVER_FEATURES) = features;

  REG(VIRTIO_REG_STATUS) |= DEV_STATUS_FEATURES_OK;

  if(!(REG(VIRTIO_REG_STATUS) & DEV_STATUS_FEATURES_OK))
    panic("virtio-blk err");

  virtq_init();

  REG(VIRTIO_REG_STATUS) |= DEV_STATUS_DRIVER_OK;
}
