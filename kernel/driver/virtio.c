#include "driver/virtio.h"
#include "printk.h"
#include "memmap.h"
#include "string.h"
#include "trap.h"

#define VIRTIO_MAGIC  0x74726976

struct virtq disk;

static int alloc_desc(struct virtq *virtq) {
  for(int i = 0; i < NQUEUE; i++) {
    if(virtq->desc[i].addr == 0)
      return i;
  }

  return -1;
}

static void free_desc(struct virtq *virtq, int n) {
  virtq->desc[n].addr = 0;
  virtq->desc[n].len = 0;

  if(virtq->desc[n].flags & VIRTQ_DESC_F_NEXT) {
    free_desc(virtq, virtq->desc[n].next);
  }

  virtq->desc[n].flags = 0;
  virtq->desc[n].next = 0;
}

int virtio_blk_rw(u64 bno, char *buf, enum diskop op) {
  u64 sector = bno * 2;
  struct virtio_blk_req_hdr hdr;

  if(op == DREAD)
    hdr.type = VIRTIO_BLK_T_IN;
  else if(op == DWRITE)
    hdr.type = VIRTIO_BLK_T_OUT;
  else
    return -1;
  hdr.reserved = 0;
  hdr.sector = sector;

  int d0 = alloc_desc(&disk);
  if(d0 < 0)
    return -1;
  disk.desc[d0].addr = (u64)&hdr;
  disk.desc[d0].len = sizeof(hdr);
  disk.desc[d0].flags = VIRTQ_DESC_F_NEXT;

  int d1 = alloc_desc(&disk);
  if(d1 < 0)
    return -1;
  disk.desc[d0].next = d1;
  disk.desc[d1].addr = (u64)buf;
  disk.desc[d1].len = 1024;
  disk.desc[d1].flags |= VIRTQ_DESC_F_NEXT;
  if(op == VIRTIO_BLK_T_OUT)
    disk.desc[d1].flags |= VIRTQ_DESC_F_WRITE;

  char status;
  int d2 = alloc_desc(&disk);
  if(d2 < 0)
    return -1;
  disk.desc[d1].next = d2;
  disk.desc[d2].addr = (u64)&status;
  disk.desc[d2].len = sizeof(status);
  disk.desc[d2].flags = VIRTQ_DESC_F_WRITE;
  disk.desc[d2].next = 0;

  disk.avail.ring[disk.avail.idx % NQUEUE] = d0;
  disk.avail.idx++;

  REG(VIRTIO_REG_QUEUE_NOTIFY) = 0;

  for(;;);

  free_desc(&disk, d0);

  return 0;
}

static int virtq_init(struct virtq *vq) {
  memset(vq, 0, sizeof(*vq));
}

static void virtio_blk_intr() {
  printk("virtio int\n");
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
  virtq_init(&disk);

  REG(VIRTIO_REG_QUEUE_NUM) = NQUEUE;

  /* Set the DRIVER_OK status bit. */
  REG(VIRTIO_REG_STATUS) |= DEV_STATUS_DRIVER_OK;

  new_irq(VIRTIO_BLK_IRQ, virtio_blk_intr);
}
