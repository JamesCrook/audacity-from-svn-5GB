#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

#include "snd.h"
#include "sndfileio.h"

#define PERMISSION 0644

#ifdef __cplusplus
extern "C" {
#endif

void snd_fail(char *msg)
{
    printf("ERROR: %s\n", msg);
}

int snd_file_open(char *fname, int mode)
{
    int file;
    if (mode == SND_RDONLY)
      mode = O_RDONLY;
    else
      mode = O_RDWR;

    file = open(fname, mode);
    return file;
}


int snd_file_creat(char *fname)
{
    int file = creat(fname, PERMISSION);
    return file;
}


long snd_file_len(int file)
{
    long len;
    struct stat statbuf;

    fstat(file ,&statbuf);
    len = (long) statbuf.st_size;  /* get length of file */
    return len;
}


long snd_file_read(int fp, char *data, long len)
{
    return read(fp, data, len);
}


long snd_file_write(int fp, char *data, long len)
{
    return write(fp, data, len);
}


int snd_file_close(int fp)
{
    return close(fp);
}


int snd_file_lseek(int file, long offset, int param)
{
    if (param == SND_SEEK_CUR) param = SEEK_CUR;
    else if (param == SND_SEEK_SET) param = SEEK_SET;
    else param = SEEK_END;
    return lseek(file, offset, param);
}


void *snd_alloc(size_t s) { return malloc(s); }


void snd_free(void *a) { free(a); }

#ifdef __cplusplus
}
#endif

