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

static inline int h_cint_compare(const cint * lhs, const cint * rhs) {
  h_cint_t res = (h_cint_t) ((lhs->end - lhs->mem) - (rhs->end - rhs->mem));
  if (res == 0 && rhs->end != rhs->mem)
    for (const h_cint_t *l = lhs->end, *r = rhs->end; !(res = *--l - *--r) && l != lhs->mem;);
  return (res > 0) - (res < 0);
}

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

static void cint_reinit(cint * num, long long int value) {
  // it's like an initialization, but there is no memory allocation here
  num->end = memset(num->mem, 0, (num->end - num->mem) * sizeof(h_cint_t));
  if ((num->nat = 1 - ((value < 0) << 1)) < 0) value = -value;
  for (; value; *num->end = (h_cint_t)(value % cint_base), value /= cint_base, ++num->end);
}

static void cint_reinit_by_string(cint *num, const char *str, const int base) {
  cint_erase(num);
  for (; *str && memchr(cint_alpha, *str, base) == 0; num->nat *= 1 - ((*str++ == '-') << 1));
  for (h_cint_t *p; *str; *num->mem += (h_cint_t) ((char *) memchr(cint_alpha, *str++, base) - cint_alpha), num->end += *num->end != 0)
    for (p = num->end; --p >= num->mem; *(p + 1) += (*p *= base) >> cint_exponent, *p &= cint_mask);
  for (h_cint_t *p = num->mem; p < num->end; *(p + 1) += *p >> cint_exponent, *p++ &= cint_mask);
  num->end += *num->end != 0, num->mem != num->end || (num->nat = 1);
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

static inline void cint_init_by_string(cint *num, const size_t bits, const char *str, const int base) {
  cint_init(num, bits, 0), cint_reinit_by_string(num, str, base);
}
static void cint_dup(cint *to, const cint *from) {
  // duplicate number (no verification about overlapping or available memory, caller must check)
  const size_t b = from->end - from->mem, a = to->end - to->mem;
  memcpy(to->mem, from->mem, b * sizeof(*from->mem));
  to->end = to->mem + b;
  to->nat = from->nat;
  if (b < a) memset(to->end, 0, (a - b) * sizeof(*to->mem));
}

static void h_cint_subi(cint *lhs, const cint *rhs) {
  // perform a subtraction (without caring about the sign, it performs high subtract low)
  h_cint_t a = 0, cmp, *l, *r, *e, *o;
  if (lhs->mem == lhs->end)
    cint_dup(lhs, rhs);
  else if (rhs->mem != rhs->end) {
    cmp = h_cint_compare(lhs, rhs);
    if (cmp) {
      if (cmp < 0) l = lhs->mem, r = rhs->mem, e = rhs->end, lhs->nat = -lhs->nat;
      else l = rhs->mem, r = lhs->mem, e = lhs->end;
      for (o = lhs->mem; r < e; *o = *r++ - *l++ - a, a = (*o & cint_base) != 0, *o++ &= cint_mask);
      for (*o &= cint_mask, o += a; --o > lhs->mem && !*o;);
      lhs->end = 1 + o;
    } else cint_erase(lhs);
  }
}

static void cint_binary_div(const cint *lhs, const cint *rhs, cint *q, cint *r) {
  // the original division algorithm, it doesn't take any temporary variable.
  cint_erase(r);
  if (rhs->end == rhs->mem)
    for (q->nat = lhs->nat * rhs->nat, q->end = q->mem; q->end < q->mem + q->size; *q->end++ = cint_mask); // DBZ
  else {
    h_cint_t a = h_cint_compare(lhs, rhs);
    if (a) {
      cint_erase(q);
      if (a > 0) {
        h_cint_t *l = lhs->end, *k, *qq = q->mem + (lhs->end - lhs->mem);
        for (; --qq, --l >= lhs->mem;)
          for (a = cint_base; a >>= 1;) {
            for (k = r->end; --k >= r->mem; *(k + 1) |= (*k <<= 1) >> cint_exponent, *k &= cint_mask);
            *r->mem += (a & *l) != 0, r->end += *r->end != 0;
            h_cint_compare(r, rhs) >= 0 ? h_cint_subi(r, rhs), *qq |= a : 0;
          }
        q->end += (lhs->end - lhs->mem) - (rhs->end - rhs->mem), q->end += *q->end != 0;
        q->nat = rhs->nat * lhs->nat, (r->end == r->mem) || (r->nat = lhs->nat); // lhs = q * rhs + r
      } else cint_dup(r, lhs);
    } else cint_reinit(q, rhs->nat * lhs->nat);
  }
}

char **divide_strings(char *a, char *b) {
  char **res = malloc(2 * sizeof(char *));
  cint lhs, rhs, q, r;
  cint_init_by_string(&lhs, 65536, a, 10);
  cint_init_by_string(&rhs, 65536, b, 10);
  cint_init(&q, 65536, 0);
  cint_init(&r, 65536, 0);
  cint_binary_div(&lhs, &rhs, &q, &r);
  res[0] = cint_to_string(&q, 10);
  res[1] = cint_to_string(&r, 10);
  free(lhs.mem);
  free(rhs.mem);
  free(q.mem);
  free(r.mem);
  return res;
}
