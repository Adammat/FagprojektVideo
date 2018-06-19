
Compile files with gcc -o OUTPUT INPUT -lm (Needs math library all of them).
Encode expects a 200x200 space seperated input channel (of 256 values, in integers) and outputs the DCT coefficients in same format.
Decode takes that file and converts it back (close) to the original.
