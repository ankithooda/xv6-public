#include "types.h"

static uint x0 = 3362891573;
static uint x1 = 1892746712;
static uint x2 = 2939381636;
static uint x3 = 4102871628;

extern uint random_uint(uint m);

/* The state must be initialized to non-zero */
uint xorshift128()
{
	/* Algorithm "xor128" from p. 5 of Marsaglia, "Xorshift RNGs" */
	uint t  = x3;

    uint s  = x0;  /* Perform a contrived 32-bit shift. */
	x3 = x2;
	x2 = x1;
	x1 = s;

	t ^= t << 11;
	t ^= t >> 8;
	return x0 = t ^ s ^ (s >> 19);
}

uint random_uint(uint m) {
  return (xorshift128() % m) + 1;
}
