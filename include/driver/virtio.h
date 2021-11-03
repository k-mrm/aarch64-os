#ifndef AARCH64_VIRTIO_H
#define AARCH64_VIRTIO_H

#include "kernel.h"

#define PVIRTIO0  0xa000000

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

struct virtq_desc {
  u64 addr;
  u32 len;
  u16 flags;
  u16 next;
};

struct virtq_used_elem {
  u32 id;
  u32 len;
};

struct virtq_used {
  u16 flags;
  u16 idx;
  struct virtq_used_elem ring[8];
  u16 avail_event;
};

#endif
