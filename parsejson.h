#pragma once

#include "3rd-party/cJSON/cJSON.c"
#include "3rd-party/uthash/uthash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// 用于存储json文件信息
typedef struct {
    const char *option_name; // key
    UT_hash_handle hh;  // 必须包含这个字段来使用uthash库
} LogOption;

typedef struct {
    const char *option_name; // key
    const char *logging_enable;
    const char *log_line_prefix;
    const char *log_directory;
    const char *log_min_messages;
    const char *log_filename;
    const char *log_truncate_on_rotation;
    const char *log_rotation_age;
    int log_rotation_size;
    UT_hash_handle hh;  // 必须包含这个字段来使用uthash库
} OptionDetail;

typedef struct {
    LogOption *log_options;
    OptionDetail *option_details;
} LogConfig;

void parse_json(const char* json, LogConfig *config) {
    cJSON* root = cJSON_Parse(json);
    if (root == NULL) {
        printf("Failed to parse JSON.\n");
        return;
    }

    // 获取 log_option 数组
    cJSON* log_option = cJSON_GetObjectItem(root, "log_option");
    if (log_option == NULL || !cJSON_IsArray(log_option)) {
        printf("Invalid log_option.\n");
        cJSON_Delete(root);
        return;
    }

    // 遍历 log_option 数组并打印每个元素
    int log_option_size = cJSON_GetArraySize(log_option);
    for (int i = 0; i < log_option_size; i++) {
        cJSON *item = cJSON_GetArrayItem(log_option, i);
        if (item != NULL && cJSON_IsString(item)) {
            LogOption *lo = (LogOption *)malloc(sizeof(LogOption));
            lo->option_name = strdup(item->valuestring);
            HASH_ADD_STR(config->log_options, option_name, lo); // 添加到哈希表
        }
    }

    // 获取 option_details 数组
    cJSON* option_details = cJSON_GetObjectItem(root, "option_details");
    if (option_details == NULL || !cJSON_IsArray(option_details)) {
        printf("Invalid option_details.\n");
        cJSON_Delete(root);
        return;
    }

    // 遍历 option_details 数组并打印每个元素的 option_name
    int option_details_size = cJSON_GetArraySize(option_details);
    for (int i = 0; i < option_details_size; i++) {
        cJSON* item = cJSON_GetArrayItem(option_details, i);
        if (item != NULL && cJSON_IsObject(item)) {
            OptionDetail *od = (OptionDetail *)malloc(sizeof(OptionDetail));

            cJSON* option_name = cJSON_GetObjectItem(item, "option_name");
            if (option_name != NULL && cJSON_IsString(option_name)) {
                od->option_name = strdup(option_name->valuestring);
            }

            cJSON* logging_enable = cJSON_GetObjectItem(item, "logging_enable");
            if (logging_enable != NULL && cJSON_IsString(logging_enable)) {
                od->logging_enable = strdup(logging_enable->valuestring);
            }

            cJSON* log_line_prefix = cJSON_GetObjectItem(item, "log_line_prefix");
            if (log_line_prefix != NULL && cJSON_IsString(log_line_prefix)) {
                od->log_line_prefix = strdup(log_line_prefix->valuestring);
            }

            cJSON* log_directory = cJSON_GetObjectItem(item, "log_directory");
            if (log_directory != NULL && cJSON_IsString(log_directory)) {
                od->log_directory = strdup(log_directory->valuestring);
            }

            cJSON* log_min_messages = cJSON_GetObjectItem(item, "log_min_messages");
            if (log_min_messages != NULL && cJSON_IsString(log_min_messages)) {
                od->log_min_messages = strdup(log_min_messages->valuestring);
            }

            cJSON* log_filename = cJSON_GetObjectItem(item, "log_filename");
            if (log_filename != NULL && cJSON_IsString(log_filename)) {
                od->log_filename = strdup(log_filename->valuestring);
            }

            cJSON* log_truncate_on_rotation = cJSON_GetObjectItem(item, "log_truncate_on_rotation");
            if (log_truncate_on_rotation != NULL && cJSON_IsString(log_truncate_on_rotation)) {
                od->log_truncate_on_rotation = strdup(log_truncate_on_rotation->valuestring);
            }

            cJSON* log_rotation_age = cJSON_GetObjectItem(item, "log_rotation_age");
            if (log_rotation_age != NULL && cJSON_IsString(log_rotation_age)) {
                od->log_rotation_age = strdup(log_rotation_age->valuestring);
            }

            cJSON* log_rotation_size = cJSON_GetObjectItem(item, "log_rotation_size");
            if (log_rotation_size != NULL && cJSON_IsNumber(log_rotation_size)) {
                od->log_rotation_size = log_rotation_size->valueint;
            }

            HASH_ADD_STR(config->option_details, option_name, od); // 添加到哈希表
        }
    }

    cJSON_Delete(root);
}

void parse_json_file(const char* path, LogConfig *config) {
    // 打开文件
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        assert("Failed to open file.\n");
    }

    // 读取文件内容
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* fileContent = (char*)malloc(fileSize + 1);
    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = '\0';

    // 解析 JSON
    parse_json(fileContent, config);

    // 关闭文件
    fclose(file);
    free(fileContent);
}
