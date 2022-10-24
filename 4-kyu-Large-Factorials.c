#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

typedef int64_t h_cint_t;

static const h_cint_t cint_exponent = 4 * sizeof(h_cint_t) - 1;
static const h_cint_t cint_base = (h_cint_t)1 << cint_exponent;
static const h_cint_t cint_mask = cint_base - 1;
static const char *cint_alpha = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

typedef struct {
  h_cint_t *mem; // reserved data pointer.
  h_cint_t *end;
  h_cint_t nat; // -1 = negative, +1 = positive, (zero is a positive)
  size_t size;
} cint;

typedef struct {
  size_t immediate_state ;
  cint temp[10];
} cint_sheet;

static void cint_init(cint * num, size_t bits, long long int value) {
  num->size = bits / cint_exponent;
  num->size += 8 - num->size % 4 ;
  num->end = num->mem = calloc(num->size, sizeof(*num->mem));
  assert(num->mem);
  if((num->nat = 1 - ((value < 0) << 1)) < 0) value = -value;
  for (; value; *num->end = (h_cint_t)(value % cint_base), value /= cint_base, ++num->end);
}

static inline void cint_erase(cint * num) {
  num->nat = 1, num->end = memset(num->mem, 0, (num->end - num->mem) * sizeof(h_cint_t));
}

static char *cint_to_string(const cint *num, const int base_out) {
  // Very interesting function, only problem is that it can be slow.
  h_cint_t a, b, *c = num->end;
  size_t d, e = 1;
  char *s = malloc(2);
  assert(s);
  strcpy(s, "0");
  for (; --c >= num->mem;) {
    for (a = *c, d = e; d;) {
      b = (h_cint_t) ((char *) memchr(cint_alpha, s[--d], base_out) - cint_alpha), b = b * cint_base + a;
      s[d] = cint_alpha[b % base_out];
      a = b / base_out;
    }
    for (; a; s = realloc(s, ++e + 1), assert(s), memmove(s + 1, s, e), *s = cint_alpha[a % base_out], a /= base_out);
  }
  if (num->nat < 0)
    s = realloc(s, e + 2), assert(s), memmove(s + 1, s, e + 1), *s = '-';
  return s;
}

static void cint_mul(const cint *lhs, const cint *rhs, cint *res) {
  // the multiplication (no Karatsuba Algorithm, it's the "slow" multiplication)
  h_cint_t *l, *r, *o, *p;
  cint_erase(res);
  if (lhs->mem != lhs->end && rhs->mem != rhs->end) {
    res->nat = lhs->nat * rhs->nat, res->end += (lhs->end - lhs->mem) + (rhs->end - rhs->mem) - 1;
    for (l = lhs->mem, p = res->mem; l < lhs->end; ++l)
      for (r = rhs->mem, o = p++; r < rhs->end; *(o + 1) += (*o += *l * *r++) >> cint_exponent, *o++ &= cint_mask);
    res->end += *res->end != 0;
  }
}

char *factorial(int n) {
  if (n < 0)
    return calloc(1, 1);
  cint a, b, c, *A, *B, *C, *D;
  cint_init(&a, 65536, 1);
  cint_init(&b, 65536, 1);
  cint_init(&c, 65536, 1);
  A = &a, B = &b, C = &c;
  for(h_cint_t i = 2; i <= n; ++i)
    *b.mem = i, cint_mul(A, B, C), D = A, A = C, C = D;
  char * res = cint_to_string(A, 10);
  free(a.mem);
  free(b.mem);
  free(c.mem);
  return res ;
}
