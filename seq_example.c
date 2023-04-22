#include "seq.h"
 
#include "memory_tests.h"
 
#include <errno.h>
 
#include <stdio.h>
 
#include <string.h>
 
#include <stdlib.h>
 
#include <assert.h>
 
/** MAKRA SKRACAJĄCE IMPLEMENTACJĘ TESTÓW **/
 
// To są możliwe wyniki testu.
#define PASS 0
#define FAIL 1
#define WRONG_TEST 2
 
// Oblicza liczbę elementów tablicy x.
#define SIZE(x)(sizeof x / sizeof x[0])
 
#define CHECK(x)\
if (!(x)) { \
  printf("TEST %d, CHECK %d NIE PRZESZEDL\n", testid, counter);\
  _Exit(0);\
}\
else { \
  ++counter;\
}
 
#define TEST_EINVAL(f)\
do { \
  errno = 0x69;\
  if ((f) != -1 || errno != EINVAL)\
    return FAIL;\
} while (0)
 
#define TEST_NULL_EINVAL(f)\
do { \
  errno = 0x69;\
  if ((f) != NULL || errno != EINVAL)\
    return FAIL;\
} while (0)
 
#define TEST_PASS(f)\
do { \
  if ((f) != 1)\
    return FAIL;\
} while (0)
 
#define TEST_FAIL(f)\
do { \
  if ((f) != 0)\
    return FAIL;\
} while (0)
 
#define TEST_COMP(f, s)\
do { \
  if (strcmp((f), (s)) != 0)\
    return FAIL;\
} while (0)
 
#define TEST_NULL_FAIL(f)\
do { \
  errno = 0x69;\
  if ((f) != NULL || errno != 0)\
    return FAIL;\
} while (0)
 
#define V(code, where)(((unsigned long) code) << (3 * where))
 
/** WŁAŚCIWE TESTY **/
 
// Testuje poprawność weryfikacji parametrów wywołań funkcji.
static int params(void) {
  assert(EINVAL != 0x69);
  assert(ENOMEM != 0x69);
 
  static
  const char bad_seq[4] = {
    0,
    1,
    2,
    3
  };
 
  seq_t * seq = seq_new();
 
  TEST_EINVAL(seq_add(NULL, "1"));
  TEST_EINVAL(seq_add(seq, NULL));
  TEST_EINVAL(seq_add(NULL, NULL));
  TEST_EINVAL(seq_add(seq, ""));
  TEST_EINVAL(seq_add(seq, "/"));
  TEST_EINVAL(seq_add(seq, "3"));
  TEST_EINVAL(seq_add(seq, "10/"));
  TEST_EINVAL(seq_add(seq, "103"));
  TEST_EINVAL(seq_add(seq, "10 "));
  TEST_EINVAL(seq_add(seq, "10a"));
  TEST_EINVAL(seq_add(seq, "a1"));
  TEST_EINVAL(seq_add(seq, "1a"));
  TEST_EINVAL(seq_add(seq, "21/"));
  TEST_EINVAL(seq_add(seq, "213"));
  TEST_EINVAL(seq_add(seq, "0/0"));
  TEST_EINVAL(seq_add(seq, "030"));
  TEST_EINVAL(seq_add(seq, bad_seq));
 
  TEST_EINVAL(seq_remove(NULL, "1"));
  TEST_EINVAL(seq_remove(seq, NULL));
  TEST_EINVAL(seq_remove(NULL, NULL));
  TEST_EINVAL(seq_remove(seq, ""));
  TEST_EINVAL(seq_remove(seq, "/"));
  TEST_EINVAL(seq_remove(seq, "3"));
  TEST_EINVAL(seq_remove(seq, "10/"));
  TEST_EINVAL(seq_remove(seq, "103"));
  TEST_EINVAL(seq_remove(seq, "10 "));
  TEST_EINVAL(seq_remove(seq, "10a"));
  TEST_EINVAL(seq_remove(seq, "a1"));
  TEST_EINVAL(seq_remove(seq, "1a"));
  TEST_EINVAL(seq_remove(seq, "21/"));
  TEST_EINVAL(seq_remove(seq, "213"));
  TEST_EINVAL(seq_remove(seq, "0/0"));
  TEST_EINVAL(seq_remove(seq, "030"));
  TEST_EINVAL(seq_remove(seq, bad_seq));
 
  TEST_EINVAL(seq_valid(NULL, "1"));
  TEST_EINVAL(seq_valid(seq, NULL));
  TEST_EINVAL(seq_valid(NULL, NULL));
  TEST_EINVAL(seq_valid(seq, ""));
  TEST_EINVAL(seq_valid(seq, "/"));
  TEST_EINVAL(seq_valid(seq, "3"));
  TEST_EINVAL(seq_valid(seq, "10/"));
  TEST_EINVAL(seq_valid(seq, "103"));
  TEST_EINVAL(seq_valid(seq, "10 "));
  TEST_EINVAL(seq_valid(seq, "10a"));
  TEST_EINVAL(seq_valid(seq, "a1"));
  TEST_EINVAL(seq_valid(seq, "1a"));
  TEST_EINVAL(seq_valid(seq, "21/"));
  TEST_EINVAL(seq_valid(seq, "213"));
  TEST_EINVAL(seq_valid(seq, "0/0"));
  TEST_EINVAL(seq_valid(seq, "030"));
  TEST_EINVAL(seq_valid(seq, bad_seq));
 
  TEST_EINVAL(seq_set_name(NULL, "0", "a"));
  TEST_EINVAL(seq_set_name(seq, NULL, "b"));
  TEST_EINVAL(seq_set_name(seq, "", "c"));
  TEST_EINVAL(seq_set_name(seq, "1", NULL));
  TEST_EINVAL(seq_set_name(seq, NULL, NULL));
  TEST_EINVAL(seq_set_name(NULL, NULL, "x"));
  TEST_EINVAL(seq_set_name(NULL, "00", NULL));
  TEST_EINVAL(seq_set_name(NULL, NULL, NULL));
  TEST_EINVAL(seq_set_name(seq, "2", ""));
  TEST_EINVAL(seq_set_name(seq, "/", "d"));
  TEST_EINVAL(seq_set_name(seq, "3", "e"));
  TEST_EINVAL(seq_set_name(seq, "3", "0"));
  TEST_EINVAL(seq_set_name(seq, "a", "0"));
  TEST_EINVAL(seq_set_name(seq, bad_seq, "f"));
  TEST_EINVAL(seq_set_name(seq, bad_seq, bad_seq));
 
  TEST_NULL_EINVAL(seq_get_name(NULL, "0"));
  TEST_NULL_EINVAL(seq_get_name(seq, NULL));
  TEST_NULL_EINVAL(seq_get_name(NULL, NULL));
  TEST_NULL_EINVAL(seq_get_name(seq, ""));
  TEST_NULL_EINVAL(seq_get_name(seq, "/"));
  TEST_NULL_EINVAL(seq_get_name(seq, "3"));
  TEST_NULL_EINVAL(seq_get_name(seq, bad_seq));
 
  TEST_EINVAL(seq_equiv(NULL, "0", "1"));
  TEST_EINVAL(seq_equiv(seq, NULL, "1"));
  TEST_EINVAL(seq_equiv(seq, "", "1"));
  TEST_EINVAL(seq_equiv(seq, "0", NULL));
  TEST_EINVAL(seq_equiv(seq, NULL, NULL));
  TEST_EINVAL(seq_equiv(NULL, NULL, "0"));
  TEST_EINVAL(seq_equiv(NULL, "00", NULL));
  TEST_EINVAL(seq_equiv(NULL, NULL, NULL));
  TEST_EINVAL(seq_equiv(seq, "0", ""));
  TEST_EINVAL(seq_equiv(seq, "/", "1"));
  TEST_EINVAL(seq_equiv(seq, "3", "1"));
  TEST_EINVAL(seq_equiv(seq, "0", "/"));
  TEST_EINVAL(seq_equiv(seq, "0", "3"));
  TEST_EINVAL(seq_equiv(seq, "0000000a", "0000000000"));
  TEST_EINVAL(seq_equiv(seq, "0000000", "000000000a"));
  TEST_EINVAL(seq_equiv(seq, "0", bad_seq));
  TEST_EINVAL(seq_equiv(seq, bad_seq, "1"));
  TEST_EINVAL(seq_equiv(seq, bad_seq, bad_seq));
 
  seq_delete(seq);
 
  return PASS;
}
 
// Testuje podstawową funkcjonalność biblioteki.
static int simple(void) {
  seq_t * seq = seq_new();
 
  TEST_PASS(seq_add(seq, "012"));
  TEST_FAIL(seq_add(seq, "01"));
  TEST_FAIL(seq_remove(seq, "0120"));
 
  TEST_PASS(seq_valid(seq, "0"));
  TEST_PASS(seq_valid(seq, "01"));
  TEST_PASS(seq_valid(seq, "012"));
  TEST_FAIL(seq_valid(seq, "0120"));
 
  TEST_PASS(seq_remove(seq, "01"));
 
  TEST_PASS(seq_valid(seq, "0"));
  TEST_FAIL(seq_valid(seq, "01"));
  TEST_FAIL(seq_valid(seq, "012"));
 
  seq_delete(seq);
  return PASS;
}
 
unsigned int _x = 548787455, _y = 842502087, _z = 3579807591, _w = 273326509;
 
void XRinit(int t) {
  _x = 548787455, _y = 842502087, _z = 3579807591, _w = 273326509;
  _x ^= t;
  _x += t;
  _z -= t;
  _w ^= (t + 777);
}
 
unsigned int XORShift() {
  unsigned int t = _x ^ (_x << 11);
  _x = _y;
  _y = _z;
  _z = _w;
  return _w = _w ^ (_w >> 19) ^ t ^ (t >> 8);
}
 
char * losowe_slowo(int maxdlugosc) {
  int dlugosc = XORShift() % maxdlugosc + 1;
  char * str = malloc(sizeof(char) * (dlugosc + 1));
  assert(str);
  for (int i = 0; i < dlugosc; i++)
    str[i] = '0' + (XORShift() % 3);
  str[dlugosc] = 0;
  return str;
}
 
const long long mod = 1066168110141613;
 
void wstaw(long long * hash, long long info) {
  ( * hash) = (( * hash) * 1031 + info) % mod;
}
 
long long uruchom(int testy, int maxdlugosc, int flagi, int ile_nazw, int debug) {
  int czy_dodawanie = 1;
  int czy_sprawdzanie = !!(flagi & 1);
  int czy_usuwanie = !!(flagi & 2);
  int czy_set_name = !!(flagi & 4);
  int czy_get_name = !!(flagi & 8);
  int ile = 1 + czy_sprawdzanie + czy_usuwanie + czy_set_name + czy_get_name;
  seq_t * t = seq_new();
  long long hash = 0;
  for (int i = 0; i < testy; i++) {
    int mode = XORShift() % ile + 1;
 
    mode -= czy_dodawanie;
    if (mode == 0) {
      char * str = losowe_slowo(maxdlugosc);
      if (debug)
        printf("add %s\n", str);
      int res = seq_add(t, str);
      if (debug)
        printf("%d\n----\n", res);
      assert(res != -1);
      wstaw( & hash, res);
      free(str);
      continue;
    }
 
    mode -= czy_sprawdzanie;
    if (mode == 0) {
      char * str = losowe_slowo(maxdlugosc);
      if (debug)
        printf("valid %s\n", str);
      int res = seq_valid(t, str);
      if (debug)
        printf("%d\n----\n", res);
      assert(res != -1);
      wstaw( & hash, res);
      free(str);
      continue;
    }
 
    mode -= czy_usuwanie;
    if (mode == 0) {
      char * str = losowe_slowo(maxdlugosc);
      if (debug)
        printf("remove %s\n", str);
      int res = seq_remove(t, str);
      if (debug)
        printf("%d\n----\n", res);
      assert(res != -1);
      wstaw( & hash, res);
      free(str);
      continue;
    }
 
    mode -= czy_set_name;
    if (mode == 0) {
      char * nazwa = malloc(sizeof(char) * 2);
      assert(nazwa);
      nazwa[0] = 'a' + (XORShift() % ile_nazw);
      nazwa[1] = 0;
 
      char * str = losowe_slowo(maxdlugosc);
      if (debug)
        printf("set_name %s %s\n", str, nazwa);
 
      int res = seq_set_name(t, str, nazwa);
      if (debug)
        printf("%d\n----\n", res);
      wstaw( & hash, res);
      free(str);
      free(nazwa);
      continue;
    }
 
    mode -= czy_get_name;
    if (mode == 0) {
      char * str = losowe_slowo(maxdlugosc);
      if (debug)
        printf("get_name %s\n", str);
      errno = 0x69;
      const char * res = seq_get_name(t, str);
      if (debug) {
        printf("%d\n", errno);
        if (res)
          printf("[NULL]\n");
        else
          printf("[%s]\n", res);
        printf("----\n");
      }
      wstaw( & hash, errno);
      if (res != NULL) {
        int dlugosc = strlen(res);
        for (int i = 0; i < dlugosc; i++)
          wstaw( & hash, res[i]);
      }
      free(str);
      errno = 0;
    }
  }
  seq_delete(t);
  return hash;
}
 
//Testy generowane przez prng z ustalonym z góry ziarnem.
//
static int losowe(void) {
 
  if (1) {
    XRinit(1234);
    int testid = 0, counter = 0;
    long long wyn = uruchom(100, 4, 0, 3, 0);
    CHECK(wyn == 285985447414318);
  }
 
  if (1) {
    XRinit(2345);
    int testid = 1, counter = 0;
    long long wyn = uruchom(500, 4, 0, 3, 0);
    CHECK(wyn == 895855615859691);
  }
 
  if (1) {
    XRinit(4567);
    int testid = 2, counter = 0;
    long long wyn = uruchom(1000, 4, 15, 3, 0);
    CHECK(wyn == 389591023474338);
  }
 
  if (1) {
    XRinit(5678);
    int testid = 3, counter = 0;
    long long wyn = uruchom(10000, 4, 15, 3, 0);
    CHECK(wyn == 920015510045334);
  }
 
  if (1) {
    XRinit(6789);
    int testid = 4, counter = 0;
    long long wyn = uruchom(100000, 6, 15 ^ 2, 3, 0);
    CHECK(wyn == 619247735844396);
  }
 
  if (1) {
    XRinit(7890);
    int testid = 5, counter = 0;
    long long wyn = uruchom(100000, 6, 15, 3, 0);
    CHECK(wyn == 558030557961299);
  }
 
  if (1) {
    XRinit(6969);
    int testid = 6, counter = 0;
    long long wyn = uruchom(100000, 5, 15, 3, 0);
    CHECK(wyn == 301387824177954);
  }
 
  if (1) {
    XRinit(2137);
    int testid = 7, counter = 0;
    long long wyn = uruchom(1000000, 9, 15, 3, 0);
    CHECK(wyn == 1004179168174930);
  }
 
  printf("PRZESZLO!\n");
  return PASS;
}
 
static int custom(void) {
 
  if (1) {
    seq_t * t = seq_new();
    const int testid = 1;
    int counter = 0;
    const int big = 1300000;
    char * str = malloc(sizeof(char) * (big + 1));
    assert(!!str);
 
    for (int i = 0; i < big; i++)
      str[i] = '0' + (i % 3);
    str[big] = 0;
 
    int res = seq_add(t, str);
    CHECK(res == 1);
 
    res = seq_set_name(t, str, str);
    CHECK(res == 1);
 
    res = seq_equiv(t, str, "012012012");
    CHECK(res == 1);
 
    res = seq_valid(t, str);
    CHECK(res == 1);
 
    const char * res2 = seq_get_name(t, str);
    CHECK(strcmp(res2, str) == 0);
 
    const char * res3 = seq_get_name(t, "012012012");
    CHECK(strcmp(res3, str) == 0);
 
    str[big / 2] = 0;
 
    res = seq_remove(t, str);
    CHECK(res == 1);
 
    res = seq_remove(t, str);
    CHECK(res == 0);
 
    seq_delete(t);
 
    free(str);
 
    printf("TEST %d OK\n", testid);
  }
 
  printf("PRZESZLO!\n");
  return PASS;
}
 
// Testuje tworzenie klas abstrakcji i przypisywanie im nazw.
static int equivalence(void) {
  seq_t * seq = seq_new();
 
  TEST_FAIL(seq_equiv(seq, "0", "1"));
 
  TEST_PASS(seq_add(seq, "00"));
  TEST_FAIL(seq_equiv(seq, "00", "00"));
  TEST_FAIL(seq_equiv(seq, "00", "11"));
 
  TEST_PASS(seq_set_name(seq, "0", "zero"));
  TEST_COMP(seq_get_name(seq, "0"), "zero");
  TEST_FAIL(seq_set_name(seq, "0", "zero"));
  TEST_PASS(seq_set_name(seq, "0", "ZERO"));
  TEST_COMP(seq_get_name(seq, "0"), "ZERO");
  TEST_FAIL(seq_set_name(seq, "000", "trzy zera"));
  TEST_NULL_FAIL(seq_get_name(seq, "00"));
  TEST_NULL_FAIL(seq_get_name(seq, "1"));
 
  TEST_PASS(seq_add(seq, "11"));
 
  TEST_NULL_FAIL(seq_get_name(seq, "1"));
  TEST_NULL_FAIL(seq_get_name(seq, "11"));
 
  TEST_PASS(seq_equiv(seq, "0", "1"));
  TEST_FAIL(seq_equiv(seq, "0", "1"));
 
  TEST_COMP(seq_get_name(seq, "0"), "ZERO");
  TEST_COMP(seq_get_name(seq, "1"), "ZERO");
 
  TEST_PASS(seq_equiv(seq, "00", "11"));
 
  TEST_PASS(seq_set_name(seq, "1", "JEDEN"));
  TEST_COMP(seq_get_name(seq, "0"), "JEDEN");
  TEST_COMP(seq_get_name(seq, "1"), "JEDEN");
  TEST_PASS(seq_set_name(seq, "11", "DWA"));
  TEST_COMP(seq_get_name(seq, "00"), "DWA");
  TEST_COMP(seq_get_name(seq, "11"), "DWA");
 
  TEST_PASS(seq_equiv(seq, "11", "0"));
  TEST_COMP(seq_get_name(seq, "0"), "DWAJEDEN");
  TEST_COMP(seq_get_name(seq, "1"), "DWAJEDEN");
  TEST_COMP(seq_get_name(seq, "00"), "DWAJEDEN");
  TEST_COMP(seq_get_name(seq, "11"), "DWAJEDEN");
 
  TEST_FAIL(seq_equiv(seq, "11", "11"));
 
  seq_delete(seq);
  return PASS;
}
 
// Testuje reakcję implementacji na niepowodzenie alokacji pamięci.
static unsigned long alloc_fail_seq_new_seq_add(void) {
  unsigned long visited = 0;
  seq_t * seq;
  int result;
 
  if ((seq = seq_new()) != NULL)
    visited |= V(1, 0);
  else if (errno == ENOMEM && (seq = seq_new()) != NULL)
    visited |= V(2, 0);
  else
    return visited |= V(4, 0);
 
  if ((result = seq_add(seq, "012")) == 1)
    visited |= V(1, 1);
  else if (result == -1 && errno == ENOMEM && seq_valid(seq, "0") == 0 &&
    seq_valid(seq, "01") == 0 && seq_valid(seq, "012") == 0 &&
    seq_add(seq, "012") == 1)
    visited |= V(2, 1);
  else
    return visited |= V(4, 1);
 
  seq_delete(seq);
 
  return visited;
}
 
// Sprawdza reakcję implementacji na niepowodzenie alokacji pamięci.
static int memory_test(unsigned long( * test_function)(void)) {
  memory_test_data_t * mtd = get_memory_test_data();
 
  unsigned fail = 0, pass = 0;
  mtd -> call_total = 0;
  mtd -> fail_counter = 1;
  while (fail < 3 && pass < 3) {
    mtd -> call_counter = 0;
    mtd -> alloc_counter = 0;
    mtd -> free_counter = 0;
    mtd -> function_name = NULL;
    unsigned long visited_points = test_function();
    if (mtd -> alloc_counter != mtd -> free_counter ||
      (visited_points & 0444444444444444444444UL) != 0) {
      fprintf(stderr,
        "fail_counter %u, alloc_counter %u, free_counter %u, "
        "function_name %s, visited_point %lo\n",
        mtd -> fail_counter, mtd -> alloc_counter, mtd -> free_counter,
        mtd -> function_name, visited_points);
      ++fail;
    }
    if (mtd -> function_name == NULL)
      ++pass;
    else
      pass = 0;
    mtd -> fail_counter++;
  }
 
  return mtd -> call_total > 0 && fail == 0 ? PASS : FAIL;
}
 
static int memory(void) {
  return memory_test(alloc_fail_seq_new_seq_add);
}
 
/** URUCHAMIANIE TESTÓW **/
 
typedef struct {
  char
  const * name;
  int( * function)(void);
}
test_list_t;
 
#define TEST(t) {#t, t}
 
static
const test_list_t test_list[] = {
  TEST(params),
  TEST(simple),
  TEST(equivalence),
  TEST(memory),
  TEST(custom),
  TEST(losowe)
};
 
static int do_test(int( * function)(void)) {
  int result = function ();
  puts(get_magic_string());
  return result;
}
 
int main(int argc, char * argv[]) {
  if (argc == 2)
    for (size_t i = 0; i < SIZE(test_list); ++i)
      if (strcmp(argv[1], test_list[i].name) == 0)
        return do_test(test_list[i].function);
 
  fprintf(stderr, "Użycie:\n%s nazwa_testu\n", argv[0]);
  return WRONG_TEST;
}

