#define INTMAX 2147483647
#define INTMIN -2147483648

int divide(int dividend, int divisor) {
  if ((dividend == INTMIN && divisor == -1) || divisor == 0) {
    return INTMAX;
  }
  
  int quotient = 0;
  long int dn = dividend < 0 ? -(long int)dividend : dividend;
  long int dr = divisor < 0 ? -(long int)divisor : divisor;

  for (int i = 31; i >= 0; i--) {
    if ((dn >> i) >= dr) {
      quotient += 1 << i;
      dn -= dr << i;
    }
  }

  return (dividend < 0) ^ (divisor < 0) ? -quotient : quotient;
}