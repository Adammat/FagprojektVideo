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
float comp=0.5;
int picWidth = 640/4;
int picHeight = 480/4;
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
long checkAvgGray = 0;
long checkAvgCol = 0;




// Setup, running once (like arduino)
public void setup(){
  // Size of stage/window
  size(1200,800);
  
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

  
  // Loadpixels called to open the images for manipulation.
  // Arrays to store YUV values created
  midImage.loadPixels();
  ginImage.loadPixels();
  int checkyY[] = new int[picWidth*picHeight];
  int checkyU[] = checkyY.clone();
  int checkyV[] = checkyY.clone();
  
  // For each pixel in input image
  for(int i=0;i<picWidth*picHeight;i++){
    // Get the brightness of the pixel (gray scale, built in function)
    int brightness = (int)brightness(resultImage.pixels[i]);
    checky[i] = brightness;
    // Make array of rgb values using built in functions
    int rgb[] = {(int)red(resultImage.pixels[i]),(int)green(resultImage.pixels[i]),(int)blue(resultImage.pixels[i])};
    // Add that array to check-array for RGB values (used to calculate differences)
    checkRGB[i] = rgb;
    
    // Calculate YUV values (function in Ent_Huff file) and save in corresponding arrays
    int yu[] = yuv(rgb);
    checkyY[i] = yu[0];
    checkyU[i] = yu[1]/downSample;
    checkyV[i] = yu[2]/downSample;
    // Set pixels of input images (mid color, gin = gray in)
    midImage.pixels[i] = color(rgb[0],rgb[1],rgb[2]);
    ginImage.pixels[i] = color(brightness);
  }
  
  // Do a full encode/decode of the gray scale image
  huffmanOutput chec2 = fullChannelEncode(checky,picWidth, picHeight, 8, 256);
  int checky2[] = fullChannelDecode(chec2,picWidth,picHeight,8,256);
  
  // Do a full encode/decode for the YUV channels, time it
  long timer = millis();
  huffmanOutput checY = fullChannelEncode(checkyY,picWidth, picHeight, 8, 256);
  int checky2Y[] = fullChannelDecode(checY, picWidth, picHeight, 8, 256);
  huffmanOutput checU = fullChannelEncode(checkyU,picWidth, picHeight, 8, 256/downSample);
  int checky2U[] = fullChannelDecode(checU, picWidth, picHeight, 8, 256/downSample);
  huffmanOutput checV = fullChannelEncode(checkyV,picWidth, picHeight, 8, 256/downSample);
  int checky2V[] = fullChannelDecode(checV, picWidth, picHeight, 8, 256/downSample);
  println("Full compression and decompression: "+(millis()-timer));
  
  // Output difference in bytes (out vs in)
  int compbits = checY.output.length()+checU.output.length()+checV.output.length();
  println("Full compression bits: "+(compbits)+" ("+(compbits/8)+" bytes) from "+((picWidth*picHeight*3)-((downSampler/8)*2*picWidth*picHeight))+" bytes (minus headers).");
  compBits = compbits; // Save the value in a globally accessible one for drawing
  
  // Update resultimages and difference images:
  resultImage.loadPixels();
  gresultImage.loadPixels();
  for(int i=0;i<picWidth*picHeight;i++){
    // Collect YUV values
     int yuv[] = {checky2Y[i],checky2U[i]*downSample,checky2V[i]*downSample};
     // Convert back to RGB
     int rg[] = rgb(yuv);
     // Set difference values (absolutes)
     checkGray[i] = (int)abs(checky[i]-checky2[i]);
     // Difference here is expressed as sum of all channels
     checkCol[i] = (int)(abs(checkRGB[i][0]-rg[0])+abs(checkRGB[i][1]-rg[1])+abs(checkRGB[i][2]-rg[2]));
     // Add to long variable for average calculation
     checkAvgGray += checkGray[i];
     checkAvgCol += checkCol[i];
     resultImage.pixels[i] = color(rg[0],rg[1],rg[2]);
     gresultImage.pixels[i] = color(brightness(checky2[i]));
     bwDiff.pixels[i] = color(brightness(checkGray[i]));
     colDiff.pixels[i] = color(brightness(checkCol[i]));
  }
  // Final averages
  float checkAvgGray1 = checkAvgGray/(picHeight*picWidth);
  //float checkAvgCol1 = checkAvgCol/(picHeight*picWidth);
  // Should be correct, if no downsampling just the three channels..
  // If 1 bit downsampled, (1/8)'th of U and V each should be removed
  float checkAvgCol1 = checkAvgCol/((picWidth*picHeight*3)-(((downSampler/8)*2)*picWidth*picHeight));
  
  println("Average error (gray): "+checkAvgGray1+" avg error (color) "+checkAvgCol1);
  // Update the pixels finally
  resultImage.updatePixels();
  midImage.updatePixels();
  gresultImage.updatePixels();
  bwDiff.updatePixels();
  colDiff.updatePixels();
}


public void draw(){
  
  
  // Drawing to window
  background(#000000);
  stroke(#FFFFFF);
  fill(#FFFFFF);
  //text("FPS: "+frameRate,stageX+1300,stageY-10);
  
  image(testImage,stageX+10,stageY+10); // original input
  image(ginImage, stageX+10+(10*1)+(picWidth*1),stageY+10); // gray input
  image(gresultImage,stageX+10+(2*10)+(picWidth*2),stageY+10);
  image(midImage,stageX+10+(3*10)+(picWidth*3),stageY+10); // color input
  image(resultImage,stageX+10+(4*10)+(picWidth*4),stageY+10);
  image(bwDiff,stageX+10+(0.5*picWidth)+(1*(10+picWidth)),stageY+20+picHeight);
  image(colDiff,stageX+10+(0.5*picWidth)+(3*(10+picWidth)),stageY+20+picHeight);
  text("Compression ratio: "+comp+" picsize: "+picWidth+"x"+picHeight+" downsampling (bits, UV channel): "+downSampler, stageX+10+(1*picWidth)+(2*(10+picWidth)),stageY+(2*(picHeight+10))+20);
  text("Full compression bytes (color): "+(compBits/8)+" from "+((picWidth*picHeight*3)-((downSampler/8)*2*picWidth*picHeight)), stageX+10+(1*picWidth)+(2*(10+picWidth)),stageY+(2*(picHeight+10))+40);
  // Save the window once
  if(saveFile == true){
    save(fileName);
    saveFile = false;
  }
}


// Shortcut-function to do full encode (and save output after DCT for testing left in)
huffmanOutput fullChannelEncode(int channel[], int wid, int hei, int blocksize, int values){
  int dc2[] = compress(channel,wid,hei,blocksize,values);
  /*saveString = ""+dc2[0];
  for(int i=1;i<channel.length;i++){
    saveString+= " "+dc2[i];
  }
  out[0] = saveString;
  saveStrings("output/compressed.txt",out);
  */
  huffmanOutput huffer = huffEncode(dc2, wid, hei, blocksize); //<>//
  return huffer;
}

// Shortcut function to do a full decode of a channel
int[] fullChannelDecode(huffmanOutput huff, int wid, int hei, int blocksize, int values){
  // Take out the values from the huffmanOutput object
  HuffmanNode dcRoot = huff.DC;
  HuffmanNode acRoot = huff.AC;
  String huffedString = huff.output;
  
  
  // Decode with huffman and then IDCT
  int checkAssembled[] = huffDecode(huffedString, dcRoot, acRoot, wid, hei, blocksize);
  int turnb[] = uncompress(checkAssembled, wid, hei, blocksize, values);
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