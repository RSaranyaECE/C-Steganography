#include <stdio.h>
#include "types.h"
#include "decode.h"
#include "common.h"
#include <stdlib.h>
#include <string.h>


// READ DECODE ARGUMENTS
Status read_decode_args(char *argv[], DecodeInfo *decInfo)
{
	// Encoded image file
	strcpy(decInfo -> stego_image_fname, argv[2]);

	// secret file name
	strcpy(decInfo -> secret_fname, argv[3]);

	return e_success;
}


// OPEN FILE
Status open_files_decode(DecodeInfo *decInfo)
{
	// Encoded image file
	decInfo -> fptr_stego_image = fopen(decInfo -> stego_image_fname, "r");

	//Do Error handling
    if (NULL == decInfo -> fptr_stego_image)
    {
    	perror("fopen");
    
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo -> stego_image_fname);
    
		return e_failure;
	}

	return e_success;
}


// DECODE BYTE FROM LSB
char decode_byte_fromlsb(char *image_buffer)
{
	char d_char;

	int index, bit;

	d_char = '\0';

	// loop for image buffer
	for (index = 0; index < sizeof (char) * 8; index++)
	{
		// Get lsb bit from image byte
		bit = image_buffer[index] & 1;

		// set those bit to character bits
		d_char |= (bit << index);
	}

	return d_char;
}


// DECODE MAGIC STRING
Status decode_magic_string(DecodeInfo *decInfo)
{
	char image_buffer[10];

	char magic_string[5];

	int index;

	// Seek 54th byte
	fseek(decInfo -> fptr_stego_image, 54L, SEEK_SET);

	// Loop for magic string
	for (index = 0; index < 2; index++)
	{
		// Read bytes from Encoded image file
		long r_size = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);

		image_buffer[r_size] = '\0';

		// Call decoding function, this will return decoded character 
		magic_string[index] = decode_byte_fromlsb(image_buffer);
	}

	magic_string[index] = '\0';

	// Error handling
	if (strcmp(magic_string, MAGIC_STRING) == 0)

		return e_success;

	else

		return e_failure;	
}


// DECODE SECRET FILE PASSWORD SIZE
Status decode_secret_file_pswd_size(DecodeInfo *decInfo)
{
	char image_buffer[10];

	int index;

	// Read 8 bytes from Encoded image file
	long r_size = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);

	image_buffer[r_size] = '\0';

	// Call decoding function, this will return decoded character 
	decInfo -> secret_file_pswd_size = decode_byte_fromlsb(image_buffer);

	return e_success;
}

// DECODE SECRET FILE PASSWORD
Status decode_secret_file_pswd(DecodeInfo *decInfo)
{
	char image_buffer[10];

	char password[10];

	int index;

	// loop for secret file password
	for (index = 0; index < decInfo -> secret_file_pswd_size; index++)
	{
		// Read 8 bytes from Encoded image file
		long r_size = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);

		image_buffer[r_size] = '\0';

		// Call decoding function, this will return decoded character 
		password[index] = decode_byte_fromlsb(image_buffer);
	}

	password[index] = '\0';

	// Error handling
	if (strcmp(password, decInfo -> secret_file_pswd) == 0)

		return e_success;

	else

		return e_failure;	
}

// DECODE SECRET FILE EXTENSION
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
	char image_buffer[10];
	
	int i_index, j_index;

	// Read 8 bytes from Encoded image file
	long r_size = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);

	image_buffer[r_size] = '\0';

	// Call decoding function, this will return decoded character 
	decInfo -> extn_size_secret_file = decode_byte_fromlsb(image_buffer);

	// loop for file extension
	for (i_index = 0; i_index < decInfo->extn_size_secret_file; i_index++)
	{
		// Read 8 bytes from Encoded image file
		long r_size = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);

		image_buffer[r_size] = '\0';

		// Call decoding function, this will return decoded character 
		decInfo -> extn_secret_file[i_index] = decode_byte_fromlsb(image_buffer);
	}

	decInfo->extn_secret_file[i_index] = '\0';

	// Secret file name provided by user
	if (strlen(decInfo -> secret_fname) != 0)
	{
		i_index = 0;

		// Get null character position in secret file name 
		while (decInfo -> secret_fname[i_index] != '\0')

			i_index++;

		// Store '.' character and increment the position
		decInfo -> secret_fname[i_index] = '.';

		i_index++;

		// Copy extension to secret file
		for (j_index = 0; decInfo->extn_secret_file[j_index] != '\0'; j_index++, i_index++)
		{
			decInfo -> secret_fname[i_index] = decInfo -> extn_secret_file[j_index];
		}

		// Atlast store the null character
		decInfo -> secret_fname[i_index] = '\0';
	}

	return e_success;
}


// DECODE SECRET FILE SIZE
Status decode_secret_file_size(DecodeInfo *decInfo)
{
	int index;

	char image_buffer[10];

	decInfo -> size_secret_file = 0;

	unsigned char size = 0;

	// loop for secret file size
	for (index = 0; index < sizeof (long); index++)
	{
		// Read 8 bytes from Encoded image file
		long r_size = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);

		image_buffer[r_size] = '\0';

		// Call decoding function, this will return decoded character 
		size = decode_byte_fromlsb(image_buffer);

		// set secret file size byte by byte
		decInfo -> size_secret_file |= size;

		if (sizeof (long) - 1 != index)

			decInfo -> size_secret_file <<= (sizeof(char) * 8);
	}

	return e_success;
}


// DECODE SECRET FILE DATA
char * decode_secret_file_data(DecodeInfo *decInfo)
{
	int index;

	// Allocate memory for secret data in Heap
	char *secret_data = calloc(sizeof (char), decInfo -> size_secret_file + 10);

	char image_buffer[10];

	// Rum loop upto secret file size
	for (index = 0; index < decInfo -> size_secret_file; index++)
	{
		// Read 8 bytes from Encoded image file
		long r_size = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);

		image_buffer[r_size] = '\0';

		// Call decoding function, this will return decoded character 
		secret_data[index] = decode_byte_fromlsb(image_buffer);
	}

	secret_data[index] = '\0';

	// Error handling
	if (index == decInfo -> size_secret_file)

		return secret_data;

	else

		return NULL;

}


// MAIN DECODING FUNCTION
Status do_decoding(DecodeInfo *decInfo)
{
	char *data_ptr;

	printf("\nEncoded file : %s\n", decInfo->stego_image_fname);

	// Open encoded image file
	if (open_files_decode(decInfo) != e_success)
	{
		return e_failure;
	}

	// Decode magic string
	if (decode_magic_string(decInfo) != e_success)
	{
		printf("\nError!.. Failed to decode magic string\n");

		return e_failure;
	}

	// Decode file password size
	if (decode_secret_file_pswd_size(decInfo) != e_success)
	{
		printf("\nError..Failed to decode password size\n");

		return e_failure;
	}

	// Decode file password 
	if (decode_secret_file_pswd(decInfo) != e_success)
	{
		printf("\nInvalid password\n");

		return e_failure;
	}

	// Decode secret file extension
	if (decode_secret_file_extn(decInfo) != e_success)
	{	
		printf("\nError!..Failed to decode secret file extension\n");

		return e_failure;
	}

	// Decode secret file size
	if (decode_secret_file_size(decInfo) != e_success)
	{
		printf("\nError!..Failed to decode secret file size\n");

		return e_failure;
	}

	// Decode secret file data
	data_ptr = decode_secret_file_data(decInfo);

	// Error handling
	if (data_ptr == NULL)
	{
		printf("\nError!.. Failed to decode secret file data\n");

		return e_failure;
	}

	// Secret file name is provided then copy decoded data to secret file
	if (strlen(decInfo->secret_fname) != 0)
	{
		printf("Secret file : %s\n", decInfo -> secret_fname);
	
		// Open secret file
    	decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    
		//Do Error handling
   	 	if (NULL == decInfo->fptr_secret)
    	{
    		perror("fopen");
    	
			fprintf(stderr, "\nERROR: Unable to open file %s\n", decInfo->secret_fname);
    	
			return e_failure;
		}
	
		// Write decoded data to secret file
		fwrite(data_ptr, 1, decInfo->size_secret_file, decInfo->fptr_secret);

		// close secret file
		fclose(decInfo->fptr_secret);
	}

	// otherwise print decoded data to stdout
	else
	{
		printf("\nDecoded Message :\n");
		printf("%s\n", data_ptr);
	}

	// Close encoded image file
	fclose(decInfo->fptr_stego_image);

	// free heap pointer and make that pointer null
	free(data_ptr);

	data_ptr = NULL;
	
	return e_success;
}

