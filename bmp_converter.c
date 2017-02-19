#include <stdio.h>
#include <string.h>
#include <math.h>
#pragma pack(1)
/*
 *   AUTHOR: ERIC DONG 
 *   DATE CREATED: 9 / 28 / 16
 */

FILE *in_file; // used for reading and writing  


 
int main(int argc, char *argv[]) {
struct header {
    char format_identifier[2]; // = {'B','M'}; //magic number
    int file_size; // size of the file in bytes
    short reserve_1; // A two-byte resereved value
    short reserve_3; // Another two-byte reserved valuea
    int offset; // offset to the start of the pixel array
}head; 

struct dib_header {
    int dib_header_size; //size of this DIB header in bytes
    int width_img; // width in pixels
    int height_img; // in pixels 
    short num_clr_planes; // number of color planes
    short num_bits; // number of bits per pixel
    int comp_scheme; // Compression scheme used
    int img_size; // image size in bytes
    int hor_res; // horizontal resolution
    int ver_res; // vertical resolution
    int num_clr; // number of colors in the palette
    int num_imp_clr; // number of important colors 
}dib_head;
struct pixel {
   unsigned char blue_inten; // blue intensity
   unsigned char green_inten; // green intensity 
   unsigned char red_inten; // red intensity 
}pix;
   // struct header head;
    //struct dib_header dib_head;
    //struct pixel pix;

    // First let's validate the input file 
    // see if format identifer is "BM" , if not display error
    int read_size;
    int height; // iterates through height of image. represents pixel column 
    int width; // iterates through width of image, represents pixel row
    float y; //based on Y = 0.2126 * R + 0.7152 * G + 0.0722 * B
    float red_norm; // normalized R value 
    float green_norm; // normalized G value
    float blue_norm; // normalized B value
    int num_pad = 0; // counts the number of padding bytes used per row 
    printf("argv1: %s, argv2: %s\n",argv[1], argv[2]);


    // open the file and prepare to load the structures with data from the file
    in_file = fopen(argv[2], "rb+");
    if (in_file == NULL) { // if the file doesn't exist, output error
        fprintf(stderr, "Error: Unable to input file '%s'\n",argv[2]);
        return 1;
    }

    // Reading in Header  
    read_size = fread((char *)&head, 1, sizeof(head), in_file);
    if (read_size != sizeof(head)) {
	fprintf(stderr, "unable to read the header\n");
  	return 1;
     }
    printf("%c %c\n", head.format_identifier[0], head.format_identifier[1]);
    if (head.format_identifier[0] != 'B' || head.format_identifier[1] != 'M') {

 	fprintf(stderr, "invalid format identifer\n");
 	return 1;
    }
    //  Reading in DIB_HEADER
    read_size = fread((char *) &dib_head, 1, sizeof(dib_head), in_file);
    if (read_size != sizeof(dib_head)) {
	fprintf(stderr, "unable to read the DIB header\n");
	return 1;
    }
    // check the size of the DiB header ... should be 40 bytes 
    printf("DIB_Header size (in bytes): %d\n", dib_head.dib_header_size); 
    

    if (dib_head.dib_header_size != 40) {
   	printf("file format not supported\n");
	return 1; 
    }
    // checking bits per pixel field, should be 24
    printf("num of bits per pixel: %hd\n", dib_head.num_bits);
    if (dib_head.num_bits != 24) {
	printf("Error: file format not supported");
        return 1;
    } 
    // moving the file pointer to the start of the pixel array.
    fseek(in_file, head.offset, SEEK_SET);
    printf("offset: %d\n", head.offset);

    printf("image width: %d\nimage height: %d\n", dib_head.width_img, dib_head.height_img);

    // if else if statement allows the user to invert the bits or grayscale them via command line arguments
    if (strcmp(argv[1],"-invert") == 0) {
        // do invert stuff here
        printf("inverting . . .\n");

	// computing the number of padding bytes in pixel array
	// if width * 3 is divisible by 4, don't need any padding bytes
	if ((dib_head.width_img*3) % 4 == 0) {	
	    num_pad = 0;
        } else { // otherwise keep adding bytes until it is a multiple of 4
	  while (((dib_head.width_img*3) + num_pad) % 4 != 0) {
 	      num_pad++;
          } 
        }
	printf("bytes to pad: %d\n", num_pad); // for im2.bmp, this value should be one 
	// using nested for loop
	// outer for loop iterates over height
	// inner for loop iterates over the width
	printf("size of pix: %d\n", sizeof(pix));  // should be 3
	for (height = 0; height < dib_head.height_img; height++) {
	    for (width = 0; width < dib_head.width_img; width++) {	
			fread(&pix, 1, sizeof(pix), in_file); 
			pix.blue_inten = ~pix.blue_inten;
			pix.green_inten = ~pix.green_inten;
			pix.red_inten = ~pix.red_inten;
 	        fseek(in_file, - sizeof(pix), SEEK_CUR);  // bring the file point 3 bytes back, preparing to write new values
			fwrite(&pix, 1, sizeof(pix), in_file);
	     }
	     // skip padding bytes before processing next row.
	     fseek(in_file, num_pad, SEEK_CUR);
        }
	printf("Inversion Complete.\n");  
     } else if (strcmp(argv[1],"-grayscale") == 0) {
        // do grayscale stuff here
        printf("grayscaling . . .\n");
	// computing the number of padding bytes in pixel array
	// if width * 3 is divisible by 4, don't need any padding bytes
	if ((dib_head.width_img*3) % 4 == 0) {	
	    num_pad = 0;
        } else { // otherwise keep adding bytes until it is a multiple of 4
	  while (((dib_head.width_img*3) + num_pad) % 4 != 0) {
 	      num_pad++;
          } 
        }
	printf("bytes to pad: %d\n", num_pad); // for im2.bmp, this value should be one 
	// using nested for loop
	// outer for loop iterates over height
	// inner for loop iterates over the width
	printf("size of pix: %d\n", sizeof(pix));  // should be 3
	for (height = 0; height < dib_head.height_img; height++) {
	    for (width = 0; width < dib_head.width_img; width++) {	
		
		fread(&pix, 1, sizeof(pix), in_file);
		// grayscaling picture work starts here

		// normalizing values color intensity values
  		red_norm = (float) pix.red_inten / (float) 255.0;
		green_norm = (float) pix.green_inten / (float) 255.0;
		blue_norm = (float) pix.blue_inten / (float) 255.0;  
		y = (0.2126 * red_norm) + (0.7152 * green_norm) + (0.0722 * blue_norm);;
		if (y <= 0.0031308) {
            red_norm = (12.92 * y)* 255.0;
		    green_norm = (12.92 * y) * 255.0;
		    blue_norm = (12.92 * y) * 255.0; 
  		} else {
		    red_norm = (1.055 * pow(y, (1.0/2.4)) - 0.055) * 255.0; 
		    green_norm = (1.055 * pow(y, (1.0/2.4)) - 0.055) * 255.0; 
		    blue_norm = (1.055 * pow(y, (1.0/2.4)) - 0.055) * 255.0;
		}
   
		// converting values back into char
		pix.red_inten = (unsigned char) red_norm;
		pix.green_inten = (unsigned char) green_norm;
		pix.blue_inten = (unsigned char) blue_norm;

		fseek(in_file, - sizeof(pix), SEEK_CUR);  // bring the file point 3 bytes back, preparing to write new values
		fwrite(&pix, 1, sizeof(pix), in_file);
		 
		
	    }
	     // skip padding bytes before processing next row.
	     fseek(in_file, num_pad, SEEK_CUR);
    }
    printf("Image Grayscaled.\n");
    }

    
    fclose(in_file);  
    return 0;
}

