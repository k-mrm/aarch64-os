#ifndef AARCH64_VIRTIO_H
#define AARCH64_VIRTIO_H

#include "kernel.h"
#include "mm.h"
#include "memmap.h"

#define NQUEUE  8

#define VIRTIO_REG_MAGICVALUE           (VIRTIO0 + 0x00)
#define VIRTIO_REG_VERSION              (VIRTIO0 + 0x04)
#define VIRTIO_REG_DEVICE_ID            (VIRTIO0 + 0x08)
#define VIRTIO_REG_VENDOR_ID            (VIRTIO0 + 0x0c)
#define VIRTIO_REG_DEVICE_FEATURES      (VIRTIO0 + 0x10)
#define VIRTIO_REG_DEVICE_FEATURES_SEL  (VIRTIO0 + 0x14)
#define VIRTIO_REG_DRIVER_FEATURES      (VIRTIO0 + 0x20)
#define VIRTIO_REG_DRIVER_FEATURES_SEL  (VIRTIO0 + 0x24)
#define VIRTIO_REG_QUEUE_SEL            (VIRTIO0 + 0x30)
#define VIRTIO_REG_QUEUE_NUM_MAX        (VIRTIO0 + 0x34)
#define VIRTIO_REG_QUEUE_NUM            (VIRTIO0 + 0x38)
#define VIRTIO_REG_QUEUE_READY          (VIRTIO0 + 0x44)
#define VIRTIO_REG_QUEUE_NOTIFY         (VIRTIO0 + 0x50)
#define VIRTIO_REG_INTERRUPT_STATUS     (VIRTIO0 + 0x60)
#define VIRTIO_REG_INTERRUPT_ACK        (VIRTIO0 + 0x64)
#define VIRTIO_REG_STATUS               (VIRTIO0 + 0x70)
#define VIRTIO_REG_QUEUE_DESC_LOW       (VIRTIO0 + 0x80)
#define VIRTIO_REG_QUEUE_DESC_HIGH      (VIRTIO0 + 0x84)
#define VIRTIO_REG_QUEUE_DRIVER_LOW     (VIRTIO0 + 0x90)
#define VIRTIO_REG_QUEUE_DRIVER_HIGH    (VIRTIO0 + 0x94)
#define VIRTIO_REG_QUEUE_DEVICE_LOW     (VIRTIO0 + 0xa0)
#define VIRTIO_REG_QUEUE_DEVICE_HIGH    (VIRTIO0 + 0xa4)
#define VIRTIO_REG_CONFIG_GENERATION    (VIRTIO0 + 0xfc)
#define VIRTIO_REG_CONFIG               (VIRTIO0 + 0x100)

#define DEV_STATUS_ACKNOWLEDGE  1
#define DEV_STATUS_DRIVER 2
#define DEV_STATUS_FAILED 128
#define DEV_STATUS_FEATURES_OK  8
#define DEV_STATUS_DRIVER_OK  4
#define DEV_STATUS_NEEDS_RESET  64

#define VIRTIO_BLK_F_SIZE_MAX 1
#define VIRTIO_BLK_F_SEG_MAX  2
#define VIRTIO_BLK_F_GEOMETRY 4
#define VIRTIO_BLK_F_RO 5
#define VIRTIO_BLK_F_BLK_SIZE 6
#define VIRTIO_BLK_F_FLUSH  9
#define VIRTIO_BLK_F_TOPOLOGY 10
#define VIRTIO_BLK_F_CONFIG_WCE 11
#define VIRTIO_BLK_F_DISCARD  13
#define VIRTIO_BLK_F_WRITE_ZEROES 14
#define VIRTIO_F_ANY_LAYOUT 27
#define VIRTIO_RING_F_INDIRECT_DESC 28
#define VIRTIO_RING_F_EVENT_IDX 29

#define VIRTIO_BLK_S_OK 0
#define VIRTIO_BLK_S_IOERR  1
#define VIRTIO_BLK_S_UNSUPP 2

#define VIRTQ_DESC_F_NEXT 1
#define VIRTQ_DESC_F_WRITE  2 
#define VIRTQ_DESC_F_INDIRECT 4
struct virtq_desc {
  u64 addr;
  u32 len;
  u16 flags;
  u16 next;
} __attribute__((packed, aligned(16)));

#define VIRTQ_AVAIL_F_NO_INTERRUPT  1
struct virtq_avail {
  u16 flags;
  u16 idx;
  u16 ring[NQUEUE];
} __attribute__((packed, aligned(2)));

struct virtq_used_elem {
  u32 id;
  u32 len;
} __attribute__((packed));

#define VIRTQ_USED_F_NO_NOTIFY  1
struct virtq_used {
  u16 flags;
  u16 idx;
  struct virtq_used_elem ring[NQUEUE];
} __attribute__((packed, aligned(4)));

struct virtq {
  struct virtq_desc *desc;
  struct virtq_avail *avail;
  struct virtq_used *used;
  u16 free_head;
  u16 last_used_idx;

  struct {
    u8 status;
    u8 done;
  } info[NQUEUE];
};

#define VIRTIO_BLK_T_IN 0
#define VIRTIO_BLK_T_OUT  1
#define VIRTIO_BLK_T_FLUSH  4
#define VIRTIO_BLK_T_DISCARD  11
#define VIRTIO_BLK_T_WRITE_ZEROES 13
struct virtio_blk_req_hdr {
  u32 type;
  u32 reserved;
  u64 sector;
};

enum diskop {
  DREAD,
  DWRITE,
};

int virtio_blk_rw(u64 bno, char *buf, enum diskop op);
void virtio_init(void);

#endif
