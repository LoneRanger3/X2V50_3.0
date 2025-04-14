#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define HEAD_MAX_SIZE 64

typedef struct {
    uint32_t file_offset;   // 文件在主文件中的起始偏移量
    uint32_t file_size;     // 文件的大小
} FileHeaderEntry;


int add_file_to_master(const char *filename, FILE *master_file, FileHeaderEntry *file_headers, int *next_offset) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Failed to open file for reading");
        return -1;
    }

    // 获取文件大小
    fseek(fp, 0, SEEK_END);
    file_headers->file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // 记录文件起始位置
    file_headers->file_offset = *next_offset;
    printf("%s size:%d  offset:%d\n", filename, file_headers->file_size, file_headers->file_offset);

    // 写入文件内容到主文件
    fseek(master_file, *next_offset, SEEK_SET);
    unsigned char buffer[1024];
    size_t read_bytes;
    while ((read_bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        int ret = fwrite(buffer, 1, read_bytes, master_file);
        if (ret != read_bytes) {
            printf("fwrite failed write:%d  real length:%ld\n", ret, read_bytes);
            fclose(fp);
            return -1;
        }
    }

    // 更新下一个文件的起始位置
    *next_offset += file_headers->file_size;

    fclose(fp);
    printf("Merge srcfile %s created successfully.\n\n", filename);

    return 0;
}

int main(int argc, char** argv) {
    if((argc == 3) && (strstr(argv[2],"-end"))){
        FILE *master_file = fopen(argv[1], "ab");
        if (!master_file) {
            perror("Failed to create master file");
            return -1;
        }
        //fseek(master_file, 0, SEEK_END);
        //写入16字节文件尾特殊字符
        unsigned char file_head[16] = "XM_360FILEKUNGEN";
        if (fwrite(file_head, 1, 16, master_file) != 16) {
            perror("Failed to write to file head info");
            fclose(master_file);
            return -1;
        }
        fclose(master_file);     
        return 0;
    }

    if (argc < 5) {
        if((argc == 4) && (strstr(argv[3],"uboot_upgrade_key"))){
        }else{
            if(argc == 4){
                printf("Usage: %s outfile update.img logo.yuv\n", argv[0]);
            }else{
                printf("Usage: %s outfile update.img logo.yuv SD_update.bin\n", argv[0]);
            }
            return -1;
        }

    }
    if ((argc - 1) > HEAD_MAX_SIZE / sizeof(FileHeaderEntry)) {
        printf("srcfile too much\n");
        return -1;
    }

    FILE *master_file = fopen(argv[1], "wb");
    if (!master_file) {
        perror("Failed to create master file");
        return -1;
    }
    fseek(master_file, 0, SEEK_SET);
    // 写入0字节
    unsigned char zero_byte = 0;
    for (size_t i = 0; i < HEAD_MAX_SIZE; ++i) {
        if (fwrite(&zero_byte, 1, 1, master_file) != 1) {
            perror("Failed to write to file");
            fclose(master_file);
            return -1;
        }
    }

    int src_file_num = argc - 2;
    int best_file_total_size = 0;

    FileHeaderEntry *file_headers = malloc(src_file_num * sizeof(FileHeaderEntry));
    if (sizeof(FileHeaderEntry) * src_file_num > HEAD_MAX_SIZE) {
        printf("head info too long:%ld\n", sizeof(FileHeaderEntry) * src_file_num);
        return -1;
    }
    int next_offset = HEAD_MAX_SIZE;

    for (int i = 0; i < src_file_num; i++) {
        int ret = add_file_to_master(argv[2 + i], master_file, &file_headers[i], &next_offset);
        if (ret) {
            printf("%s merge failed\n", argv[2 + i]);
            fclose(master_file);
            free(file_headers);
            file_headers = NULL;
            return -1;
        }
        best_file_total_size += file_headers[i].file_size;
    }

    //写入16字节文件尾特殊字符
    unsigned char file_head[16] = "XM_360FILEKUNGEN";
    if (fwrite(file_head, 1, 16, master_file) != 16) {
        perror("Failed to write to file head info");
        fclose(master_file);
        free(file_headers);
        file_headers = NULL;
        return -1;
    }

    // 将文件头写入主文件
    fseek(master_file, 0, SEEK_SET);
    fwrite(file_headers, sizeof(FileHeaderEntry), src_file_num, master_file);

    fclose(master_file);
    free(file_headers);
    file_headers = NULL;

    printf("Master file %s created successfully.\n", argv[1]);
    printf("Master file header:%d \ntotal head size:%d\n data size:%d\n file end size 16\n", (int)(sizeof(FileHeaderEntry) * src_file_num), HEAD_MAX_SIZE, best_file_total_size);


    if((argc == 4) && (strstr(argv[3],"uboot_upgrade_key"))){
        FILE *fp = fopen(argv[1], "rb");
        if (!fp) {
            perror("Failed to open file for reading");
            return 0;
        }
        // 获取文件大小
        fseek(fp, 0, SEEK_END);
        int file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        fclose(fp);
        if(file_size < (16*1024*1024+1)){
            master_file = fopen(argv[1], "ab");
            if(master_file){
                char *str = malloc(16*1024*1024+1-file_size);
                if(str){
                    memset(str,0,16*1024*1024+1-file_size);
                    fwrite(str, 16*1024*1024+1-file_size, 1, master_file);
                }
                fclose(master_file);
                free(str);
            }
        }
    }
    return 0;
}
