#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types

/* Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 100 
#define MAX_IMAGE_BUF_SIZE 1000000
#define MAX_FILE_SUFFIX 4
#define MAX_PSWD_BUF_SIZE 10

typedef struct _EncodeInfo
{
    /* Source Image info */
    char src_image_fname[32];
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;

    /* Secret File Info */
    char secret_fname[32];
    FILE *fptr_secret;
	char secret_file_pswd[MAX_PSWD_BUF_SIZE];
	uchar secret_file_pswd_size;
	char extn_secret_file[MAX_FILE_SUFFIX];
	uchar extn_size_secret_file;
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;

    /* Stego Image Info */
    char stego_image_fname[32];
    FILE *fptr_stego_image;

} EncodeInfo;


/* Encoding function prototype */

/* Read and validate Encode args from argv */
Status read_encode_args(char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_encode(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);
 
/* Copy bmp image header */
Status copy_bmp_header(FILE * fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Encode secret file password size */
Status encode_secret_file_pswd(EncodeInfo *encInfo);

/* Encode secret file password */
Status encode_secret_file_pswd(EncodeInfo *encInfo);

/* Get secret file extension */
Status get_secret_file_extn(EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode a byte into LSB of image data array */
Status encode_byte_tolsb(char data, char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif
