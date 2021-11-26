#include "kernel.h"
#include "driver/psci.h"
#include "printk.h"

void psci_shutdown(void) __attribute__((noreturn));
void psci_reset(void) __attribute__((noreturn));

int psci_call(enum PSCI_OP op) {
  switch(op) {
    case PSCI_OP_SHUTDOWN:
      printk("system shutdown\n");
      psci_shutdown();
    case PSCI_OP_RESET:
      printk("system reboot\n");
      psci_reset();
    default:
      return -1;
  }

  panic("unreachable");
}
