#include "testoverflow.h"
#include "btorexit.h"
#include "btormain.h"
#include "btorutil.h"
#include "testrunner.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define BTOR_TEST_OVERFLOW_TEMP_FILE_NAME "of.tmp"

#define BTOR_TEST_OVERFLOW_U_LOW 1
#define BTOR_TEST_OVERFLOW_U_HIGH 8

#define BTOR_TEST_OVERFLOW_S_LOW 2
#define BTOR_TEST_OVERFLOW_S_HIGH 8

static int g_argc     = 3;
static char *g_argv[] = {
    "./boolector", "-q", BTOR_TEST_OVERFLOW_TEMP_FILE_NAME};

void
init_overflow_tests (void)
{
  FILE *f = fopen (BTOR_TEST_OVERFLOW_TEMP_FILE_NAME, "w");
  assert (f != NULL);
  fclose (f);
}

static void
u_overflow_test (int (*func) (int, int),
                 const char *func_name,
                 int low,
                 int high)
{
  FILE *f                = NULL;
  int i                  = 0;
  int j                  = 0;
  int result             = 0;
  int overflow_test      = 0;
  int overflow_boolector = 0;
  int num_bits           = 0;
  int max                = 0;
  assert (func != NULL);
  assert (func_name != NULL);
  assert (low > 0);
  assert (low <= high);
  BtorExitCode exit_code = 0;
  for (num_bits = low; num_bits <= high; num_bits++)
  {
    max = btor_pow_2_util (num_bits);
    for (i = 0; i < max; i++)
    {
      for (j = 0; j < max; j++)
      {
        overflow_test      = 0;
        overflow_boolector = 0;
        result             = func (i, j);
        if (result < 0 || result >= max) overflow_test = 1;
        f = fopen (BTOR_TEST_OVERFLOW_TEMP_FILE_NAME, "w");
        assert (f != NULL);
        fprintf (f, "1 %d constd %d\n", num_bits, i);
        fprintf (f, "2 %d constd %d\n", num_bits, j);
        fprintf (f, "3 1 %s 1 2\n", func_name);
        fprintf (f, "4 1 root 3\n");
        fclose (f);
        exit_code = btor_main (g_argc, g_argv);
        assert (exit_code == BTOR_SAT_EXIT || exit_code == BTOR_UNSAT_EXIT);
        if (exit_code == BTOR_SAT_EXIT) overflow_boolector = 1;
        if (overflow_boolector) assert (overflow_test);
        if (overflow_test) assert (overflow_boolector);
      }
    }
  }
}

static void
neg_overflow_test (int low, int high)
{
  FILE *f                = NULL;
  int i                  = 0;
  int overflow_test      = 0;
  int overflow_boolector = 0;
  int const_id           = 0;
  int result             = 0;
  int num_bits           = 0;
  int max                = 0;
  BtorExitCode exit_code = 0;
  assert (low > 1);
  assert (low <= high);
  for (num_bits = low; num_bits <= high; num_bits++)
  {
    max = btor_pow_2_util (num_bits - 1);
    for (i = -max; i < max; i++)
    {
      overflow_test      = 0;
      overflow_boolector = 0;
      result             = -i;
      if (!(result >= -max && result < max)) overflow_test = 1;
      f = fopen (BTOR_TEST_OVERFLOW_TEMP_FILE_NAME, "w");
      assert (f != NULL);
      if (i < 0)
      {
        fprintf (f, "1 %d constd %d\n", num_bits, -i);
        fprintf (f, "2 %d neg 1\n", num_bits);
        const_id = 2;
      }
      else
      {
        fprintf (f, "1 %d constd %d\n", num_bits, i);
        const_id = 1;
      }
      fprintf (f, "%d 1 nego %d\n", const_id + 1, const_id);
      fprintf (f, "%d 1 root %d\n", const_id + 2, const_id + 1);
      fclose (f);
      exit_code = btor_main (g_argc, g_argv);
      assert (exit_code == BTOR_SAT_EXIT || exit_code == BTOR_UNSAT_EXIT);
      if (exit_code == BTOR_SAT_EXIT) overflow_boolector = 1;
      if (overflow_boolector) assert (overflow_test);
      if (overflow_test) assert (overflow_boolector);
    }
  }
}

static void
s_overflow_test (int (*func) (int, int),
                 const char *func_name,
                 int exclude_second_zero,
                 int low,
                 int high)
{
  FILE *f                = NULL;
  int i                  = 0;
  int j                  = 0;
  int overflow_test      = 0;
  int overflow_boolector = 0;
  int const1_id          = 0;
  int const2_id          = 0;
  int result             = 0;
  int num_bits           = 0;
  int max                = 0;
  BtorExitCode exit_code = 0;
  assert (func != NULL);
  assert (func_name != NULL);
  assert (low > 1);
  assert (low <= high);
  for (num_bits = low; num_bits <= high; num_bits++)
  {
    max = btor_pow_2_util (num_bits - 1);
    for (i = -max; i < max; i++)
    {
      for (j = -max; j < max; j++)
      {
        if (!exclude_second_zero || j != 0)
        {
          overflow_test      = 0;
          overflow_boolector = 0;
          result             = func (i, j);
          if (!(result >= -max && result < max)) overflow_test = 1;
          f = fopen (BTOR_TEST_OVERFLOW_TEMP_FILE_NAME, "w");
          assert (f != NULL);
          if (i < 0)
          {
            fprintf (f, "1 %d constd %d\n", num_bits, -i);
            fprintf (f, "2 %d neg 1\n", num_bits);
            const1_id = 2;
          }
          else
          {
            fprintf (f, "1 %d constd %d\n", num_bits, i);
            const1_id = 1;
          }
          if (j < 0)
          {
            fprintf (f, "%d %d constd %d\n", const1_id + 1, num_bits, -j);
            fprintf (
                f, "%d %d neg %d\n", const1_id + 2, num_bits, const1_id + 1);
            const2_id = const1_id + 2;
          }
          else
          {
            fprintf (f, "%d %d constd %d\n", const1_id + 1, num_bits, j);
            const2_id = const1_id + 1;
          }

          fprintf (f,
                   "%d 1 %s %d %d\n",
                   const2_id + 1,
                   func_name,
                   const1_id,
                   const2_id);
          fprintf (f, "%d 1 root %d\n", const2_id + 2, const2_id + 1);
          fclose (f);
          exit_code = btor_main (g_argc, g_argv);
          assert (exit_code == BTOR_SAT_EXIT || exit_code == BTOR_UNSAT_EXIT);
          if (exit_code == BTOR_SAT_EXIT) overflow_boolector = 1;
          if (overflow_boolector) assert (overflow_test);
          if (overflow_test) assert (overflow_boolector);
        }
      }
    }
  }
}

static int
add (int x, int y)
{
  return x + y;
}

static int
sub (int x, int y)
{
  return x - y;
}

static int
mul (int x, int y)
{
  return x * y;
}

static int
divide (int x, int y)
{
  assert (y != 0);
  return x / y;
}

static void
test_uaddo_overflow (void)
{
  u_overflow_test (
      add, "uaddo", BTOR_TEST_OVERFLOW_U_LOW, BTOR_TEST_OVERFLOW_U_HIGH);
}

static void
test_usubo_overflow (void)
{
  u_overflow_test (
      sub, "usubo", BTOR_TEST_OVERFLOW_U_LOW, BTOR_TEST_OVERFLOW_U_HIGH);
}

static void
test_umulo_overflow (void)
{
  u_overflow_test (
      mul, "umulo", BTOR_TEST_OVERFLOW_U_LOW, BTOR_TEST_OVERFLOW_U_HIGH);
}

static void
test_nego_overflow (void)
{
  neg_overflow_test (BTOR_TEST_OVERFLOW_S_LOW, BTOR_TEST_OVERFLOW_S_HIGH);
}

static void
test_saddo_overflow (void)
{
  s_overflow_test (
      add, "saddo", 0, BTOR_TEST_OVERFLOW_S_LOW, BTOR_TEST_OVERFLOW_S_HIGH);
}

static void
test_ssubo_overflow (void)
{
  s_overflow_test (
      sub, "ssubo", 0, BTOR_TEST_OVERFLOW_S_LOW, BTOR_TEST_OVERFLOW_S_HIGH);
}

static void
test_smulo_overflow (void)
{
  s_overflow_test (
      mul, "smulo", 0, BTOR_TEST_OVERFLOW_S_LOW, BTOR_TEST_OVERFLOW_S_HIGH);
}

static void
test_sdivo_overflow (void)
{
  s_overflow_test (
      divide, "sdivo", 1, BTOR_TEST_OVERFLOW_S_LOW, BTOR_TEST_OVERFLOW_S_HIGH);
}

void
run_overflow_tests (int argc, char **argv)
{
  BTOR_RUN_TEST (uaddo_overflow);
  BTOR_RUN_TEST (usubo_overflow);
  BTOR_RUN_TEST (umulo_overflow);
  BTOR_RUN_TEST (nego_overflow);
  BTOR_RUN_TEST (saddo_overflow);
  BTOR_RUN_TEST (ssubo_overflow);
  BTOR_RUN_TEST (smulo_overflow);
  BTOR_RUN_TEST (sdivo_overflow);
}

void
finish_overflow_tests (void)
{
  int result = remove (BTOR_TEST_OVERFLOW_TEMP_FILE_NAME);
  assert (result == 0);
}
