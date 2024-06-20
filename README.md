## Image-Processing


---

1. Assignment 2 is an image addition implementation, where we add 2 images and merge them into one output image.
2. Assignment 3 is the DPCM Encoder, Decoder. It has 2 files, Assignment3_Enc.c, Assignment3_Dec.c. The encoder uses the original image, and uses fixed length code for the quantization value.
   Here, I used a 3 bit fixed length code. The fixed length code is written in a text file(bitstream.txt) for the decoder to use when reconstructing the image. The decoder uses the text file to reconstruct the image.
   Note that the decoder should only use the text file and not the original image file. As a result, the output of the encoder file and decoder file should be the exact same. You can use MSE to check the error of the two outputs.
3. Assignment 4 is an upsampling code. We take a 128*128 input image and upsample it to 512x512 as an output. 
