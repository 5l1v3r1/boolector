#include "testmodelgen.h"
#include "testrunner.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
init_modelgen_tests (void)
{
}

static void
modelgen_test (const char *fname, int rwl)
{
  char *btor_fname, *log_fname, *syscall_string;
  size_t len;
  int ret_val;

  assert (rwl >= 0);
  assert (rwl <= 3);

  len = strlen (fname);

  btor_fname = (char *) malloc (sizeof (char) * (len + 6));
  sprintf (btor_fname, "%s.btor", fname);

  log_fname = (char *) malloc (sizeof (char) * (len + 5));
  sprintf (log_fname, "%s.log", fname);

  syscall_string = (char *) malloc (sizeof (char)
                                    * (+len + 5 + len + 4
                                       + strlen ("boolector -rwl3 -m log/")
                                       + strlen (" -o log/") + 1));

  sprintf (syscall_string,
           "boolector -rwl%d -m log/%s -o log/%s",
           rwl,
           btor_fname,
           log_fname);

  system (syscall_string);
  free (syscall_string);

  syscall_string = (char *) malloc (sizeof (char)
                                    * (len + 5 + len + 4
                                       + strlen ("contrib/btorcheckmodel log/")
                                       + strlen (" log/") + 1));

  sprintf (syscall_string,
           "contrib/btorcheckmodel log/%s log/%s",
           btor_fname,
           log_fname);
  ret_val = system (syscall_string);
  assert (ret_val == 0);

  free (syscall_string);
  free (log_fname);
  free (btor_fname);
}

static void
test_modelgen1 ()
{
  modelgen_test ("modelgen1", 1);
}

static void
test_modelgen2 ()
{
  modelgen_test ("modelgen2", 3);
}

static void
test_modelgen3 ()
{
  modelgen_test ("modelgen3", 3);
}

static void
test_modelgen4 ()
{
  modelgen_test ("modelgen4", 3);
}

static void
test_modelgen5 ()
{
  modelgen_test ("modelgen5", 3);
}

static void
test_modelgen6 ()
{
  modelgen_test ("modelgen6", 3);
}

static void
test_modelgen7 ()
{
  modelgen_test ("modelgen7", 3);
}

static void
test_modelgen8 ()
{
  modelgen_test ("modelgen8", 0);
}

static void
test_modelgen9 ()
{
  modelgen_test ("modelgen9", 3);
}

static void
test_modelgen10 ()
{
  modelgen_test ("modelgen10", 3);
}

static void
test_modelgen11 ()
{
  modelgen_test ("modelgen11", 3);
}

static void
test_modelgen12 ()
{
  modelgen_test ("modelgen12", 3);
}

static void
test_modelgen13 ()
{
  modelgen_test ("modelgen13", 3);
}

static void
test_modelgen14 ()
{
  modelgen_test ("modelgen14", 3);
}

static void
test_modelgen15 ()
{
  modelgen_test ("modelgen15", 3);
}

static void
test_modelgen16 ()
{
  modelgen_test ("modelgen16", 1);
}

static void
test_modelgen17 ()
{
  modelgen_test ("modelgen17", 1);
}

static void
test_modelgen18 ()
{
  modelgen_test ("modelgen18", 3);
}

static void
test_modelgen19 ()
{
  modelgen_test ("modelgen19", 2);
}

static void
test_modelgen20 ()
{
  modelgen_test ("modelgen20", 3);
}

static void
test_modelgen21 ()
{
  modelgen_test ("modelgen21", 3);
}

static void
test_modelgen22 ()
{
  modelgen_test ("modelgen22", 3);
}

static void
test_modelgen23 ()
{
  modelgen_test ("modelgen23", 3);
}

static void
test_modelgen24 ()
{
  modelgen_test ("modelgen24", 3);
}

static void
test_modelgen25 ()
{
  modelgen_test ("modelgen25", 3);
}

static void
test_modelgen26 ()
{
  modelgen_test ("modelgen26", 3);
}

static void
test_modelgen27 ()
{
  modelgen_test ("modelgen27", 3);
}

void
run_modelgen_tests (int argc, char **argv)
{
  BTOR_RUN_TEST (modelgen1);
  BTOR_RUN_TEST (modelgen2);
  BTOR_RUN_TEST (modelgen3);
  BTOR_RUN_TEST (modelgen4);
  BTOR_RUN_TEST (modelgen5);
  BTOR_RUN_TEST (modelgen6);
  BTOR_RUN_TEST (modelgen7);
  BTOR_RUN_TEST (modelgen8);
  BTOR_RUN_TEST (modelgen9);
  BTOR_RUN_TEST (modelgen10);
  BTOR_RUN_TEST (modelgen11);
  BTOR_RUN_TEST (modelgen12);
  BTOR_RUN_TEST (modelgen13);
  BTOR_RUN_TEST (modelgen14);
  BTOR_RUN_TEST (modelgen15);
  BTOR_RUN_TEST (modelgen16);
  BTOR_RUN_TEST (modelgen17);
  BTOR_RUN_TEST (modelgen18);
  BTOR_RUN_TEST (modelgen19);
  BTOR_RUN_TEST (modelgen20);
  BTOR_RUN_TEST (modelgen21);
  BTOR_RUN_TEST (modelgen22);
  BTOR_RUN_TEST (modelgen23);
  BTOR_RUN_TEST (modelgen24);
  BTOR_RUN_TEST (modelgen25);
  BTOR_RUN_TEST (modelgen26);
  BTOR_RUN_TEST (modelgen27);
}

void
finish_modelgen_tests (void)
{
}
