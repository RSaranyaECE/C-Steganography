# C-Steganography

Introduction

Steganography is the art and science of writing hidden messages in such a way that no one, apart from the sender and intended recipient, suspects the existence of the message, a form of security through obscurity.

Algorithm

Encoding:

S1: Start

S2: Read files : 
  
  Secret file
  
  Source image file
  
  Destination image file  
  
S3: Create password

S4: Open all files

S5: Check capacity of source file, Source file size is not enough for secret file then print error and Stop

S6: Copy header file from source image to destination image

S7: Encode magic string

S8: Encode secret file password size and password

S9: Encode secret file extension

S10: Encode secret file size

S11: Encode secret file data

S12: Copy remaining bytes from source image file to destination image file

S13: Stop

Decoding:

S1: Start

S2: Read files : 

  Secret file
  
  Encoded image file

S3: Read the password

S4: Open all files

S5: Decode magic string

S6: Decode secret file password size and password

S7: Compare decoded password with user entered passsword , if they are not matching then print error and Stop 

S8: Decode secret file extension

S9: Decode secret file size

S10: Decode secret file data

S11: Print secret information to user or store the information to new text file

S12: Stop

Advantages:

  1. Secret file can be encoded  and hidden in given Image.

  2. There will be no clue that a message is hidden in the source image.

  3. There is negligible difference between source image and image with hidden file.

Future Implementation:
  
  1. Compressing the encoded image without losing the secret file data
  
