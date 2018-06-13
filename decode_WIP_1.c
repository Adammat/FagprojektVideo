#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>


int * backward_DCT_path;
int forward_DCT_path[64] = {0, 1, 8, 16, 9, 2, 3, 10, 
			17, 24, 32, 25, 18, 11, 4, 5,
			12, 19, 26, 33, 40, 48, 41, 34,
			27, 20, 13, 6, 7, 14, 21, 28,
			35, 42, 49, 56, 57, 50, 43, 36,
			29, 22, 15, 23, 30, 37, 44, 51,
			58, 59, 52, 45, 38, 31, 39, 46,
			53,60,61,54,47,55,62,63};

int fiftyQuant[64]=	{16, 11, 10, 16, 24, 40, 51, 61,
			12, 12, 14, 19, 26, 58, 60, 55,
			14, 13, 16, 24, 40, 57, 69, 56,
			14, 17, 22, 29, 51, 87, 80, 62,
			18, 22, 37, 56, 68, 109,103,77,
			24, 35, 55, 64, 81, 104,113,92,
			49, 64, 78, 87, 103,121,120,101,
			72, 92, 95, 98, 112,100,103,99};

int * decode(int * input, int height, int width, int block_size, int bit_depth, float ratio){
	float bit_value = pow(2,bit_depth);
	int pixels = height*width;
	int pixels_block = block_size*block_size;
	int blocks = pixels/pixels_block;
	int block_width = (int)width/block_size;
	int block_height = (int)blocks/block_width;
	int *output = malloc(sizeof(int)*pixels);
	double buffer[pixels];
	double multiplier = 2.0/sqrt(pixels_block);
	double sqtwo = 1.0/sqrt(2);
	double cu = 1.0;
	double cv = 1.0;
	int bx, by, x, y;
	double sum, checkSum;
	double compression_ratio;

	for(int b=0; b<blocks; b++){
		for(int j=0;j<pixels_block;j++){
				compression_ratio = ((float)(fiftyQuant[(forward_DCT_path[j])]))*ratio;
				buffer[(b*pixels_block)+(forward_DCT_path[j])] = input[(b*pixels_block)+j]*compression_ratio;
		}
		for(int i=0;i<pixels_block;i++){
			x = i%block_size;
			y = floor(i/block_size);
			sum = 0.0;
			for(int v=0; v<block_size; v++){
				for(int u=0; u<block_size; u++){
					cu = 1.0;
					cv = 1.0;
					if(u==0){
						cu = sqtwo;
					}
					if(v==0){
						cv = sqtwo;
					}
					sum+= (cu*cv*cos((((2*x)+1)*(u*M_PI))/(2*block_size))*cos((((2*y)+1)*(v*M_PI))/(2*block_size))*buffer[(b*pixels_block)+u+(v*block_size)]);
				}
			}
		
			bx = b%block_width;
			by = floor(b/block_width);
			checkSum = (sum*multiplier)+(bit_value/2);
			if(checkSum<0){
				checkSum=0;
			}
			if(checkSum>(bit_value-1)){
				checkSum=bit_value-1;
			}
			if(b==0 && i<10){
				printf("checkSum: %f.3\n", round(checkSum));
			}
			output[(by*block_size*width)+(bx*block_size)+(y*width)+x] = (int)round(checkSum);
		}
	}
	return output;
}


int main(int argc, const char* argv[]){

	FILE *myFile;
	myFile = fopen("c_out.txt", "r");

	int image_size = 200*200;
	int input_test[image_size];
	int i;
	clock_t start,end;
	double cpu_time;
	for (i=0; i<image_size ; i++){
		fscanf(myFile, "%d", &input_test[i]);
	}
	/*for(int i=0; i<10; i++){
		printf("input_test[%d] = %d\n", i, input_test[i]);
	}
*/
	printf("input was read..\n");
	start = clock();
	int *result = decode(input_test,200,200,8,8,0.5);
	end = clock();
	FILE *output = fopen("c_out_decode.txt","w+");

	for(int i=0; i<image_size; i++){
		fprintf(output, "%d ", result[i]);
	}
	printf("decode() ran for %f seconds\n", ((double) (end-start)/CLOCKS_PER_SEC));
	fclose(output);
	free(result);
}
