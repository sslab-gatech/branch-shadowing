About Linux's LAPIC
================================================================================

Sangho Lee <sangho@gatech.edu>

I checked Linux kernel version 3.19 and 4.4, and found that Linux uses
TSC-deadline mode, not periodic timer mode. Namely, it does not rely on divide
configuration register (DCR), initial-count register (ICR), and current-count
register (CCR).

Instead, Linux writes the next deadline into *IA32_TSC_DEADLINE* MSR whenever a
timer interrupt has happened.

### arch/x86/kernel/apic/apic.c
```
static int lapic_next_deadline(unsigned long delta, ...)
levt->set_next_event = lapic_next_deadline;

...
#define APIC_DEVISOR 16
#define TSC_DEVISOR  32
...

static int lapic_next_deadline(unsigned long delta, struct clock_event_device *evt)
{
  u64 tsc;

  tsc = rdtsc();
  wrmsrl(MSR_IA32_TSC_DEADLINE, tsc + (((u64) delta) * TSC_DIVISOR));
  // SL: change it like the below?
  wrmsrl(MSR_IA32_TSC_DEADLINE, tsc + 1);
  return 0;
}
```

### from Intel SDM
The IA32_TSC_DEADLINE MSR (MSR address 6E0H) is a per-logical processor MSR
that specifies the time at which a timer interrupt should occur. Writing a
non-zero 64- bit value into IA32_TSC_DEADLINE arms the timer. An interrupt is
generated when the logical processor’s time-stamp counter equals or exceeds the
target value in the IA32_TSC_DEADLINE MSR. 2 When the timer generates an
interrupt, it disarms itself and clears the IA32_TSC_DEADLINE MSR. Thus, each
write to the IA32_TSC_DEADLINE MSR generates at most one timer interrupt.
