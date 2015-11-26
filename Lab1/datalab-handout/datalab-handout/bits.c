/*
 * CS:APP Data Lab
 *
 * <Ji Junseop, 20150739>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
` *
` * WARNING: Do not include the <stdio.h> header; it confuses the dlc
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
 * bitNor - ~(x|y) using only ~ and &
 *   Example: bitNor(0x6, 0x5) = 0xFFFFFFF8
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */
int bitNor(int x, int y) {
  /* ~(x | y) = ~x & ~y */
  x = ~x;
  y = ~y;

  return x & y;

}

/*
 * copyLSB - set all bits of result to least significant bit of x
 *   Example: copyLSB(5) = 0xFFFFFFFF, copyLSB(6) = 0x00000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int copyLSB(int x) {
  /* uses arithmetic shift to copy LSB */
  x = x << 31;

  return x >> 31;
}

/*
 * isEqual - return 1 if x == y, and 0 otherwise
 *   Examples: isEqual(5,5) = 1, isEqual(4,5) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int isEqual(int x, int y) {
  /* we do XOR to check if x and y are completely same or not */
  /* we use ! to make result 1 or 0 */
  int z = x ^ y;

  return !z;
}

/*
 * bitMask - Generate a mask consisting of all 1's
 *   lowbit and highbit
 *   Examples: bitMask(5,3) = 0x38
 *   Assume 0 <= lowbit <= 31, and 0 <= highbit <= 31
 *   If lowbit > highbit, then mask should be all 0's
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int bitMask(int highbit, int lowbit) {
  /*we make highmask and lowmask and 'or' it to make bitmask*/
  /* (ex) 8-bit INT and highbit = 4, highmask = 11100000 */
  /* (ex) 8-bit INT and lowbit = 2, ~lowmask = 00000011 */
  int highmask;
  int lowmask;
  highmask = ~1 << highbit;
  lowmask = ~0 << lowbit;

  return ~(highmask | (~lowmask));
}

/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
  int bitmask, res;
  //the main idea of this solution is dividing 32bits into four 8bits,
  //and calculate the number of 1 bit for each parts and summit them

  bitmask = 1;
  bitmask = bitmask | (bitmask << 8);
  bitmask = bitmask | (bitmask << 16);

  res = x & bitmask;
  res = res + ((x >> 1) & bitmask);
  res = res + ((x >> 2) & bitmask);
  res = res + ((x >> 3) & bitmask);
  res = res + ((x >> 4) & bitmask);
  res = res + ((x >> 5) & bitmask);
  res = res + ((x >> 6) & bitmask);
  res = res + ((x >> 7) & bitmask);

  res = res + (res >> 16);
  res = res + (res >> 8);

  return res & 0xff;
}

/*
 * TMax - return maximum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmax(void) {
  /* ~(1 << 31) = 0111...11 */

  return ~(1 << 31);
}

/*
 * isNonNegative - return 1 if x >= 0, return 0 otherwise
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 3
 */
int isNonNegative(int x) {
  /* x >> 31 to determine the number is negative or positive */
  x = x >> 31;

  return !x;
}

/*
 * addOK - Determine if can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0,
 *            addOK(0x80000000,0x70000000) = 1,
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int addOK(int x, int y) {
  /*compare (x + y)'s sign, x's sign, and y's sign */
  int z, sign_z;
  int sign_x, sign_y;
  int if_sign_xy_same, if_sign_xz_diff;

  z = x + y;
  sign_z = z >> 31;
  sign_x = x >> 31;
  sign_y = y >> 31;

  if_sign_xy_same = ~(sign_x ^ sign_y);
  if_sign_xz_diff = (sign_x ^ sign_z);

  return !(if_sign_xy_same & if_sign_xz_diff);
}

/*
 * rempwr2 - Compute x%(2^n), for 0 <= n <= 30
 *   Negative arguments should yield negative remainders
 *   Examples: rempwr2(15,2) = 3, rempwr2(-35,3) = -3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int rempwr2(int x, int n) {
  /* remove the bits those are above nth bit */
  int sign_x, pwrbit, res, chk;

  sign_x = x >> 31;
  pwrbit = ~(~0 << n);

  res = x & pwrbit;

  chk = !res;
  chk = chk << 31;
  chk = chk >> 31;

  return res + ((~chk << n) & sign_x);
}

/*
 * isLess - if x < y  then return 1, else return 0
 *   Example: isLess(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLess(int x, int y) {
  /* calculate x - y and determine by the sign of the result. */
  /* Check the overflow and underflow */
  int res, sign_x, sign_y, sign_res;
  int chk1, chk2;

  res = x + (~y + 1);

  sign_x = !((x >> 31) ^ ~0);
  sign_y = !((y >> 31) ^ ~0);
  sign_res = res >> 31;

  chk1 = (sign_x & ~sign_y);
  chk2 = (~sign_x & sign_y);
  chk2 = (!chk2) << 31;
  chk2 = chk2 >> 31;

  return chk1 | ((!!(sign_res ^ 0)) & chk2);
}

/*
 * absVal - absolute value of x
 *   Example: absVal(-1) = 1.
 *   You may assume -TMax <= x <= TMax
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 4
 */
int absVal(int x) {
  /* check the sign of x, and changes the sign if x < 0 */
  int sign_x = x >> 31;

  return (x ^ sign_x) + !(sign_x ^ ~0);
}

/*
 * isPower2 - returns 1 if x is a power of 2, and 0 otherwise
 *   Examples: isPower2(5) = 0, isPower2(8) = 1, isPower2(0) = 0
 *   Note that no negative number is a power of 2.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int isPower2(int x) {
  /* calculate x & (x - 1). if x has only one 1 bit (power of 2) x & (x - 1) == 0. */
    int x_minus_one, sign_x;
    x_minus_one = ~0 + x;
    //x_minus_one calculates x-1
    sign_x = x >> 31;

    return (!(x & x_minus_one) & (!sign_x)) & !!(x ^ 0) ;
}

/*
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
  /* change MSB (sign bit) opposite */
    int make_neg, exp_part;
    make_neg = 1 << 31;
    //10000...00
    exp_part = (uf << 1) >> 23;

    if(!((exp_part + 1) << 23)) return uf;

	  return uf ^ make_neg;
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
  /* minus 1 from the exponential part(exp part). */
  /* if exp part was 1, add 1 at the left end of the fraction part */
  /* if exp part was 0, just shift the fraction part to the right */
  int sign_uf, exp_part, frac_part;

  sign_uf = uf >> 31;
  exp_part = (uf << 1) >> 24;
  if(!((exp_part + 1) << 24)) return uf;
  frac_part = uf & (~(~0 << 23));
  sign_uf = sign_uf << 31;

  if (exp_part > 1) {
    exp_part = exp_part + ~0;
    exp_part = exp_part << 23;
    exp_part = exp_part & ~(1 << 31);
    return exp_part + sign_uf + frac_part;
  }

  if (exp_part == 1)
    frac_part = frac_part | 1 << 23;
  if (!((frac_part & 3) ^ 3))
    frac_part = frac_part + 1;
  frac_part = frac_part >> 1;

  return sign_uf | frac_part;
}

/*
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
  /* shift bit while there are bits above 24th bit */
  /* shift bit while there are not bits above 23th bit */
  /* put together sign part, exponetial part, fraction part */
  int sign_x, exp_part, frac_part;
  int chk_bit, chk_round, not_zero, chk;

  not_zero = ~0;
  sign_x = x >> 31;
  chk_bit = not_zero << 24;
  chk_round = 0;
  exp_part = 150;
  chk = 0;

  if (!x) return x;
  if (x == (1 << 31)) return 0xcf << 24;

  if (sign_x ^ 0) x = ~x + 1;

  while (x & chk_bit) {
    if (x & 1){
      if(!(x & (chk_bit << 1))) {
        if(chk_round)
          chk = 1;
        if (x & 2)
          chk = 1;
        if (chk)
          x = x + 1;
      }
      chk_round = 1;
    }
    x = x >> 1;
    exp_part = exp_part + 1;
  }

  chk_bit = chk_bit >> 1;
  while (!(x & chk_bit)) {
    x = x << 1;
    exp_part = exp_part + not_zero;
  }
  exp_part = exp_part << 23;
  frac_part = x & ~chk_bit;

  return (sign_x << 31) | exp_part | frac_part;
}
