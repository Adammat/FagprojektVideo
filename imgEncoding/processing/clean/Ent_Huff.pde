// Conversion between YUV and RGB (probably needs gamma correction).
int[] yuv(int rgb[]){
  int Y = clamp((int)((0.257*rgb[0])+(0.504*rgb[1])+(0.098*rgb[2])+16));
  int V = clamp((int)((0.439*rgb[0])-(0.368*rgb[1])-(0.071*rgb[2])+128));
  int U = clamp((int)((-0.148*rgb[0])-(0.291*rgb[1])+(0.439*rgb[2])+128));
  int yuv[] = {Y,U,V};
  return yuv;
}
int[] rgb(int yuv[]){
  int B = clamp((int)((1.164*(yuv[0]-16))+(2.018*(yuv[1]-128))));
  int G = clamp((int)((1.164*(yuv[0]-16))-(0.813*(yuv[2]-128))-(0.391*(yuv[1]-128))));
  int R = clamp((int)((1.164*(yuv[0]-16))+(1.596*(yuv[2]-128))));
  int rgb[] = {R,G,B};
  return rgb;
}

// Alternative conversion
/*
int[] yuv(int rgb[]){
  int Y = clamp((int)((0.299*rgb[0])+(0.587*rgb[1])+(0.114*rgb[2])));
  int U = clamp((int)((-0.14713*rgb[0])-(0.28886*rgb[1])+(0.436*rgb[2])));
  int V = clamp((int)((0.615*rgb[0])-(0.51499*rgb[1])-(0.10001*rgb[2])));
  int yuv[] = {Y,U,V};
  return yuv;
}
int[] rgb(int yuv[]){
  int R = clamp((int)((1*yuv[0])+(1.13983*yuv[2])));
  int G = clamp((int)((1*yuv[0])-(0.39465*yuv[1])-(0.58060*yuv[2])));
  int B = clamp((int)((1*yuv[0])+(2.03211*yuv[1])));
  int rgb[] = {R,G,B};
  return rgb;
}
*/

// Helper function to clamp a value between 0 and 255 
//(used in YUV only, could be rewritten and used for en-/decoding also)
int clamp(int value){
  if(value<0){
    value=0;
  }else if(value>255){
    value=255;
  }
  return value;
}


// Class/structure to output everything from the same function
class huffmanOutput{
  HuffmanNode DC; // Huffmantree for dc
  HuffmanNode AC; // huffmantree for AC
  String output; // bitstring..
}


// Function to do huffman-encoding
// Expects an array with size divisible by 64 (which will be the only sizes output from DCT).
huffmanOutput huffEncode(int channel[], int wid, int hei, int blocksize){
  // Setup arrays (and size) to store values
  int blocks = (wid*hei)/(blocksize*blocksize);
  int acValues[] = new int[(wid*hei)-blocks];
  int dcDPCMValues[] = new int[blocks];
  // Variable to make predictive coding of DC values easier (DPCM)
  int lastDC = 0;
  
  // Go through every value in input and..
  for(int i=0;i<wid*hei;i++){
    // Take every 64th value starting in 0 as DC value
    if(i%(blocksize*blocksize)==0){
      // Calculate DPCM (predictive encoding) for DC values
      dcDPCMValues[i/(blocksize*blocksize)] = channel[i]-lastDC;
      lastDC = channel[i];  
    }else{ // and every 1-63 indexes from each block (the rest) as AC values
      acValues[i-(i/(blocksize*blocksize))-1] = channel[i];
    }
    
  }
  
  // Find bitlengths of the DPCM'ed (predicted) dc values and store in an array
  int dcDPCM_bit_lengths[] = new int[dcDPCMValues.length];
  for(int i=0;i<dcDPCM_bit_lengths.length;i++){
    dcDPCM_bit_lengths[i] = additionalBits(dcDPCMValues[i]).length();
  }
  
  // Find frequencies of entries and output {freqA, valA, freqB, valB ...}
  int dcCheck[] = huffCheck(dcDPCM_bit_lengths);
  
  // Make a huffmantree from the frequencies
  HuffmanNode dcRoot = makeHuffTree(dcCheck, dcCheck.length);
  
  // Reorder that tree to be compatible with the standard
  HuffmanNode dc1Root = reorderTree(dcRoot);
  
  // Make arraylists and intlists for easy access to the values and bitsequences
  ArrayList<String> bits;
  bits = new ArrayList<String>();
  IntList value;
  value = new IntList();
  
  // Fill up the arrays with leafnodes from the huffmantree
  printCode(dc1Root, "", value, bits);
  
  // Printing tables to console
  if(printTables){
    println("Huffman tables (DC) start ------------------------------");
    // Make a copy to sort according to bitlengths to output
    IntList valueOut = new IntList();
    ArrayList<String> bitsOut = new ArrayList<String>();
    for(int i=0;i<bits.size();i++){
      bitsOut.add(bits.get(i));
    }
    Collections.sort(bitsOut, new MyStringComparator());
    
    for(int i=0; i<bits.size(); i++){
      int index = bits.indexOf(bitsOut.get(i));
      valueOut.append(value.get(index));
      println(i+": "+value.get(index)+" - \t"+bitsOut.get(i)+"\t\t"+valueOfBits(bitsOut.get(i)));
    }
   println("Huffman tables (DC) end ------------------------------");
  }
  
  // Make an array of strings to store the DC out bitstrings for each block
  // (and a long string for quick checking)
  String[] huffedDC = new String[dcDPCM_bit_lengths.length];
  String huffedDCString = "";
  for(int i=0; i<huffedDC.length; i++){
    int index = 0;
    // Finding the bitcode for the bitlength of value in index i
    for(int j=0;j<value.size();j++){
      if(value.get(j) == dcDPCM_bit_lengths[i]){
        index = j;
        break;
      }
    }
    // Adding huffman bitcode and if a nonzero value the one's compliment of the value
    // additionalBits returns an empty string if value given is 0
    huffedDC[i] = bits.get(index)+additionalBits(dcDPCMValues[i]);
    huffedDCString += bits.get(index);
    huffedDCString += additionalBits(dcDPCMValues[i]);
  }
  // console output quick check
  println("Bits DC Huffed: "+huffedDCString.length()+", from "+dcDPCMValues.length+" values (~bytes), huffedDC.length: "+huffedDC.length);
  // Print first 64 bits of dc values if printtests set
  if(printTests){
    for(int i=0;i<64;i++){
      print(huffedDCString.charAt(i));
    }
    println();
  }
  
  //!!!! AC values...--------------------------------------------------------------------------------------------
  
  // Get bit lengths of ac values..
  int ac_bit_lengths[] = new int[acValues.length];
  for(int i=0;i<ac_bit_lengths.length;i++){
    ac_bit_lengths[i] = additionalBits(acValues[i]).length();
  }
  
  // Setup lists to store the runlength codes and non-zero values
  IntList acCodes = new IntList();
  IntList acNZValues = new IntList();
  
  // Go through each block..
  for(int i=0; i<blocks;i++){
    // Go through each entry in that block
    // Setup variables to count zeros and how many entries added for a block
    int zeros = 0, entryAdded = 0;
    for(int j=0; j<(blocksize*blocksize)-1; j++){
      // Going through each element of a block, first check if enough entries has been added, if not break
      if(entryAdded<(blocksize*blocksize)){
        // If the entry at this index is a zero, begin counting
        if(ac_bit_lengths[i*((blocksize*blocksize)-1)+j]==0){
          zeros++;
          // If 16 zeros has been found in a row, check if there are any more non-zero values in that block
          if(zeros==16){
            boolean moreValues = false;
            for(int k=j+1; k<((blocksize*blocksize)-1); k++){
              if(ac_bit_lengths[(i*((blocksize*blocksize)-1))+k]!=0){
                moreValues = true;
                break;
              }
            }
            // If there ARE more non-zero values, append the code 240 (15*16 / F0) 
            // and put 16 values added, reset zero counter
            if(moreValues){
                acCodes.append(240);
                entryAdded+=16;
                zeros = 0;
            // If no more non-zero values append EOB (code 0) and end the block
            }else{ // end of block
              acCodes.append(0);
              entryAdded = blocksize*blocksize;
            }
          }//if zeros=16 end
        }else{// If bitlength[i] not equal zero:
        // Append the runlength (huffman) code and the nonzero value to their respective lists and advance
          acCodes.append(zeros*16+ac_bit_lengths[(i*((blocksize*blocksize)-1))+j]);
          acNZValues.append(acValues[(i*((blocksize*blocksize)-1))+j]);
          entryAdded++;
          zeros=0;
        }
        // A check of the last value
        if(j==((blocksize*blocksize)-1)-1 && ac_bit_lengths[(i*((blocksize*blocksize)-1))+j]==0 && acCodes.get(acCodes.size()-1)!=0){
          acCodes.append(0);
          entryAdded++;
        }
      }else{ // End of list check
        break;
      }
    }// For j entry in block
  } // for i block
  
  // If error from underflow might be needed to add a value (overkill for test)
  /*for(int i=0;i<100;i++){
    acNZValues.append(0);
  }*/
  println("acCodes.size() ="+acCodes.size()+" acNZValues.size()= "+acNZValues.size());
   
   // Convert the huffman-runlength codes into an array to do frequency checks
  int acCodeArray[] = acCodes.array();
  int acCheck[] = huffCheck(acCodeArray);
  // Build and reorder the tree
  HuffmanNode acRoot = makeHuffTree(acCheck,acCheck.length);
  HuffmanNode ac1Root = reorderTree(acRoot);
  
  // Setup arrays
  ArrayList<String> acbits;
  acbits = new ArrayList<String>();
  IntList acvalue;
  acvalue = new IntList();
  
  // Fill up the arrays with leafnodes from the huffmantree
  printCode(ac1Root, "", acvalue, acbits);
  
  // Print sorted huffman table
  if(printTables){
    println("Huffman tables (AC) start ------------------------------");
    IntList acvalueOut = new IntList();
    ArrayList<String> acbitsOut = new ArrayList<String>();
    for(int i=0; i<acbits.size();i++){
      acbitsOut.add(acbits.get(i));
    }
    Collections.sort(acbitsOut, new MyStringComparator());
    for(int i=0;i<acbits.size();i++){
      int index = acbits.indexOf(acbitsOut.get(i));
      acvalueOut.append(acvalue.get(index));
      println(i+": "+acvalue.get(index)+" - "+acbitsOut.get(i)+"\t"+valueOfBits(acbitsOut.get(i)));
    }
    println("Huffman tables (AC) end ------------------------------");
  }
  
  
  // Collect the bitstrings for each block
  String acStrings[] = new String[blocks];
  acStrings[0]= "";
  String huffedACString = "";
  
  // Variables to check if a whole block added and where to add the strings
  int stringIndex = 0;
  int valuesAdded = 0;
  
  // While there are still runlength codes left
  while(acCodes.size()>0){
    int index = 0;
    // Find the index in Huffman tree for that code
    for(int j=0;j<acvalue.size();j++){
      if(acvalue.get(j) == acCodes.get(0)){
        index = j;
        break;
      }
    }
    // If EOB, encode that and advance the block
    if(acCodes.get(0)==0){
      acStrings[stringIndex]+=acbits.get(index);
      huffedACString+=acbits.get(index);
      valuesAdded=(blocksize*blocksize);
    }else if(acCodes.get(0)==15*16){
      // if ZRL insert a ZRL and advance the string
      acStrings[stringIndex]+=acbits.get(index);
      huffedACString+=acbits.get(index);
      valuesAdded+=16;
    }else{
      // If not EOB or ZRL add the runlength and the one's compliment of the next non-zero value
      acStrings[stringIndex] += ""+acbits.get(index)+""+additionalBits(acNZValues.get(0));
      huffedACString+= ""+acbits.get(index)+""+additionalBits(acNZValues.get(0));
      // Remove that nonzero value.
      acNZValues.remove(0);
      // Advance according to how many leading zeros that value had.
      valuesAdded+=1+floor(acCodes.get(0)/16);
    }
    // If a full block has been reached
    if(valuesAdded>=(blocksize*blocksize)-1){
      // Advance block index, reset valuesAdded variable and empty next string
      valuesAdded=0;
      stringIndex++;
      if(stringIndex<blocks){
        acStrings[stringIndex] = "";
      }
    }
    // Remove the code read and worked on
    acCodes.remove(0);
  }
  
  // Some console printing info
  println("huffedACString.length(): "+huffedACString.length());
  println("Bits AC Huffed: "+huffedACString.length()+", from "+acValues.length+" values (~bytes)");
  if(printTests){
    for(int i=0;i<64;i++){
      print(huffedACString.charAt(i));
    }
    println();
  }
  
  
  //!!!! AC Values end...--------------------------------------------------------------------------------------------
  
  // Collecting AC and DC values:
  String huffedString = "";
  for(int i=0; i<blocks; i++){
    huffedString+= huffedDC[i]+""+acStrings[i];
  }
  /*println("huffedString.length(): "+huffedString.length()+" (bits), "+(huffedString.length()/8.0)+" bytes from "+channel.length+" values (~bytes)");
  println(huffedString.substring(0,128)); */
  
  // Collecting for output
  huffmanOutput outy = new huffmanOutput();
  outy.DC = dc1Root;
  outy.AC = ac1Root;
  outy.output = huffedString;
  return outy;
}





int[] huffDecode(String huffedString, HuffmanNode dcRoot, HuffmanNode acRoot, int wid, int hei, int blocksize){
  int atChar = 0;
  int blocks = (wid*hei)/(blocksize*blocksize);
  int[] checkAssembled = new int[wid*hei];
  int lastDC = 0;
  for(int i=0; i<blocks; i++){
    HuffmanNode checkDC = dcRoot;
    int foundValues = 0;
    while(checkDC.left!=null && checkDC.right!=null){
      if(huffedString.charAt(atChar)=='0'){
        checkDC = checkDC.left;
      }else{
        checkDC = checkDC.right;
      }
      atChar++;
    }
    int bitsToRead = checkDC.data;
    String assembly = "";
    for(int j=0;j<bitsToRead;j++){
      assembly+=huffedString.charAt(atChar);
      atChar++;
    }
    checkAssembled[i*(blocksize*blocksize)+foundValues] = svalueOfBits(assembly)+lastDC;
    lastDC = checkAssembled[i*(blocksize*blocksize)+foundValues];
    foundValues++;
    //DC value done
    while(foundValues<(blocksize*blocksize)){
      HuffmanNode checkAC = acRoot;
      while(checkAC.left!=null && checkAC.right!=null){
        if(huffedString.charAt(atChar)=='0'){
          checkAC = checkAC.left;
        }else{
          checkAC = checkAC.right;
        }
        atChar++;
      }
      bitsToRead = checkAC.data%16;
      int addZeros = floor(checkAC.data/16);
      if(addZeros == 0 && bitsToRead == 0){
        while(foundValues<(blocksize*blocksize)){
          checkAssembled[i*(blocksize*blocksize)+foundValues] = 0;
          foundValues++;
        }
      }else{
        assembly = "";
        for(int j=0;j<bitsToRead;j++){
          assembly+=huffedString.charAt(atChar);
          atChar++;
        }
        for(int j=0;j<addZeros;j++){
          checkAssembled[i*(blocksize*blocksize)+foundValues] = 0;
          foundValues++;
        }
        checkAssembled[i*(blocksize*blocksize)+foundValues] = svalueOfBits(assembly);
        foundValues++;
      }
    }
  }
  return checkAssembled;
}







// Class/structure of a node
class HuffmanNode{
  int frequency;
  int data;
  HuffmanNode left;
  HuffmanNode right;
}

// Function  to recursively build up bitstrings from a treeroot and put them in given lists
void printCode(HuffmanNode root, String s, IntList values, ArrayList<String> bitcodes){
  // If a leafnode is found append the value(code) and bitcode and return
  if(root.left == null && root.right == null){
    values.append(root.data);
    bitcodes.add(s);
    return;
  }else{ // If not recursively call itself, building up the bitstring
    printCode(root.left, s+"0", values, bitcodes);
    printCode(root.right, s+"1", values, bitcodes);
  }
}

// Helper class to compare frequency
class MyComparator implements Comparator<HuffmanNode>{
  public int compare(HuffmanNode x, HuffmanNode y){
    return x.frequency - y.frequency;
  }
}

// Helper class for simple sorting (IntList)
class MyDataComparator implements java.util.Comparator<Integer>{
  public int compare(Integer x, Integer y){
    return x - y;
  }
}

// Helper class for sorting according to length
class MyStringComparator implements java.util.Comparator<String>{
  public int compare(String s1, String s2){
    return s1.length()-s2.length();
  }
}



// Function to return the value of a (1's compliment) bitstring
int svalueOfBits(String bitty){
  // Setup summing/return variable
  int sum = 0;
  // Check that length is over 0 (not empty string)
  if(bitty.length()>0){
    // Setup variable calculate powers
    int start = bitty.length()-1;
    // If first character is a zero, it is a negative value and the bitstring is inverted, added up and then negated
    if(bitty.charAt(0)=='0'){
      for(int i=0;i<bitty.length();i++){
        if(bitty.charAt(i)=='0'){
          sum += pow(2,(start-i));
        }
      }
      sum=-sum;
    }else{// If the first character is a one, it is a positive value and can be summed up as usual.
      for(int i=0;i<bitty.length();i++){
        if(bitty.charAt(i)=='1'){
          sum += pow(2,(start-i));
        }
      }
    }
  }
  return sum;
}

// Function to return (normal) bitvalue of a string (not 1's compliment)
int valueOfBits(String bitty){
  int sum = 0;
  int start = bitty.length()-1;
  for(int i=0;i<bitty.length();i++){
    if(bitty.charAt(i)=='1'){
      sum += pow(2,(start-i));
    }
  }
  return sum;
}

// Two helper functions to calculate ceiling(log2(x)) and floor of same
int ceilLog2(int value){
  return ceil(log(value)/log(2));
}
int floorLog2(int value){
  return floor(log(value)/log(2));
}

// Function to return the bitstring of a value
String additionalBits(int value){
  String returner = "";
  if(value==0){
     //Return empty string
  }else{
    // Setup holder variables
    int temp = value;
    int tempMax = value;
    if(value<0){
      tempMax = -value;
      temp=-value;
    }
    // For loop going from 0 to bitlength
    for(int i=0; i<floorLog2(tempMax)+1;i++){
      // If temporary value is still bigger than the value of that bit (2^x) remove that value from temp.
      // If the incoming value was negative, invert the output string
      if(temp>=pow(2,floorLog2(tempMax)-i)){
        temp-=pow(2,floorLog2(tempMax)-i);
        if(value<0){
          returner+="0";
        }else{
          returner+="1";
        }
      }else{// If (2^x) can't be subtracted, still invert the bitstring for negative incoming value
        if(value<0){
          returner+="1";
        }else{
          returner+="0";
        }
      }
    }
  }
  // Return the bitstring built
  return returner;
}


HuffmanNode reorderTree(HuffmanNode inputNode){
  
  // Setup lists and fill them up with codes from the given tree node
  ArrayList<String> bits = new ArrayList<String>();
  IntList values = new IntList();
  printCode(inputNode, "" , values, bits);
  
  
  // Setup copies to sort
  ArrayList<String> bitsOut = new ArrayList<String>();
  ArrayList<Integer> valuesOut = new ArrayList<Integer>();
  for(int i=0;i<values.size();i++){
    valuesOut.add(values.get(i));
  }
  Collections.sort(valuesOut, new MyDataComparator());
  for(int i=0; i<valuesOut.size(); i++){
    int index = 0;
    for(int j=0; j<values.size();j++){
      if(values.get(j) == valuesOut.get(i)){
        index = j;
      }
    }
    bitsOut.add(bits.get(index));
  }
  
  // Convert the output into a format compatible with JPEG headers
  // e.g. an array lengths[i] where i is the bitlength and the value it holds is the number of codes that bitlength
  // holds.
  int valuesArray[] = new int[bitsOut.size()];
  int lengths[] = new int[17];
  
  // Setup some checks to fill up the codes (valuesArray) and the longest bitlength to shorten next step a little
  int valueFound = 0;
  int longest = 0;
  // For each entry in bitlength array
  for(int i=0; i<17;i++){
    int count = 0;
    // For each value in the sorted (by bitlength and value) list, check if the bitlength matches
    for(int j=0; j<valuesArray.length; j++){
      if(bitsOut.get(j).length() == i){
        // if so, add it as next entry in valuesArray (codes) and increase counters
        valuesArray[valueFound] = valuesOut.get(j);
        valueFound++;
        count++;
      }
    }
    if(count!=0){
      longest = i;
    }
    // Record the number of occurences with a bitlength of "i" in lengths[i]
    lengths[i] = count;
  }
  
  
  // Make the codes in a rising manner:
  // Beginning in code 0 (create variables and array for output and check)
  int code = 0;
  String codes[] = new String[valuesArray.length];
  int found = 0;
  // Starting in bitlength 0 since we don't care if there is no bitlengths, going to the longest bitlength found
  for(int i=1; i<=longest; i++){
    // Go through each element for this bitlength
    for(int j=0; j<lengths[i];j++){
      // Add the binary string for that code
      codes[found] = additionalBits(code);
      // Add leading zeros if necessary to get to i bitlength
      for(int k=codes[found].length(); k<i; k++){
        codes[found] = "0"+codes[found];
      }
      // Increase code and found variables
      code++;
      found++;
    }
    // Moving down a level in the tree, bitshift one to the right
    code = code<<1;
  }
  
  // Build a new huffmantree from the codes made
  // Make a root
  HuffmanNode newRoot = new HuffmanNode();
  
  
  // Go through each of the entries
  for(int i=0; i<codes.length; i++){
    // Make a copy of the root to traverse the tree, and a new node + put the code for this entry in it
    HuffmanNode atRoot = newRoot;
    HuffmanNode thisNode = new HuffmanNode();
    thisNode.data = valuesArray[i];
    
    // for each bit in the bitstring
    for(int j=0; j<codes[i].length();j++){
      // If this is the last bit in the bitstring add the leafnode accordingly (0 = left, 1 = right)
      if(j==codes[i].length()-1){
        if(codes[i].charAt(j)=='0'){
          atRoot.left = thisNode;
        }else{
          atRoot.right = thisNode;
        }
      }else{
        // If it's not an end-bit traverse the tree and add new inner nodes if necessary
        if(codes[i].charAt(j)=='0'){
          if(atRoot.left == null){// adding inner nodes (left,0)
            HuffmanNode newNode = new HuffmanNode();
            atRoot.left = newNode;
            atRoot = newNode;
          }else{// Traversing tree (left,0)
            atRoot = atRoot.left;
          }
        }else{
          if(atRoot.right == null){ // Adding inner nodes (right,1)
            HuffmanNode newNode = new HuffmanNode();
            atRoot.right = newNode;
            atRoot = newNode;
          }else{ // traversing tree (right,1)
            atRoot = atRoot.right;
          }
        }
      }
    }
  }
  return newRoot;
}

HuffmanNode makeHuffTree(int input[], int inputsize){
  // Recieves input from huffCheck() method, it will be ordered as (freq),(val),(freq),(val) ...
  int nodes = inputsize/2;
  // Create a priority queue (min-heap) for the nodes.
  PriorityQueue<HuffmanNode> tree = new PriorityQueue<HuffmanNode>(nodes, new MyComparator());
  // For each second value make a node and add those data to them, add them to the min-heap (based on frequency)
  for(int i=0;i<inputsize;i++){
    HuffmanNode addNode = new HuffmanNode();
    if(i%2==0){
      addNode.data = input[i+1];
      addNode.frequency = input[i];
      addNode.left = null;
      addNode.right = null;
      tree.add(addNode);
    }
  }
  
  // Make a node object to work on and return
  HuffmanNode root = null;
  
  // Keep polling the lowest 2 frequencies, collecting them into a new node totalling 
  // the sum of frequencies of daughternodes untill the priority queue has only one
  // node left. Return that node (the root).
  while(tree.size()>1){
    // Extract minimum values
    HuffmanNode x = tree.peek();
    tree.poll();
    HuffmanNode y = tree.peek();
    tree.poll();
    // New node that equals sum of frequencies x and y
    HuffmanNode newNode = new HuffmanNode();
    newNode.frequency = x.frequency + y.frequency;
    newNode.left = x;
    newNode.right = y;
    root = newNode;
    tree.add(root);
  }
  return root;
}



// Function to find unique occurences and frequencies of an input vector
int[] huffCheck(int veccy[]){
  // Setup lists for checking
  IntList output = new IntList();
  IntList input = new IntList();
  for(int i=0; i<veccy.length; i++){
    input.append(veccy[i]);
  }
  // Sort the (listed) input
  input.sort();
  // While there are more values left
  while(input.size()>0){
    // Take out first value and set a frequency counter
    int checkValue = input.get(0);
    int frequency = 1;
    // While the next value is the same as the last, keep taking out values and count up frequency
    while(input.min()==checkValue){
      input.remove(0);
      frequency++;
      // Making sure to break out so as not to invoke min() on an empty list
      if(input.size()==0){break;}
    }
    // Append the value and frequency to output list
    output.append(frequency);
    output.append(checkValue);
  }
  
  // Convert to array and return
  return output.array();
}