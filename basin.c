////////////////////////////////////////////////////////////////////////
// COMP1521 23T2 --- Assignment 2: `basin', a simple file synchroniser
// <https://cgi.cse.unsw.edu.au/~cs1521/23T2/assignments/ass2/index.html>
//
// Written by YOUR-NAME-HERE (z5555555) on INSERT-DATE-HERE.
// INSERT-DESCRIPTION-OF-PROGAM-HERE
//
// 2023-07-16   v1.1    Team COMP1521 <cs1521 at cse.unsw.edu.au>


#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "basin.h"

void fwrite_little_endian_16(FILE *fout, u_int16_t number) {
    //number = (number << 8) | (number >> 8);
    fwrite(&number, 2, 1, fout);
}

void fwrite_little_endian_24(FILE *fout, u_int32_t number) {
    //number = ((number << 16) & 0x00FF0000) | (number & 0x0000FF00) | ((number >> 16) & 0x000000FF);
    //number = number << 8;
    fwrite(&number, 3, 1, fout);
}

void fwrite_little_endian_64(FILE *fout, u_int64_t number) {
/*     uint64_t result = 0;

    for (int i = 0; i < sizeof(uint64_t); i++) {
        result |= ((number >> (i * 8)) & 0xFF) << ((sizeof(uint64_t) - 1 - i) * 8);
    } */
    fwrite(&number, 8, 1, fout);
}

void fwrite_big_endian_64(FILE *fout, u_int64_t number, int byte_length) {
    uint64_t result = 0;

    for (int i = 0; i < byte_length; i++) {
        result |= ((number >> (i * 8)) & 0xFF) << ((byte_length - 1 - i) * 8);
    }
    fwrite(&number, byte_length, 1, fout);
}

int fread_next_256byte_block(FILE *fin, char block[]) {
    int result = 0;
    for (int j = 0; j < 256; j++) {
        //printf("reading %dth char\n", j);
        int c;
        if ((c = fgetc(fin)) == EOF ) {
            //printf("char is EOF\n", j);
            return result;
        }
        //printf("assigning block[%d] = %c\n", j, c);
        result++;
        block[j] = c;
    }
    return result;
}

void fwrite_hash(FILE *fout, FILE *fin) {
    char block[BLOCK_SIZE];
    memset(block, '\0', sizeof(block));
    int block_size = fread_next_256byte_block(fin, block);
    uint64_t hash = hash_block(block, block_size);
    fwrite_little_endian_64(fout, hash);
}

uint64_t fread_hash(FILE *ftabi) {
    uint64_t hash;
    if (fread(&hash, 8, 1, ftabi) == 0) {
        perror("EOF reached while reading hash");
        exit(1);
    }
    return hash;
}

uint64_t generate_hash(FILE *fin) {
    if (fin == NULL) {
        return 0;
    }
    char block[BLOCK_SIZE];
    memset(block, '\0', sizeof(block));
    int block_size = fread_next_256byte_block(fin, block);
    uint64_t hash = hash_block(block, block_size);
    return hash;
}

int fwrite_record(FILE *fout, FILE *fin, char *in_filename) {
    struct stat s;
    if (stat(in_filename, &s) != 0) {
        perror(in_filename);
        return 1;
    }

    u_int16_t pathname_length = strlen(in_filename);
    //printf("pathname_lenght %u\n", pathname_length);
    fwrite_little_endian_16(fout, pathname_length);
    for (int i = 0; i < pathname_length; i++) {
        fputc(in_filename[i], fout);
    }

    uint32_t num_of_blocks = number_of_blocks_in_file(s.st_size);
    fwrite_little_endian_24(fout, num_of_blocks);

    for (int i = 0; i < num_of_blocks; i++) {
        //printf("\nprinting blocks: i = %d\n", i);
        fwrite_hash(fout, fin);
    }
return 0;
}

void fwrite_magic_tabi(FILE *fout) {
    char magic_number_tabi[] = {0x54, 0x41, 0x42, 0x49};
    for (int i = 0; i < 4; i++) {
        fputc(magic_number_tabi[i], fout);
    }
}

void read_tabi_magic(FILE *ftabi) {
    char magic_number_tabi[] = {0x54, 0x41, 0x42, 0x49};
    for (int i = 0; i < 4; i++) {
        if (fgetc(ftabi) != magic_number_tabi[i]) {
            perror("tabi magic wrong");
            exit(1);
        }
    }
}

void fwrite_magic_tbbi(FILE *ftbbi) {
    char magic_number_tbbi[] = {0x54, 0x42, 0x42, 0x49};
    for (int i = 0; i < 4; i++) {
        fputc(magic_number_tbbi[i], ftbbi);
    }
}

int copy_number_of_records_from_tabi_to_tbbi(FILE *ftabi, FILE *ftbbi) {
    int read = fgetc(ftabi);
    if (read == EOF) {
        perror("EOF reached while reading number of records");
        exit(1);
    }
    u_int8_t num_of_records = read;
    fputc(num_of_records, ftbbi);
    return read;
}

char *copy_pathname_and_length_from_tabi_to_tbbi(FILE *ftabi, FILE *ftbbi) {
    //printf("pathname_lenght %u\n", pathname_length);
    u_int16_t pathname_length;
    if (fread(&pathname_length, 2, 1, ftabi) == 0) {
        perror("reached EOF at pathlength");
        exit(1);
    };

    fwrite(&pathname_length, 2, 1, ftbbi);
    char *pathname = malloc(sizeof(char)*(pathname_length + 1));
    pathname[pathname_length] = '\0';
    for (int i = 0; i < pathname_length; i++) {
        int read = fgetc(ftabi);
        if (read == EOF) {
            perror("reached EOF at pathname");
            exit(1);
        }
        fputc((char)read, ftbbi);
        pathname[i] = (char)read;
    }
    return pathname;
}

int copy_num_blocks_from_tabi_to_tbbi(FILE *ftabi, FILE *ftbbi) {
    uint32_t num_blocks;
    if (fread(&num_blocks, 3, 1, ftabi) == 0) {
        perror("EOF reached while reading num_blocks");
        exit(1);
    }
    fwrite(&num_blocks, 3, 1, ftbbi);
    return (int)num_blocks;
}

void write_matches(int num_blocks, char *pathname, FILE *ftabi, FILE *ftbbi) {
    FILE *in_file = fopen(pathname, "r");
    uint64_t matches = 0;
    int matches_length = num_tbbi_match_bytes(num_blocks);
    for (int i = 0; i < (matches_length * 8); i++) {
        if (i < num_blocks) {
            uint64_t hash_read = fread_hash(ftabi);
            printf("hash_read at i = %d: 0x%lx\n", i, hash_read);

            if (in_file != NULL) {
                uint64_t hash_generated = generate_hash(in_file);
                printf("generate_hash at i = %d: 0x%lx\n", i, hash_read);
                if (hash_read == hash_generated) {
                    matches += 1;
                }
            }
        }
        printf("matches array at i = %d: 0x%lx\n", i, matches);
        matches <<= 1;
    }
    fwrite_big_endian_64(ftbbi, matches, matches_length);
    //fwrite(&matches, matches_length, 1, ftbbi);
    fclose(in_file);
}

/// @brief Create a TABI file from an array of filenames.
/// @param out_filename A path to where the new TABI file should be created.
/// @param in_filenames An array of strings containing, in order, the files
//                      that should be placed in the new TABI file.
/// @param num_in_filenames The length of the `in_filenames` array. In
///                         subset 5, when this is zero, you should include
///                         everything in the current directory.
void stage_1(char *out_filename, char *in_filenames[], size_t num_in_filenames) {
    
    FILE *fout = fopen(out_filename, "w");
    if (fout == NULL) {
        perror(out_filename);
    } 

    fwrite_magic_tabi(fout);
    fputc((u_int8_t)num_in_filenames, fout);

    for (int i = 0; i < num_in_filenames; i++) {
        char *in_filename = in_filenames[i];
        FILE *fin = fopen(in_filename, "r");
        if (fin == NULL) {
            perror(in_filename);
            return;
        }
        fwrite_record(fout, fin, in_filename);
        fclose(fin);
    }
    fclose(fout);
}

/// @brief Create a TBBI file from a TABI file.
/// @param out_filename A path to where the new TBBI file should be created.
/// @param in_filename A path to where the existing TABI file is located.
void stage_2(char *out_filename, char *in_filename) {

    FILE *ftabi = fopen(in_filename, "r");
    if (ftabi == NULL) {
        perror(in_filename);
        exit(1);
    }
    FILE *ftbbi = fopen(out_filename, "w");
    if (ftbbi == NULL) {
        perror(out_filename);
        exit(1);
    }
    read_tabi_magic(ftabi);
    fwrite_magic_tbbi(ftbbi);

    int num_of_records = copy_number_of_records_from_tabi_to_tbbi(ftabi, ftbbi); // exit if reached EOF
    for (int i = 0; i < num_of_records; i++) {
        printf("reading i = %d\n", i);

        char *pathname = copy_pathname_and_length_from_tabi_to_tbbi(ftabi, ftbbi); // exit if EOF found
        // read number of blocks from tabi
        int num_blocks = copy_num_blocks_from_tabi_to_tbbi(ftabi, ftbbi); // exit if EOF found
        // open filestream
        // if file == NULL assign all bits to 0
        write_matches(num_blocks, pathname, ftabi, ftbbi);
        free(pathname);
    }

    if (fgetc(ftabi) != EOF) {
        // you read somethi ng else even though youre supposed to be done => exit(1)
        perror("EOF not reached after reading given number of records");
        exit(1);
    }
    fclose(ftabi);
    fclose(ftbbi);
}


/// @brief Create a TCBI file from a TBBI file.
/// @param out_filename A path to where the new TCBI file should be created.
/// @param in_filename A path to where the existing TBBI file is located.
void stage_3(char *out_filename, char *in_filename) {
    // TODO: implement this.
}


/// @brief Apply a TCBI file to the filesystem.
/// @param in_filename A path to where the existing TCBI file is located.
void stage_4(char *in_filename) {
    // TODO: implement this.
}
