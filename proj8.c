/*
Class: CPSC 346-02
Team Member 1: Andrew Zenoni
Team Member 2: N/A 
GU Username of project lead: azenoni
Pgm Name: proj8.c 
Pgm Desc: Simulates virtual memory
To compile: gcc proj8.c
Usage: 1) ./a.out BACKING_STORE.bin addresses.txt
*/

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define TLB_SIZE 16
#define PAGES 256
#define PAGE_MASK 255
#define PAGE_SIZE 256
#define OFFSET_BITS 8
#define OFFSET_MASK 255
#define MEMORY_SIZE PAGES * PAGE_SIZE 


/*
********Important Types, Data Structures and Variables************************ 
Structs
tlb: struct representing an entry in translation lookaside buffer 

Storage
backing: pointer to memory mapped secondary storage
main_memory: array of integers simulating RAM

Tables
tlb: array of type tlb representing the translation lookaside buffer
pagetable: array of signed chars simulating the page table

Counters
numPageRefs: number of page table references
numPageFaults: counts number of page faults
numTLBRefs: number of TLB tries
numTLBHits: number of TLB references that resulted in a hit

Addresses 
logical_address: address read from addresses.txt, simulating the address bus 
physical_page: page frame number
physical_address: bytes from the 0th byte of RAM, i.e., the actual physical address 
logical_page: page table number
offset: displacement within page table/frame 

Output
Virtual address: logical_address, in Addresses, above
Physical address: physical_address in Addresses, above 
value: value stored in main_memory at physical_page displacement plus offset
*/

struct tlbentry 
   {
    unsigned char page_number;
    unsigned char frame_number;
    int tlb_ref;
   };
typedef struct tlbentry tlbentry;  

void extractPageAndOffset(int, int*, int*);
int checkIfInTLB(int, tlbentry[]);
int checkIfInPageTable(int, int[]);
int getTLBEntryIndex(int, tlbentry[]);



int main(int argc, char *argv[])
{ 
  int pagetable[PAGES];
  tlbentry tlb[TLB_SIZE];
  signed char main_memory[MEMORY_SIZE];
  signed char *backing;
  int logical_address;
  int offset;
  int logical_page;
  int physical_page;
  int physical_address;
  int numPageFaults = 0;
  int numPageRefs = 0;
  int numTLBRefs = 0;
  int numTLBHits = 0;

  // Initialize arrays to values that will never be found
  int i;
  for(i = 0; i < PAGES; i++) {
    pagetable[i] = -1;
  }

  for(i = 0; i < TLB_SIZE; i++) {
    tlb[i].page_number = -1;
  }


  //open simulation of secondary storage     
  const char *backing_filename = argv[1]; 					//BACKING_STORE.bin 
  int backing_fd = open(backing_filename, O_RDONLY);
  backing = mmap(0, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, backing_fd, 0); 	//backing can be accessed as an array  

  //open simulation of address bus and read the first line 
  FILE *ifp = fopen(argv[2],"r"); 						//addresses.txt 

  // set up initial values
  physical_page = 0;
  int count = 0;
  // Read through the entire file until reaching the end
  while(fscanf(ifp,"%d", &logical_address) != EOF) {
    // grab the offset and logical_page from the logical_address
    extractPageAndOffset(logical_address, &offset, &logical_page);

    // Increase the number of TLBRefs
    numTLBRefs += 1;
    
    // Check to see if the page is in the TLB
    if(checkIfInTLB(logical_page, tlb) == 1) {
      // Increase the tlb hits counter
      numTLBHits += 1;

      // get the passed tlb entry index
      int j = getTLBEntryIndex(logical_page, tlb);
      tlbentry tmp = tlb[j];

      // calculate the physical address
      physical_address = (tmp.frame_number << OFFSET_BITS) | offset;

      // get the value from memory
      signed char value = main_memory[tmp.frame_number * PAGE_SIZE + offset];
      
      // display the results
      printf("Virtual address: %d Physical address: %d Value: %d\n", 
              logical_address, physical_address, value);

      // set when the tlb was last referenced
      tmp.tlb_ref = count;
    } else {
      // increase the number of page references
      numPageRefs += 1;
      // determine if in the page is in the page table
      if(checkIfInPageTable(logical_page, pagetable) == 1) {
        // create a new entry for the tlb
        tlbentry newEntry;
        newEntry.page_number = logical_page;
        newEntry.frame_number = pagetable[logical_page];
        newEntry.tlb_ref = count;

        // determine which entry needs to be removed from the tlb
        int i;
        int lowest = 1000;
        int lowestIndex = 0;
        tlbentry lowestEntry;
        for(i = 0; i < TLB_SIZE; i++) {
          if(tlb[i].tlb_ref < lowest) {
            lowestEntry = tlb[i];
            lowest = tlb[i].tlb_ref;
            lowestIndex = i;
          }
        }

        // replace in tlb using LRU
        tlb[lowestIndex] = newEntry;

        // calculate the physical address
        physical_address = (pagetable[logical_page] << OFFSET_BITS) | offset;
        signed char value = main_memory[pagetable[logical_page] * PAGE_SIZE + offset];
          
        printf("Virtual address: %d Physical address: %d Value: %d\n", 
                logical_address, physical_address, value);
      } else {
        // increase the number of page faults
        numPageFaults += 1;
        
        // create a new tlb entry
        tlbentry newEntry;
        newEntry.page_number = logical_page;
        newEntry.frame_number = physical_page;
        newEntry.tlb_ref = count;

        // assign a location in the page table
        pagetable[logical_page] = physical_page;

        // determine which tlb entry needs to be removed
        int i;
        int lowest = 1000;
        int lowestIndex = 0;
        tlbentry lowestEntry;
        for(i = 0; i < TLB_SIZE; i++) {
          if(tlb[i].tlb_ref < lowest) {
            lowestEntry = tlb[i];
            lowest = tlb[i].tlb_ref;
            lowestIndex = i;
          }
        }

        // replace the tlb entry with the newly created entry
        tlb[lowestIndex] = newEntry;

        //copy from secondary storage to simulated RAM. The address on secondary storage
        //is an offset into backing, computed by multiplying the logical
        //page number by 256 and adding the offset 
        memcpy(main_memory + physical_page * PAGE_SIZE, 
                backing + logical_page * PAGE_SIZE, PAGE_SIZE);

        //Shift the physical page left 8 bits and or with the offset
        //This has the effect of adding the offset to the physical_page
        physical_address = (physical_page << OFFSET_BITS) | offset;

        //extract the value stored at offset bytes within the page
        signed char value = main_memory[physical_page * PAGE_SIZE + offset];

        printf("Virtual address: %d Physical address: %d Value: %d\n", 
            logical_address, physical_address, value);

        // increase the physical page
        physical_page += 1;
      }
    }
    // increase count of addresses
    count += 1;
  }

  // display stats
  printf("Number of Translated Addresses = %d\n", count);
  printf("Page Faults = %d\n", numPageFaults);
  printf("Page Fault Rate = %f\n", (double)numPageFaults/count);
  printf("TLB Hits = %d\n", numTLBHits);
  printf("TLB Hit Rate = %f\n", (double)numTLBHits/count);
  
  return 0;
}

// get the page and offset from the logical address
void extractPageAndOffset(int logical_address, int *offset, int *logical_page) {
    //extract low order 8 bits from the logical_address.
    *offset = logical_address & OFFSET_MASK;

    //extract bits 8 through 15. This is the page number gotten by shifting right 8 bits 
    *logical_page = (logical_address >> OFFSET_BITS) & PAGE_MASK;
}

// determine if page is in the tlb
int checkIfInTLB(int page_number, tlbentry tlb[]) {
  int i;
  for(i = 0; i < TLB_SIZE; i++) {
    if (tlb[i].page_number == page_number) {
      return 1;
    }
  }
  return 0;
}

// get the index of the page
int getTLBEntryIndex(int page_number, tlbentry tlb[]) {
  int i;
  for(i = 0; i < TLB_SIZE; i++) {
    if (tlb[i].page_number == page_number) {
      return i;
    }
  }
  
  return -1;
}

// check if page is in page table
int checkIfInPageTable(int page_number, int pagetable[]) {
  if(pagetable[page_number] != -1) {
    return 1;
  }
  return 0;
}