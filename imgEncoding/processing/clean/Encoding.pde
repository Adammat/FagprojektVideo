int[] compress(int channel[], int wid, int hei, int blockSize, int values){
  // Setup variables
  int elements = wid*hei;
  int blockElems = blockSize*blockSize;
  int blocks = elements/blockElems;
  int blockWidth = (wid/blockSize);
  int blockHeight= blocks/blockWidth;
  
  // Setup return array
  int qdct[] = channel.clone();
  
  // For each block (going through them in x and y fashion)
  for(int i=0;i<blockHeight;i++){
    int ystart = i*blockSize;
    for(int j=0;j<blockWidth;j++){
      // Calculate x and y for pixels
      int xstart = j*blockSize;
      
      // For each pixel in that block
      for(int v=0;v<blockSize;v++){ // = y
        for(int u=0; u<blockSize; u++){ // = x
          // function c(u) = 1 if u>0 else is 1/root(2)
          float Cu = 1;
          float Cv = 1;
          if(u==0){
            Cu = 1/sqrt(2);
          }
          if(v==0){
            Cv = 1/sqrt(2);
          }
          // Multiplier for the pixel and summing variable
          float multip = (2*Cu*Cv)/sqrt(blockElems);
          float summy = 0;
          // Performing the DCT and levelshifting (levelshifting could be moved outside and be performed before,
          // like described for another function in decoding, to save a bit of calculations - 64*2 instead of 64*64 
          // levelshifting).
          for(int y=0;y<blockSize;y++){
            for(int x=0;x<blockSize;x++){
              summy+=cos((((2*x)+1)*(u*PI))/(2*blockSize))*cos((((2*y)+1)*(v*PI))/(2*blockSize))*(channel[((ystart+y)*(wid))+(xstart+x)]-(values/2));
            }
          }
          // Quantization value (with the VCL and compression ratio)
          float compy = fiftyQuant[u+(blockSize*v)]*comp;
          // Doing the last multiplication of DCT and quantization
          int dcValue = round((multip*summy)/compy);
          // Outputting in a zigzag pattern for the block
          qdct[((j+(i*blockWidth))*blockElems)+(ziggb[(u+(blockSize*v))])]=dcValue;
        }
      }
    }
  }
  return qdct;
}