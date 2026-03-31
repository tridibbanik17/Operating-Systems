/*
 * fs_indexed.c
 * Implementation of the Indexed Allocation File System simulation.
 *
 * This file implements all file system operations defined in fs_indexed.h,
 * including initialization, file creation, file deletion, directory listing,
 * and free block management.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs_indexed.h"

/* Global file system instance */
FileSystem fs;

/*
 * allocateFreeBlock
 * Removes and returns the first free block from the free block linked list.
 * Returns NULL_BLOCK if no free blocks are available.
 */
int allocateFreeBlock(void)
{
    if (fs.vcb.freeHead == NULL) {
        return NULL_BLOCK;
    }

    FreeBlockNode *node = fs.vcb.freeHead;
    int blockNum = node->blockNumber;

    fs.vcb.freeHead = node->next;

    if (fs.vcb.freeHead == NULL) {
        fs.vcb.freeTail = NULL;
    }

    free(node);
    fs.vcb.freeBlockCount--;

    return blockNum;
}

/*
 * returnFreeBlock
 * Adds the given block number back into the free block linked list.
 * This simulates returning a previously allocated block to free storage.
 */
void returnFreeBlock(int blockNumber)
{
    FreeBlockNode *node = (FreeBlockNode *)malloc(sizeof(FreeBlockNode));
    if (node == NULL) {
        fprintf(stderr, "Error: malloc failed in returnFreeBlock.\n");
        return;
    }

    node->blockNumber = blockNumber;
    node->next = NULL;

    if (fs.vcb.freeTail == NULL) {
        fs.vcb.freeHead = node;
        fs.vcb.freeTail = node;
    } else {
        fs.vcb.freeTail->next = node;
        fs.vcb.freeTail = node;
    }

    fs.vcb.freeBlockCount++;
}

/*
 * printFreeBlocks
 * Prints all currently free blocks by traversing the free block linked list.
 */
void printFreeBlocks(void)
{
    printf("Free Blocks (%d): ", fs.vcb.freeBlockCount);

    FreeBlockNode *cur = fs.vcb.freeHead;
    while (cur != NULL) {
        printf("[%d] -> ", cur->blockNumber);
        cur = cur->next;
    }

    printf("NULL\n");
}

/*
 * returnFIBID
 * Returns a FIB ID back into the FIFO queue so it can be reused for future files.
 */
void returnFIBID(int fibID)
{
    if (fs.fibCount == MAX_FILES) {
        return;
    }

    fs.fibTail = (fs.fibTail + 1) % MAX_FILES;
    fs.fibQueue[fs.fibTail] = fibID;
    fs.fibCount++;
}

/*
 * getFileInformationBlockID
 * Retrieves the next available FIB ID from the FIFO queue.
 * Returns -1 if no FIB IDs are available.
 */
int getFileInformationBlockID(void)
{
    if (fs.fibCount == 0) {
        return -1;
    }

    int id = fs.fibQueue[fs.fibHead];
    fs.fibHead = (fs.fibHead + 1) % MAX_FILES;
    fs.fibCount--;

    return id;
}

/*
 * initFS
 * Initializes the file system by setting up the VCB, clearing the directory,
 * initializing the FIB ID queue, and loading all blocks into the free list.
 */
void initFS(void)
{
    fs.vcb.totalBlocks = MAX_BLOCKS;
    fs.vcb.blockSize = BLOCK_SIZE;
    fs.vcb.freeBlockCount = 0;
    fs.vcb.freeHead = NULL;
    fs.vcb.freeTail = NULL;

    fs.fileCount = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        fs.fib[i].inUse = 0;
    }

    fs.fibHead = 0;
    fs.fibTail = MAX_FILES - 1;
    fs.fibCount = MAX_FILES;

    for (int i = 0; i < MAX_FILES; i++) {
        fs.fibQueue[i] = i;
    }

    for (int i = 0; i < MAX_BLOCKS; i++) {
        returnFreeBlock(i);
    }

    if (fs.vcb.freeBlockCount == MAX_BLOCKS) {
        printf("Filesystem initialized with %d blocks of %d bytes each.\n",
               fs.vcb.totalBlocks, fs.vcb.blockSize);
    } else {
        fprintf(stderr, "Error: filesystem initialization failed.\n");
    }
}

/*
 * createFile
 * Creates a new file entry in the root directory and allocates an index block
 * and the required number of data blocks using indexed allocation.
 */
void createFile(const char *filename, int size)
{
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.fib[i].inUse && strcmp(fs.fib[i].filename, filename) == 0) {
            fprintf(stderr, "Error: file '%s' already exists.\n", filename);
            return;
        }
    }

    if (fs.fileCount >= MAX_FILES) {
        fprintf(stderr,
                "Error: cannot create '%s' - maximum file limit (%d) reached.\n",
                filename, MAX_FILES);
        return;
    }

    int numDataBlocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if (numDataBlocks > MAX_BLOCKS_PER_FILE) {
        fprintf(stderr,
                "Error: '%s' requires %d data blocks, exceeding per-file limit (%d).\n",
                filename, numDataBlocks, MAX_BLOCKS_PER_FILE);
        return;
    }

    int totalNeeded = numDataBlocks + 1;

    if (totalNeeded > fs.vcb.freeBlockCount) {
        fprintf(stderr,
                "Error: cannot create '%s' - not enough free blocks "
                "(need %d, have %d).\n",
                filename, totalNeeded, fs.vcb.freeBlockCount);
        return;
    }

    int indexBlockNum = allocateFreeBlock();
    if (indexBlockNum == NULL_BLOCK) {
        fprintf(stderr, "Error: failed allocating index block for '%s'.\n", filename);
        return;
    }

    int dataBlocks[MAX_BLOCKS_PER_FILE];

    for (int i = 0; i < numDataBlocks; i++) {
        dataBlocks[i] = allocateFreeBlock();

        if (dataBlocks[i] == NULL_BLOCK) {
            fprintf(stderr, "Error: failed allocating data block for '%s'.\n", filename);

            for (int j = 0; j < i; j++) {
                returnFreeBlock(dataBlocks[j]);
            }
            returnFreeBlock(indexBlockNum);
            return;
        }
    }

    int fibID = getFileInformationBlockID();
    if (fibID == -1) {
        fprintf(stderr, "Error: no FIB slot available for '%s'.\n", filename);

        for (int i = 0; i < numDataBlocks; i++) {
            returnFreeBlock(dataBlocks[i]);
        }
        returnFreeBlock(indexBlockNum);
        return;
    }

    FileInformationBlock *f = &fs.fib[fibID];

    strncpy(f->filename, filename, MAX_FILENAME_LEN - 1);
    f->filename[MAX_FILENAME_LEN - 1] = '\0';

    f->fileSize = size;
    f->numDataBlocks = numDataBlocks;
    f->indexBlockNum = indexBlockNum;
    f->fibID = fibID;
    f->inUse = 1;

    f->indexBlock.count = numDataBlocks;
    for (int i = 0; i < numDataBlocks; i++) {
        f->indexBlock.dataBlocks[i] = dataBlocks[i];
    }

    fs.fileCount++;

    printf("File '%s' created with %d data blocks + 1 index block.\n",
           filename, numDataBlocks);
}

/*
 * deleteFile
 * Deletes the specified file by returning all its data blocks and index block
 * back to the free list and freeing its directory entry.
 */
void deleteFile(const char *filename)
{
    int fibID = -1;

    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.fib[i].inUse && strcmp(fs.fib[i].filename, filename) == 0) {
            fibID = i;
            break;
        }
    }

    if (fibID == -1) {
        fprintf(stderr, "Error: file '%s' not found.\n", filename);
        return;
    }

    FileInformationBlock *f = &fs.fib[fibID];

    for (int i = 0; i < f->indexBlock.count; i++) {
        returnFreeBlock(f->indexBlock.dataBlocks[i]);
    }

    returnFreeBlock(f->indexBlockNum);

    f->inUse = 0;
    returnFIBID(fibID);
    fs.fileCount--;

    printf("File '%s' deleted.\n", filename);
}

/*
 * listFiles
 * Prints all active files in the root directory, including size, number of
 * data blocks, and the assigned FIB ID.
 */
void listFiles(void)
{
    printf("\nRoot Directory Listing (%d files):\n", fs.fileCount);

    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.fib[i].inUse) {
            printf("  %-10s | %6d bytes | %2d data blocks | FIBID=%d\n",
                   fs.fib[i].filename,
                   fs.fib[i].fileSize,
                   fs.fib[i].numDataBlocks,
                   fs.fib[i].fibID);
        }
    }

    printf("\n");
}