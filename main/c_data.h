#pragma once
typedef struct{void* content; unsigned int size;} c_data;
extern const c_data c_data_default;

#ifdef __cplusplus
extern "C" {
#endif
c_data c_data_spawn();
void c_data_spawn_ptr(c_data* in);
void c_data_set_size(c_data* in, const unsigned int size);
void c_data_resize(c_data* in, const unsigned int size);
void c_data_set_content(c_data* in, const void* content);
void c_data_set(c_data* in, const void* content, const unsigned int size);
void c_data_delete(c_data in);
void c_data_delete_ptr(c_data* in);
void c_data_extend_raw(c_data* in, const void* content, const unsigned int size);
void c_data_extend(c_data* in, const c_data* adding);
#ifdef __cplusplus
}
#endif
