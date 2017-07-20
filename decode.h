#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define D_MAX_FILE_SUFFIX 4
#define D_MAX_PSWD_BUF_SIZE 10

typedef struct _DecodeInfo
{
	/* Stego Image Info */
    char stego_image_fname[32];
    FILE *fptr_stego_image;
    
	/* Secret File Info */
    char secret_fname[32];
    FILE *fptr_secret;
	char secret_file_pswd[D_MAX_PSWD_BUF_SIZE];
	uchar secret_file_pswd_size;
    char extn_secret_file[D_MAX_FILE_SUFFIX];
	uchar extn_size_secret_file;
    long size_secret_file;

} DecodeInfo;


/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

Status decode_secret_file_pswd(DecodeInfo *decInfo);

/* Decode secret file extdestion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
char * decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
char decode_byte_tolsb(char *image_buffer);

#endif
