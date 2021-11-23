#include "kernel.h"
#include "driver/psci.h"
#include "printk.h"

int psci_call(enum PSCI_OP op) {
  switch(op) {
    case PSCI_OP_SHUTDOWN:
      printk("system shutdown...\n");
      psci_shutdown();
      break;
  }
}
