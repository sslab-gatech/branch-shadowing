#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/smp.h>

// Ref: https://github.com/tsujamin/comp3300-cache-disable-kmod

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sangho Lee");
MODULE_DESCRIPTION("Disable CPU Cache");
MODULE_VERSION("0.1");

static char *name = "sgx-bshadow-disable-cache";
module_param(name, charp, S_IRUGO);
MODULE_PARM_DESC(name, "The name to display in /var/log/kern.log");

/* static int target_cpu = 3; // default target cpu */
static int target_cpu = 1; // default target cpu

void cache_set(void *enable) {
	if (enable) {
		printk(KERN_ALERT "Enabling L1/L2 caches.\n");
		__asm__("push %%rax\n\t"
			"movq %%cr0,%%rax\n\t"
			"and $~(1<<30),%%rax\n\t"  /* Unset CD */					
			"movq %%rax,%%cr0\n\t"
			"wbinvd\n\t"
			"pop  %%rax\n\t"
			: : );
	}
	else {
		printk(KERN_ALERT "Disabling L1/L2 caches.\n");
		__asm__("push %%rax\n\t"
			"movq %%cr0,%%rax\n\t"
			"or $(1<<30),%%rax\n\t"  /* Set CD */
			"movq %%rax,%%cr0\n\t"
			"wbinvd\n\t"
			"pop  %%rax\n\t"
			: : );
	}
}

void smp_cache_set(int enable) {
	//	cache_set((void*) enable);
	//	smp_call_function(cache_set, (void*) enable, true);
	smp_call_function_single(target_cpu, cache_set, (void*) enable, true);
}

static int __init disable_cache_init(void){
	smp_cache_set(0); // disable L1/L2 cache
	return 0;
}

static void __exit disable_cache_exit(void) {
	smp_cache_set(1); // enable L1/L2 cache
}

module_init(disable_cache_init);
module_exit(disable_cache_exit);
