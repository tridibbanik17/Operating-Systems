#ifndef FS_INDEXED_H
#define FS_INDEXED_H

/* ─── File System Configuration ─────────────────────────────────────────── */
#define BLOCK_SIZE          1024   /* bytes per block                        */
#define MAX_BLOCKS          64     /* total blocks in the volume             */
#define MAX_FILES           10     /* maximum files the FS can hold          */
#define MAX_FILENAME_LEN    32     /* max chars in a filename (incl. '\0')   */
#define MAX_BLOCKS_PER_FILE 16     /* max data blocks a single file may use  */
#define NULL_BLOCK         -1      /* sentinel: "no block"                   */

/* ─── Free-Block List Node ───────────────────────────────────────────────── */
typedef struct FreeBlockNode {
    int blockNumber;
    struct FreeBlockNode *next;
} FreeBlockNode;

/* ─── Volume Control Block ───────────────────────────────────────────────── */
typedef struct {
    int totalBlocks;          /* total blocks in the volume                  */
    int blockSize;            /* bytes per block                             */
    int freeBlockCount;       /* number of currently free blocks             */
    FreeBlockNode *freeHead;  /* head of the free-block linked list          */
    FreeBlockNode *freeTail;  /* tail of the free-block linked list          */
} VolumeControlBlock;

/* ─── Index Block ────────────────────────────────────────────────────────── */
typedef struct {
    int dataBlocks[MAX_BLOCKS_PER_FILE]; /* block numbers of data blocks     */
    int count;                           /* how many data blocks are used     */
} IndexBlock;

/* ─── File Information Block (directory entry) ───────────────────────────── */
typedef struct {
    char        filename[MAX_FILENAME_LEN];
    int         fileSize;       /* size in bytes                               */
    int         numDataBlocks;  /* number of data blocks allocated             */
    int         indexBlockNum;  /* block number holding the index block        */
    IndexBlock  indexBlock;    /* the actual index block (in-memory copy)     */
    int         fibID;          /* FIB slot index                              */
    int         inUse;          /* 1 = slot occupied, 0 = free                 */
} FileInformationBlock;

/* ─── File System Structure ──────────────────────────────────────────────── */
typedef struct {
    VolumeControlBlock  vcb;
    FileInformationBlock fib[MAX_FILES];
    int fibQueue[MAX_FILES]; /* 1 = slot free, 0 = taken */
    int fibHead;
    int fibTail;
    int fibCount;
    int fileCount;
} FileSystem;

/* ─── Function Prototypes ────────────────────────────────────────────────── */

/* Core operations */
void initFS(void);
void createFile(const char *filename, int size);
void deleteFile(const char *filename);
void listFiles(void);

/* Utility */
int  allocateFreeBlock(void);
void returnFreeBlock(int blockNumber);
void printFreeBlocks(void);
void returnFIBID(int fibID);

/* Internal helper */
int  getFileInformationBlockID(void);

#endif /* FS_INDEXED_H */
