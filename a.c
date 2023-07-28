/* 
 * Write a 64-bit number in little-endian byte order to a file with a specified number of bytes.
 */
void fwrite_little_endian(FILE *fout, u_int64_t number, int num_bytes) {
    // Implementation details...
}

/* 
 * Convenience function to write a 16-bit number in little-endian byte order to a file.
 */
void fwrite_little_endian_16(FILE *fout, u_int16_t number) {
    // Implementation details...
}

/* 
 * Convenience function to write a 24-bit number in little-endian byte order to a file.
 */
void fwrite_little_endian_24(FILE *fout, u_int32_t number) {
    // Implementation details...
}

/* 
 * Convenience function to write a 32-bit number in little-endian byte order to a file.
 */
void fwrite_little_endian_32(FILE *fout, u_int32_t number) {
    // Implementation details...
}

/* 
 * Convenience function to write a 64-bit number in little-endian byte order to a file.
 */
void fwrite_little_endian_64(FILE *fout, u_int64_t number) {
    // Implementation details...
}

/* 
 * Write a 64-bit number in big-endian byte order to a file with a specified byte length.
 */
void fwrite_big_endian_64(FILE *fout, u_int64_t number, int byte_length) {
    // Implementation details...
}

/* 
 * Read a little-endian 64-bit number from the file with the specified number of bytes.
 */
uint64_t fread_little_endian(FILE *fout, int num_bytes) {
    // Implementation details...
}

/* 
 * Convenience function to read a little-endian 16-bit number from the file.
 */
uint16_t fread_little_endian_16(FILE *fout) {
    // Implementation details...
}

/* 
 * Convenience function to read a little-endian 24-bit number from the file.
 */
uint32_t fread_little_endian_24(FILE *fout) {
    // Implementation details...
}

/* 
 * Convenience function to read a little-endian 32-bit number from the file.
 */
uint32_t fread_little_endian_32(FILE *fout) {
    // Implementation details...
}

/* 
 * Convenience function to read a little-endian 64-bit number from the file.
 */
uint64_t fread_little_endian_64(FILE *fout) {
    // Implementation details...
}

/* 
 * Read the next 256-byte block from the file into the provided buffer.
 */
int fread_next_256byte_block(FILE *fin, char block[]) {
    // Implementation details...
}

/* 
 * Read the next block from the file into the provided buffer with the specified size.
 */
int fread_next_block(FILE *fin, char block[], int block_size) {
    // Implementation details...
}

/* 
 * Write the hash of the next 256-byte block from the file to the output file.
 */
void fwrite_hash(FILE *fout, FILE *fin) {
    // Implementation details...
}

/* 
 * Read the hash value from the file and return it as a 64-bit unsigned integer.
 */
uint64_t fread_hash(FILE *ftabi) {
    // Implementation details...
}

/* 
 * Generate the hash value of the next 256-byte block from the file and return it as a 64-bit unsigned integer.
 */
uint64_t generate_hash(FILE *fin) {
    // Implementation details...
}

/* 
 * Write the record directory for a file to the output file.
 */
int fwrite_record_directory(FILE *fout, char *in_filename) {
    // Implementation details...
}

/* 
 * Write the record for a file, including its hash values, to the output file.
 */
int fwrite_record(FILE *fout, FILE *fin, char *in_filename) {
    // Implementation details...
}

/* 
 * Write the magic number to the output file.
 */
void write_magic(FILE *fout, char magic[]) {
    // Implementation details...
}

/* 
 * Verify the magic number in the file.
 */
void verify_magic(FILE *file, char magic[]) {
    // Implementation details...
}

/* 
 * Copy the number of records from one file to another and return the value.
 */
int copy_number_of_records_from_file1_to_file2(FILE *file1, FILE *file2) {
    // Implementation details...
}

/* 
 * Read the number of records from the file.
 */
int read_num_records(FILE *fin) {
    // Implementation details...
}

/* 
 * Copy the pathname and length from one file to another and return the pathname as a string.
 */
char *copy_pathname_and_length_from_file1_to_file2(FILE *file1, FILE *file2) {
    // Implementation details...
}

/* 
 * Read the pathname from the file and return it as a string.
 */
char *read_pathname(FILE *fin) {
    // Implementation details...
}

/* 
 * Copy the number of blocks from one file to another and return the value.
 */
int copy_num_blocks_from_file1_to_file2(FILE *ftabi, FILE *ftbbi) {
    // Implementation details...
}

/* 
 * Get the number of blocks from the TBBI file.
 */
int get_number_blocks_ftbbi(FILE *ftbbi) {
    // Implementation details...
}

/* 
 * Write the matches (hash comparison result) to the TBBI file.
 */
void write_matches(int num_blocks, char *pathname, FILE *ftabi, FILE *ftbbi) {
    // Implementation details...
}

/* 
 * Convert file mode to a string representation and write to the output file.
 */
void strmode(FILE *file, mode_t mode) {
    // Implementation details...
}

/* 
 * Print the file mode to the output file for a given pathname.
 */
void print_mode_to_file(FILE *ftcbi, char *pathname) {
    // Implementation details...
}


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define BLOCK_SIZE 256

/*
* Prints the filesize of a file to a given TCBI file.
*/
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

/*
* Reads the filesize from a given file.
*/
int read_filesize_from_file(char *pathname) {
    struct stat s;
    if (stat(pathname, &s) != 0) {
        perror(pathname);
        exit(1);
    }
    return s.st_size;
}

/*
* Reads matches from a file and retrieves updates.
*/
int read_matches_and_get_updates(FILE *file, int updates[], int num_blocks) {
    // ... implementation ...
}

/*
* Prints the number of updates to a given TCBI file.
*/
void print_number_of_updates_to_file(FILE *file, int num_updates) {
    u_int32_t number = num_updates;
    fwrite_little_endian_24(file, number);
}

/*
* Writes updates to a given TCBI file based on input data.
*/
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
        if (updates[i] == true) {
            fwrite_little_endian_24(file, i);
            fwrite_little_endian_16(file, block_size);
            for (int j = 0; j < block_size; j++) {
                fputc(block[j], file);
            }
        }
    }
}

/*
* Gets the number of sub-entries (files and directories) in a directory.
*/
size_t get_num_sub_entries(DIR *dir) {
    // ... implementation ...
}

/*
* Retrieves filenames of sub-entries in a directory.
*/
void get_filenames_of_entries(DIR *dir, char *sub_entries_filenames[], size_t num_sub_entries_expected) {
    // ... implementation ...
}

/*
* Writes a record to the specified file.
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

/*
* Writes records for an array of filenames to the specified file.
*/
void write_records(FILE *fout, char *in_filenames[], size_t num_in_filenames) {
    for (int i = 0; i < num_in_filenames; i++) {
        char *in_filename = in_filenames[i];
        write_record(fout, in_filename);
    }
}

/*
* Writes records for sub-entries (files and directories) in a directory recursively.
*/
size_t write_sub_entries(FILE *fout, DIR *dir, char path_from_working_directory[]) {
    // ... implementation ...
}

/*
* Checks if a given pathname is within a specified directory.
*/
int isPathInDirectory(const char* directory, const char* pathname) {
    // ... implementation ...
}

/*
* Creates a TABI file from an array of filenames.
*/
void stage_1(char *out_filename, char *in_filenames[], size_t num_in_filenames) {
    char magic_number_tabi[] = {0x54, 0x41, 0x42, 0x49};

    // ... implementation ...
}

/*
* Creates a TBBI file from a TABI file.
*/
void stage_2(char *out_filename, char *in_filename) {
    char magic_number_tabi[] = {0x54, 0x41, 0x42, 0x49};
    char magic_number_tbbi[] = {0x54, 0x42, 0x42, 0x49};

    // ... implementation ...
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define BLOCK_SIZE 256

/*
* Reads the mode from a TCBI file and returns the new mode.
*/
mode_t read_mode_from_tcbi_file(FILE *ftcbi) {
    mode_t new_mode = 0;

    // Read the file type from the TCBI file
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

    // Read the user permissions from the TCBI file
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

    // Read the group permissions from the TCBI file
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

    // Read the other permissions from the TCBI file
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

/*
* Updates the mode of a file with the specified new_mode.
*/
void update_mode(char *filename, mode_t new_mode) {
    if (chmod(filename, new_mode) != 0) {
        perror("Error changing file permissions");
        exit(1);
    }
}

/*
* Reads the filesize from a TCBI file and returns it.
*/
int read_filesize(FILE *ftcbi) {
    uint32_t filesize = fread_little_endian_32(ftcbi);
    return (int)filesize;
}

/*
* Updates a block in the target file at the given block_index.
*/
void update_block(FILE *target, int block_index, char block[], int block_size) {
    // ... implementation ...
}

/*
* Reads the number of updates from a TCBI file and returns it.
*/
int read_num_updates(FILE *ftcbi) {
    uint32_t num_updates = fread_little_endian_24(ftcbi);
    return (int)num_updates;
}

/*
* Reads the block index from a TCBI file and returns it.
*/
int read_block_index(FILE *ftcbi) {
    uint32_t index = fread_little_endian_24(ftcbi);
    return (int)index;
}

/*
* Reads the block size from a TCBI file and returns it.
*/
int read_block_size(FILE *ftcbi) {
    u_int16_t block_size = fread_little_endian_16(ftcbi);
    return (int)block_size;
}

/*
* Reads and executes updates from a TCBI file to the target file.
*/
void read_and_execute_updates(FILE *ftcbi, FILE *target, int num_updates) {
    // ... implementation ...
}

/*
* Applies a TCBI file to the filesystem.
*/
void stage_4(char *in_filename) {
    char magic_number_tcbi[] = {0x54, 0x43, 0x42, 0x49};

    FILE *ftcbi = fopen(in_filename, "r");
    if (ftcbi == NULL) {
        perror(in_filename);
        exit(1);
    }
    verify_magic(ftcbi, magic_number_tcbi);
    int num_records = read_num_records(ftcbi);

    for (int i = 0;
