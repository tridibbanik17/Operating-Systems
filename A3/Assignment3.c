/*
Program: Memory management in OS
Description:
    A simple memory management simulator. This translates logical addresses to physical addresses,
    and simulates a programs memory address request. It outputs the virtual address, the physical address and the signed byte value.
    It also outputs the total number of page faults and total number of TLB hits.

Compile: 
    gcc Assignment3.c -o assignment3
Run:
    ./assignment3
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE 256
#define PAGE_TABLE_SIZE 256
#define FRAME_SIZE 256
#define FRAME_COUNT 128
#define MEMORY_SIZE FRAME_COUNT * FRAME_SIZE
#define TLB_SIZE 16

/* A structure that represents an entry in the TLB. Each entry stores a mapping between page number and frame number*/
typedef struct {
    int page;
    int frame;
} TLBentry;

/* Simulated Physical memory. Each frame holds 256 bytes */
signed char physical_memory[MEMORY_SIZE];

/* Page table */
int page_table[PAGE_TABLE_SIZE];

/* Frame -> page mapping (needed for replacement) */
int frame_page[FRAME_COUNT];

/* TLB */
TLBentry tlb[TLB_SIZE];

int tlb_index = 0;

/* FIFO pointer for memory replacement */
int next_frame = 0;

/* Stats */
int page_faults = 0;
int tlb_hits = 0;


/* Search TLB */
int search_TLB(int page_number) {

    for (int i = 0; i < TLB_SIZE; i++) {

        if (tlb[i].page != -1 && tlb[i].page == page_number) {
            return tlb[i].frame;
        }

    }

    return -1;
}

/* Adds entry to TLB (FIFO circular) */
void TLB_Add(int page, int frame) {

    tlb[tlb_index].page = page;
    tlb[tlb_index].frame = frame;

    tlb_index = (tlb_index + 1) % TLB_SIZE;
}

/* Updates TLB when page is replaced */
void TLB_Update(int old_page) {

    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].page == old_page) {
            tlb[i].page = -1;
            tlb[i].frame = -1;
        }
    }
}

int main() {

    FILE *address_file;
    FILE *backing_file;
    signed char *backing_store;

    backing_store = (signed char *)malloc(65536);

    if (backing_store == NULL) {
        printf("Error allocating memory for backing store\n");
        return 1;
    }

    /*opens addresses.txt*/
    address_file = fopen("addresses.txt", "r");

    if (address_file == NULL) {
        printf("Error opening addresses.txt\n");
        free(backing_store);
        return 1;
    }

    /*opens the backing store*/
    backing_file = fopen("BACKING_STORE.bin", "rb");

    if (backing_file == NULL) {
        printf("Error opening BACKING_STORE.bin\n");
        fclose(address_file);
        free(backing_store);
        return 1;
    }

    fread(backing_store, 1, 65536, backing_file);
    fclose(backing_file);

    /* Initialize page table */
    for (int i = 0; i < PAGE_TABLE_SIZE; i++)
        page_table[i] = -1;

    /* Initialize frame_page */
    for (int i = 0; i < FRAME_COUNT; i++)
        frame_page[i] = -1;

    /* Initialize TLB */
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].page = -1;
        tlb[i].frame = -1;
    }

    int logical_address;
    int total_addresses = 0;

    while (fscanf(address_file, "%d", &logical_address) != EOF) {

        total_addresses++;

        /* Extract page number and offset */
        int page_number = (logical_address >> 8) & 0xFF;
        int offset = logical_address & 0xFF;

        int frame_number = search_TLB(page_number);

        /* TLB HIT */
        if (frame_number != -1) {
            tlb_hits++;
        }

        /* TLB MISS */
        else {

            frame_number = page_table[page_number];

            /* PAGE FAULT */
            if (frame_number == -1) {

                page_faults++;

                frame_number = next_frame;

                /* If frame occupied -> replace page */
                int old_page = frame_page[frame_number];

                if (old_page != -1) {
                    page_table[old_page] = -1;
                    TLB_Update(old_page);
                }

                /* Copy page from backing store */
                memcpy(physical_memory + frame_number * PAGE_SIZE,
                       backing_store + page_number * PAGE_SIZE,
                       PAGE_SIZE);

                page_table[page_number] = frame_number;
                frame_page[frame_number] = page_number;

                next_frame = (next_frame + 1) % FRAME_COUNT;
            }

            /* Add to TLB */
            TLB_Add(page_number, frame_number);
        }

        int physical_address = frame_number * PAGE_SIZE + offset;

        signed char value = physical_memory[physical_address];

        printf("Virtual address: %d Physical address = %d Value=%d\n",
               logical_address,
               physical_address,
               value);
    }

    /*printing the required statistics*/
    printf("\nNumber of Translated Addresses = %d\n", total_addresses);
    printf("Page Faults = %d\n", page_faults);
    printf("TLB Hits = %d\n", tlb_hits);

    /* cleaning up the resources*/
    free(backing_store);
    fclose(address_file);

    return 0;
}