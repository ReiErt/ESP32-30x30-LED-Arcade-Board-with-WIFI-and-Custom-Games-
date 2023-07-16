// Author Lukas Landgraf
#include "Fileserver.h"

// mount spiffs Dateisystem
esp_err_t mount_storage(const char * base_path){
    esp_vfs_spiffs_conf_t conf = {
        .base_path = base_path,
        .partition_label = NULL,
        .max_files = 10,
        .format_if_mount_failed = true
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        printf(esp_err_to_name(ret));
        return ret;
    }
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        printf(esp_err_to_name(ret));
        return ret;
    }
    printf("Partition size: total: %d, used %d\n", total, used);
    return ESP_OK;
}