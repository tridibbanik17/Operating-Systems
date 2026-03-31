/*
 * main.c
 * Driver program for testing the Indexed Allocation File System simulation.
 *
 * This program initializes the file system, creates and deletes multiple
 * files, and prints the directory listing and free block list after each
 * operation to verify correctness.
 */

#include <stdio.h>
#include "fs_indexed.h"

int main(void)
{
    initFS();

    createFile("alpha.txt", 3072);
    createFile("beta.txt", 5120);

    listFiles();
    printFreeBlocks();

    deleteFile("alpha.txt");

    listFiles();
    printFreeBlocks();

    createFile("gamma.txt", 4096);
    createFile("delta.txt", 8192);

    listFiles();
    printFreeBlocks();

    deleteFile("beta.txt");
    deleteFile("gamma.txt");
    deleteFile("delta.txt");

    listFiles();
    printFreeBlocks();

    return 0;
}