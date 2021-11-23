#include "kernel.h"
#include "driver/psci.h"
#include "printk.h"

int psci_call(enum PSCI_OP op) {
  switch(op) {
    case PSCI_OP_SHUTDOWN:
      printk("system shutdown...\n");
      psci_shutdown();
      return 0;
    case PSCI_OP_RESET:
      printk("system rebooting...\n");
      psci_reset();
      return 0;
    default:
      return -1;
  }
}
