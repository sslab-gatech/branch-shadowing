#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/smp.h>
#include <asm/msr.h>
#include <asm/tsc.h>

MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Sangho Lee");      ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Adjust Local APIC Timer Frequency");  ///< The description -- see modinfo
MODULE_VERSION("0.1");              ///< The version of the module

static char *name = "sgx-bshadow";        ///< An example LKM argument -- default value is "world"
module_param(name, charp, S_IRUGO); ///< Param desc. charp = char ptr, S_IRUGO can be read/not changed
MODULE_PARM_DESC(name, "The name to display in /var/log/kern.log");  ///< parameter description

// modified Linux kernel is needed
extern void (*timer_interrupt_hook)(void*);
extern unsigned int lapic_next_deadline_delta;
extern int lapic_target_cpu;

static int target_cpu  = 3;    // defalt cpu
static int attack_freq = 2000; // default freq
module_param(target_cpu, int, 0);
module_param(attack_freq, int, 0);

int sgx_bshadow_acc = 0;

void sgx_bshadow_fn(void *arg) {
	//	int cpu = *((int*)arg);
	//	if (cpu == 3) {
		++sgx_bshadow_acc;
		if (sgx_bshadow_acc == 1000) {
			printk(KERN_INFO "sgx_bshadow_fn: %d: %llu\n", *((int*)arg), rdtsc());
			sgx_bshadow_acc = 0;
		}
		//	}
}

void register_timer_hook(void *info) {
	lapic_target_cpu          = target_cpu;  // attack target
	lapic_next_deadline_delta = attack_freq; // attack frequency
	timer_interrupt_hook = sgx_bshadow_fn;

    // change the delta
    // lapic_next_deadline_delta = 1;
    
    // lapic_next_deadline_delta = 10000000; // default in my laptop
    /* lapic_next_deadline_delta =      100000; */
    /* lapic_next_deadline_delta =      10000; */
    
    //    lapic_next_deadline_delta =      2000;
    // this seems the highest frequency (CPU utilization is 100% at 1000)
    // lapic hanlder is called every ~4000 cycles
}

void deregister_timer_hook(void *info) {
	lapic_target_cpu = -1;
	lapic_next_deadline_delta = 0;
	timer_interrupt_hook = NULL;
}

/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init lapic_freq_init(void){
    //    printk(KERN_INFO "%s: smp_processor_id : %d\n", name, smp_processor_id());

	smp_call_function_single(target_cpu, register_timer_hook, NULL, true);

	return 0;

    /*
    printk(KERN_INFO "%s at %p\n", name, helloBBB_init);
    printk(KERN_INFO "SL: apic: %p\n", apic);
    printk(KERN_INFO "SL: irq_delivery_mode: %u\n", apic->irq_delivery_mode);
    printk(KERN_INFO "SL: irq_dest_mode: %u\n", apic->irq_dest_mode);

    printk(KERN_INFO "SL: apic_icr_read: %llu\n", apic->icr_read());

    printk(KERN_INFO "SL: APIC_LVTT: %08x\n", apic->read(0x320));
    printk(KERN_INFO "SL: APIC_LVT1: %08x\n", apic->read(0x360));
    printk(KERN_INFO "SL: APIC_LVTERR: %08x\n", apic->read(0x370));
    printk(KERN_INFO "SL: APIC_TMICT: %08x\n", apic->read(0x380));
    printk(KERN_INFO "SL: APIC_TMCCT: %08x\n", apic->read(0x390));
    printk(KERN_INFO "SL: APIC_TDCR: %08x\n", apic->read(0x3E0));
    printk(KERN_INFO "SL: APIC_ICR: %08x\n", apic->read(0x300));

    printk(KERN_INFO "SL: apic_icr_read: %llu\n", apic->icr_read());

    for (i = 0; i < 10; ++i) {
	rdmsrl(MSR_IA32_TSC_DEADLINE, deadline);
	printk(KERN_INFO "SL: : MSR_IA32_TSC_DEADLINE: %llu\n", deadline - rdtsc());
	//	printk(KERN_INFO "SL: : rdtsc(): %llu\n", rdtsc());
    }
    */
}

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit lapic_freq_exit(void){
	printk(KERN_INFO "%s at %p\n", name, lapic_freq_exit);

	smp_call_function_single(target_cpu, deregister_timer_hook, NULL, true);
}

/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(lapic_freq_init);
module_exit(lapic_freq_exit);
