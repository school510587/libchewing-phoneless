/**
 * reverse_index_test.c
 *
 * Copyright (c) 2012
 *      libchewing Core Team. See ChangeLog for details.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */
#include <stdio.h>
#include <string.h>
#include "chewing-utf8-util.h"
#include "config.h"
#include "phl.h"
#include "plat_mmap.h"
void lookup(const char* bin, const char* path, const char* rev_dict, size_t dict_size){
        plat_mmap input_map;
        char buffer[8];
        const char *input, *p, *q;
        size_t csize, offset=0, wl;
        long file_size;
        int c, i, j, k, t;
        plat_mmap_set_invalid(&input_map);
        file_size=plat_mmap_create(&input_map, path, FLAG_ATTRIBUTE_READ);
        csize=file_size;
        input=(const char*)plat_mmap_set_view(&input_map, &offset, &csize);
        if(input){
                for(p=input; p<input+file_size; ){
                        wl=ueBytesFromChar(*p);
                        if(wl>1){ // May be Chinese characters.
                                strncpy(buffer, p, wl);
                                buffer[wl]='\0';
                                q=ueStrstr(rev_dict, dict_size, p, wl);
                                printf("%s:%d:%s:", path, p-input, buffer);
                                if(q){
#ifdef USE_BINARY_DATA
                                        memcpy(&c, q+=wl, sizeof(c));
#else
                                        sscanf(q+=wl, "%x", &c);
#endif
                                        for(i=0; i<c; i++){
#ifdef USE_BINARY_DATA
                                                memcpy(&k, q+=sizeof(int), sizeof(k));
#else
                                                q=strchr(q+1, '\t');
                                                sscanf(q, "%x", &k);
#endif
                                                putchar(' ');
                                                for(j=0; j<4; j++){
                                                        t=buffer[j]=(k>>(24-8*j))&0x000000ff;
                                                        if(t) putchar(t);
                                                }
                                        }
                                        putchar('\n');
                                }
                                else puts(" Not found.");
                                p+=wl;
                        }
                }
                plat_mmap_close(&input_map);
        }
        else fprintf(stderr, "%s: Cannot open %s file.\n", bin, path);
}
int main(int argc, char* argv[]){
        char rev_path[PATH_MAX]={".." PLAT_SEPARATOR "data" PLAT_SEPARATOR "phoneless" PLAT_SEPARATOR};
        plat_mmap rev_map;
        const char* rev_dict;
        size_t csize, offset=0;
        long file_size;
        int i;
        plat_mmap_set_invalid(&rev_map);
        if(argc<3){
                fprintf(stderr, "%s: Too few argument.\n", argv[0]);
                puts("The program needs a phoneless id and at least 1 input file.");
                return 1;
        }
        strcat(strcat(strcat(rev_path, argv[1]), PLAT_SEPARATOR), REV_INDEX_FILE);
        file_size=plat_mmap_create(&rev_map, rev_path, FLAG_ATTRIBUTE_READ);
        csize=file_size;
        rev_dict=(const char*)plat_mmap_set_view(&rev_map, &offset, &csize);
        if(!rev_dict){
                fprintf(stderr, "%s: Cannot open reverse index file.\n", argv[0]);
                return 1;
        }
        for(i=2; i<argc; i++) lookup(argv[0], argv[i], rev_dict, file_size);
        plat_mmap_close(&rev_map);
        return 0;
}

