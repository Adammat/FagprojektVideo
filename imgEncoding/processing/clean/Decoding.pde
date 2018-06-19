int[] uncompress(int channel[], int wid, int hei, int blockSize, int values){
  // setup variables for decoding
  int elements = wid*hei;
  int blockElems = blockSize*blockSize;
  int blocks = elements/blockElems;
  int blockWidth = (wid/blockSize);
  // Setup multiplier (used in the IDCT after summations)
  float multip = (2)/sqrt(blockElems);
  
  
  // Setup temp and return arrays
  float returner[] = new float[elements];
  int qdct[] = channel.clone();
  
  // Go through each block
  for(int b=0;b<blocks;b++){
    // For each element in that block
    for(int j=0;j<blockElems;j++){
      // Take the reverse zigzag values and quantize them (with the reverse zigzag quantized value * compression ratio)
      float compy = fiftyQuant[zigga[j]]*comp;
      returner[(b*(blockElems))+(zigga[j])] = channel[(b*(blockElems))+j]*compy;
    }

    // Elements in order, go through each again (could most likely be one loop with earlier, 
    // but some advantage of doing these first is also here since these calculations happens once now,
    // and not 64*64 times in each block.)
    for(int j=0;j<blockElems;j++){
      // Find x value and y value from the variable j (modulus and divided by blocksize)
      int x=j%blockSize;
      int y=floor(j/blockSize);
      // Setup a summing variable
      float summy = 0;
      // Going through each element again (for each element)
      for(int v=0;v<blockSize;v++){
        for(int u=0;u<blockSize;u++){
          // function c(u) = 1 if u>0 else is 1/root(2)
          float Cu = 1;
          float Cv = 1;
          if(u==0){
            Cu = 1/sqrt(2);
          }
          if(v==0){
            Cv = 1/sqrt(2);
          }
          // Do the summations of IDCT
          summy+=Cu*Cv*cos((((2*x)+1)*(u*PI))/(2*blockSize))*cos((((2*y)+1)*(v*PI))/(2*blockSize))*returner[(b*blockElems)+u+(v*8)];
        }
      }
      // Calculate block's x and block's y
      int bx = b%blockWidth;
      int by = b/blockWidth;
      // Level shifting and then clipping the value
      float myValue = summy*multip+(values/2);
      if(myValue<0){
        myValue=0;
      }
      if(myValue>255){
        myValue = 255;
      }
      // And finally rounding and setting into the correct position of array 
      // ((almost) original picture, assuming settings are the same)
      qdct[(by*blockSize*wid)+(bx*blockSize)+(y*wid)+x]= round(myValue);
    }
  }
  return qdct;
}