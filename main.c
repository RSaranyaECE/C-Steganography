#include <stdio.h>
#include <string.h>
#include "common.h"
#include "types.h"
#include "encode.h"
#include "decode.h"

// CHECK OPERATION TYPE
OperationType check_operation_type(char **argv)
{
	// is true then return select encode
	if (strncmp(*argv, "-e", 2) == 0)
	{
		return e_encode;
	}

	// is true then return select decode
	else if (strncmp(*argv, "-d", 2) == 0)
	{
		return e_decode;
	}

	// return e_unsupported 
	else

		return e_unsupported;
}


// READ PASSWORD
void read_password(EncodeInfo *encInfo, DecodeInfo *decInfo, int option)
{
	char pswd[10], re_pswd[10], ch;
	
	int index = 0, flag_1 = 0, flag_2 = 0, flag_3 = 0;

	getchar();

	// Create the password
	if (option == e_encode)
	{
		printf("Create the password (your password should be lessthan 8 characters): ");

		do
		{
			do
			{
				// check the character
				while ((ch = getchar()) != '\n')
				{
					// characters not space and \ then pass into array
					if (ch != 32 && ch != 92)
					{
						pswd[index] = ch;

						index++;

						flag_3 = 0;
					}

					// print error and make index value 0 and recreate the password
					else
					{
						printf("Dont use space and \\ \n\n");
						
						while (getchar() != '\n');
						
						printf("Recreate the password: ");

						index = 0;

						flag_2 = 1;

						flag_3 = 1;
					}

				}

				pswd[index] = '\0';

				// password size exceeding 8 characters then print error
				if (strlen(pswd) > 8)
				{
					index = 0;

					printf("Your password length must be lessthan 8 characters\n\n");
					
					while (getchar() != '\n');
					
					printf("Re create the password : ");
				
					flag_2 = 1;
				}

				else if (flag_3)

					flag_2 = 1;

				// No error 
				else

					flag_2 = 0;

			// run the loop if error ouccurs
			}while (flag_2);
		
			while (getchar() != '\n');

			// Re enter the password
			printf("Re enter the password : ");
			scanf("%s", re_pswd);

			// password matches then copy to secret file pswd array
			if (strncmp(pswd, re_pswd, 8) == 0)
			{
				strcpy(encInfo->secret_file_pswd, pswd);

				flag_1 = 0;
			}

			// print error make index value 0 recreate the password
			else
			{
				printf("Password does not created\n\n");
				
				index = 0;

				getchar();

				printf("Recreate the password : ");

				flag_1 = 1;
			}

		// run if error ouccurs
		}while (flag_1);

		// password successfuly created then calculate the size and store
		encInfo->secret_file_pswd_size = strlen(encInfo->secret_file_pswd);
	}

	// Read the secret file password
	else if (option == e_decode)
	{
		printf("Enter the secret file password :  ");

		scanf("%s",	decInfo->secret_file_pswd);
	}

}

// READ INPUT FROM STDIN
void read_input_from_stdin(EncodeInfo *encInfo, DecodeInfo *decInfo, int option)
{
	int e_option, d_option;

	char buffer[500], ch;

	// Encode
	if (option == e_encode)
	{
		printf("\nOPTIONS\n\t1.File\n\t2.Text Message\n");

		// Read e_option
		printf("\nEnter the options : ");
		scanf("%d", &e_option);

		// 1
		if (1 == e_option)
		{
			// read secret file name
			printf("\nEnter the secret file name : ");
			scanf("%s", encInfo->secret_fname);
		}

		// Read secret information from stdin
		else if (2 == e_option)
		{
			getchar();

			fgets(buffer, 500, stdin);

			// Copy that data into tmp file
			strcpy(encInfo->secret_fname, "tmpfile001.txt");
		
			encInfo->fptr_secret = fopen(encInfo->secret_fname, "w");

			if (NULL == encInfo->fptr_secret)
			{
				printf("Error!..File could not be opend\n");
			}

			fwrite(buffer, 1, strlen(buffer), encInfo->fptr_secret);
			
			fclose(encInfo->fptr_secret);
		}
		
		// Read source image file
		printf("\nEnter the source file name : ");
		scanf("%s", encInfo->src_image_fname);

		// Read destination image file
		printf("\nEnter the destination file name : ");
		scanf("%s", encInfo->stego_image_fname);

		// Create password 
		read_password(encInfo, decInfo, option);
	}

	// Decode
	else if (option == e_decode)
	{
		// Enter Encoded file
		printf("\nEnter the encrypted file name : ");
		scanf("%s", decInfo->stego_image_fname);

		printf("\nOPTIONS\n\t1.To File\n\t2.To Stdout\n\n");

		// Read d_option
		printf("Enter the option : ");
		scanf("%d", &d_option);

		// 1
		if (1 == d_option)
		{
			// Read secret file name
			printf("\nEnter the secret file name : ");
			scanf("%s", decInfo->secret_fname);
		}

		// Make secret file name NULL
		else
		{
			decInfo->secret_fname[0] = '\0';
		}
		
		// Read password
		read_password(encInfo, decInfo, option);
	}
}



// MAIN FUNCTION
int main(int argc, char **argv)
{
	int option = 0;

	char decision;

	// 	Encode and Decode variable declaration
	EncodeInfo encInfo;

	DecodeInfo decInfo;

	do
	{
		// argc is 1 then read file names from stdin
		if (1 == argc)
		{
			printf("\nOPTIONS :\n\t1.Encryption\n\t2.Decryption\n\n");

			printf("Enter the option :  ");
			scanf("%d", &option);

			read_input_from_stdin(&encInfo, &decInfo, option);
		}

		// caommand line arguments
		else
		{
			// get option
			option = check_operation_type(&argv[1]);

			// Encode
			if (option == e_encode)
			{
				// pass file names from commandline
				if (5 == argc)
				{
					read_encode_args(argv, &encInfo);
					
					// create password
					read_password(&encInfo, &decInfo, option);
				}

				// Error handling
				else
				{
					printf("Error!..Usage : ./a.out -e <source> <text> <destination>\n");

					return e_failure;
				}

			}

			// Decode
			else if (option == e_decode)
			{
				// Pass file names from command line
				if (4 == argc)
				{
					//printf("\n");
					read_decode_args(argv, &decInfo);
					
					// read password
					read_password(&encInfo, &decInfo, option);
				}

				// Error handling
				else 
				{
					printf("Error!..Usage : ./a.out -d <destination> <text>\n");

					return e_failure;
				}
			}
		}

		// Select case
		switch (option)
		{
			// Encode
			case e_encode:

				// Call encoding function
				if (do_encoding(&encInfo) != e_success)
				{
					printf("\nError!..Failed to encode\n\n");
				}

				else

					printf("\nSuccessfully Encoded\n\n");

				break;

			// Decode 
			case e_decode:

				// Call decoding function
				if (do_decoding(&decInfo) != e_success)
				{
					printf("\nError!..Failed to decode\n\n");
				}
				
				else

					printf("\nSuccessfully Decoded\n\n");

				break;

			// print error
			default:

				printf("Your option is wrong\n");	
		}

		// files passed by commandline then return 0
		if (argc == 5 || argc == 4)

			return 0;

		else

			// read decision
			printf("\nDo you want to continue(y/Y)...  ");
			scanf("\n%c", &decision);

	// decision is y/Y then run this loop
	}while (decision == 'y' || decision == 'Y');

	return 0;
}
