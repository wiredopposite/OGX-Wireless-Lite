#include <stdlib.h>
#include "nvs_flash.h"
#include "nvs.h"

#include "user_settings/nvs_helpers.h"

bool erase_all_nvs_keys(const char *namespace_name)
{
    esp_err_t err;
    nvs_handle_t handle;

    err = nvs_open(namespace_name, NVS_READWRITE, &handle);

    if (err != ESP_OK) 
    {
        printf("Failed to open NVS for reading namespace '%s'. Error: %s\n", namespace_name, esp_err_to_name(err));
        return false;
    }

    err = nvs_erase_all(handle);
    nvs_close(handle);

    if (err != ESP_OK) 
    {
        printf("Failed 'nvs_erase_all' for namespace '%s'. Error: %s\n", namespace_name, esp_err_to_name(err));
        return false;
    }

    return true;
}

bool retrieve_blob_from_nvs(uint8_t* buffer, size_t* len, const char* key)
{
    esp_err_t err;
    nvs_handle_t handle;

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);

    if (err != ESP_OK) 
    {
        printf("Failed to open NVS for reading '%s'. Error: %s\n", key, esp_err_to_name(err));
        return false;
    }

    err = nvs_get_blob(handle, key, buffer, len);

    nvs_close(handle);

    if (err != ESP_OK) 
    {
        printf("Failed to get blob for '%s'. Error: %s\n", key, esp_err_to_name(err));
        return false;
    }

    return true;
}

bool write_blob_to_nvs(const uint8_t* blob, size_t len, const char* key)
{
    esp_err_t err;
    nvs_handle_t handle;

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);

    if (err != ESP_OK) 
    {
        printf("Failed to open NVS for writing '%s'. Error: %s\n", key, esp_err_to_name(err));
        return false;
    }

    err = nvs_set_blob(handle, key, blob, len);

    if (err != ESP_OK) 
    {
        printf("Failed to set blob for '%s' in NVS. Error: %s\n", key, esp_err_to_name(err));
        nvs_close(handle);
        return false;
    } 

    err = nvs_commit(handle);

    if (err != ESP_OK) 
    {
        printf("Failed to commit blob for '%s' to NVS. Error: %s\n", key, esp_err_to_name(err));
        nvs_close(handle);
        return false;
    }

    nvs_close(handle);
    printf("Successfully stored blob '%s' in NVS.\n", key);
    
    return true;
}


bool write_uint8_to_nvs(const uint8_t value, const char* key)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);

    if (err != ESP_OK) 
    {
        printf("Failed to open NVS for '%s' storage. Error: %s\n", key, esp_err_to_name(err));
        return false;
    }

    err = nvs_set_u8(handle, key, value);

    if (err != ESP_OK) 
    {
        printf("Failed to set '%s' in NVS. Error: %s\n", key, esp_err_to_name(err));
        nvs_close(handle);
        return false;
    }

    err = nvs_commit(handle);
    nvs_close(handle);

    if (err != ESP_OK) 
    {
        printf("Failed to commit '%s' to NVS. Error %s\n", key, esp_err_to_name(err));
        return false;
    }

    return true;
}

bool retrieve_uint8_from_nvs(uint8_t* buffer, const char* key)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);

    if (err != ESP_OK) 
    {
        printf("Failed to open NVS for '%s' retrieval. Error: %s\n", key, esp_err_to_name(err));
        return false;
    }

    err = nvs_get_u8(handle, key, buffer);
    nvs_close(handle);

    if (err == ESP_ERR_NVS_NOT_FOUND) 
    {
        printf("Key '%s' not found in NVS.\n", key);
        return false;
    }
    else if (err != ESP_OK) 
    {
        printf("Failed to retrieve '%s' from NVS. Error: %s\n", key, esp_err_to_name(err));
        return false;
    }

    return true;
}

bool blob_exists_in_nvs(const char* key, size_t* len)
{
    esp_err_t err;
    nvs_handle_t handle;

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);

    if (err != ESP_OK) 
    {
        printf("Failed to open NVS for reading '%s'. Error: %s\n", key, esp_err_to_name(err));
        return false;
    }

    err = nvs_get_blob(handle, key, NULL, len);

    nvs_close(handle);

    if (err == ESP_ERR_NVS_NOT_FOUND) 
    {
        printf("Blob for '%s' not found.\n", key);
        return false;
    } 
    else if (err != ESP_OK) 
    {
        printf("Failed to get blob for '%s'. Error: %s\n", key, esp_err_to_name(err));
        return false;
    }

    return true;
}