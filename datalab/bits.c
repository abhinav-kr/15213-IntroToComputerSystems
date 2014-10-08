/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* 
 * evenBits - return word with all even-numbered bits set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */
int evenBits(void) {

/* 85 is equivalent to 0x55 (0000 ... 0101 0101). Replicating this pattern by ORing it to other parts of the word. */
    
  int a = 85;
  int b = a;
  b = (b<<8) + a;
  a = b;
  b = (b<<16) + a;
  return b;
}
/* 
 * isEqual - return 1 if x == y, and 0 otherwise 
 *   Examples: isEqual(5,5) = 1, isEqual(4,5) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int isEqual(int x, int y) {
    
/* XORing X & Y will reveal if there are any differences in bits */
    
  return !(x^y);
}
/* 
 * byteSwap - swaps the nth byte and the mth byte
 *  Examples: byteSwap(0x12345678, 1, 3) = 0x56341278
 *            byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
 *  You may assume that 0 <= n <= 3, 0 <= m <= 3
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 2
 */
int byteSwap(int x, int n, int m) {
    
/* Extracting the word into 3 different parts - 2 bytes that need to be swapped and the rest of the word.
   Swapping the extracted bytes by ORing them to required positions in the original word */
    
    int mthByteMask = 0x000000FF;
    int nthByteMask = 0x000000FF;
    int mthByte, nthByte, mthByteInNthPlace, nthByteInMthPlace;
    mthByteMask = mthByteMask << (m<<3);
    nthByteMask = nthByteMask << (n<<3);
    
    mthByte = ((x & mthByteMask) >> (m<<3)) & 0xFF;
    nthByte = ((x & nthByteMask) >> (n<<3)) & 0xFF;
    
    mthByteInNthPlace = mthByte << (n<<3);
    nthByteInMthPlace = nthByte << (m<<3);
    
    x = (x & ~(mthByteMask | nthByteMask)) | mthByteInNthPlace | nthByteInMthPlace;
    
	return (x);
}
/* 
 * rotateRight - Rotate x to the right by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateRight(0x87654321,4) = 0x18765432
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 25
 *   Rating: 3 
 */

int rotateRight (int x, int n) {
    
/* Generating the mask that will push the required set of bits to the other end. */
 
	int a = 0x7f;
	int mask = a;
	int bitsPushedOut = 0;
	
	mask = (mask<<8)+a;
	a = 0xFF;
	mask = (mask<<8)+a;
	mask = (mask<<8)+a;
	mask = (mask<<8)+a;
	
	mask = mask >> (31+(~n+1));
    
	bitsPushedOut = x & mask;
	bitsPushedOut = (bitsPushedOut << (32+(~n+1)));
    
	x = x >> n;
	x = x & ~(mask << (32+(~n+1)));
    
	x = x | bitsPushedOut;
	return x;
}

/* 
 * logicalNeg - implement the ! operator using any of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
    
/* The requirement here is to be able to OR all the bits within the given word to see if any of the bit is 1. 
    This technique was taught by one of my professors during my undergrad. */
    
    x = (x | (x>>16));
    x = (x | (x>>8));
    x = (x | (x>>4));
    x = (x | (x>>2));
    x = (x | (x>>1));
    
    return (~x&1);
}
/* 
 * TMax - return maximum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmax(void) {
/* Need to generate 0x7FFFFFFF*/
    
  return ~(1<<31);
}

/* 
 * sign - return 1 if positive, 0 if zero, and -1 if negative
 *  Examples: sign(130) = 1
 *            sign(-23) = -1
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 10
 *  Rating: 2
 */
int sign(int x) {
    
    /* Using the sign bit and setting up the required conditions */
    
    int result = 0;
    int negResult;
    result = (x >> 31);
    negResult = (((~x) >> 31)&1) & (~(!x) &1);
    
    return result^negResult;
}
/*
 * isGreater - if x > y  then return 1, else return 0 
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isGreater(int x, int y) {
    
    /* Subtract y from x and see if the result is positive or negative.
     Here if overflow occurs, one of the inputs will be negative and the other input will be greater */
    
    int result = 0;
    
    int xSignBit = (x>>31)&1;
    int negYSignBit = !(y>>31)&1;
    
    int difference = x+(~y+1);
    int differenceSignBit = (difference>>31)&1;
    
    int sameSign = !(xSignBit^negYSignBit);
    int overflowHappened = (sameSign & ((xSignBit^differenceSignBit)));
    
    int overflowHappenedAndXIsPositive = overflowHappened & !xSignBit;
    
    result =(!!difference) & ((!overflowHappened & !differenceSignBit) | overflowHappenedAndXIsPositive );

    
    return result;
}

/* 
 * subOK - Determine if can compute x-y without overflow
 *   Example: subOK(0x80000000,0x80000000) = 1,
 *            subOK(0x80000000,0x70000000) = 0, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int subOK(int x, int y) {
    
    /* Overflow occurs when the sign of the resultant sum is not the same as the inputs. And if the inputs,
     that are going to be added have different signs, overflow will not occur. Applying this logic here. */
    
    int negY = ~y;
    int difference = x+negY+1;
    
    int result = 0;
    int differenceSignBit = (difference>>31)&1;
    int xSignBit = (x>>31)&1;
    int negYSignBit = (negY>>31)&1;
    
    int areTheTwoSignsDifferent = xSignBit^negYSignBit;
    
    int differenceSignBitAndXSignBitAreSame = !(differenceSignBit^xSignBit);
    int differenceSignBitAndYSignBitAreSame = !(differenceSignBit^negYSignBit);
    
    result = areTheTwoSignsDifferent | (differenceSignBitAndXSignBitAreSame & differenceSignBitAndYSignBitAreSame);
    
    return result;
}

/*
 * satAdd - adds two numbers but when positive overflow occurs, returns
 *          maximum possible value, and when negative overflow occurs,
 *          it returns minimum possible value.
 *   Examples: satAdd(0x40000000,0x40000000) = 0x7fffffff
 *             satAdd(0x80000000,0xffffffff) = 0x80000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 4
 */
int satAdd(int x, int y) {
  
    /* Logic involved is similar to previous problem. Setting the right values when overflow occurs. */
    
    int sum = x+y;
    
    int xSignBit = (x>>31)&1;
    int ySignBit = (y>>31)&1;
    int sumSignBit = (sum>>31)&1;
    
    int sameSign = !(xSignBit^ySignBit);
    int overflowHappened = (sameSign & (xSignBit^sumSignBit));
    
    int wordWithOnlyMSB1 = (1<<31);
    int wordWithAllBitsAsXSignBits = (((!xSignBit)<<31)>>31);
    int wordWithAllBitsAsOverflowResult = (overflowHappened<<31)>>31;
    int result;
    
    int sumIsTheResult = !(overflowHappened); //Will make result zero if 1
    
    sum = sum & ((sumIsTheResult<<31)>>31);
    
    result = (((( wordWithAllBitsAsOverflowResult & wordWithAllBitsAsXSignBits)^wordWithOnlyMSB1))& (((!sumIsTheResult)<<31)>>31)) | sum;
    
    return result;
}

/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
    
    /* Converting the value into a positive value, without losing the no of bits required to represent it.
       Performing binary search by choosing the 2 halves of the next stage depending on the half where the first '1' resides. */
    
    int xIsZeroMask = (!x<<31)>>31;
    int xIsMinusOneMask = (!(~x)<<31)>>31;
    
    int wordWithAllSignBits = (x>>31);
    int positiveRepresentationOfX = x^wordWithAllSignBits;
    int count = 0;
    
    int firstSixteen, secondSixteen, wordWithFirstSixteenIsZeroBits, nextStage16Bits, firstEight, secondEight;
    int wordWithFirstEightIsZeroBits, nextStage8Bits, firstFour, secondFour, wordWithFirstFourIsZeroBits, nextStage4Bits;
    int firstTwo, secondTwo, wordWithFirstTwoIsZeroBits, nextStage2Bits, firstBit, wordWithfirstBitIsZero;
    
    x = positiveRepresentationOfX;
    
    firstSixteen = (x & (( 0xFF << 24) | ( 0xFF << 16))) >> 16;
    secondSixteen = x & ( 0xFF | 0xFF<<8);
    wordWithFirstSixteenIsZeroBits = ((!firstSixteen)<<31)>>31;
    count = count + (~wordWithFirstSixteenIsZeroBits & 16);
    
    nextStage16Bits = ((wordWithFirstSixteenIsZeroBits & secondSixteen) | ((~wordWithFirstSixteenIsZeroBits) & firstSixteen));
    
    firstEight = (nextStage16Bits & (0xFF<<8) ) >>8;
    secondEight = (nextStage16Bits & (0xFF));
    wordWithFirstEightIsZeroBits = ((!firstEight)<<31)>>31;
    count = count + (~wordWithFirstEightIsZeroBits & 8);
    
    nextStage8Bits = ((wordWithFirstEightIsZeroBits & secondEight) | (~wordWithFirstEightIsZeroBits & firstEight));
    
    firstFour = (nextStage8Bits & (0xF0)) >> 4;
    secondFour = (nextStage8Bits & (0x0F));
    wordWithFirstFourIsZeroBits = ((!firstFour)<<31)>>31;
    count = count + (~wordWithFirstFourIsZeroBits & 4);
    
    nextStage4Bits = ((wordWithFirstFourIsZeroBits & secondFour) | (~wordWithFirstFourIsZeroBits & firstFour));
    
    firstTwo = (nextStage4Bits & 0xC) >> 2;
    secondTwo = (nextStage4Bits & 0x3);
    
    wordWithFirstTwoIsZeroBits = ((!firstTwo)<<31)>>31;
    
    count = count + (~wordWithFirstTwoIsZeroBits  & 2);
    
    nextStage2Bits = ((wordWithFirstTwoIsZeroBits & secondTwo) | (~wordWithFirstTwoIsZeroBits & firstTwo));
    
    firstBit = (nextStage2Bits & 0x2) >> 1;
    wordWithfirstBitIsZero = (!firstBit<<31)>>31;
    
    count = (~(xIsZeroMask | xIsMinusOneMask) & (count + (~wordWithfirstBitIsZero & 1) + 2)) | ((xIsZeroMask | xIsMinusOneMask)&1) ;
    
    return count;
}

/* 
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_half(unsigned uf) {
    
    /* Essentially right shifting frac in case of denormalized values. Reducing the exponent in case of Normalized values.
       Handling conrner cases and rounding off to the nerest even. */
    
    int msbAs1 = 1<<31;
    
    int sign = msbAs1 & uf;
    
    int expMask = 0xFF<<23;
    int exp = uf & expMask;
    
    int fracMask = ~(msbAs1 >> 8);
    int frac = uf & fracMask;
    
    int expValue, shouldValueBeRounded;
    
    if (!(expMask^exp)) {
        return uf;
    }
    
    expValue = exp>>23;
    
    shouldValueBeRounded = !(((frac & (0x3))^(0x3)));
    
    if ( !(expValue - 1) ) {
        uf = 0;
        uf = exp | frac;
        uf = (uf >> 1) + shouldValueBeRounded;
        uf = sign | uf;
        return uf;
        
    }
    
    if ( exp ) {
        exp = (expValue - 1)<<23;
        exp = exp & expMask;
    }
    
    if ( !exp ) {
        frac = (frac >> 1) + shouldValueBeRounded;
    }
    uf = sign | exp | frac;
    
    return uf;
}
/* 
 * float_f2i - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int float_f2i(unsigned uf) {
    
    /* Using the mathematical formula - Result = ((−1)^s × M × 2^E). */
    
    int sign = uf>>31;
    
    int expMask = 0xFF<<23;
    int exp = uf & expMask;
    
    int fracMask = ~((1<<31) >> 8);
    int frac = uf & fracMask;
    
    int biasForSinglePrecisionRepresentation = 127;
    int noOfBitsInFrac = 23;
    int expValue = exp>>noOfBitsInFrac;
    int powerAtWhichTwoShouldBeRaisedTo;
    
    expValue = expValue - biasForSinglePrecisionRepresentation;
    
    
    // will be of the form "0.something" which is 0 when converted to integer.
    if ( expValue < 0 ) {
        return 0;
    }
    
    // 23+8 = 31.
    // So there is no space for more than 31 bits.
    
    if ( expValue >= 31 ) {
        return 0x80000000u;
    }
    
    
    //product = (sign) * (2^expValue) * (((frac) * (2^-23)+1)) = (sign) * (frac + 2^23) * (2^(expValue-23))
    
    // (frac + 2^23)
    frac = frac | (1<<noOfBitsInFrac);
    
    powerAtWhichTwoShouldBeRaisedTo = (expValue-noOfBitsInFrac);
    
    if ( powerAtWhichTwoShouldBeRaisedTo >= 0 ) {
        frac = (frac << (powerAtWhichTwoShouldBeRaisedTo));
    } else {
        frac = (frac >> (-powerAtWhichTwoShouldBeRaisedTo));
    }
    
    
    if ( sign==1 ) {
        frac = -frac;
    }
    
    return frac;

}
