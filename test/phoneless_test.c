/**
 * phoneless_test.c
 * 
 * Copyright (c) 2012
 *      libchewing Core Team. See ChangeLog for details.
 * 
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "global-private.h"
#include "phl.h"
#include "plat_mmap.h"
int main(int argc, char* argv[]){
        char path[PATH_MAX]={".." PLAT_SEPARATOR "data" PLAT_SEPARATOR}, *dir_end=path+strlen(path);
        plat_mmap ch_dict_map, index2chdict_map, index_begin_map, index_key_map;
        long file_size[4];
        size_t csize[4], offset[4]={0};
        const char *ch_dict;
        const int *index2chdict, *index_begin;
        const uint32* index_key;
        int status=EXIT_SUCCESS;
        if(argc<2){
                fprintf(stderr, "%s: No input method id is given.\n", argv[0]);
                return EXIT_FAILURE;
        }
        plat_mmap_set_invalid(&ch_dict_map);
        plat_mmap_set_invalid(&index2chdict_map);
        plat_mmap_set_invalid(&index_begin_map);
        plat_mmap_set_invalid(&index_key_map);
        strcat(path, CHAR_FILE);
        file_size[0]=plat_mmap_create(&ch_dict_map, path, FLAG_ATTRIBUTE_READ);
        csize[0]=file_size[0];
        ch_dict=(const char*)plat_mmap_set_view(&ch_dict_map, &offset[0], &csize[0]);
        sprintf(dir_end, "phoneless" PLAT_SEPARATOR "%s" PLAT_SEPARATOR, argv[1]);
        dir_end=path+strlen(path);
        strcpy(dir_end, PHL_INDEX2CHDICT);
        file_size[1]=plat_mmap_create(&index2chdict_map, path, FLAG_ATTRIBUTE_READ);
        csize[1]=file_size[1];
        index2chdict=(const int*)plat_mmap_set_view(&index2chdict_map, &offset[1], &csize[1]);
        strcpy(dir_end, PHL_INDEX_BEGIN_FILE);
        file_size[2]=plat_mmap_create(&index_begin_map, path, FLAG_ATTRIBUTE_READ);
        csize[2]=file_size[2];
        index_begin=(const int*)plat_mmap_set_view(&index_begin_map, &offset[2], &csize[2]);
        file_size[2]/=sizeof(*index_begin);
        strcpy(dir_end, PHL_INDEX_KEY_FILE);
        file_size[3]=plat_mmap_create(&index_key_map, path, FLAG_ATTRIBUTE_READ);
        csize[3]=file_size[3];
        index_key=(const uint32*)plat_mmap_set_view(&index_key_map, &offset[3], &csize[3]);
        file_size[3]/=sizeof(*index_key);
        if(!ch_dict || !index_begin || !index_key){
                fprintf(stderr, "%s: Cannot open dictionary/index file(s).\n", argv[0]);
                status=EXIT_FAILURE;
        }
        else{
                char buffer[8];
                uint32 sequence;
                size_t p;
                while(scanf("%s", buffer)==1){
                        sequence=key2uint(buffer);
                        for(p=0; p<file_size[3] && index_key[p]!=sequence; p++);
                        if(p<file_size[3]){
                                p=index2chdict[index_begin[p]/sizeof(int)];
                                strncpy(buffer, &ch_dict[p+1], ch_dict[p]);
                                buffer[ch_dict[p]]='\0';
                                fputs(buffer, stdout);
                        }
                        else{
                                fprintf(stderr, "%s: `%s' is not supported.\n", argv[0], buffer);
                                status=EXIT_FAILURE;
                        } // Although exception raises, parsing continues.
                }
        }
        if(plat_mmap_is_valid(&ch_dict_map)) plat_mmap_close(&ch_dict_map);
        if(plat_mmap_is_valid(&index2chdict_map)) plat_mmap_close(&index2chdict_map);
        if(plat_mmap_is_valid(&index_begin_map)) plat_mmap_close(&index_begin_map);
        if(plat_mmap_is_valid(&index_key_map)) plat_mmap_close(&index_key_map);
        return status;
}

