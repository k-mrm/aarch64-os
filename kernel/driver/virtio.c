#include "driver/virtio.h"
#include "kalloc.h"
#include "aarch64.h"
#include "printk.h"
#include "memmap.h"
#include "string.h"
#include "trap.h"
#include "log.h"
#include "spinlock.h"
#include "proc.h"

#define VIRTIO_MAGIC  0x74726976

struct disk {
  struct spinlock lk;
  struct virtq virtq;
} disk;

static void desc_init(struct virtq *virtq) {
  for(int i = 0; i < NQUEUE; i++) {
    if(i != NQUEUE - 1) {
      virtq->desc[i].flags = VIRTQ_DESC_F_NEXT;
      virtq->desc[i].next = i + 1;
    }
  }
}

static int alloc_desc(struct virtq *virtq) {
  if(virtq->nfree == 0)
    panic("virtq kokatu");

  u16 d = virtq->free_head;
  if(virtq->desc[d].flags & VIRTQ_DESC_F_NEXT)
    virtq->free_head = virtq->desc[d].next;
  
  virtq->nfree--;

  return d;
}

static void free_desc(struct virtq *virtq, u16 n) {
  u16 head = n;
  int empty = 0;

  if(virtq->nfree == 0)
    empty = 1;

  while(virtq->nfree++, (virtq->desc[n].flags & VIRTQ_DESC_F_NEXT)) {
    n = virtq->desc[n].next;
  }

  virtq->desc[n].flags = VIRTQ_DESC_F_NEXT;
  if(!empty)
    virtq->desc[n].next = virtq->free_head;
  virtq->free_head = head;

  kinfo("freehead %d\n", head);
}

int virtio_blk_op(u64 bno, char *buf, enum diskop op) {
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

  acquire(&disk.lk);

  int d0 = alloc_desc(&disk.virtq);
  if(d0 < 0)
    return -1;
  disk.virtq.desc[d0].addr = (u64)V2P(&hdr);
  disk.virtq.desc[d0].len = sizeof(hdr);
  disk.virtq.desc[d0].flags = VIRTQ_DESC_F_NEXT;

  int d1 = alloc_desc(&disk.virtq);
  if(d1 < 0)
    return -1;
  disk.virtq.desc[d0].next = d1;
  disk.virtq.desc[d1].addr = (u64)V2P(buf);
  disk.virtq.desc[d1].len = 1024;
  disk.virtq.desc[d1].flags = VIRTQ_DESC_F_NEXT;
  if(op == DREAD)
    disk.virtq.desc[d1].flags |= VIRTQ_DESC_F_WRITE;

  int d2 = alloc_desc(&disk.virtq);
  if(d2 < 0)
    return -1;
  disk.virtq.desc[d1].next = d2;
  disk.virtq.desc[d2].addr = (u64)V2P(&disk.virtq.info[d0].status);
  disk.virtq.desc[d2].len = sizeof(disk.virtq.info[d0].status);
  disk.virtq.desc[d2].flags = VIRTQ_DESC_F_WRITE;
  disk.virtq.desc[d2].next = 0;

  kinfo("d0 %d d1 %d d2 %d\n", d0, d1, d2);

  disk.virtq.avail->ring[disk.virtq.avail->idx % NQUEUE] = d0;
  disk.virtq.avail->idx++;

  disk.virtq.info[d0].buf = buf;

  REG(VIRTIO_REG_QUEUE_NOTIFY) = 0;

  while(!disk.virtq.info[d0].done)
    sleep(buf, &disk.lk);

  disk.virtq.info[d0].done = 0;

  free_desc(&disk.virtq, d0);

  release(&disk.lk);

  return 0;
}

static void virtio_blk_intr() {
  acquire(&disk.lk);

  REG(VIRTIO_REG_INTERRUPT_ACK) = REG(VIRTIO_REG_INTERRUPT_STATUS) & 0x3;

  int d0;
  while(disk.virtq.last_used_idx != disk.virtq.used->idx) {
    d0 = disk.virtq.used->ring[disk.virtq.last_used_idx % NQUEUE].id;

    kinfo("d0 %d\n", d0);

    if(disk.virtq.info[d0].status != 0)
      panic("disk op error");

    disk.virtq.info[d0].done = 1;
    wakeup(disk.virtq.info[d0].buf);
    disk.virtq.info[d0].buf = NULL;

    disk.virtq.last_used_idx++;
  }

  release(&disk.lk);
}

static int virtq_init(struct virtq *vq) {
  memset(vq, 0, sizeof(*vq));

  vq->desc = kalloc();
  vq->avail = kalloc();
  vq->used = kalloc();
  if(!vq->desc || !vq->avail || !vq->used)
    panic("virtq init failed");

  vq->nfree = NQUEUE;

  desc_init(vq);

  return 0;
}

#define LO(addr)  (u32)((addr) & 0xffffffff)
#define HI(addr)  (u32)(((addr) >> 32) & 0xffffffff)

void virtio_init() {
  kinfo("magicval %p\n", REG(VIRTIO_REG_MAGICVALUE));
  kinfo("version %p\n", REG(VIRTIO_REG_VERSION));
  kinfo("devid %p\n", REG(VIRTIO_REG_DEVICE_ID));

  if(REG(VIRTIO_REG_MAGICVALUE) != VIRTIO_MAGIC ||
     REG(VIRTIO_REG_VERSION) != 2 ||
     REG(VIRTIO_REG_DEVICE_ID) != 2) {
    panic("no block device");
  }

  /* Reset the device. */
  REG(VIRTIO_REG_STATUS) = 0;

  u32 status = 0;

  /* Set the ACKNOWLEDGE status bit: the guest OS has noticed the device. */
  status |= DEV_STATUS_ACKNOWLEDGE;
  REG(VIRTIO_REG_STATUS) = status;

  /* Set the DRIVER status bit: the guest OS knows how to drive the device. */
  status |= DEV_STATUS_DRIVER;
  REG(VIRTIO_REG_STATUS) = status;

  /* Read device feature bits, and write the subset of feature bits understood by the OS and driver to the device. */
  REG(VIRTIO_REG_DEVICE_FEATURES_SEL) = 0;
  REG(VIRTIO_REG_DRIVER_FEATURES_SEL) = 0;

  u32 features = REG(VIRTIO_REG_DEVICE_FEATURES);
  features &= ~(1 << VIRTIO_BLK_F_SEG_MAX);
  features &= ~(1 << VIRTIO_BLK_F_GEOMETRY);
  features &= ~(1 << VIRTIO_BLK_F_RO);
  features &= ~(1 << VIRTIO_BLK_F_BLK_SIZE);
  features &= ~(1 << VIRTIO_BLK_F_FLUSH);
  features &= ~(1 << VIRTIO_BLK_F_TOPOLOGY);
  features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
  features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
  features &= ~(1 << VIRTIO_RING_F_INDIRECT_DESC);
  features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
  REG(VIRTIO_REG_DRIVER_FEATURES) = features;

  /* Set the FEATURES_OK status bit. */
  status |= DEV_STATUS_FEATURES_OK;
  REG(VIRTIO_REG_STATUS) = status;

  /* Re-read device status to ensure the FEATURES_OK bit is still set: otherwise, the device does not support our subset of features and the device is unusable. */
  status = REG(VIRTIO_REG_STATUS);
  dsb();
  if(!(status & DEV_STATUS_FEATURES_OK))
    panic("virtio-blk err");

  /* Perform device-specific setup, including discovery of virtqueues for the device, optional per-bus setup, reading and possibly writing the device's virtio configuration space, and population of virtqueues. */
  virtq_init(&disk.virtq);

  int qmax = REG(VIRTIO_REG_QUEUE_NUM_MAX);
  if(qmax < NQUEUE)
    panic("virtqueue");

  REG(VIRTIO_REG_QUEUE_SEL) = 0;
  REG(VIRTIO_REG_QUEUE_NUM) = NQUEUE;

  u64 phy_desc = V2P(disk.virtq.desc);
  REG(VIRTIO_REG_QUEUE_DESC_LOW) = LO(phy_desc);
  REG(VIRTIO_REG_QUEUE_DESC_HIGH) = HI(phy_desc);
  u64 phy_avail = V2P(disk.virtq.avail);
  REG(VIRTIO_REG_QUEUE_DRIVER_LOW) = LO(phy_avail);
  REG(VIRTIO_REG_QUEUE_DRIVER_HIGH) = HI(phy_avail);
  u64 phy_used = V2P(disk.virtq.used);
  REG(VIRTIO_REG_QUEUE_DEVICE_LOW) = LO(phy_used);
  REG(VIRTIO_REG_QUEUE_DEVICE_HIGH) = HI(phy_used);

  dsb();

  REG(VIRTIO_REG_QUEUE_READY) = 1;

  /* Set the DRIVER_OK status bit. */
  status |= DEV_STATUS_DRIVER_OK;
  REG(VIRTIO_REG_STATUS) = status;

  new_irq(VIRTIO_BLK_IRQ, virtio_blk_intr);

  lock_init(&disk.lk);

  kinfo("virtio_init done\n");
}
