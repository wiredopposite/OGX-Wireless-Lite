#ifndef NVS_HELPERS_H_
#define NVS_HELPERS_H_

#include <stdint.h>

#define NVS_NAMESPACE "user_data"

bool erase_all_nvs_keys(const char *namespace_name);
bool blob_exists_in_nvs(const char* key, size_t* len);

bool write_blob_to_nvs(const uint8_t* blob, size_t len, const char* key);
bool retrieve_blob_from_nvs(uint8_t* buffer, size_t* len, const char* key);

bool write_uint8_to_nvs(uint8_t value, const char* key);
bool retrieve_uint8_from_nvs(uint8_t* buffer, const char* key);

#endif