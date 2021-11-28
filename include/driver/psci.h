#ifndef AARCH64_OS_DRIVER_PSCI_H
#define AARCH64_OS_DRIVER_PSCI_H

#define PSCI_SYSTEM_OFF   0x84000008
#define PSCI_SYSTEM_RESET   0x84000009
#define PSCI_SYSTEM_CPUON   0xc4000003

#ifndef __ASSEMBLER__

int psci_shutdown(void);
int psci_reset(void);
int psci_cpu_on(u64 cpuid, u64 ep);

#endif

#endif
