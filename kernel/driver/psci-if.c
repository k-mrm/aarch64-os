#include "kernel.h"
#include "driver/psci.h"
#include "printk.h"

void psci_fn(u64 id, u64 arg1, u64 arg2, u64 arg3);

int psci_shutdown() {
  printk("system shutdown\n");
  psci_fn(PSCI_SYSTEM_OFF, 0, 0, 0);
  return 0;
}

int psci_reset() {
  printk("system reboot\n");
  psci_fn(PSCI_SYSTEM_RESET, 0, 0, 0);
  return 0;
}

int psci_cpu_on(u64 cpuid, u64 ep) {
  printk("CPU%d enable\n", cpuid);
  psci_fn(PSCI_SYSTEM_CPUON, cpuid, ep, 0);
  return 0;
}
