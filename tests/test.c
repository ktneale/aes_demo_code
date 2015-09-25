#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("AES file encryption/decryption tool - UNIT TEST.  Written by Kevin Neale (c) 2015\n");
    printf("[TEST_START]\n");

    /* This test will:

       1) Encrypt an plaintext input file (The entire text of 'King Lear').
       2) Decrypt the result from step 1.
       3) Perform a binary diff between the the original plaintext and the plaintext produced at step 2.
     */

    int ret = -1;


    printf("\n*** STEP1 ***\n");
    ret =system("./encrypt tests/king_lear_pt.txt output/new_king_lear_ct.txt 471C6ABB3CAD5CD41509F961EDD3A2E08E76F61FF6D63412B79E9D500257A06A 743D32B3BEDEEAF566BE6A444B89AA2E");

    if (ret != 0) {
	    printf("Error! Step 1 failed, could not encrypt the input.\n");
	    exit(EXIT_FAILURE);
    }


    printf("\n*** STEP2 ***\n");
    ret =system("./decrypt output/new_king_lear_ct.txt output/new_king_lear_pt.txt 471C6ABB3CAD5CD41509F961EDD3A2E08E76F61FF6D63412B79E9D500257A06A 743D32B3BEDEEAF566BE6A444B89AA2E");

    if (ret != 0) {
	    printf("Error! Step 2 failed, could not decrypt the input.\n");
	    exit(EXIT_FAILURE);
    }


    printf("\n*** STEP3 ***\n");
    ret =system("/usr/bin/diff tests/king_lear_pt.txt output/new_king_lear_pt.txt");

    if (ret != 0) {
	    printf("Error! Step 3 failed, the new plaintext does not match the original plaintext.\n");
	    exit(EXIT_FAILURE);
    }

    printf("SUCCESS! Unit test passed.\n");

    printf("[TEST_END]\n");
    return 0;
}
