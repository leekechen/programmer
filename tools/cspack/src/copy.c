#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BLK_READ_SIZE       (4096)


int copy_by_name(const char* dst, const char* src)
{
    int      err;
    FILE*    fp_src;
    FILE*    fp_dst;
    size_t   r_len;
    size_t   w_len;
    uint8_t* buf;

    err    = 0;
    fp_src = NULL;
    fp_dst = NULL;

    buf = malloc(BLK_READ_SIZE);
    if (buf == NULL) {
        goto COPY_ERROR;
    }

    fp_src = fopen(src, "rb");
    if (fp_src == NULL) {
        goto COPY_ERROR;
    }

    fp_dst = fopen(dst, "wb");
    if (fp_dst == NULL) {
        goto COPY_ERROR;
    }
    
    while (r_len != BLK_READ_SIZE) {
        r_len = fread(buf, 1, r_len, fp_src);
        w_len = fwrite(buf, 1, r_len, fp_dst);
        if (w_len != r_len) {
            goto COPY_ERROR;
        }
    }


COPY_ERROR:
    if (err) {        
        perror(src);
    }
    
    if (buf != NULL) {
        free(buf);
    }
    
    if (fp_src != NULL) {
        fclose(fp_src);
    }
    if (fp_dst != NULL) {
        fclose(fp_dst);
    }

    return err;
}

int copy_by_fp(FILE* fp_dst, FILE* fp_src)
{
    int      err;
    size_t   r_len;
    size_t   w_len;
    uint8_t* buf;

    err    = 0;

    buf = malloc(BLK_READ_SIZE);
    if (buf == NULL) {
        goto COPY_ERROR;
    }
    
    for (r_len = BLK_READ_SIZE; r_len == BLK_READ_SIZE; ) {
        r_len = fread(buf, 1, r_len, fp_src);
        w_len = fwrite(buf, 1, r_len, fp_dst);
        if (w_len != r_len) {
            err = -1;
            goto COPY_ERROR;
        }
    }


COPY_ERROR:
    if (err) {        
        perror("copy_by_fp() failed");
    }
    
    if (buf != NULL) {
        free(buf);
    }

    return err;
}


