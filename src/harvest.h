/**
 * 
 * 
 */

#ifndef HARVEST_H
#define HARVEST_H

float get_loadavg();
bool get_kernel(char *buffer);
struct sysinfo get_sysinfo();
char *get_distro();
bool get_interfaces();

#endif
