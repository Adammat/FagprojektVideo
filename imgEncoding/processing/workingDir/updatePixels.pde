void updateMyPixels(){
  fileName = ""+picWidth+"x"+picHeight+" c"+stringy(comp)+" "+frameNO+".png";
  frameNO++;
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
    midImage.pixels[i] = color(resultImage.pixels[i]);
    ginImage.pixels[i] = color(brightness);
  }
  midImage.updatePixels();
  ginImage.updatePixels();
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
  bwDiff.loadPixels();
  colDiff.loadPixels();
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
     checkAvgGray += sqrt(checkGray[i]);
     checkAvgCol += sqrt(checkCol[i]);
     resultImage.pixels[i] = color(rg[0],rg[1],rg[2]);
     gresultImage.pixels[i] = color(brightness(checky2[i]));
     bwDiff.pixels[i] = color(brightness(checkGray[i]));
     colDiff.pixels[i] = color(brightness(checkCol[i]));
  }
  resultImage.updatePixels();
  gresultImage.updatePixels();
  bwDiff.updatePixels();
  colDiff.updatePixels();
  // Final averages
  checkAvgGray1 = (float)(checkAvgGray/(picHeight*picWidth));
  //float checkAvgCol1 = checkAvgCol/(picHeight*picWidth);
  // Should be correct, if no downsampling just the three channels..
  // If 1 bit downsampled, (1/8)'th of U and V each should be removed
  checkAvgCol1 = (float)(checkAvgCol/((picWidth*picHeight*3)-(((downSampler/8)*2)*picWidth*picHeight)));
  PSNRGray = 10*log(pow(255,2)/checkAvgGray1);
  PSNRCol = 10*log(pow(255,2)/checkAvgCol1);
  
  println("Average error (gray): "+checkAvgGray1+" avg error (color) "+checkAvgCol1);
  println("PSNR (gray): "+PSNRGray+" PSNR (color) "+PSNRCol);
  // Update the pixels finally
  
  background(#FFFFFF);
  stroke(#000000);
  fill(#000000);
  //text("FPS: "+frameRate,stageX+1300,stageY-10);
  
  image(testImage,stageX+10,stageY+10); // original input
  image(ginImage, stageX+10+(10*1)+(picWidth*1),stageY+10); // gray input
  image(gresultImage,stageX+10+(2*10)+(picWidth*2),stageY+10);
  image(midImage,stageX+10+(3*10)+(picWidth*3),stageY+10); // color input
  image(resultImage,stageX+10+(4*10)+(picWidth*4),stageY+10);
  image(bwDiff,stageX+10+(0.5*picWidth)+(1*(10+picWidth)),stageY+20+picHeight);
  image(colDiff,stageX+10+(0.5*picWidth)+(3*(10+picWidth)),stageY+20+picHeight);
  text("Compression ratio: "+comp+" picsize: "+picWidth+"x"+picHeight, stageX+10+(1*picWidth)+(2*(10+picWidth)),stageY+(2*(picHeight+10))+20);
  text("Full compression bytes (color): "+(compBits/8)+" from "+((picWidth*picHeight*3)-((downSampler/8)*2*picWidth*picHeight)), stageX+10+(1*picWidth)+(2*(10+picWidth)),stageY+(2*(picHeight+10))+40);
  
  text("Average error per pixel - gray: "+(checkAvgGray1)+" color: "+(checkAvgCol1),stageX+10+(1*picWidth)+(2*(10+picWidth)),stageY+(2*(picHeight+10))+60);
  text("PSNR (gray): "+PSNRGray+" PSNR (color) "+PSNRCol,stageX+10+(1*picWidth)+(2*(10+picWidth)),stageY+(2*(picHeight+10))+90);
}