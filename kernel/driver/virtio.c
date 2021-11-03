#include "driver/virtio.h"
#include "printk.h"
#include "memmap.h"

void virtio_init() {
  printk("magicval %p\n", REG(VIRTIO_REG_MAGICVALUE));
  printk("version %p\n", REG(VIRTIO_REG_VERSION));
  printk("devid %p\n", REG(VIRTIO_REG_DEVICE_ID));
}
