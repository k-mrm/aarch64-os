#include "driver/psci.h"

.global psci_shutdown
psci_shutdown:
  ldr x0, =PSCI_SYSTEM_OFF 
  hvc #0

.global psci_reset
psci_reset:
  ldr x0, =PSCI_SYSTEM_RESET
  hvc #0
