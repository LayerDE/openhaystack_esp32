#include "data.hpp"
#include "c_data.h"
#include <stdio.h>
#include <stdlib.h>

data::data() : data((unsigned int)0){
}

data::data(unsigned int lenght){
    id = rand();
    content = c_data_spawn();
    c_data_set_size(&content,lenght);
    printf("created data len=%u: %u\n",content.size,id);
}
data::data(const uint8_t* hData,unsigned int lenght) : data(lenght){
    set_data(hData,lenght);
}
data::data(data* in) : data(in->get_lenght()){
    set_data(in);
}

data::data(const c_data* in) : data(in->size){
    c_data_set_content(&content,in->content);
}

data::~data(){
    printf("deleted data len=%u: %u\n",content.size,id);
    c_data_delete_ptr(&content);
}
unsigned int data::get_lenght(){
    if(content.content != nullptr)
        return content.size;
    c_data_set_size(&content,0);
    return 0;
}
void data::set_lenght(unsigned int hLenght){
    c_data_set_size(&content,hLenght);
}
void data::add_data(const uint8_t* hData,unsigned int lenght){
    printf("in add_data: len=%i add_len=%i: %u\n",content.size,lenght, id);
    c_data_extend_raw(&content,hData,lenght);
}
void data::add_data(data* in){
    add_data(in->get_data(),in->get_lenght());
}
const uint8_t* data::get_data(){
    return (uint8_t*)content.content;
}   

void data::set_data(const uint8_t* data,unsigned int lenght){
    c_data_set(&content,data,lenght);
}

void data::set_data(data* in){
    set_data(in->get_data(),in->get_lenght());
}

void data::out_c_data(c_data* in){
    c_data_set(in, content.content, content.size);
}

