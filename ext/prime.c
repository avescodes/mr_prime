#include <stdbool.h>

#include "ruby.h"

typedef unsigned long long ullong;

ullong
expmod(ullong base, ullong exponent, ullong modulus)
{
  ullong result = 1;
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
mrprime(VALUE obj)
{
  rb_require("better_prime/mr_prime");
  VALUE mrprime = rb_define_class("MR_Prime", rb_cObject);
  return rb_funcall(mrprime, rb_intern("[]"), 1, obj);
}

/* Miller-Rabin algorithm to determine primality of a number.
 * Uses values for a that guarantee correct results up to 314T.
 * Above that, uses ryan@ryanneufeld.ca's Mr.Prime.
 */ 
VALUE
integer_is_prime(VALUE self)
{
  if (!FIXNUM_P(self)) {
    return mrprime(self);
  }

  ullong n = NUM2ULL(self);
  
  int *primes;
  int nprimes;
  int k, i, j;
  ullong m, b;
  bool prime;
  
  static int primes_1[] = {2, 3};
  static int primes_2[] = {31, 73};
  static int primes_3[] = {2, 7, 61};
  static int primes_4[] = {2, 3, 5, 7, 11};
  static int primes_5[] = {2, 3, 5, 7, 11, 13};
  static int primes_6[] = {2, 3, 5, 7, 11, 13, 17};
  
  if (n < 2) {
    return Qfalse;
  } else if (n < 4) {
    return Qtrue;
  } else if (n < 1373653) {
    primes = primes_1;
    nprimes = 2;
  } else if (n < 9080191) {
    primes = primes_2;
    nprimes = 2;
  } else if (n < 4759123141ULL) { 
    primes = primes_3;
    nprimes = 3;
  } else if (n < 2152302989747ULL) {
    primes = primes_4;
    nprimes = 5;
  } else if (n < 3474749660383ULL) {
    primes = primes_5;
    nprimes = 6;
  } else if (n < 341550071728321ULL) {
    primes = primes_6;
    nprimes = 7;
  } else {
    // Quick, do something sensible!
    return mrprime(self);
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
