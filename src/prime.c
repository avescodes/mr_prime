#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ruby.h"

unsigned long
expmod(unsigned long base, unsigned long exponent, unsigned long modulus)
{
  unsigned long result = 1;
  while(exponent > 0) {
    if ((exponent & 1) == 1) {
      result = (result * base) % modulus;
    }
    exponent >>= 1;
    base = (base * base) % modulus;
  }
  return result;
}

VALUE
integer_is_prime(VALUE self)
{
  unsigned long n = NUM2ULONG(self);
  
  int *primes;
  int nprimes;
  int k, i, j;
  unsigned long m, b;
  bool prime;

  static int primes_1[] = {2, 3};
  static int primes_2[] = {31, 73};
  static int primes_3[] = {2, 7, 61};
  static int primes_4[] = {2, 3, 5, 7, 11};
  static int primes_5[] = {2, 3, 5, 7, 11, 13};
  static int primes_6[] = {2, 3, 5, 7, 11, 13, 17};
  
  if      (n < 2) {
    return Qfalse;
  } else if (n < 4) {
    return Qtrue;
  } else if (n < 1373653) {
    primes = primes_1;
    nprimes = 2;
  } else if (n < 9080191) {
    primes = primes_2;
    nprimes = 2;
/*  } else if (n < 4759123141) { 
    primes = primes_3;
    nprimes = 3;
  } else if (n < 2152302989747) {
    primes = primes_4;
    nprimes = 5;
  } else if (n < 3474749660383) {
    primes = primes_5;
    nprimes = 6;
  } else if (n < 341550071728321) {
    primes = primes_6;
    nprimes = 7; */
  } else {
    return Qfalse; // DO SOMETHING BETTER HERE
  }

  k = 0;
  m = n - 1;
  while (m & 1) {
    m >>= 1;
    k += 1;
  }

  for (i = 0; i < nprimes; i++) {
    b = expmod(primes[i], m, n);

    if (b != 1) {
      prime = false;

      for (j = 0; j < k; j++) {
        if (b == n - 1) {
          prime = true;
          break;
        }
        b = expmod(b, 2, n);
      }
      
      if (!prime) {
        return Qfalse;
      }
    }
  }
  
  return Qtrue;
}

void
Init_better_prime()
{
  VALUE cNumeric = rb_define_class("Numeric", rb_cObject);

  rb_define_method(cNumeric, "prime?", integer_is_prime, 0);
}
