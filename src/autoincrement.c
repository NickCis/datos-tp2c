#include <stdio.h>

unsigned int getLastId(const char* path){
	unsigned int ret = 0;
	FILE* fd = fopen(path, "rb");
	if(!fd)
		return 0;

	fread(&ret, sizeof(unsigned int), 1, fd);
	fclose(fd);
	return ret;
}

int writeLastId(const char* path, unsigned int id){
	FILE* fd = fopen(path, "wb");
	if(!fd)
		return 1;

	fwrite(&id, sizeof(unsigned int), 1, fd);
	fclose(fd);
	return 0;
}
