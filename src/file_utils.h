/**
 * 
 * 
 */

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

bool file_exists(char *path);
bool file_access(char *path, char *perms);
char *fread_key(FILE *fp, char const *keyname);
#endif
