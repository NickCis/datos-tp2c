#ifndef __AUTOINCREMENT_H__
#define __AUTOINCREMENT_H__

int writeLastId(const char* path, unsigned int id);
unsigned int getLastId(const char* path);

#endif
