#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>

/* Encode Function Definitions */

// READ ALL FILES
Status read_encode_args(char *argv[], EncodeInfo *encInfo)
{
	// Source image file
	strcpy(encInfo -> src_image_fname, argv[2]);

	// Destination image file
	strcpy(encInfo -> stego_image_fname, argv[4]);
	
	// Secret file
	strcpy(encInfo -> secret_fname, argv[3]);

	return e_success;
}


// OPEN ALL FILES
Status open_files_encode(EncodeInfo *encInfo)
{
    // Src Image file
 	encInfo -> fptr_src_image = fopen(encInfo -> src_image_fname, "r");

	//Do Error handling
    if (NULL == encInfo -> fptr_src_image)
    {
    	perror("fopen");
    	
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo -> src_image_fname);
    	
		return e_failure;
    }

    // Secret file
    encInfo -> fptr_secret = fopen(encInfo -> secret_fname, "r");
    
	//Do Error handling
    if (NULL == encInfo -> fptr_secret)
    {
    	perror("fopen");
    	
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo -> secret_fname);
    	
		return e_failure;
    }

    // Stego Image file
    encInfo -> fptr_stego_image = fopen(encInfo -> stego_image_fname, "w");
    
	//Do Error handling
    if (NULL == encInfo -> fptr_stego_image)
    {
    	perror("fopen");
    
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo -> stego_image_fname);
    	
		return e_failure;
    }

    // No failure return e_success
    return e_success;
}


// GET IMAGE FILE SIZE
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    // Seek to 18th byte
    fseek(fptr_image, 18L, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
//  printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
//  printf("height = %u\n", height);

    fseek(fptr_image, 0L, SEEK_SET);
   
	// Return image capacity
    return width * height * 3;
}


// GET SECRET FILE SIZE
long get_secret_file_size(FILE *fptr)
{
	// Seek to EOF
	fseek(fptr, 0L, SEEK_END);

	// Position return by ftell
	long file_size = ftell(fptr); 

	// Seek to Begining
	fseek(fptr, 0L, SEEK_SET);

	// Return file size
	return file_size;
}


// CHECK CAPACITY
Status check_capacity(EncodeInfo *encInfo)
{	
	// Secret file size
	encInfo -> size_secret_file = get_secret_file_size(encInfo -> fptr_secret);

	// Image file size
	encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);

	// Capacity check
	if (encInfo -> image_capacity > (encInfo -> size_secret_file * 8))

		// True return success
		return e_success;

	else

		// False return failure
		return e_failure;

}


// COPY BMP IMAGE FILE HEADER
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	char buffer[60];

	// Read header from source image file
	long r_size = fread(buffer, 1, 54, fptr_src_image);
	
	buffer[r_size] = '\0';
	
	// Write header into destination image file
	long w_size = fwrite(buffer, 1, r_size, fptr_dest_image);

	// Error handling
	if (r_size == w_size)
	
		return e_success;

	else

		return e_failure;

}

// ENCODE BYTE TO LSB 
Status encode_byte_tolsb(char data, char *image_buffer)
{
	int index, bit;

	// Encode single character
	for (index = 0; index < (sizeof (char) * 8); index++)
	{
		// Read a bit from data
		bit = data & (1 << index);

		// Shift bit to lsb
		bit >>= index; 

		// Encode that bit into lsb of image byte
		if (image_buffer[index] & 1)

			image_buffer[index] &= (~1);

		image_buffer[index] |= bit;
	}

	return e_success;

}


// ENCODE MAGIC STRING
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	char image_buffer[10];

	int index;

	// Magic string loop
	for (index = 0; magic_string[index] != '\0'; index++)
	{
		// Read 8 bytes from source image
		long r_size = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);
		
		image_buffer[r_size] = '\0';

		// Call encoding function
		encode_byte_tolsb(magic_string[index], image_buffer);
		
		// Write encoded 8 bytes to destination image file
		long w_size = fwrite(image_buffer, 1, r_size, encInfo -> fptr_stego_image);

		// Error handling
		if (r_size != w_size)
	
			return e_failure;
	}
	
	return e_success;
}


// ENCODE SECRET FILE PASSWORD SIZE
Status encode_secret_file_pswd_size(EncodeInfo *encInfo)
{
	char image_buffer[10];

	int index;

	// Read 8 bytes from source image file
	long r_size = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);

	image_buffer[r_size] = '\0';

	// Call encoding function
	encode_byte_tolsb(encInfo -> secret_file_pswd_size, image_buffer);

	// Write encoded bytes to destination image file
	long w_size = fwrite(image_buffer, 1, r_size, encInfo -> fptr_stego_image);

	// Error Handling
	if (r_size != w_size)

		return e_failure;

	else

		return e_success;
}

// ENCODE SECRET FILE PASSWORD
Status encode_secret_file_pswd(EncodeInfo *encInfo)
{
	char image_buffer[10];

	int index;

	// Secret file pswd loop
	for (index = 0; encInfo -> secret_file_pswd[index] != '\0'; index++)
	{
		// Read 8 bytes from source image
		long r_size = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);
		
		image_buffer[r_size] = '\0';

		// Call encoding function
		encode_byte_tolsb(encInfo -> secret_file_pswd[index], image_buffer);
		
		// Write encoded bytes to destination image file
		long w_size = fwrite(image_buffer, 1, r_size, encInfo -> fptr_stego_image);

		// Error handling
		if (r_size != w_size)
	
			return e_failure;
	}
	
	return e_success;
}



// GET SECRET FILE EXTENSION
Status get_secret_file_extn(EncodeInfo *encInfo)
{
	char *cptr = encInfo -> secret_fname;

	int index = 0;

	while (*cptr != '\0')

		cptr++;

	cptr--;

	while (*cptr != '.')

		cptr--;

	cptr++;
	
	// Copying file extension
	while (*cptr != '\0')
	{
		encInfo -> extn_secret_file[index] = *cptr;

		index++;

		cptr++;
	}

	encInfo -> extn_secret_file[index] = '\0';

	encInfo->extn_size_secret_file = index;

	return e_success;
}


/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
	char image_buffer[10];

	int index, bit;

	long r_size, w_size;

	// Read 8 bytes from source image
	r_size = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);

	image_buffer[r_size] = '\0';

	// Call encoding function
	encode_byte_tolsb(encInfo->extn_size_secret_file, image_buffer);

	// Write encoded bytes to destination image file
	w_size = fwrite(image_buffer, 1, r_size, encInfo -> fptr_stego_image);
	
	// file extn loop
	for (index = 0; file_extn[index] != '\0'; index++)
	{
		// Read 8 bytes from source image
		r_size = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);

		image_buffer[r_size] = '\0';

		// Call encoding function
		encode_byte_tolsb(file_extn[index], image_buffer);

		// Write encoded bytes to destination image file
		w_size = fwrite(image_buffer, 1, r_size, encInfo -> fptr_stego_image);

		// Error handling
		if (r_size != w_size)

			return e_failure;
	}

	return e_success;
}

// ENCODE SECRET FILE SIZE
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
	char image_buffer[10];

	int i_index, j_index, bit;

	unsigned int c_num = 0;

	char s_data;

	int size = sizeof(long) * 8;

	// secret file size loop
	for (i_index = 0; i_index < sizeof (long); i_index++)
	{
		s_data = '\0';

		c_num = 0;

		// copying byte by byte
		for(j_index = 0; j_index < 8 ; j_index++)
		{
			size--;

			bit = file_size & (1 << size);

			c_num |= bit;
		}

		c_num >>= size;

		s_data |= c_num;

		// Read 8 bytes from source image
		long r_size = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);

		image_buffer[r_size] = '\0';

		// Call encoding function
		encode_byte_tolsb(s_data, image_buffer);

		// Write encoded bytes to destination image file
		long w_size = fwrite(image_buffer, 1, r_size, encInfo -> fptr_stego_image);

		// Error handling
		if (r_size != w_size)

			return e_failure;
	}

	return e_success;
}


// ENCODE SECRET FILE DATA
Status encode_secret_file_data(EncodeInfo *encInfo)
{   
	char image_buffer[10];
	char ch;

	// loop run upto EOF
	while (feof(encInfo->fptr_secret) == 0)
	{
		// Read bulk of data from secret file
		fread(&ch, 1, 1, encInfo->fptr_secret);

		long r_size = fread(image_buffer, 1, 8, encInfo->fptr_src_image);

		image_buffer[r_size] = '\0';

		// Call encoding function
		encode_byte_tolsb(ch, image_buffer);

		// Write encoded bytes to destination image file
		long w_size = fwrite(image_buffer, 1, r_size, encInfo->fptr_stego_image);
	}

	return e_success;
}


/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{

	char image_data[MAX_IMAGE_BUF_SIZE + 10];

	while (feof(fptr_src) == 0)
	{
		// Read bulk of bytes from source image
		long r_size = fread(image_data, 1, MAX_IMAGE_BUF_SIZE, fptr_src);

		image_data[r_size] = '\0';

		// Write those bytes to destination image file
		long w_size = fwrite(image_data, 1, r_size, fptr_dest);

		// Error handling
		if (r_size != w_size)

			return e_failure;
	}

	return e_success;
}

// MAIN FUNCTION OF ENCODING
Status do_encoding(EncodeInfo *encInfo)
{
	printf("\n\nSource image file : %s\n", encInfo -> src_image_fname);

	printf("Secret file : %s\n", encInfo -> secret_fname);

	printf("Destination file : %s\n\n", encInfo -> stego_image_fname);
	
	// Open all the files and get pointers of files
	if (open_files_encode(encInfo) != e_success)
	{
		return e_failure;
	}

	// Capacity check
	if (check_capacity(encInfo) != e_success)
	{
		printf("\nError!..your source file size is lessthan secret file\n");

		return e_failure;
	}

	// Copy bmp header
	if (copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) != e_success)
	{
		printf("\nError!..Copying header failed\n");

		return e_failure;
	}

	// Encode Magic String 
	if (encode_magic_string(MAGIC_STRING, encInfo) != e_success)
	{
		return e_failure;
	}

	// Encode secret file password size
	if (encode_secret_file_pswd_size(encInfo) != e_success)
	{
		printf("\nError!..Failed to encode the password size\n");

		return e_failure;
	}

	// Encode secret file password
	if (encode_secret_file_pswd(encInfo) != e_success)
	{
		printf("\nError!..Failed to encode the password\n");

		return e_failure;
	}

	// Get secret file extension
	if (get_secret_file_extn(encInfo) != e_success)
	{
		printf("\nError!..Check your file extension\n");

		return e_failure;
	}

	// Encode secret file extenstion 
	if (encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) != e_success)
	{
		printf("\nError!..File extension encoding failed\n");

		return e_failure;
	}


	// Encode secret file size 
	if (encode_secret_file_size(encInfo -> size_secret_file, encInfo) != e_success)
	{
		printf("\nError!..Secret file size encoding failed\n");

		return e_failure;
	}

	// Encode secret file data
	if (encode_secret_file_data(encInfo) != e_success)
	{
		printf("\nError!..Secret file encoding failed\n");

		return e_failure;
	}

	// Copy remaining image bytes from src to stego image after encoding 
	if (copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) != e_success)
	{
		printf("\nError!..Failed to copying remaing image data\n");

		return e_failure;
	}

	// Close all files
	fclose(encInfo -> fptr_src_image);

	fclose(encInfo -> fptr_secret);

	fclose(encInfo -> fptr_stego_image);

	return e_success;
}

