/*
 * fs_indexed.h
 * Header file for the Indexed Allocation File System simulation.
 *
 * This file contains all constants, structure definitions, and function
 * prototypes used in the implementation.
 *
 * The file system uses indexed allocation, where each file has an index
 * block that stores pointers to its data blocks.
 */

#ifndef FS_INDEXED_H
#define FS_INDEXED_H

#define BLOCK_SIZE          1024
#define MAX_BLOCKS          64
#define MAX_FILES           10
#define MAX_FILENAME_LEN    32
#define MAX_BLOCKS_PER_FILE 16
#define NULL_BLOCK         -1

/* Free block linked list node */
typedef struct FreeBlockNode {
    int blockNumber;
    struct FreeBlockNode *next;
} FreeBlockNode;

/* Volume Control Block (stores global metadata about the entire file system volume) */
typedef struct {
    int totalBlocks;
    int blockSize;
    int freeBlockCount;
    FreeBlockNode *freeHead;
    FreeBlockNode *freeTail;
} VolumeControlBlock;

/* Index block (stores pointers to data blocks) */
typedef struct {
    int dataBlocks[MAX_BLOCKS_PER_FILE];
    int count;
} IndexBlock;

/* File Information Block (directory entry) (stores metadata for one file in the root directory) */
typedef struct {
    char filename[MAX_FILENAME_LEN];
    int fileSize;
    int numDataBlocks;
    int indexBlockNum;
    IndexBlock indexBlock;
    int fibID;
    int inUse;
} FileInformationBlock;

/* File system structure (represents the entire indexed allocation file system) */
typedef struct {
    VolumeControlBlock vcb;
    FileInformationBlock fib[MAX_FILES];

    /* FIFO queue for available FIB IDs */
    int fibQueue[MAX_FILES];
    int fibHead;
    int fibTail;
    int fibCount;

    int fileCount;
} FileSystem;

/* Core operations */
void initFS(void);
void createFile(const char *filename, int size);
void deleteFile(const char *filename);
void listFiles(void);

/* Utility functions */
int  allocateFreeBlock(void);
void returnFreeBlock(int blockNumber);
void printFreeBlocks(void);

/* FIB queue functions */
void returnFIBID(int fibID);
int  getFileInformationBlockID(void);

#endif