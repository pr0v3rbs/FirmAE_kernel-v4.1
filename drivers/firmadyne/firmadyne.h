#ifndef INCLUDE_FIRMADYNE_H
#define INCLUDE_FIRMADYNE_H

#define MAX_MONITOR_PATH 100

extern short devfs;
extern short execute;
extern short reboot;
extern short procfs;
extern short syscall;
extern short path_count;
extern char path_list[MAX_MONITOR_PATH][PATH_MAX];
extern int path_length_list[MAX_MONITOR_PATH];

#define MODULE_NAME "firmadyne"

#endif
