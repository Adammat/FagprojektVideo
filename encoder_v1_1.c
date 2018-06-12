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
						35,36,48,49,57,55,62,63};
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
	int block_width =(int) width/block_size;
	//Height of blocks
	int block_height = (int) blocks/block_width;
	//Allocate memory for buffer and output
	int buffer[pixels];
	int *output = malloc(sizeof(int)*pixels);
//	printf("blocks: %d, block_width: %d, block_height: %d\n", blocks, block_width, block_height);	
	/*
	Docs goes here


	Calculate indecies for each pixel and save it in the  buffer
	Two first nestings handle each block, and following handles
	each pixel in every block
	*/
	float 1overConst=0.7071067811865475;
	int buffer_index=0;
	int input_index=0;
	float multiplier;
	float Cu;
	float Cv;
	float sum;
	int y_offset;
	int x_offset;
	int compression_product;
	for(int i=0;i<block_height;i++){
		y_offset = i*block_size;
		for(int j=0;j<block_width;j++){
			x_offset = j*block_size;
			for(int v=0;v<block_size;v++){
				for(int u=0;u<block_size;u++){
					Cu = 1.0;
					Cv = 1.0;
					if(u==0){
						Cu = 1overConst;
					}
					if(v==0){
						Cv = 1overConst;
					}
					multiplier = (2.0*Cu*Cv)/sqrt(pixels_block);
					sum = 0.0;
					for(int y=0;y<block_size;y++){
						for(int x=0;x<block_size;x++){
							sum= sum+(float) cos((((2.0*x)+1.0)*(u*M_PI))/(2.0*block_size))*cos((((2.0*y)+1.0)*(v*M_PI))/(2.0*block_size))*(input[((y_offset+y)*width)+(x_offset+x)]-(bit_value/2));
						}
					}
					compression_product = (int) round(fiftyQuant[u+(block_size*v)]*ratio);
					output[(((j+(i*block_width))*pixels_block))+(backward_DCT_path[u+(block_size*v)])] =  round((multiplier*sum)/compression_product);
				}
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
 int *result= encode(input_test,200,200,8,8,0.5);
end= clock();
 FILE *output = fopen("c_out.txt","w+");


 for (i = 0; i < image_size; i++)
 {
 	fprintf(output,"%d ",*(result+i));
 }

 printf("encode() ran for %f seconds\n",((double) (end - start)) / CLOCKS_PER_SEC );
 fclose(output);
free(result);
}
