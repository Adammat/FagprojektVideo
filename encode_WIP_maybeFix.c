#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
//Compile with: gcc -o enctest enc.c -lm
int * forward_DCT_path;
int  backward_DCT_path[64]={0,1,5,6,14,15,27,28,
						2,4,7,13,16,26,29,42,
						3,8,12,17,25,30,41,43,
						9,11,18,24,31,40,44,53,
						10,19,23,32,39,45,52,54,
						20,22,33,38,46,51,55,60,
						21,34,37,58,50,56,59,61,
						35,36,48,49,57,0,62,63};
// Arrays definition
int fiftyQuant[64] = {16, 11, 10, 16, 24, 40, 51, 61,
                    12, 12, 14, 19, 26, 58, 60, 55,
                    14, 13, 16, 24, 40, 57, 69, 56,
                    14, 17, 22, 29, 51, 87, 80, 62,
                    18, 22, 37, 56, 68, 109,103,77,
                    24, 35, 55, 64, 81, 104,113,92,
                    49, 64, 78, 87, 103,121,120,101,
                    72, 92, 95, 98, 112,100,103,99};







int * encode(int * input, int height, int width, int block_size, int bit_depth, float ratio){



	// Calculate the values each pixel can assume
	int bit_value = (int) pow(2,bit_depth);
	// Total pixels in image
	int pixels = height * width;
  // Total pixels per block
	int pixels_block = block_size * block_size;
	// Total blocks
	int blocks = pixels/pixels_block;
	// Width of blocks
	int block_height = width/block_size;
	//Height of blocks
	int block_width = blocks/block_height;
	//Allocate memory for buffer and output
	int buffer[pixels];
	int *output = malloc(sizeof(int)*pixels);

	/*
	Docs goes here


	Calculate indecies for each pixel and save it in the  buffer
	Two first nestings handle each block, and following handles
	each pixel in every block
	*/
	int buffer_index=0;
	int input_index=0;
	for(int i=0;i<block_width;i++){
		int y_offset = i*block_size;
		for(int j=0;j<block_height;j++){
			int x_offset = j*block_size;
			for(int k=0;k<block_size;k++){
				for(int l=0;l<block_size;l++){
					buffer_index = l+(block_size*k)+((j+(block_width*i))*pixels_block);
					input_index = (y_offset+k)*(pixels/width)+(x_offset+l);
					buffer[buffer_index]=input[input_index]-(bit_value/2);
				}
			}
			for(int l=0;l<pixels_block;l++){
				int u = l % block_size;
				int v = (int) floor(i/block_size);
				float Cu = 1.0;
				float Cv = 1.0;
				if(u==0){
					Cu = 1/sqrt(2);
				}
				if(v==0){
					Cv = 1/sqrt(2);
				}
				float multiplier = (2*Cu*Cv)/sqrt(pixels_block);
				float sum =0.0;
				for(int y=0;y<block_size;y++){
					for(int x=0;x<block_size;x++){
						sum+= (float) cos((((2*x)+1)*(u*M_PI))/(2*block_size))*cos((((2*x)+1)*(u*M_PI))/(2*block_size))*buffer[((j+(i*block_width))*64)+x+(y*8)];
					}
				}
				int compression_product = (int) round(fiftyQuant[l]*ratio);
				//if(compression_product>bit_value-1){compression_product=bit_value-1;}
				//if(compression_product<0){compression_product=0;}
				output[(((j+(i*block_width))*pixels_block))+(backward_DCT_path[l])]= (int) round((compression_product*sum)/ratio);
			}
		}
	}
	return (output);
}
int main( int argc, const char* argv[] )
{

	FILE *myFile;
	myFile = fopen("1_input.txt", "r");

	//read file into array
	int image_size=200*200;
	int input_test[image_size];
	int i;
	clock_t start,end;
	double cpu_time;

	for (i = 0; i < image_size; i++)
	{
		fscanf(myFile, "%d", &input_test[i]);
	}
	printf("We read the input\n");
	/*for (i = 0; i < image_size; i++)
	{
		printf("%d ", input_test[i]);
	}*/

start = clock();
 int *result= encode(input_test,200,200,8,8,1.0);
end= clock();
 FILE *output = fopen("c_out.txt","w+");


 for (i = 0; i < image_size; i++)
 {
 	fprintf(output,"%d ",result[i]);
 }

 printf("encode() ran for %f seconds\n",((double) (end - start)) / CLOCKS_PER_SEC );
 fclose(output);
free(result);
}
