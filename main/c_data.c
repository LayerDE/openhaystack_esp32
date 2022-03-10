#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c_data.h"

const c_data c_data_default = {NULL, 0};

c_data c_data_spawn(){
    c_data tmp = c_data_default;
    return tmp;
}

void c_data_spawn_ptr(c_data* in){
    *in = c_data_spawn();
}

void c_data_set_size(c_data* in, const unsigned int size){
    if(in->content != NULL){
        free(in->content);
    }
    if(size != 0){
        in->content = malloc(size);
        }
    in->size = size;
}

void c_data_resize(c_data* in, const unsigned int size){
    if(size == 0)
        return;
    if(in->content == NULL || in->size == 0){
        c_data_set_size(in, size);
        return;
    }
    if(size == in->size)
        return;
    void* tmp = malloc(in->size+size);
    if(in->size < size)
        memcpy(tmp,in->content,in->size);
    else
        memcpy(tmp,in->content,size);
    free(in->content);
    in->size = size;
    in->content = tmp;
}

void c_data_set_content(c_data* in, const void* content){
    memcpy(in->content,content,in->size);
}

void c_data_set(c_data* in, const void* content, const unsigned int size){
    c_data_set_size(in, size);
    c_data_set_content(in, content);
}

void c_data_delete(c_data in){
    c_data_delete_ptr(&in);
}

void c_data_delete_ptr(c_data* in){
    c_data_set_size(in,0);
}

void c_data_extend_raw(c_data* in, const void* content, const unsigned int size){
    unsigned int old_size = in->size;
    c_data_resize(in,old_size+size);
    memcpy(&((unsigned char*)in->content)[old_size],content,size);
}

void c_data_extend(c_data* in, const c_data* adding){
    c_data_extend_raw(in,adding->content,adding->size);
}