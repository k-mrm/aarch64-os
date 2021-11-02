#ifndef AARCH64_VIRTIO_H
#define AARCH64_VIRTIO_H

#include "kernel.h"

#define VIRTIO0 0xa000000

#define VIRTIO_REG_MAGICVALUE           0x00
#define VIRTIO_REG_VERSION              0x04
#define VIRTIO_REG_DEVICE_ID            0x08
#define VIRTIO_REG_VENDOR_ID            0x0c
#define VIRTIO_REG_DEVICE_FEATURES      0x10
#define VIRTIO_REG_DEVICE_FEATURES_SEL  0x14
#define VIRTIO_REG_DRIVER_FEATURES      0x20
#define VIRTIO_REG_DRIVER_FEATURES_SEL  0x24
#define VIRTIO_REG_QUEUE_SEL            0x30
#define VIRTIO_REG_QUEUE_NUM_MAX        0x34
#define VIRTIO_REG_QUEUE_NUM            0x38
#define VIRTIO_REG_QUEUE_READY          0x44
#define VIRTIO_REG_QUEUE_NOTIFY         0x50
#define VIRTIO_REG_INTERRUPT_STATUS     0x60
#define VIRTIO_REG_INTERRUPT_ACK        0x64
#define VIRTIO_REG_STATUS               0x70
#define VIRTIO_REG_QUEUE_DESC_LOW       0x80
#define VIRTIO_REG_QUEUE_DESC_HIGH      0x84
#define VIRTIO_REG_QUEUE_DRIVER_LOW     0x90
#define VIRTIO_REG_QUEUE_DRIVER_HIGH    0x94
#define VIRTIO_REG_QUEUE_DEVICE_LOW     0xa0
#define VIRTIO_REG_QUEUE_DEVICE_HIGH    0xa4
#define VIRTIO_REG_CONFIG_GENERATION    0xfc
#define VIRTIO_REG_CONFIG               0x100

struct virtq_desc {
  u64 addr;
  u32 len;
  u16 flags;
  u16 next;
};

#endif
