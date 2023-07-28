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
#include <stdio.h>
#include <limits.h>

#include "basin.h"

void fwrite_little_endian(FILE *fout, u_int64_t number, int num_bytes) {
    for (int i = 0; i < num_bytes; i++) {
        fputc((uint8_t)(number >> (8 * i)), fout);   
    }
}

void fwrite_little_endian_16(FILE *fout, u_int16_t number) {
    fwrite_little_endian(fout, number, 2);
}

void fwrite_little_endian_24(FILE *fout, u_int32_t number) {
    fwrite_little_endian(fout, number, 3);
}

void fwrite_little_endian_32(FILE *fout, u_int32_t number) {
    fwrite_little_endian(fout, number, 4);
}

void fwrite_little_endian_64(FILE *fout, u_int64_t number) {
    fwrite_little_endian(fout, number, 8);
}

void fwrite_big_endian_64(FILE *fout, u_int64_t number, int byte_length) {
    for (int i = 0; i < byte_length; i++) {
        fputc((uint8_t)(number >> ((byte_length - 1 - i) * 8)), fout);
    }
}

uint64_t fread_little_endian(FILE *fout, int num_bytes) {
    u_int64_t number = 0;
    for (int i = 0; i < num_bytes; i++) {
        int c = fgetc(fout);
        if (c == EOF) {
            perror("EOF reached while reading little endian");
            exit(1);
        }
        number |= ((uint64_t)c) << (8 * i);
    }
    return number;
}

uint16_t fread_little_endian_16(FILE *fout) {
    return (uint16_t)fread_little_endian(fout, 2);
}

uint32_t fread_little_endian_24(FILE *fout) {
    return (uint32_t)fread_little_endian(fout, 3);
}

uint32_t fread_little_endian_32(FILE *fout) {
    return (uint32_t)fread_little_endian(fout, 4);
}

uint64_t fread_little_endian_64(FILE *fout) {
    return (uint64_t)fread_little_endian(fout, 8);
}

int fread_next_256byte_block(FILE *fin, char block[]) {
    int result = 0;
    for (int j = 0; j < 256; j++) {
        int c;
        if ((c = fgetc(fin)) == EOF ) {
            return result;
        }
        result++;
        block[j] = c;
    }
    return result;
}

int fread_next_block(FILE *fin, char block[], int block_size) {
    int result = 0;
    if (block_size > BLOCK_SIZE) {
        perror("block_size greater than 256");
        exit(1);
    }
    for (int j = 0; j < block_size; j++) {
        int c;
        if ((c = fgetc(fin)) == EOF ) {
            return result;
        }
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
    uint64_t hash = fread_little_endian_64(ftabi);
    return hash;
}

uint64_t generate_hash(FILE *fin) {
    char block[BLOCK_SIZE];
    memset(block, '\0', sizeof(block));
    int block_size = fread_next_256byte_block(fin, block);
    uint64_t hash = hash_block(block, block_size);
    return hash;
}

int fwrite_record_directory(FILE *fout, char *in_filename) {
    struct stat s;
    if (stat(in_filename, &s) != 0) {
        perror(in_filename);
        return 1;
    }

    u_int16_t pathname_length = strlen(in_filename);
    fwrite_little_endian_16(fout, pathname_length);
    for (int i = 0; i < pathname_length; i++) {
        fputc(in_filename[i], fout);
    }

    uint32_t num_of_blocks = 0;
    fwrite_little_endian_24(fout, num_of_blocks);

    return 0;
}

int fwrite_record(FILE *fout, FILE *fin, char *in_filename) {
    struct stat s;
    if (stat(in_filename, &s) != 0) {
        perror(in_filename);
        return 1;
    }

    u_int16_t pathname_length = strlen(in_filename);
    fwrite_little_endian_16(fout, pathname_length);
    for (int i = 0; i < pathname_length; i++) {
        fputc(in_filename[i], fout);
    }

    uint32_t num_of_blocks = number_of_blocks_in_file(s.st_size);
    fwrite_little_endian_24(fout, num_of_blocks);

    for (int i = 0; i < num_of_blocks; i++) {
        fwrite_hash(fout, fin);
    }
return 0;
}

void write_magic(FILE *fout, char magic[]) {
    for (int i = 0; i < 4; i++) {
        fputc(magic[i], fout);
    }
}

void verify_magic(FILE *file, char magic[]) {
    for (int i = 0; i < 4; i++) {
        if (fgetc(file) != magic[i]) {
            perror("magic wrong");
            exit(1);
        }
    }
} 

int copy_number_of_records_from_file1_to_file2(FILE *file1, FILE *file2) {
    int read = fgetc(file1);
    if (read == EOF) {
        perror("EOF reached while reading number of records");
        exit(1);
    }
    u_int8_t num_of_records = read;
    fputc(num_of_records, file2);
    return read;
}

int read_num_records(FILE *fin) {
    int read = fgetc(fin);
    if (read == EOF) {
        perror("EOF reached while reading number of records");
        exit(1);
    }
    return read;
}

char *copy_pathname_and_length_from_file1_to_file2(FILE *file1, FILE *file2) {
    u_int16_t pathname_length = fread_little_endian_16(file1);

    fwrite_little_endian_16(file2, pathname_length);
    char *pathname = malloc(sizeof(char)*(pathname_length + 1));
    pathname[pathname_length] = '\0';
    for (int i = 0; i < pathname_length; i++) {
        int read = fgetc(file1);
        if (read == EOF) {
            perror("reached EOF at pathname");
            exit(1);
        }
        fputc((char)read, file2);
        pathname[i] = (char)read;
    }
    return pathname;
}

char *read_pathname(FILE *fin) {
    u_int16_t pathname_length = fread_little_endian_16(fin);

    char *pathname = malloc(sizeof(char)*(pathname_length + 1));
    pathname[pathname_length] = '\0';
    for (int i = 0; i < pathname_length; i++) {
        int read = fgetc(fin);
        if (read == EOF) {
            perror("reached EOF at pathname");
            exit(1);
        }
        pathname[i] = (char)read;
    }
    return pathname;
}

int copy_num_blocks_from_file1_to_file2(FILE *ftabi, FILE *ftbbi) {
    uint32_t num_blocks = fread_little_endian_24(ftabi);
    fwrite_little_endian_24(ftbbi, num_blocks);
    return (int)num_blocks;
}

int get_number_blocks_ftbbi(FILE *ftbbi) {
    uint32_t num_blocks = fread_little_endian_24(ftbbi);

    return (int)num_blocks;
}

void write_matches(int num_blocks, char *pathname, FILE *ftabi, FILE *ftbbi) {
    FILE *in_file = fopen(pathname, "r");
    int matches_length = num_tbbi_match_bytes(num_blocks);

    for (int i = 0; i < matches_length; i++) {
        uint8_t matches = 0;
        for (int j = 0; j < 8; j++) {
            matches <<= 1;
            if (((i * 8) + j) < num_blocks) {
                    uint64_t hash_read = fread_hash(ftabi);
                if (in_file != NULL) {

                    uint64_t hash_generated = generate_hash(in_file);
                    if (hash_read == hash_generated) {
                        matches++;
                    } else {
                    }
                }
            }
        }
        fputc(matches, ftbbi);
    }
    
    if (in_file != NULL) {
        fclose(in_file);    
    }
}

void strmode(FILE *file, mode_t mode) {
  const char chars[] = "rwxrwxrwx";
  for (size_t i = 0; i < 9; i++) {
    fputc((mode & (1 << (8-i))) ? chars[i] : '-', file);
  }
}

void print_mode_to_file(FILE *ftcbi, char *pathname) {
    struct stat s;
	if (stat(pathname, &s) != 0) {
		perror(pathname);
		exit(1);
	}
    if (S_ISREG(s.st_mode)) {
        fputc('-', ftcbi);
    } else {
        fputc('0', ftcbi);
    }
    strmode(ftcbi, s.st_mode);
}

int print_filesize_to_file(FILE *ftcbi, char *pathname) {
    struct stat s;
	if (stat(pathname, &s) != 0) {
		perror(pathname);
		exit(1);
	}
    uint32_t filesize = s.st_size;
    fwrite_little_endian_32(ftcbi, filesize);
    return (int)filesize;
}

int read_filesize_from_file(char *pathname) {
    struct stat s;
	if (stat(pathname, &s) != 0) {
		perror(pathname);
		exit(1);
	}
    return s.st_size;
}

int read_matches_and_get_updates(FILE *file, int updates[], int num_blocks) {
    int matches_length = num_tbbi_match_bytes(num_blocks);
    int num_updates = 0;

    for (int i = 0; i < matches_length; i++) {
        int c;
        if ((c = fgetc(file)) == EOF) {
            perror("Unexpected end of file when reading matches");
            exit(1);
        }
        uint8_t b = c;
        for (int j = 0; j < 8; j++) {
            int k = (i * 8) + j;
            bool match = ((b >> (7 - j)) & 1);
            if (k < num_blocks) {
                if (match) {
                    updates[k] = false;
                } else {
                    updates[k] = true;
                    num_updates++;
                } 
            } else {
                if (match) {
                    perror("incorrect padding");
                    exit(1);
                }
            }
        }
    }
    return num_updates;
}

void print_number_of_updates_to_file(FILE *file, int num_updates) {
    u_int32_t number = num_updates;
    //fwrite(&number, 3, 1, file);
    fwrite_little_endian_24(file, number);
}

void write_updates_to_file(FILE *file, char* pathname, int updates[], int num_blocks) {
    FILE *readfile = fopen(pathname, "r");
    if (readfile == NULL) {
        perror("readfile unreadable");
        exit(1);
    }
    for (int i = 0; i < num_blocks; i++) {
        char block[BLOCK_SIZE];
        memset(block, '\0', sizeof(block));
        int block_size = fread_next_256byte_block(readfile, block);
        //printf("\ni = %d: block size = %d\n", i, block_size);
        if (updates[i] == true) {
            //fwrite(&i, 3, 1, file);
            fwrite_little_endian_24(file, i);
            //fwrite(&block_size, 2, 1, file);
            fwrite_little_endian_16(file, block_size);
            for (int j = 0; j < block_size; j++) {
                //printf("writing %dth char %c\n", j, block[j]);
                fputc(block[j], file);
            }
        }
    }
}

size_t get_num_sub_entries(DIR *dir) {
    size_t num_sub_entries = 0;
    struct dirent *entry;
    struct stat fileStat;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        num_sub_entries++;
        // Obtain file stats using stat function
        if (stat(entry->d_name, &fileStat) == -1) {
            perror("stat");
            exit(1);
        }

        // Check if the entry is a directory
        if (S_ISDIR(fileStat.st_mode)) {
            DIR *sub_dir = opendir(entry->d_name);
            num_sub_entries += get_num_sub_entries(sub_dir);

            if (closedir(sub_dir) == -1) {
                perror("unable to close sub_dir");
                exit(1);
            }
        }
    }
    return num_sub_entries;
}

void get_filenames_of_entries(DIR *dir, char *sub_entries_filenames[], size_t num_sub_entries_expected) {
    size_t num_sub_entries = 0;
    struct dirent *entry;
    struct stat fileStat;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        num_sub_entries++;
        // Obtain file stats using stat function
        if (stat(entry->d_name, &fileStat) == -1) {
            perror("stat");
            exit(1);
        }

        // Check if the entry is a directory
        if (S_ISDIR(fileStat.st_mode)) {
            DIR *sub_dir = opendir(entry->d_name);
            num_sub_entries += get_num_sub_entries(sub_dir);

            if (closedir(sub_dir) == -1) {
                perror("unable to close sub_dir");
                exit(1);
            }
        }
    }

    if (num_sub_entries != num_sub_entries_expected) {
        perror("did not read sub_entries ");
    }
    

    struct stat fileStat;
        // Check if the entry is a regular file
        if (S_ISREG(fileStat.st_mode)) {
            printf("%s is a regular file.\n", entry->d_name);

        }

        // Check if the entry is a directory
        if (S_ISDIR(fileStat.st_mode))
            printf("%s is a directory.\n", entry->d_name);
    
    }
 */

void write_record(FILE *fout, char *in_filename) {
    FILE *fin = fopen(in_filename, "r");
    if (fin == NULL) {
        perror(in_filename);
        exit(1);
    }
    fwrite_record(fout, fin, in_filename);
    fclose(fin);
}

// iterate through all filenames and write each record
void write_records(FILE *fout, char *in_filenames[], size_t num_in_filenames) {
    for (int i = 0; i < num_in_filenames; i++) {
        char *in_filename = in_filenames[i];
        write_record(fout, in_filename);
    }
}

size_t write_sub_entries(FILE *fout, DIR *dir, char path_from_working_directory[]) {
    size_t num_sub_entries = 0;
    struct dirent *entry;
    struct stat fileStat;
    char extended_path[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        sprintf(extended_path, "%s%s", path_from_working_directory, entry->d_name);
        if (stat(extended_path, &fileStat) == -1) {
            perror("stat");
            exit(1);
        }
        num_sub_entries++;


        // write record for current entry
        if (S_ISDIR(fileStat.st_mode)) {
            fwrite_record_directory(fout, extended_path);   
            DIR *sub_dir = opendir(extended_path);
            strcat(extended_path, "/");
            num_sub_entries += write_sub_entries(fout, sub_dir, extended_path);

            if (closedir(sub_dir) == -1) {
                perror("unable to close sub_dir");
                exit(1);
            }
        } else {
            FILE *fin = fopen(extended_path, "r");
            if (fin == NULL) {
                perror(extended_path);
                exit(1);
            }
            fwrite_record(fout, fin, extended_path);   
        }
    }
    return num_sub_entries;
}

int isPathInDirectory(const char* directory, const char* pathname) {
    // Convert the directory and pathname to absolute paths
    char absoluteDirectory[PATH_MAX];
    char absolutePathname[PATH_MAX];

    if (realpath(directory, absoluteDirectory) == NULL) {
        perror("realpath");
        exit(1);
    }

    if (realpath(pathname, absolutePathname) == NULL) {
        perror("realpath");
        exit(1);
    }

    size_t directoryLen = strlen(absoluteDirectory);

    // Use strstr to check if pathname starts with the directory
    if (strstr(absolutePathname, absoluteDirectory) == absolutePathname) {
        // Make sure the next character after the directory is '/' or '\0'
        if (absolutePathname[directoryLen] == '/' || absolutePathname[directoryLen] == '\0') {
            return 1; // Pathname is within the directory
        }
    }

    return 0; // Pathname is not within the directory
}

/// @brief Create a TABI file from an array of filenames.
/// @param out_filename A path to where the new TABI file should be created.
/// @param in_filenames An array of strings containing, in order, the files
//                      that should be placed in the new TABI file.
/// @param num_in_filenames The length of the `in_filenames` array. In
///                         subset 5, when this is zero, you should include
///                         everything in the current directory.
void stage_1(char *out_filename, char *in_filenames[], size_t num_in_filenames) {
    char magic_number_tabi[] = {0x54, 0x41, 0x42, 0x49};


    // check if 
    FILE *fout = fopen(out_filename, "w");
    if (fout == NULL) {
        perror(out_filename);
        exit(1);
    } 
    write_magic(fout, magic_number_tabi);
    fputc((u_int8_t)num_in_filenames, fout);

    if (num_in_filenames == 0) {
        
        DIR *working_dir = opendir(".");
        if (working_dir == NULL) {
            perror("unable to open working dir");
            exit(1);
        }
        num_in_filenames = write_sub_entries(fout, working_dir, "");
        // move file pointer of fout to position 4 to overwrite the filesize 
        fputc((u_int8_t)num_in_filenames, fout);
    } else {
        for (int i = 0; i < num_in_filenames; i++) {
            if (!isPathInDirectory(".", in_filenames[i])) {
                perror("path not in directory");
                exit(1);
            }
        }

        // write records of input filenames
        write_records(fout, in_filenames, num_in_filenames);
    }
    fclose(fout);
}

/// @brief Create a TBBI file from a TABI file.
/// @param out_filename A path to where the new TBBI file should be created.
/// @param in_filename A path to where the existing TABI file is located.
void stage_2(char *out_filename, char *in_filename) {
    char magic_number_tabi[] = {0x54, 0x41, 0x42, 0x49};
    char magic_number_tbbi[] = {0x54, 0x42, 0x42, 0x49};

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
    verify_magic(ftabi, magic_number_tabi);
    write_magic(ftbbi, magic_number_tbbi);

    int num_of_records = copy_number_of_records_from_file1_to_file2(ftabi, ftbbi);
    for (int i = 0; i < num_of_records; i++) {

        char *pathname = copy_pathname_and_length_from_file1_to_file2(ftabi, ftbbi);
        int num_blocks = copy_num_blocks_from_file1_to_file2(ftabi, ftbbi);
        write_matches(num_blocks, pathname, ftabi, ftbbi);
        free(pathname);
    }

    if (fgetc(ftabi) != EOF) {
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
    char magic_number_tbbi[] = {0x54, 0x42, 0x42, 0x49};
    char magic_number_tcbi[] = {0x54, 0x43, 0x42, 0x49};

    FILE *ftbbi = fopen(in_filename, "r");
    if (ftbbi == NULL) {
        perror(in_filename);
        exit(1);
    }
    FILE *ftcbi = fopen(out_filename, "w");
    if (ftcbi == NULL) {
        perror(out_filename);
        exit(1);
    }
    verify_magic(ftbbi, magic_number_tbbi);
    write_magic(ftcbi, magic_number_tcbi);

    int num_of_records = copy_number_of_records_from_file1_to_file2(ftbbi, ftcbi);
    
    for (int i = 0; i < num_of_records; i++) {

        char *pathname = copy_pathname_and_length_from_file1_to_file2(ftbbi, ftcbi);
        int num_blocks = get_number_blocks_ftbbi(ftbbi);
        print_mode_to_file(ftcbi, pathname);
        int filesize = print_filesize_to_file(ftcbi, pathname);
            // fread for length (matches_length). read the first num_blocks of
            // bits and assign to updates array if 0. return number of non-padding zero bits in the array.
            int updates[num_blocks + 1];
            updates[num_blocks] = -1;
            int num_updates = read_matches_and_get_updates(ftbbi, updates, num_blocks);
            print_number_of_updates_to_file(ftcbi, num_updates);
        if (num_blocks != 0) {
            write_updates_to_file(ftcbi, pathname, updates, num_blocks);
        }

        if (num_blocks > number_of_blocks_in_file(filesize)) {
            perror("num blocks too high for filesize");
            exit(1);
        }
        free(pathname);
    }

    if (fgetc(ftbbi) != EOF) {
        perror("EOF not reached after reading given number of records");
        exit(1);
    }
    fclose(ftbbi);
    fclose(ftcbi);

}

mode_t read_mode_from_tcbi_file(FILE *ftcbi) {
    mode_t new_mode = 0;

    int filetype;
    if ((filetype = fgetc(ftcbi)) == EOF) {
        perror("Found EOF while reading filetype from tcbi");
        exit(1);
    };
    if (filetype == '-') {
        new_mode |= S_IFREG;
    } else if (filetype == 'd') {
        new_mode |= S_IFDIR;
    } else {
        perror("filetype not -/d");
        exit(1);
    }

    int userperm;
    if ((userperm = fgetc(ftcbi)) == EOF) {
        perror("Found EOF while reading userperm from tcbi");
        exit(1);
    };
    if (userperm == 'r') {
        new_mode |= S_IRUSR;
    } else if (userperm == '-') {
    } else {
        perror("userperm not r/-");
        exit(1);
    }
    if ((userperm = fgetc(ftcbi)) == EOF) {
        perror("Found EOF while reading userperm from tcbi");
        exit(1);
    };
    if (userperm == 'w') {
        new_mode |= S_IWUSR;
    } else if (userperm == '-') {
    } else {
        perror("userperm not w/-");
        exit(1);
    }
    if ((userperm = fgetc(ftcbi)) == EOF) {
        perror("Found EOF while reading userperm from tcbi");
        exit(1);
    };
    if (userperm == 'x') {
        new_mode |= S_IXUSR;
    } else if (userperm == '-') {
    } else {
        perror("userperm not x/-");
        exit(1);
    }

    int groupperm;
    if ((groupperm = fgetc(ftcbi)) == EOF) {
        perror("Found EOF while reading groupperm from tcbi");
        exit(1);
    };
    if (groupperm == 'r') {
        new_mode |= S_IRGRP;
    } else if (groupperm == '-') {
    } else {
        perror("groupperm not r/-");
        exit(1);
    }
    if ((groupperm = fgetc(ftcbi)) == EOF) {
        perror("Found EOF while reading groupperm from tcbi");
        exit(1);
    };
    if (groupperm == 'w') {
        new_mode |= S_IWGRP;
    } else if (groupperm == '-') {
    } else {
        perror("groupperm not w/-");
        exit(1);
    }
    if ((groupperm = fgetc(ftcbi)) == EOF) {
        perror("Found EOF while reading groupperm from tcbi");
        exit(1);
    };
    if (groupperm == 'x') {
        new_mode |= S_IXGRP;
    } else if (groupperm == '-') {
    } else {
        perror("groupperm not x/-");
        exit(1);
    }


    int otherperm;
    if ((otherperm = fgetc(ftcbi)) == EOF) {
        perror("Found EOF while reading otherperm from tcbi");
        exit(1);
    };
    if (otherperm == 'r') {
        new_mode |= S_IROTH;
    } else if (otherperm == '-') {
    } else {
        perror("otherperm not r/-");
        exit(1);
    }
    if ((otherperm = fgetc(ftcbi)) == EOF) {
        perror("Found EOF while reading otherperm from tcbi");
        exit(1);
    };
    if (otherperm == 'w') {
        new_mode |= S_IWOTH;
    } else if (otherperm == '-') {
    } else {
        perror("otherperm not w/-");
        exit(1);
    }
    if ((otherperm = fgetc(ftcbi)) == EOF) {
        perror("Found EOF while reading otherperm from tcbi");
        exit(1);
    };
    if (otherperm == 'x') {
        new_mode |= S_IXOTH;
    } else if (otherperm == '-') {
    } else {
        perror("otherperm not x/-");
        exit(1);
    }
    return new_mode;
}

void update_mode(char *filename, mode_t new_mode) {
    if (chmod(filename, new_mode) != 0) {
        perror("Error changing file permissions");
        exit(1);
    }
}

int read_filesize(FILE *ftcbi) {
    uint32_t filesize = fread_little_endian_32(ftcbi);
    return (int)filesize;
}

void update_block(FILE *target, int block_index, char block[], int block_size) {
    if (fseek(target, (block_index * BLOCK_SIZE), SEEK_SET) != 0) {
        perror("Error while seeking file");
        exit(1);
    }
    for (int i=0; i < block_size; i++) {
        fputc(block[i], target);    
    }
    if (block_size != BLOCK_SIZE) {
        long currentPosition = ftell(target);
        if (currentPosition == -1) {
            perror("Error getting current file position");
            exit(1);
        }

        // Step 4: Truncate the file from the current position to delete the content
        if (ftruncate(fileno(target), currentPosition) != 0) {
            perror("Error truncating the file");
            exit(1);
        } 
    }
}

int read_num_updates(FILE *ftcbi) {
    uint32_t num_updates = fread_little_endian_24(ftcbi);
    return (int)num_updates;
}

int read_block_index(FILE *ftcbi) {
    uint32_t index = fread_little_endian_24(ftcbi);
    return (int)index;
}

int read_block_size(FILE *ftcbi) {
    u_int16_t block_size = fread_little_endian_16(ftcbi);
    return (int)block_size;
}

void read_and_execute_updates(FILE *ftcbi, FILE *target, int num_updates) {
    for (int i = 0; i < num_updates; i++) {
        int block_index = read_block_index(ftcbi);
        int block_size = read_block_size(ftcbi);

        char block[BLOCK_SIZE];
        memset(block, '\0', sizeof(block));
        fread_next_block(ftcbi, block, block_size);
        update_block(target, block_index, block, block_size);
    }
}

/// @brief Apply a TCBI file to the filesystem.
/// @param in_filename A path to where the existing TCBI file is located.
void stage_4(char *in_filename) {
    char magic_number_tcbi[] = {0x54, 0x43, 0x42, 0x49};

    FILE *ftcbi = fopen(in_filename, "r");
    if (ftcbi == NULL) {
        perror(in_filename);
        exit(1);
    }
    verify_magic(ftcbi, magic_number_tcbi);
    int num_records = read_num_records(ftcbi);

    for (int i = 0; i < num_records; i++) {
        char *pathname = read_pathname(ftcbi);
        FILE *target;
        if (access(pathname, F_OK) == 0) {
            mode_t modify_permissions = 0644;
            if (chmod(pathname, modify_permissions) != 0) {
                perror("Error changing file permissions");
                exit(1);
            }
            target = fopen(pathname, "r+");  
            if (target == NULL) {
                perror(pathname);
                exit(1);
            }
        } else {
            target = fopen(pathname, "w");
            if (target == NULL) {
                perror(pathname);
                exit(1);
            }
        }
        mode_t new_mode = read_mode_from_tcbi_file(ftcbi);
        update_mode(pathname, new_mode);
        long filesize = read_filesize(ftcbi);
        int num_updates = read_num_updates(ftcbi);
        read_and_execute_updates(ftcbi, target, num_updates);
        free(pathname);

        // Step 4: Truncate the file from the current position to delete the content
        if (ftruncate(fileno(target), filesize) != 0) {
            perror("Error truncating the file");
            exit(1);
        }

        fclose(target);
    }
    if (fgetc(ftcbi) != EOF) {
        perror("EOF not reached after reading given number of records");
        exit(1);
    }
    fclose(ftcbi);
}
