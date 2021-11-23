#ifndef AARCH64_OS_DRIVER_PSCI_H
#define AARCH64_OS_DRIVER_PSCI_H

#define PSCI_SYSTEM_OFF   0x84000008

#ifndef __ASSEMBLER__

enum PSCI_OP {
  PSCI_OP_SHUTDOWN,
};

#endif

#endif
