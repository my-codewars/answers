#ifndef CINT_MASTER
#define CINT_MASTER

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

static cint_sheet * cint_new_sheet(const size_t bits) {
  // a computation sheet is required by function needing temporary vars.
  cint_sheet * sheet = calloc(1, sizeof(cint_sheet));
  assert(sheet);
  const size_t num_size = 2 + bits / cint_exponent;
  if (sheet) for (size_t i = 0; i < 10; ++i) {
      sheet->temp[i].nat = 1 ;
      sheet->temp[i].mem = sheet->temp[i].end = calloc(num_size, sizeof(h_cint_t));
      assert(sheet->temp[i].mem);
      sheet->temp[i].size = num_size;
    }
  return sheet ;
}

void cint_clear_sheet(cint_sheet *sheet) {
  for (size_t i = 0; i < 10; ++i)
    free(sheet->temp[i].mem);
  free(sheet);
}

static size_t cint_count_bits(const cint * num) {
  size_t res = 0;
  if (num->end != num->mem) {
    for (; *(num->end - 1) >> ++res;);
    res += (num->end - num->mem - 1) * cint_exponent;
  }
  return res;
}

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

static void cint_reinit_by_string(cint *num, const char *str, const int base) {
  cint_erase(num);
  for (; *str && memchr(cint_alpha, *str, base) == 0; num->nat *= 1 - ((*str++ == '-') << 1));
  for (h_cint_t *p; *str; *num->mem += (h_cint_t) ((char *) memchr(cint_alpha, *str++, base) - cint_alpha), num->end += *num->end != 0)
    for (p = num->end; --p >= num->mem; *(p + 1) += (*p *= base) >> cint_exponent, *p &= cint_mask);
  for (h_cint_t *p = num->mem; p < num->end; *(p + 1) += *p >> cint_exponent, *p++ &= cint_mask);
  num->end += *num->end != 0, num->mem != num->end || (num->nat = 1);
}

static inline void cint_init_by_string(cint *num, const size_t bits, const char *str, const int base) {
  cint_init(num, bits, 0), cint_reinit_by_string(num, str, base);
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

static void cint_dup(cint *to, const cint *from) {
  // duplicate number (no verification about overlapping or available memory, caller must check)
  const size_t b = from->end - from->mem, a = to->end - to->mem;
  memcpy(to->mem, from->mem, b * sizeof(*from->mem));
  to->end = to->mem + b;
  to->nat = from->nat;
  if (b < a) memset(to->end, 0, (a - b) * sizeof(*to->mem));
}

static void cint_rescale(cint *num, const size_t bits) {
  // rarely tested, it should allow to resize a number transparently.
  size_t curr_size = num->end - num->mem ;
  size_t new_size = 1 + bits / cint_exponent ;
  new_size = new_size + 8 - new_size % 8 ;
  if (curr_size < new_size)
    cint_erase(num), curr_size = 0;
  num->mem = realloc(num->mem, new_size * sizeof(h_cint_t));
  assert(num->mem);
  if (num->size < new_size)
    memset(num->mem + num->size, 0, (new_size - num->size) * sizeof(h_cint_t));
  num->end = num->mem + curr_size ;
  num->size = new_size ;
}

static inline cint * h_cint_tmp(cint_sheet * sheet, const int id, const cint * least){
  // request at least the double of "least" to allow performing multiplication then modulo...
  size_t required = (1 + least->end - least->mem) << 1 ;
  if (sheet->temp[id].size <= required) {
    required = (1 + ((required * cint_exponent) >> 10)) << 10 ;
    cint_rescale(&sheet->temp[id], required);
  }
  return &sheet->temp[id] ;
}

static void h_cint_addi(cint *lhs, const cint *rhs) {
  // perform an addition (without caring of the sign)
  h_cint_t *l = lhs->mem;
  for (const h_cint_t *r = rhs->mem; r < rhs->end;)
    *l += *r++, *(l + 1) += *l >> cint_exponent, *l++ &= cint_mask;
  for (; *l & cint_base; *(l + 1) += *l >> cint_exponent, *l++ &= cint_mask);
  if (rhs->end - rhs->mem > lhs->end - lhs->mem)
    lhs->end = lhs->mem + (rhs->end - rhs->mem);
  lhs->end += *lhs->end != 0;
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

static void cint_left_shifti(cint *num, const size_t bits) {
  // execute a left shift immediately over the input, for any amount of bits (no verification about available memory)
  if (num->end != num->mem) {
    const size_t a = bits / cint_exponent, b = bits % cint_exponent, c = cint_exponent - b;
    if (a) {
      memmove(num->mem + a, num->mem, (num->end - num->mem + 1) * sizeof(h_cint_t));
      memset(num->mem, 0, a * sizeof(h_cint_t));
      num->end += a;
    }
    if (b) for (h_cint_t *l = num->end, *e = num->mem + a; --l >= e; *(l + 1) |= *l >> c, *l = *l << b & cint_mask);
    num->end += *(num->end) != 0;
  }
}

static void cint_right_shifti(cint *num, const size_t bits) {
  size_t a = bits / cint_exponent, b = bits % cint_exponent, c = cint_exponent - b;
  if (num->end - a > num->mem) {
    if (a) {
      if(num->mem + a > num->end) a = num->end - num->mem;
      memmove(num->mem, num->mem + a, (num->end - num->mem) * sizeof(h_cint_t));
      memset(num->end -= a, 0, a * sizeof(h_cint_t));
    }
    if (b) for (h_cint_t *l = num->mem; l < num->end; *l = (*l >> b | *(l + 1) << c) & cint_mask, ++l);
    if(num->end != num->mem ) num->end -= *(num->end - 1) == 0, num->end == num->mem && (num->nat = 1);
  } else cint_erase(num);
}

static void cint_sqrt(cint_sheet * sheet, const cint *num, cint *res, cint *rem) {
  // original square root algorithm.
  cint_erase(res), cint_dup(rem, num); // answer ** 2 + rem = num
  if (num->nat > 0 && num->end != num->mem) {
    cint *a = h_cint_tmp(sheet, 0, num), *b = h_cint_tmp(sheet, 1, num);
    cint_erase(a), *a->end++ = 1;
    cint_left_shifti(a, cint_count_bits(num) & ~1);
    for (; a->mem != a->end;) {
      cint_dup(b, res);
      h_cint_addi(b, a);
      cint_right_shifti(res, 1);
      if (h_cint_compare(rem, b) >= 0)
        h_cint_subi(rem, b), h_cint_addi(res, a);
      cint_right_shifti(a, 2);
    }
  }
}

#endif

char *integerSquareRoot(char *str) {
  cint_sheet * s = cint_new_sheet(4096);
  cint n, q, r ;
  cint_init_by_string(&n, 4096, str, 10);
  cint_init(&q, 4096, 0);
  cint_init(&r, 4096, 0);
  cint_sqrt(s, &n, &q, &r);
  char * res = cint_to_string(&q, 10);
  free(n.mem);
  free(q.mem);
  free(r.mem);
  cint_clear_sheet(s);
  return res ;
}
