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

void fread_next_256byte_block(FILE *fin, char block[]) {
    for (int j = 0; j < 256; j++) {
        //printf("reading %dth char\n", j);
        int c;
        if ((c = fgetc(fin)) == EOF ) {
            //printf("char is EOF\n", j);
            return;
        }
        //printf("assigning block[%d] = %c\n", j, c);
        block[j] = c;
    }
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
        printf("\nprinting blocks: i = %d\n", i);
        char block[BLOCK_SIZE];
        memset(block, '\0', sizeof(block));
        fread_next_256byte_block(fin, block);
        for (int j = 0; j < BLOCK_SIZE; j++) {
            printf("%c", block[j]);
        }
        uint64_t hash = hash_block(block, 256);
        fwrite_little_endian_64(fout, hash);
    }
    return 0;
}

void fwrite_magic_tabi(FILE *fout) {
    char magic_number_tabi[] = {0x54, 0x41, 0x42, 0x49};
    for (int i = 0; i < 4; i++) {
        fputc(magic_number_tabi[i], fout);
    }
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
    char block[1];
    block[0] = 'a';

    uint64_t hash = hash_block(block, 1);
    printf("\nhash of a : %lx", hash);
}

/// @brief Create a TBBI file from a TABI file.
/// @param out_filename A path to where the new TBBI file should be created.
/// @param in_filename A path to where the existing TABI file is located.
void stage_2(char *out_filename, char *in_filename) {
    // TODO: implement this.
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
