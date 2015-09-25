#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include <unistd.h>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#define AES_KEY_LENGTH 32
#define AES_IV_LENGTH 16
#define AES_BLOCK_SIZE 16

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

void crypto_init()
{
    /* Initialise the library */
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);
}

//The user must supply a 256 bit key as a hex string e.g. 471C6ABB3CAD5CD41509F961EDD3A2E08E76F61FF6D63412B79E9D500257A06A
int get_key(unsigned char *supplied_key, unsigned char *key)
{
    if (!supplied_key || !key) {
        printf("Error! Bad key parameters!");
        return -1;
    }

    if (strlen(supplied_key) != AES_KEY_LENGTH * 2) //64 character hex string represents 32 bytes (256 bits).
    {
        printf("Error! Bad key length!");
        return -1;
    }

    int i = 0, count = 0;
    unsigned char *pos = supplied_key;

    //Convert the hex string into a byte array
    for (i = 0; i < AES_KEY_LENGTH; i++) {
        sscanf(pos, "%2hhx", &key[i]);
        pos += 2 * sizeof(char);
    }

#ifdef DEBUG
    printf("\n[KEY BYTE ARRAY - START]\n");

    for (i = 0; i < AES_KEY_LENGTH; i++) {
        printf("%02x ", key[i]);
    }

    printf("\n[KEY BYTE ARRAY - END]\n\n");
#endif

    return 0;
}


//The user must supply a 128 bit iv as a hex string e.g. 743D32B3BEDEEAF566BE6A444B89AA2E
int get_iv(unsigned char *supplied_iv, unsigned char *iv)
{
    if (!supplied_iv || !iv) {
        printf("Error! Bad iv parameters!");
        return -1;
    }

    if (strlen(supplied_iv) != AES_IV_LENGTH * 2)   //32 character hex string represents 16 bytes (128 bits).
    {
        printf("Error! Bad iv length!");
        return -1;
    }

    int i = 0, count = 0;
    unsigned char *pos = supplied_iv;

    //Convert the hex string into a byte array
    for (i = 0; i < AES_IV_LENGTH; i++) {
        sscanf(pos, "%2hhx", &iv[i]);
        pos += 2 * sizeof(char);
    }

#ifdef DEBUG
    printf("\n[IV BYTE ARRAY - START]\n");

    for (i = 0; i < AES_IV_LENGTH; i++) {
        printf("%02x ", iv[i]);
    }

    printf("\n[IV BYTE ARRAY - END]\n\n");
#endif

    return 0;
}


int encrypt(unsigned char *plaintext,
        int plaintext_len,
        unsigned char *key,
        unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    crypto_init();

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
    handleErrors();

    /* Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits */
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    handleErrors();

    /* Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if (1 !=EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    handleErrors();
    ciphertext_len = len;

    /* Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
    handleErrors();
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int decrypt(unsigned char *ciphertext,
        int ciphertext_len,
        unsigned char *key,
        unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
    handleErrors();

    /* Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits */
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    handleErrors();

    /* Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    if (1 !=EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext,
              ciphertext_len))
    handleErrors();
    plaintext_len = len;

    /* Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
    handleErrors();
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

int main(int argc, char *argv[])
{
    /* Set up the key and iv. Do I need to say to not hard code these in a
     * real application? :-)
     */

    /* A 256 bit key */
    unsigned char key[AES_KEY_LENGTH] = { 0 };

    /* A 128 bit IV */
    unsigned char iv[AES_IV_LENGTH] = { 0 };

    /* Message to be decrypted */
    unsigned char *ciphertext = NULL;


    /* Buffer for plaintext.
     */

    unsigned char *plaintext = NULL;

    int plaintext_len = 0;
    int ciphertext_len = 0;

    int i = 0;

    printf("AES file decryption tool.  Written by Kevin Neale 2015\n\n");

    if (argc < 5 || !argv[1] || !argv[2] || !argv[3] || !argv[4]) {
    printf("Usage: ./decrypt <in> <out> <key> <iv>\n\n");
    printf("    <in>         - Filepath to the file to be decrypted\n");
    printf("    <out>        - Filepath to the file which will contain the encrypted data\n\n");
    printf("    <key>        - 256 bit key to be used for the encryption represented as a 64 character hex string\n");
    printf("                   e.g. 471C6ABB3CAD5CD41509F961EDD3A2E08E76F61FF6D63412B79E9D500257A06A\n\n");
    printf("    <iv>         - 128 bit initialisation vector to be used for the encryption represented as a 32 character hex string\n");
    printf("                   e.g. 743D32B3BEDEEAF566BE6A444B89AA2E\n\n");
    exit(EXIT_FAILURE);
    }

    //Open the input file
    FILE *in = fopen(argv[1], "r");

    if (!in) {
        printf("Error! Could not open input file\n");
        exit(EXIT_FAILURE);
    }
    //Open the output file
    FILE *out = fopen(argv[2], "w");

    if (!out) {
        printf("Error! Could not open output file\n");
        exit(EXIT_FAILURE);
    }
    //Get key and iv
    if (!out) {
        printf("Error! Could not open output file\n");
        exit(EXIT_FAILURE);
    }

    if (get_key(argv[3], key) != 0 || get_iv(argv[4], iv) != 0) {
        printf("Error! Bad parameters\n");
        exit(EXIT_FAILURE);
    }

    //Prepare input buffer

    //Determine lenght of the input and allocate a buffer to store it.
    fseek(in, 0L, SEEK_END);
    ciphertext_len = ftell(in);

    printf("length of ciphertext: %d\n", ciphertext_len);
    fseek(in, 0L, SEEK_SET);

    if (ciphertext_len <= 0) {
        printf("Error! Input file is empty\n");
        exit(EXIT_FAILURE);
    }

    ciphertext = malloc(sizeof(char) * ciphertext_len);

    if (!ciphertext) {
        printf("Error! Could not allocate memory for input buffer\n");
        exit(EXIT_FAILURE);
    }

    memset(ciphertext, '\0', sizeof(char) * ciphertext_len);

    //Read the encrypted file data into the input buffer
    fread(ciphertext, 1, ciphertext_len, in);


    //Prepare the output buffer 

    plaintext_len = ciphertext_len; //It certainly will be less or equal to the cipher text length.

    plaintext = malloc(sizeof(char) * plaintext_len);

    if (!plaintext) {
        printf("Error! Could not allocate memory for output buffer\n");
        exit(EXIT_FAILURE);
    }

    memset(plaintext, '\0', sizeof(char) * plaintext_len);


    /* Decrypt the ciphertext */
    plaintext_len =
    decrypt(ciphertext, ciphertext_len, key, iv, plaintext);

    printf("length of plaintext_len: %d\n", plaintext_len);

    for (i = 0; i < plaintext_len; i++) {
        if (plaintext[i] != '\0')   //No need to write out null characters.
            fwrite(&plaintext[i], sizeof(unsigned char), 1, out);
    }

    sync();         //Force output flush

    free(ciphertext);
    free(plaintext);

    fclose(in);
    fclose(out);

    /* Clean up */
    EVP_cleanup();
    ERR_free_strings();

    return 0;
}
