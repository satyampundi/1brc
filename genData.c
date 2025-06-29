#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define nelem(x) (sizeof(x) / sizeof(*(x)))
#define assert(x)                                                              \
  if (!(x))                                                                    \
  __builtin_trap()

char buf[256];
struct city {
  char *name;
  double mean;
  int select;
} selectcities[10000];

void fail(char *msg) {
  fprintf(stderr, "gendata: %s\n", msg);
  exit(1);
}

double randomdouble(void) {
  return (double)random() / ((double)((1L << 31) - 1));
}

struct marsagliapolar {
  int hasnext;
  double next;
};

/* Based on https://en.wikipedia.org/wiki/Marsaglia_polar_method#C++ */
double randomgaussian(struct marsagliapolar *mp) {
  double u, v, s;
  if (mp->hasnext) {
    mp->hasnext = 0;
    return mp->next;
  }

  do {
    u = randomdouble() * 2.0 - 1.0;
    v = randomdouble() * 2.0 - 1.0;
    s = u * u + v * v;
  } while (s >= 1.0 || s == 0.0);
  s = sqrt(-2.0 * log(s) / s);
  mp->next = v * s;
  mp->hasnext = 1;
  return u * s;
}

int main(int argc, char **argv) {
  struct city *cities = 0;
  int ncities = 0, nrows, i;
  struct marsagliapolar mp = {0};

  if (argc != 2)
    fail("Usage: gendata NUM");
  nrows = atoi(argv[1]);
  if (nrows < 0)
    fail("number of rows must be non-negative");
  srandom(getpid());

  while (fgets(buf, sizeof(buf), stdin)) {
    char *p;
    struct city *c;
    if (*buf == '#')
      continue;
    assert(p = strchr(buf, ';'));
    *p = 0;
    assert(cities = realloc(cities, ++ncities * sizeof(*cities)));
    c = cities + ncities - 1;
    assert(c->name = strdup(buf));
    assert(sscanf(p + 1, "%lf", &c->mean) == 1);
  }

  assert(ncities >= nelem(selectcities));
  for (i = 0; i < nelem(selectcities);) {
    struct city *c = cities + random() % ncities;
    if (!c->select) {
      c->select = 1;
      selectcities[i++] = *c;
    }
  }

  while (nrows--) {
    struct city *c = selectcities + random() % nelem(selectcities);
    int x, n;
    char buf[6];
    do
      x = 10.0 * (c->mean + 10.0 * randomgaussian(&mp));
    while (x < -999 || x > 999);
    n = 0;
    if (x < 0) {
      buf[n++] = '-';
      x = -x;
    }
    if (x / 100)
      buf[n++] = '0' + x / 100;
    buf[n++] = '0' + (x / 10) % 10;
    buf[n++] = '.';
    buf[n++] = '0' + x % 10;
    buf[n++] = 0;
    assert(n <= sizeof(buf));

    fputs(c->name, stdout);
    putchar(';');
    puts(buf);
  }

  return 0;
}