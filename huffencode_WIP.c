#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define MAX_TREE_HEIGHT 17


// Struct for a node
struct huffNode{
	int data;
	int frequency;
	struct huffNode *left, *right;
};

// Collection of nodes
struct treeNodes{
	// Current size of tree
	int size;
	// Capacity
	int capacity;
	// Array of node pointers
	struct huffNode** array;
};

// Function to allocate space and create a new node
struct huffNode* newNode(int data, int frequency){
	struct huffNode* temp = (struct huffNode*)malloc(sizeof(struct huffNode));
	temp->left = temp->right = NULL;
	temp->data = data;
	temp->frequency = frequency;
	return temp;

}

struct treeNodes* createMinHeap(int capacity){
	struct treeNodes* tree = (struct treeNodes*)malloc(sizeof(struct treeNodes));
	tree->size=0;
	tree->capacity = capacity;
	tree->array = (struct huffNode**)(malloc(tree->capacity * sizeof(struct tree*)));
	return tree;
}

// function to swap nodes
void swapMinHeapNode(struct huffNode** a, struct huffNode** b){
	struct huffNode* t = *a;
	*a = *b;
	*b = t;
	return;
}

void minHeapify(struct treeNodes* tree, int id_x){
	int smallest = id_x;
	int left = 2*id_x+1;
	int right = 2*id_x+2;
	if(left < tree->size && tree->array[left]->frequency < tree->array[smallest]->frequency){
		smallest = left;
	}
	if(right < tree->size && tree->array[right]->frequency < tree->array[smallest]->frequency){
		smallest = right;
	}
	if(smallest!= id_x){
		swapMinHeapNode(&tree->array[smallest], &tree->array[id_x]);
		minHeapify(tree,smallest);
	}
	return;
}


int isSizeOne(struct treeNodes* tree){
	return (tree->size == 1);

}


struct huffNode* extractMin(struct treeNodes* tree){
	struct huffNode* temp = tree->array[0];
	tree->array[0] = tree->array[tree->size-1];
	--tree->size;
	minHeapify(tree,0);
	return temp;
}


// Function to insert a node into a tree
void insertMinHeap(struct treeNodes* tree, struct huffNode* node){
	++tree->size;
	int i=tree->size-1;
	while(i && node->frequency < tree->array[(i-1)/2]->frequency){
		tree->array[i] = tree->array[(i-1)/2];
		i = (i-1)/2;
	}
	tree->array[i] = node;
	return;
}

void buildMinHeap(struct treeNodes* tree){
	int n=tree->size-1;
	int i;
	for(i=(n-1)/2; i>=0; --i){
		minHeapify(tree,i);
	}
	return;
}

void printArray(int arr[], int n){
	
	int i;
	for(i=0; i<n; i++){
		printf("%d",arr[i]);
	}
	printf("\n");
	return;
}

int isLeaf(struct huffNode* root){
	return !(root->left) && !(root->right);

}


struct treeNodes* createAndBuildMinHeap(int data[], int frequency[], int size){
	struct treeNodes* tree = createMinHeap(size);
	for(int i=0; i<size; ++i){
		tree->array[i] = newNode(data[i], frequency[i]);
	}
	tree->size = size;
	buildMinHeap(tree);	
	return tree;
}

struct huffNode* buildHuffmanTree(int data[], int frequency[], int size){
	struct huffNode* left, *right, *top;
	//Create with capacity equal to size
	struct treeNodes* minHeap = createAndBuildMinHeap(data, frequency, size);
	// Extract lowest two (frequency) nodes and combine them untill only one left (root)
	while(!isSizeOne(minHeap)){
		//printf("!isSizeOne\n");
		left = extractMin(minHeap);
		right = extractMin(minHeap);
		top = newNode(-1, left->frequency+right->frequency);
		//printf("newNode done \n");
		top->left = left;
		top->right = right;
		insertMinHeap(minHeap, top);
		//printf("Insert done \n");
	}
	struct huffNode *returnee = extractMin(minHeap);
	return returnee;
}


void printCodes(struct huffNode* root, int arr[], int top){
	// Recursively print values
//	printf("reached printcodes \n");
	if(root->left){
		arr[top] = 0;
		printCodes(root->left,arr,top+1);
	}
	if(root->right){
		arr[top] = 1;
		printCodes(root->right, arr, top+1);
	}
	if(isLeaf(root)){
		printf("%d: ",root->data);
		printArray(arr,top);
	}
	return;
}


void HuffmanCodes(int data[], int frequency[], int size){
//	printf("ehh");
	struct huffNode* root = buildHuffmanTree(data,frequency,size);
//	printf("ehhhhh");
	int arr[MAX_TREE_HEIGHT], top=0;
	printCodes(root,arr,top);
	return;
}


// ----------------------------------------------------------------------

struct listNode{
	int value;
	int frequency;
	struct listNode *next;
};


struct list{
	int size;
	struct listNode **nodes;
};


void append(struct listNode * root, int value){
	struct listNode * current = root;
	while(current->next != NULL){
		current = current->next;
	}
	current->next = (struct listNode*)malloc(sizeof(struct listNode));
	current->next->value = value;
	current->next->frequency = 1;
	current->next->next = NULL;

}

struct listNode * createList(int value){
	struct listNode* newNode = (struct listNode*)malloc(sizeof(struct listNode));
	newNode->next = NULL;
	newNode->frequency = 1;
	return newNode;
}


int hasValue(struct listNode * root, int value){
	if(root->value == value){
		root->frequency++;
		return 0;
	}
	int output = 999;
	int atIndex = 0;
	struct listNode * current = root;
	while(current->next != NULL){
		if(current->next->value == value){
			output = atIndex;
			current->next->frequency++;
			break;
		}
		atIndex++;
		current = current->next;
	}
	return output;
}

int listSize(struct listNode * root){
	int atIndex = 0;
	struct listNode * current = root;
	while(current->next != NULL){
		atIndex++;
		current = current->next;
	}
	return atIndex;
}


typedef enum{false, true} bool;


void swap(struct listNode* a, struct listNode *b){
	int tempValue = a->value;
	int tempFreq = a->frequency;
	a->value = b->value;
	a->frequency = b->frequency;
	b->value = tempValue;
	b->frequency = tempFreq;
}
void bubbleSort(struct listNode* root){
	int swapped;
	if(root==NULL){return;}
	struct listNode *pointer1;
	struct listNode *pointer2 = NULL;;
	do{
		swapped=0;
		pointer1 = root;
		while(pointer1->next != pointer2 && pointer1->next != NULL){
			if(pointer1->frequency > pointer1->next->frequency){
				swap(pointer1, pointer1->next);
				swapped=1;
			}
			pointer1 = pointer1->next;
			
		}
		pointer2=pointer1;
	}while(swapped);
}

struct intList{
	int value;
	struct intList * next;
};
struct intList* create_intList(){
	struct intList* node = (struct intList*)malloc(sizeof(struct intList));
	node->value = 0;
	node->next = NULL;
}
int size_intList(struct intList* root){
	return root->value;
}
void append_intList(struct intList* root, int value){
	if(root==NULL)return;
	root->value++;
	struct intList* current = root;
	while(current->next != NULL){
		current = current->next;
	}
	current->next = (struct intList*)malloc(sizeof(struct intList));
	current->next->value = value;
	current->next->next = NULL;
}

int pop_intList(struct intList** root){
	if(*root==NULL||(*root)->next==NULL)return 999;
	(*root)->value--;
	struct intList* next_node = NULL;
	int returnVal = 999;
	next_node = (*root)->next->next;
	returnVal = (*root)->next->value;
	free((*root)->next);
	(*root)->next = next_node;
	return returnVal;

}




struct bitString{
	int size;
	int * string;
};
struct bitString* makeBitStringFromCode(int code){
	struct bitString* string = (struct bitString*)malloc(sizeof(struct bitString));
	int temp = code;
	if(code<0){
		temp = -code;
	}
	int tempMax = temp;
	int size = ceil(log2(tempMax));
	string->size = size;
	string->string[size];
	int checkValue;
	for(int i=0; i<floor(log2(tempMax))+1;i++){
		checkValue = pow(2,floor(log2(tempMax))-i);
		if(temp>=checkValue){
			temp -= checkValue;
			if(code<0){
				string->string[i] = 0;
			}else{
				string->string[i] = 1;
			}
		}else{
			if(code<0){
				string->string[i] = 1;
			}else{
				string->string[i] = 0;
			}
		}
	}
}



// Same, but of fixed length
struct bitString* makeBitStringFromCodeLength(int code, int length){
	struct bitString* string = (struct bitString*)malloc(sizeof(struct bitString));
	int temp = code;
	if(code<0){
		temp = -code;
	}
	int tempMax = temp;
	int size = length;
	string->size = size;
	string->string[size];
	int checkValue;
	for(int i=0; i<length;i++){
		checkValue = pow(2,floor(log2(tempMax))-i);
		if(temp>=checkValue){
			temp -= checkValue;
			if(code<0){
				string->string[i] = 0;
			}else{
				string->string[i] = 1;
			}
		}else{
			if(code<0){
				string->string[i] = 1;
			}else{
				string->string[i] = 0;
			}
		}
	}
}


struct bitStringList{
	int code;
	struct bitString* string;
	struct bitStringList* next;
};

void append_bitList(struct bitStringList* list, int code, int valueToBits){
	struct bitStringList * current = list;
	while(current->next!=NULL){
		current=current->next;
	}
	struct bitString* string = makeBitStringFromCode(valueToBits);
	struct bitStringList * addon = (struct bitStringList*)malloc(sizeof(struct bitStringList));
	addon->string = string;
	addon->code = code;
	addon->next = NULL;
	current->next=addon;
}

void append_bitListString(struct bitStringList* list, int code, int bitString[], int size){
	struct bitStringList * current = list;
	while(current->next!=NULL){
		current=current->next;
	}
	struct bitString * string = (struct bitString*)malloc(sizeof(struct bitString));
	string->size = size;
	string->string[size];
	for(int i=0; i<size; i++){
		string->string[i] = bitString[i];
	}
	struct bitStringList* addon = (struct bitStringList*)malloc(sizeof(struct bitStringList));
	addon->code = code;
	addon->string = string;
	addon->next = NULL;
	current->next = addon;
}

void append_bitListBitString(struct bitStringList* list, struct bitString* string, int code){
	struct bitStringList *current = list;
	while(current->next!=NULL){
		current=current->next;
	}
	struct bitStringList *addon = (struct bitStringList*)malloc(sizeof(struct bitStringList));
	addon->code = code;
	addon->string = string;
	addon->next=NULL;
	current->next = addon;
}

// Special function to make a list with huffmancodes
void getHuffCodes(struct huffNode* root, struct bitStringList* list,  int array[], int top){
	if(root->left){
		array[top] = 0;
		getHuffCodes(root->left, list, array, top+1);
	}
	if(root->right){
		array[top] = 1;
		getHuffCodes(root->right,list,array,top+1);
	}

	if(isLeaf(root)){
		append_bitListString(list, root->data, array, top);
	}
}

int bitStringListSize(struct bitStringList* list){
	int atIndex = 0;
	struct bitStringList * current = list;
	while(current->next!=NULL){
		current= current->next;
		atIndex++;
	}
	return atIndex;

}

struct huffNode* reorderTree(struct bitStringList * list){
	int valuesArray[bitStringListSize(list)];
	int lengths[MAX_TREE_HEIGHT];
	int valueFound;
	int valueIndex = 0;
	int count;
	struct bitStringList* current;
	for(int i=0; i<MAX_TREE_HEIGHT; i++){
		count = 0;
		current = list;
		do{
			if(current->string->size==i){
				valuesArray[valueFound] = current->code;
				valueFound++;
				count++;
			}
			current = current->next;
		}while(current->next!=NULL);
		lengths[i] = count;
	}
	

	int code=0;
	valueFound = 0;
	//String codes[] ...
	struct bitStringList* temp = (struct bitStringList*)malloc(sizeof(struct bitStringList));
	temp->next=NULL;
	for(int i=0; i<17; i++){
		for(int j=0;j<lengths[i];j++){
			if(code==0){
				temp->code=valuesArray[valueFound];
				temp->string = makeBitStringFromCodeLength(code, lengths[i]);
			}else{
				append_bitListBitString(temp, makeBitStringFromCodeLength(code, lengths[i]), code);
			}
			valueFound++;
			code++;
		}
		code = code<<1;
	}

	// Need to sort valuesArray (in each bitlength block).

	struct huffNode * newRoot = newNode(999,999);
	struct huffNode * atNode;
	struct huffNode * thisNode;
	struct huffNode * newHuffNode;
	struct bitStringList * currentBitString = temp;

	for(int i=0; i<bitStringListSize(list); i++){
		atNode = newRoot;
		thisNode = newNode(valuesArray[i],0);
		for(int j=0; j<lengths[i]; j++){
			if(j==lengths[i]-1){
				if(currentBitString->string->string[j]==0){
					atNode->left = thisNode;
				}else{
					atNode->right = thisNode;
				}
			}else{
				
				if(currentBitString->string->string[j]==0){
					if(atNode->left == NULL){
						newHuffNode = newNode(999,999);
						atNode->left = newHuffNode;
						atNode = atNode->left;
					}else{
						atNode = atNode->left;
					}
				}else{
					if(atNode->right == NULL){
						newHuffNode = newNode(999,999);
						atNode->right = newHuffNode;
						atNode = atNode->right;
					}else{
						atNode = atNode->right;
					}	
				}
			}
		}
		if(current->next!=NULL){
			currentBitString=currentBitString->next;
		}else{ printf("Error, not enough items in bitstringlist\n"); break; }
	}
	return newRoot;
}



// Outtput structure, two tree roots and an array of bits (+size of it)
struct huffmanOutput{
	int size;
	struct huffNode *dcRoot, *acRoot;
	struct intList *string;;

};



struct huffmanOutput * huffEncode(int input[], int img_width, int img_height, int block_size){
	// Setup variables
	int input_size = img_width*img_height;
	int pixels_block = (block_size*block_size);
	int img_blocks = input_size/pixels_block;
	// Setup arrays for split DC and AC values
	int DC_DPCM_values[img_blocks];
	int AC_values[input_size-img_blocks];
	int lastDC = input[0]; // To do predictive coding (DPCM)
	// Find bitlengths at same time
	int DC_DPCM_bitlengths[img_blocks];
	int AC_bitlengths[input_size-img_blocks];
	struct listNode * DC_unique_lengths = createList(input[0]);
	struct listNode * AC_unique_lengths = createList(input[1]);
	for(int i=1; i<input_size; i++){
		//Take every 64th value (starting in 0) as DC, perform DPCM
		//(predictive coding from last value)
		if(i%pixels_block==0){
			DC_DPCM_values[i%pixels_block] = input[i]-lastDC;
			lastDC = input[i];
			DC_DPCM_bitlengths[i%pixels_block] = ceil(log2(DC_DPCM_values[i%pixels_block]));
			if(hasValue(DC_unique_lengths,DC_DPCM_bitlengths[i%pixels_block])==999){
				append(DC_unique_lengths, DC_DPCM_values[i%pixels_block]);
			}
			// Not so important here, but should make execution bit faster by sorting 
			if(i%(pixels_block*8)){
				//bubbleSort(AC_unique_lengths);
				bubbleSort(DC_unique_lengths);
			}
		}else if(i<1){ // take the other 63 values as AC values (no DPCM)
			AC_values[i-(i/pixels_block)-1] = input[i];
			AC_bitlengths[i-1-(i/pixels_block)] = ceil(log2(input[i]));
			/*if(hasValue(AC_unique_lengths, AC_values[i-1-(i/pixels_block)])==999){
				append(AC_unique_lengths, AC_values[i-1-(i/pixels_block)]);
			}*/
		}
	}
	
	// Make arrays to transfer found values and frequencies to
	int sizeDC = listSize(DC_unique_lengths);
	int dc_value_array[sizeDC];
	int dc_frequencies[sizeDC];
	struct listNode* atList = DC_unique_lengths;
	int at_index = 0;
	while(atList->next != NULL){
		dc_value_array[at_index] = atList->value;
		dc_frequencies[at_index] = atList->frequency;
		atList = atList->next;
		at_index++;
	}


	int zeros = 0; 
	int entryAdded=0;
	struct intList * acNZValues = create_intList();
	struct intList * acCodes = create_intList();
	for(int i=0; i<img_blocks; i++){
		zeros = 0;
		entryAdded = 0;
		for(int j=0;j<pixels_block-1;j++){
			if(entryAdded<pixels_block){
				if(AC_bitlengths[i*pixels_block+j]==0){
					zeros++;
					if(zeros==15){
						bool moreValues = false;
						for(int k=j+1; k<pixels_block-1; k++){
							if(AC_bitlengths[(i*pixels_block)-1+k]!=0){
								moreValues = true;
								break;
							}
						}
						if(moreValues){
							append_intList(acCodes,240),
							j++;
							entryAdded+=16;
							zeros = 0;
						}else{
							append_intList(acCodes,0);
							entryAdded = pixels_block;
						}
					}// if zeros == 15
				}else{ // if bit_lengths(block+j!=0
					append_intList(acCodes,(zeros*16)+AC_bitlengths[(i*pixels_block)-1-j]);
					append_intList(acNZValues, AC_values[(i*pixels_block)+j]);
					entryAdded++;
					zeros = 0;
				}
				if(j==pixels_block-2 && AC_bitlengths[(i*(pixels_block-1))+j]==0){ // && acCodes.get(acCodes.size()-1)!=0
					append_intList(acCodes,0);
					entryAdded++;
				}
			}else{// entryAdded>=pixels_block (now)
				break;
			}
		}// For j in each block
	} // for I block


	for(int i=0; i<100; i++){append_intList(acNZValues,0);}
	printf("acCodes size: %d", size_intList(acCodes));
	struct intList * atIntList = acCodes;
	while(atIntList->next!=NULL){
		if(hasValue(AC_unique_lengths,atIntList->next->value)==999){
			append(AC_unique_lengths, atIntList->next->value);
		}
		atIntList = atIntList->next;
	}
	int sizeAC = listSize(AC_unique_lengths);
	int ac_value_array[sizeAC];
	int ac_frequencies[sizeAC];
	atList = AC_unique_lengths;
	at_index = 0;
	while(atList->next != NULL){
		ac_value_array[at_index] = atList->value;
		ac_frequencies[at_index] = atList->frequency;
		atList = atList->next;
		at_index++;
	}



	struct huffNode * dc_root = buildHuffmanTree(dc_value_array, dc_frequencies, sizeDC);
	struct huffNode * ac_root = buildHuffmanTree(ac_value_array, ac_frequencies, sizeAC);
	struct bitStringList * dc_temp = (struct bitStringList*)malloc(sizeof(struct bitStringList));
	struct bitStringList * ac_temp = (struct bitStringList*)malloc(sizeof(struct bitStringList));
	dc_temp->next=NULL;
	ac_temp->next=NULL;
	int inArr[MAX_TREE_HEIGHT];
	int top = 0;
	getHuffCodes(dc_root,dc_temp, inArr, top);
	getHuffCodes(ac_root,ac_temp, inArr, top);

	if(!(dc_temp==NULL||dc_temp->next==NULL)){
		struct bitStringList* next_node = NULL;
		next_node = dc_temp->next->next;
		free(dc_temp->next);
		dc_temp->next = next_node;
	}
	if(!(ac_temp==NULL || ac_temp->next==NULL)){
		struct bitStringList* next_node = NULL;
		next_node = ac_temp->next->next;
		free(ac_temp->next);
		ac_temp->next = next_node;
	}


	dc_root = reorderTree(dc_temp);
	ac_root = reorderTree(ac_temp);



	struct bitStringList * dc_treeList = (struct bitStringList*)malloc(sizeof(struct bitStringList));
	struct bitStringList * ac_treeList = (struct bitStringList*)malloc(sizeof(struct bitStringList));
	dc_treeList->next == NULL;
	ac_treeList->next == NULL;

	if(!(dc_treeList==NULL||dc_treeList->next==NULL)){
		struct bitStringList* next_node = NULL;
		next_node = dc_treeList->next->next;
		free(dc_treeList->next);
		dc_treeList->next = next_node;
	}
	if(!(ac_treeList==NULL || ac_treeList->next==NULL)){
		struct bitStringList* next_node = NULL;
		next_node = ac_treeList->next->next;
		free(ac_treeList->next);
		ac_treeList->next = next_node;
	}


	struct intList* dcStrings[img_blocks];
	struct intList* acStrings[img_blocks];
	struct bitStringList* dc_startList = dc_treeList;
	struct bitStringList* ac_startList = ac_treeList;
	struct bitString* tempString = NULL;
	int index = 0;
	for(int i=0;i<img_blocks; i++){
		index = 0;
		dc_startList = dc_treeList;
		do{
			if(dc_startList->code == DC_DPCM_bitlengths[i]){
				break;
			}
			index++;
			dc_startList = dc_startList->next;
		}while(dc_startList->next != NULL);
		dcStrings[i] = create_intList();
		for(int j=0; j<dc_startList->string->size; j++){
			append_intList(dcStrings[i], dc_startList->string->string[j]);
		}
		tempString = makeBitStringFromCode(DC_DPCM_values[i]);
		for(int j=0; j<tempString->size; j++){
			append_intList(dcStrings[i], tempString->string[j]);
		}
		
		

	}

	struct intList* acCodeAt = acCodes;
	struct intList* acNZValueAt = acNZValues;
	int stringIndex = 0;
	int code0;
	int valuesAdded = 0;
	while(size_intList(acCodeAt)>0){
		index = 0;
		code0 = pop_intList(&acCodeAt);
		ac_startList = ac_treeList;
		do{
			if(ac_startList->code==code0){
				break;
			}
			index++;
			ac_startList = ac_startList->next;
		}while(ac_startList->next != NULL);
		acStrings[stringIndex] = create_intList();
		for(int j=0; j<ac_startList->string->size; j++){
			append_intList(acStrings[stringIndex], ac_startList->string->string[j]);
		}
		if(code0==0){
			valuesAdded = pixels_block;
		}else if(code0==15*16){
			valuesAdded+=16;
		}else{
			tempString = makeBitStringFromCode(pop_intList(&acNZValueAt));
			for(int j=0; j<tempString->size; j++){
				append_intList(acStrings[stringIndex], tempString->string[j]);
			}
			valuesAdded+=1+(floor(ac_startList->code % 16));
		}
		if(valuesAdded>=pixels_block-1){
			valuesAdded = 0;
			stringIndex++;
		}
	}

	

	struct huffmanOutput* out = (struct huffmanOutput*)malloc(sizeof(struct huffmanOutput));
	out->dcRoot = dc_root;
	out->acRoot = ac_root;
	out->string = create_intList();
	struct intList * tempInt = NULL;
	for(int i=0; i<img_blocks; i++){
		tempInt = dcStrings[i];

		while(size_intList(tempInt)>0){
			append_intList(out->string, pop_intList(&tempInt));
		}
		tempInt = acStrings[i];
		while(size_intList(tempInt)>0){
			append_intList(out->string,pop_intList(&tempInt));
		}
	}
	return out;
}






int main(){
	FILE *myFile;
	myFile = fopen("c_out_decode.txt","r");
	int image_size = 200*200;
	int input_test[image_size];
	int i;
	double cpu_time;
	clock_t start,end;
	for(i=0; i<image_size; i++){
		fscanf(myFile, "%d", &input_test[i]);
	}	
	printf("Input was read...\n");
	start = clock();
	struct huffmanOutput * getty = huffEncode(input_test,200,200,8);
	end = clock();

	FILE *output = fopen("c_huffed.txt", "w+");
	while(size_intList(getty->string)>0){
		fprintf(output, "%d ", pop_intList(&getty->string));
	}
	printf("Huffencode ran for %f seconds..\n", ((double)(end-start)/CLOCKS_PER_SEC));
	fclose(output);
	free(getty);

	return 0;
}
