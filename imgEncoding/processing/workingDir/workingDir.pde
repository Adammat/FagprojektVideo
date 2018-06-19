import java.util.Comparator;
import java.util.PriorityQueue;
import java.util.Collections;

// Some control booleans
boolean checkDifference = !true;
boolean printTests = !true;
boolean printTables = true;

// For controlling stage with mouse (bigger pictures)
int stageX = 0;
int stageY = 0;
boolean locked = false;
int xOffset,yOffset;




// Control variables for picture and compression
float comp=0.2;
int picWidth = 640/2;
int picHeight = 480/2;
// Downsampling of chrominance... Bits... 4 crasher
int downSampler = 0;
int downSample = round(pow(2,downSampler));
// Variables for saving picture of stage
int frameNO = 0;
String fileName = ""+picWidth+"x"+picHeight+" c"+stringy(comp)+" DS"+downSample+" "+frameNO+".png";
boolean saveFile = true;
String imageFile = "catTest.jpg";

// Other variables
int compBits = 0;



// Globally (for encoding functions) used arrays/variables 
// Quantization matrix (in vector form)
int fiftyQuant[] = {16, 11, 10, 16, 24, 40, 51, 61, 
                    12, 12, 14, 19, 26, 58, 60, 55, 
                    14, 13, 16, 24, 40, 57, 69, 56,
                    14, 17, 22, 29, 51, 87, 80, 62,
                    18, 22, 37, 56, 68, 109,103,77,
                    24, 35, 55, 64, 81, 104,113,92,
                    49, 64, 78, 87, 103,121,120,101,
                    72, 92, 95, 98, 112,100,103,99};
// Zigzag arrays, zigga is forwards and ziggb backwards
// (forwards used in decoding and back in encoding)
int ziggb[] = new int[64];                 
int zigga[] = {0, 1, 8, 16, 9, 2, 3, 10,
              17, 24, 32, 25, 18, 11, 4, 5, 
              12, 19, 26, 33, 40, 48, 41, 34, 
              27, 20, 13, 6, 7, 14, 21, 28, 
              35, 42, 49, 56, 57, 50, 43, 36, 
              29, 22, 15, 23, 30, 37, 44, 51, 
              58, 59, 52, 45, 38, 31, 39, 46, 
              53, 60, 61, 54, 47, 55, 62, 63 };




// Image objects, arrays for encoding/decoding and variables to calculate average difference
PImage testImage, midImage, resultImage, gresultImage, diffPic, ginImage, bwDiff, colDiff;
int checky[] = new int[picWidth*picHeight];
int checkGray[] = new int[picWidth*picHeight];
int checkCol[] = new int[picWidth*picHeight];
int checkRGB[][] = new int[picWidth*picHeight][3];
double checkAvgGray = 0;
double checkAvgCol = 0;
float checkAvgGray1;
float checkAvgCol1;

float PSNRGray;
float PSNRCol;




// Setup, running once (like arduino)
public void setup(){
  // Size of stage/window
  
  //size(3500,1200);
  size(1700,650);
  
  
  // Reverse the zigzag array
  for(int i=0; i<64; i++){
    ziggb[zigga[i]] = i;
  }
  // Load image, resize and make copies
  testImage = loadImage(imageFile);
  testImage.resize(picWidth,picHeight);
  resultImage = testImage.copy();
  gresultImage = testImage.copy();
  midImage = testImage.copy();
  ginImage = testImage.copy();
  bwDiff = testImage.copy();
  colDiff = testImage.copy();

  
  updateMyPixels();
}


public void draw(){
  
  

  if(saveFile == true){
    save(fileName);
    
    comp = 0.4;
    updateMyPixels();
    save(fileName);
    
    comp = 0.6;
    updateMyPixels();
    save(fileName);
    
    comp = 0.8;
    updateMyPixels();
    save(fileName);
    
    comp = 1;
    updateMyPixels();
    save(fileName);
    
    comp = 2;
    updateMyPixels();
    save(fileName);
    
    comp = 4;
    updateMyPixels();
    save(fileName);
    
    comp = 8;
    updateMyPixels();
    save(fileName);
    
    comp = 16;
    updateMyPixels();
    save(fileName);
    
    comp = 32;
    updateMyPixels();
    save(fileName);
    
    comp = 64;
    updateMyPixels();
    save(fileName);
    
    saveFile = false;
  }
}


// Shortcut-function to do full encode (and save output after DCT for testing left in)
huffmanOutput fullChannelEncode(int channel[], int wid, int hei, int blocksize, int values){
  long timey = millis();
  int dc2[] = compress(channel,wid,hei,blocksize,values);
  timey = millis()-timey;
  println("DCT Encode: "+timey+"--------------------------------------------------------------------------");
  /*saveString = ""+dc2[0];
  for(int i=1;i<channel.length;i++){
    saveString+= " "+dc2[i];
  }
  out[0] = saveString;
  saveStrings("output/compressed.txt",out);
  */
  timey = millis();
  huffmanOutput huffer = huffEncode(dc2, wid, hei, blocksize); //<>//
  timey = millis()-timey;
  println("Huff encode: "+timey+"--------------------------------------------------------------------------");
  return huffer;
}

// Shortcut function to do a full decode of a channel
int[] fullChannelDecode(huffmanOutput huff, int wid, int hei, int blocksize, int values){
  // Take out the values from the huffmanOutput object
  HuffmanNode dcRoot = huff.DC;
  HuffmanNode acRoot = huff.AC;
  String huffedString = huff.output;
  
  
  // Decode with huffman and then IDCT
  long timey = millis();
  int checkAssembled[] = huffDecode(huffedString, dcRoot, acRoot, wid, hei, blocksize);
  timey = millis()-timey;
  println("Huffman decode: "+timey+"--------------------------------------------------------------------------");
  timey = millis();
  int turnb[] = uncompress(checkAssembled, wid, hei, blocksize, values);
  timey = millis()-timey;
  println("DCT Decode: "+timey+"--------------------------------------------------------------------------");
  // Save if wanted
  /*String saveString = ""+turnb[0];
  for(int i=0;i<turnb.length;i++){
    saveString+= " "+turnb[i];
  }
  String out[] = new String[1];
  out[0] = saveString;
  saveStrings("output/uncompressed.txt",out);
  */
  return turnb;
}