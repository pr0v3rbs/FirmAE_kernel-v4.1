#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/reboot.h>

#include "firmadyne.h"
#include "hooks.h"
#include "devfs_stubs.h"
#include "procfs_stubs.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dominic Chen <ddchen@cmu.edu>");
MODULE_DESCRIPTION("Firmware analysis and emulation support");
MODULE_VERSION("0.1");

short devfs = 1;
short execute = 1;
short reboot = 1;
short procfs = 1;
short syscall = 255;
char* monitor_paths = "";
short path_count = 0;
char path_list[MAX_MONITOR_PATH][PATH_MAX];
int path_length_list[MAX_MONITOR_PATH];

module_param(devfs, short, 0660);
MODULE_PARM_DESC(devfs, "Enable devfs stub device emulation");
module_param(execute, short, 0660);
MODULE_PARM_DESC(execute, "Execute predefined binary at next binary execution");
module_param(reboot, short, 0660);
MODULE_PARM_DESC(reboot, "Enable prevention/simulation of system shutdown/reboot");
module_param(procfs, short, 0660);
MODULE_PARM_DESC(procfs, "Enable procfs stub device emulation");
module_param(syscall, short, 0660);
MODULE_PARM_DESC(syscall, "Loglevel bitmask for interception and printing of system calls");
module_param(monitor_paths, charp, 0660);
MODULE_PARM_DESC(monitor_paths, "Monitoring paths");

static void parse_monitor_paths(void)
{
	int i = 0;
        int curIdx = 0;
        int monitorIdx = 0;
        while (monitor_paths[curIdx])
        {
                switch (monitor_paths[curIdx])
                {
                        case ' ':
                                break;
                        case ';':
				if (monitorIdx == 1)
	                                path_length_list[path_count] = 0;
				else
					path_length_list[path_count] = monitorIdx;
                                path_list[path_count][monitorIdx] = '\0';
                                path_count++;
                                monitorIdx = 0;
                                break;
                        default:
                                path_list[path_count][monitorIdx++] = monitor_paths[curIdx];
                                break;
                }
                curIdx++;
        }

        if (monitor_paths[curIdx - 1] != ';')
        {
		if (monitorIdx == 1)	// maybe '/'
			path_length_list[path_count] = 0;
		else
	                path_length_list[path_count] = monitorIdx;
	        path_list[path_count][monitorIdx] = '\0';
                path_count++;
        }

        for (i = 0; i < path_count; i++)
                printk("\n[MONITOR] monitor path%d : %s\n", i + 1, path_list[i]);
}

static int reboot_notify(struct notifier_block *nb, unsigned long code, void *unused) {
	unregister_probes();

	return NOTIFY_DONE;
}

static struct notifier_block reboot_cb = {
	.notifier_call = reboot_notify,
};

int __init init_module(void) {
	int ret = 0, tmp = 0;

	printk(KERN_INFO MODULE_NAME": devfs: %d, execute: %d, procfs: %d, syscall: %d\n", devfs, execute, procfs, syscall);

	if ((tmp = register_devfs_stubs()) < 0) {
		printk(KERN_WARNING MODULE_NAME": register_devfs_stubs() = %d\n", tmp);
		ret = tmp;
	}


	if ((tmp = register_probes()) < 0) {
		printk(KERN_WARNING MODULE_NAME": register_probes() = %d\n", tmp);
		ret = tmp;
	}

	if ((tmp = register_procfs_stubs()) < 0) {
		printk(KERN_WARNING MODULE_NAME": register_procfs_stubs() = %d\n", tmp);
		ret = tmp;
	}

	parse_monitor_paths();

	register_reboot_notifier(&reboot_cb);

	return ret;
}

void __exit cleanup_module(void) {
	unregister_devfs_stubs();
	unregister_probes();
	unregister_procfs_stubs();

	unregister_reboot_notifier(&reboot_cb);
}

module_init(init_module);
module_exit(cleanup_module);
