/*  Boolector: Satisfiablity Modulo Theories (SMT) solver.
 *  Copyright (C) 2007-2012 Robert Daniel Brummayer, Armin Biere
 *
 *  This file is part of Boolector.
 *
 *  Boolector is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Boolector is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "testrunner.h"
#include "btorexp.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int g_rwwrites;
FILE *g_logfile = NULL;

static int g_skip_broken;

static int g_speed = BTOR_NORMAL_TEST_CASE;
static int g_num_tests;
static int g_num_skipped_tests;
static int g_compared;
static int g_compared_succ;

static struct
{
  const char *std;
  const char *green;
  const char *blue;
  const char *red;
  char nl[100];
} terminal;

/* By default a test is 'fast'.  Tests that are a little bit slower but can
 * still be run in a regression run within a minute are 'normal' and are
 * listed below in 'normaltests'.  Slow tests take considerable more time
 * and are listed in 'slowtests'.  As a rule of thumb a slow test takes
 * defintely more than 10 seconds to run.
 */

static const char *slowtests[] = {
    "smtaxiombvsmod7",
    "smtaxiombvsmod8",
    "smtaxiombvsdiv7",
    "smtaxiombvsdiv8",
    "smtaxiombvsrem7",
    "smtaxiombvsrem8",
    "factor4294967295_special",
    "inc_count8nondet",
    "binarysearch32s016",
    "fifo32ia04k05",
    "mulassoc6",
    "hd10",
    "hd14",
    "problem_130",

    0, /* NOTE: DO NOT REMOVE AND KEEP AT SENTINEL */
};

/* Again these are the tests that are slightly faster than slow tests.  They
 * run in the order of 1 to 10 seconds each of them.  Fast tests definitely
 * take less than a second.
 */
static const char *normaltests[] = {
    "sll_shift",
    "srl_shift",
    "sra_shift",
    "rol_shift",
    "ror_shift",
    "sll_shift",
    "srl_shift",
    "sra_shift",
    "rol_shift",
    "ror_shift",
    "smtaxiombvsmod5",
    "smtaxiombvsmod6",
    "smtaxiombvsdiv5",
    "smtaxiombvsdiv6",
    "smtaxiombvsrem5",
    "smtaxiombvsrem6",
    "udiv8castdown6",
    "udiv8castdown7",
    "udiv16castdown8",
    "mulassoc6",
    "inc_lt8",
    "hd4",
    "hd9",
    "hd11",
    "headline1",
    "headline6",
    "headline7",
    "headline8",

    0, /* NOTE: DO NOT REMOVE AND KEEP AT SENTINEL */
};

static const char *brokentests[] = {
    0, /* NOTE: DO NOT REMOVE AND KEEP AT SENTINEL */
};

void
init_tests (BtorTestCaseSpeed speed, int skip_broken)
{
  g_skip_broken       = skip_broken;
  g_speed             = speed;
  g_num_tests         = 0;
  g_num_skipped_tests = 0;
  g_compared          = 0;
  g_compared_succ     = 0;

  if (isatty (1)) /* check for non bash terminals as well */
  {
    terminal.std   = "\e[0;39m";
    terminal.green = "\e[1;32m";
    terminal.blue  = "\e[1;34m";
    terminal.red   = "\e[1;31m";
    sprintf (terminal.nl, "\r%80s\r", "");
  }
  else
  {
    terminal.std   = "";
    terminal.green = "";
    terminal.blue  = "";
    terminal.red   = "";
    strcpy (terminal.nl, "\n");
  }
}

static void
nl (void)
{
  fputs (terminal.nl, stdout);
  fflush (stdout);
}

void
print_test_suite_name (const char *name)
{
  assert (name != NULL);
  printf ("Registered %s tests\n", name);
}

static int
cmp_file (const char *a, const char *b)
{
  FILE *f, *g;
  int res, c, d;

  assert (a);
  assert (b);

  f = fopen (a, "r");
  if (!f) return 0;

  g = fopen (b, "r");
  if (!g)
  {
    fclose (f);
    return 0;
  }

  do
  {
    c   = getc (f);
    d   = getc (g);
    res = (c == d);
  } while (res && c != EOF);

  fclose (f);
  fclose (g);

  return res;
}

static void
check_log (char *logfile_name, char *outfile_name)
{
  assert (logfile_name);
  assert (outfile_name);

  g_compared++;

  if (cmp_file (logfile_name, outfile_name))
  {
    nl ();
    g_compared_succ++;
  }
  else
  {
    printf ("  %s[ %sFAILED %s]%s\n",
            terminal.blue,
            terminal.red,
            terminal.blue,
            terminal.std);
  }
}

static int
match (const char *str, const char *pattern)
{
  return strstr (str, pattern) != NULL;
}

// currently unused (but maybe useful in the future)
#if 0
static int
find (const char *str, const char **testset, int testset_size)
{
  assert (testset_size > 0);

  int min_idx = 0, max_idx = testset_size - 1;
  int pivot = min_idx + (max_idx - min_idx) / 2;
  int cmp;

  while ((cmp = strcmp (str, testset[pivot])))
    {
      if (cmp < 0)
	max_idx = pivot - 1;
      else
	min_idx = pivot + 1;
      pivot = min_idx + (max_idx - min_idx) / 2;

      if (max_idx < min_idx)
	return 0;
    }

  return 1;
}
#endif

void
run_test_case (
    int argc, char **argv, void (*funcp) (void), char *name, int check_log_file)
{
  int i, count, skip, len;
  char *logfile_name, *outfile_name;
  const char **p;

  g_num_tests++;
  skip = 0;

  if (g_skip_broken)
    for (p = brokentests; !skip && *p; p++) skip = match (name, *p);

  if (g_speed < BTOR_NORMAL_TEST_CASE)
    for (p = normaltests; !skip && *p; p++) skip = match (name, *p);

  if (g_speed < BTOR_SLOW_TEST_CASE)
    for (p = slowtests; !skip && *p; p++) skip = match (name, *p);

  count      = 0;
  g_rwwrites = 0;
  for (i = 1; i < argc; i++)
  {
    count += (argv[i][0] != '-');
    if (strcmp (argv[i], "-r") == 0 || strcmp (argv[i], "--rww") == 0)
      g_rwwrites = 1;
  }

  if (!skip && count)
  {
    skip = 1;
    for (i = 1; skip && i < argc; i++)
      if (argv[i][0] != '-') skip = !match (name, argv[i]);
  }

  if (skip)
  {
    g_num_skipped_tests++;
    printf (" Skipping %s ", name);
  }
  else
  {
    printf (" Running %s ", name);

    fflush (stdout);
    fflush (stdout); /* for assertion failures */

    if (check_log_file)
    {
      len = 0;
      /* "log/" + name + ".log" or ".out" + \0 */
      len          = 4 + strlen (name) + 4 + 1;
      logfile_name = (char *) malloc (len);
      outfile_name = (char *) malloc (len);
      sprintf (logfile_name, "%s%s.log", "log/", name);
      sprintf (outfile_name, "%s%s.out", "log/", name);

      g_logfile = fopen (logfile_name, "w");
    }

    funcp ();

    if (check_log_file)
    {
      fclose (g_logfile);
      check_log (logfile_name, outfile_name);
      free (logfile_name);
      free (outfile_name);
    }
  }

  nl ();
}

void
finish_tests (void)
{
  nl ();

  printf ("%sFinished %d tests:\n%s",
          (g_compared == g_compared_succ) ? terminal.green : terminal.red,
          g_num_tests,
          terminal.std);
  if (g_num_skipped_tests > 0)
    printf ("  %sNumber of tests skipped: %d%s\n",
            terminal.blue,
            g_num_skipped_tests,
            terminal.std);
  printf ("  %sNumber of tests succeeded: %d%s\n",
          terminal.green,
          g_num_tests - g_num_skipped_tests,
          terminal.std);

  printf ("  %sNumber of files successfully compared: %d/%d%s\n",
          (g_compared == g_compared_succ) ? terminal.green : terminal.red,
          g_compared_succ,
          g_compared,
          terminal.std);
}
