// Mouse functions

void mousePressed(){
  locked=true;
  xOffset=mouseX-stageX;
  yOffset=mouseY-stageY;
}

void mouseDragged(){
  if(locked){
    stageX=mouseX-xOffset;
    stageY=mouseY-yOffset;
  }
}

void mouseReleased(){
  locked=false;
}

// Old functions-------------------------------------------------------------------------------------


// First version remnant
int[] oldDCT(int channel[], int wid, int hei, int blocksize, int values){
  int elements = wid*hei;
  int myBlock[][]= blocky(channel, elements, wid, blocksize);
  int myVector[] = vectrify(myBlock, elements, blocksize*blocksize);
  int centy[] = center(myVector, elements, values/2);
  float dc[] = dct(centy, elements, blocksize);
  int quant[] = quantize(dc, elements, blocksize);
  int turn[] = ziggy(quant, elements, blocksize);
  int turna[] = revZiggy(turn, elements, blocksize);
  int turnb[] = revQuant(turna, elements, blocksize);
  int turnc[] = revDct(turnb, elements, blocksize);
  int turnd[] = addOn(turnc,elements,values/2);
  return turnd;
}

// ----- Old encoding functions
String stringy(float number){
  String output1 = Float.toString(number);
  String output = "";
  for(int i=0; i<output1.length(); i++){
    if(output1.charAt(i)=='.'){
      output+="_";
      if(output1.length()-(i) >=3){
        output+=output1.substring(i+1,i+3);
        return output;
      }
      
    }else{
      output+=output1.charAt(i);
    }
  }
  return output;
}

// Old function, unused now
int[][] blocky(int inChannel[], int bytes, int imgWidth, int blockSize){
  int partSize = blockSize*blockSize;
  int blocks = bytes/(partSize);
  int blocky[][] = new int[blocks][partSize];
  //println(blocks+" "+partSize);
  
  int blockWidth = blocks/(imgWidth/blockSize);
  int blockHeight = blocks/blockWidth;
  
  
  for(int i=0;i<blockWidth;i++){
    for(int j=0;j<blockHeight;j++){
      for(int k=0;k<blockSize;k++){
        for(int h=0;h<blockSize;h++){
          int xstart = (i*blockSize);
          int ystart = (j*blockSize);
          blocky[(j*blockHeight)+i][(h*blockSize)+k]=inChannel[((ystart+h)*(bytes/imgWidth))+(xstart+k)];
        }
      }
    }
  }
  return blocky;
}

// Old function, unused now
int[] vectrify(int matrix[][], int elements, int elemWidth){
  int elemHeight = elements/elemWidth;
  int retVect[] = new int[elements];
  for(int i=0;i<elemHeight;i++){
    for(int j=0;j<elemWidth;j++){
      retVect[(i*elemWidth)+j] = matrix[i][j];
    }
  }
  return retVect;
}

// Old function, unused now
int[] center(int vector[], int elements, int minus){
  int returny[] = vector.clone();
  for(int i=0;i<elements;i++){
    returny[i]=returny[i]-minus;
  }
  return returny;
}

// Old function, unused now
float[] dct(int vector[], int elements, int blocksize){
  float turny[] = float(vector.clone());
  int elems = blocksize*blocksize;
  int blocks = elements/elems;
  for(int i=0;i<blocks;i++){
    for(int j=0;j<elems;j++){
      int u=j%blocksize;
      int v=floor(j/blocksize);
      float Cu = 1;
      float Cv = 1;
      if(u==0){
        Cu = 1/sqrt(2);
      }
      if(v==0){
        Cv = 1/sqrt(2);
      }
      float multip = (2*Cu*Cv)/sqrt(elems);
      // x=i, y=j...
      float summy = 0;
      for(int y=0;y<blocksize;y++){
        for(int x=0;x<blocksize;x++){
          //println(i+" "+j+" "+x+" "+y);
          summy+=cos((((2*x)+1)*(u*PI))/(2*blocksize))*cos((((2*y)+1)*(v*PI))/(2*blocksize))*vector[(i*64)+x+(y*8)];
        }
      }
      turny[(i*(blocksize*blocksize))+j]=multip*summy;
    }
  }
  return turny;
}

// Old function, unused now
int[] quantize(float vector[], int elements, int blocksize){
  int turny[] = int(vector.clone());
  for(int i=0;i<elements;i++){
    turny[i]=round(vector[i]/round(fiftyQuant[i%(blocksize*blocksize)]*comp));
  }
  return turny;
}

// Old function, unused now
int[] ziggy(int veccy[], int elements, int blocksize){
  int turny[] = veccy.clone();
  for(int i=0;i<elements/(blocksize*blocksize);i++){
    for(int j=0;j<blocksize*blocksize;j++){
      
      turny[(i*(blocksize*blocksize))+j] = veccy[((i*(blocksize*blocksize))+(zigga[j]))];
      
    }
  }
  return turny;
}
// ----- Old encoding functions end



// ----- Old decoding functions
// Old function, unused now
int [] revZiggy(int veccy[], int elements, int blocksize){
  int turny[] = veccy.clone();
  for(int i=0;i<elements/(blocksize*blocksize);i++){
    for(int j=0;j<blocksize*blocksize;j++){
      turny[(i*(blocksize*blocksize))+(zigga[j])] = veccy[(i*(blocksize*blocksize))+j];
    }
  }
  return turny;
}


// Old function, unused now
int[] revQuant(int veccy[], int elements, int blocksize){
  int turny[] = veccy.clone();
  for(int i=0;i<elements;i++){
    turny[i]=round(veccy[i]*(fiftyQuant[i%(blocksize*blocksize)])*comp);
  }
  return turny;
}

// Old function, unused now
int[] revDct(int veccy[], int elements, int blocksize){
  int turny[] = veccy.clone();
  int elems = blocksize*blocksize;
  int blocks = elements/elems;
  for(int i=0;i<blocks;i++){
    for(int j=0;j<elems;j++){
      int x=j%blocksize;
      int y=floor(j/blocksize);
      float multip = (2)/sqrt(elems);
      float summy = 0;
      for(int v=0;v<blocksize;v++){
        for(int u=0;u<blocksize;u++){
          float Cu = 1;
          float Cv = 1;
          if(u==0){
            Cu = 1/sqrt(2);
          }
          if(v==0){
            Cv = 1/sqrt(2);
          }
          summy+=Cu*Cv*cos((((2*x)+1)*(u*PI))/(2*blocksize))*cos((((2*y)+1)*(v*PI))/(2*blocksize))*veccy[(i*64)+u+(v*8)];
        }
      }
      turny[(i*64)+j]=round(summy*multip);
    }
  }
  return turny;
}

// Old function, unused now
int[] addOn(int veccy[], int elements, int plus){
  int out[] = veccy.clone();
  for(int i=0;i<elements;i++){
    out[i] = veccy[i]+plus;
  }
  return out;
}

// Old function, unused now
int[] revBlock(int veccy[], int elements, int wid, int blocksize){
  int out[] = new int[elements];
  int blocks = elements/(blocksize*blocksize);
  int blockWid = (wid/blocksize);
  for(int b=0;b<blocks;b++){
    int bx = b%blockWid;
    int by = b/blockWid;
    int startx = bx*blocksize;
    int starty = by*blocksize*wid;
    for(int i=0;i<blocksize;i++){
      for(int j=0;j<blocksize;j++){
        out[startx+starty+j+(i*wid)] = veccy[(b*(blocksize*blocksize))+(j+(i*blocksize))];
      }
    }
  }
  return out;
}
// ------- Old decoding functions end