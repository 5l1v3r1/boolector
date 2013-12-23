/*  Boolector: Satisfiablity Modulo Theories (SMT) solver.
 *  Copyright (C) 2007-2012 Robert Daniel Brummayer, Armin Biere
 *  Copyright (C) 2012-2013 Aina Niemetz
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

#include "testinc.h"
#include "boolector.h"
#include "btorexit.h"
#include "testrunner.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <limits.h>

static Btor *g_btor = NULL;

static void
init_inc_test (void)
{
  g_btor = boolector_new ();
  if (g_rwreads) boolector_enable_beta_reduce_all (g_btor);
}

static void
finish_inc_test (void)
{
  boolector_delete (g_btor);
}

static void
test_inc_true_false (void)
{
  BoolectorNode *ff;
  BoolectorNode *tt;
  int res;

  init_inc_test ();

  ff = boolector_false (g_btor);
  tt = boolector_true (g_btor);
  boolector_enable_inc_usage (g_btor);
  boolector_assume (g_btor, tt);
  res = boolector_sat (g_btor);
  assert (res == BOOLECTOR_SAT);

  boolector_assume (g_btor, ff);
  res = boolector_sat (g_btor);
  assert (res == BOOLECTOR_UNSAT);
  assert (boolector_failed (g_btor, ff));

  boolector_release (g_btor, ff);
  boolector_release (g_btor, tt);
  finish_inc_test ();
}

static void
test_inc_counter (int w, int nondet)
{
  BoolectorNode *nonzero, *allzero, *one, *oracle;
  BoolectorNode *current, *next, *inc;
  char name[100];
  int i, res;

  assert (w > 0);

  init_inc_test ();

  boolector_enable_inc_usage (g_btor);
  one     = boolector_one (g_btor, w);
  current = boolector_zero (g_btor, w);
  i       = 0;

  for (;;)
  {
    inc = boolector_add (g_btor, current, one);

    if (nondet)
    {
      sprintf (name, "oracle%d", i);
      if (i)
        oracle = boolector_var (g_btor, 1, name);

      else
        oracle = boolector_true (g_btor);
      next = boolector_cond (g_btor, oracle, inc, current);
      boolector_release (g_btor, oracle);
    }
    else
      next = boolector_copy (g_btor, inc);

    boolector_release (g_btor, inc);
    boolector_release (g_btor, current);
    current = next;

    nonzero = boolector_redor (g_btor, current);
    allzero = boolector_not (g_btor, nonzero);
    boolector_release (g_btor, nonzero);

    i++;

    boolector_assume (g_btor, allzero);
    boolector_release (g_btor, allzero);

    res = boolector_sat (g_btor);
    if (res == BOOLECTOR_SAT) break;

    assert (res == BOOLECTOR_UNSAT);
    assert (boolector_failed (g_btor, allzero));
    assert (i < (1 << w));
  }

  assert (i == (1 << w));

  boolector_release (g_btor, one);
  boolector_release (g_btor, current);

  finish_inc_test ();
}

static void
test_inc_count1 (void)
{
  test_inc_counter (1, 0);
}

static void
test_inc_count2 (void)
{
  test_inc_counter (2, 0);
}

static void
test_inc_count3 (void)
{
  test_inc_counter (3, 0);
}

static void
test_inc_count4 (void)
{
  test_inc_counter (4, 0);
}

static void
test_inc_count8 (void)
{
  test_inc_counter (8, 0);
}

static void
test_inc_count1nondet (void)
{
  test_inc_counter (1, 1);
}

static void
test_inc_count2nondet (void)
{
  test_inc_counter (2, 1);
}

static void
test_inc_count3nondet (void)
{
  test_inc_counter (3, 1);
}

static void
test_inc_count4nondet (void)
{
  test_inc_counter (4, 1);
}

static void
test_inc_count8nondet (void)
{
  test_inc_counter (8, 1);
}

static void
test_inc_lt (int w)
{
  BoolectorNode *prev, *next, *lt;
  char name[100];
  int i, res;

  assert (w > 0);

  init_inc_test ();
  boolector_enable_inc_usage (g_btor);

  i    = 0;
  prev = 0;
  for (;;)
  {
    i++;

    sprintf (name, "%d", i);
    next = boolector_var (g_btor, w, name);

    if (prev)
    {
      lt = boolector_ult (g_btor, prev, next);
      boolector_assert (g_btor, lt);
      boolector_release (g_btor, lt);
      boolector_release (g_btor, prev);
    }

    prev = next;

    res = boolector_sat (g_btor);
    if (res == BOOLECTOR_UNSAT) break;

    assert (res == BOOLECTOR_SAT);
    assert (i <= (1 << w));
  }

  assert (i == (1 << w) + 1);

  boolector_release (g_btor, prev);

  finish_inc_test ();
}

static void
test_inc_lt1 (void)
{
  test_inc_lt (1);
}

static void
test_inc_lt2 (void)
{
  test_inc_lt (2);
}

static void
test_inc_lt3 (void)
{
  test_inc_lt (3);
}

static void
test_inc_lt4 (void)
{
  test_inc_lt (4);
}

static void
test_inc_lt8 (void)
{
  test_inc_lt (8);
}

static void
test_inc_assume_assert1 (void)
{
  int sat_result;
  BoolectorNode *array, *index1, *index2, *read1, *read2, *eq_index, *ne_read;

  init_inc_test ();
  boolector_enable_inc_usage (g_btor);
  boolector_set_rewrite_level (g_btor, 0);
  array    = boolector_array (g_btor, 1, 1, "array1");
  index1   = boolector_var (g_btor, 1, "index1");
  index2   = boolector_var (g_btor, 1, "index2");
  read1    = boolector_read (g_btor, array, index1);
  read2    = boolector_read (g_btor, array, index2);
  eq_index = boolector_eq (g_btor, index1, index2);
  ne_read  = boolector_ne (g_btor, read1, read2);
  boolector_assert (g_btor, ne_read);
  sat_result = boolector_sat (g_btor);
  assert (sat_result == BOOLECTOR_SAT);
  boolector_assume (g_btor, eq_index);
  sat_result = boolector_sat (g_btor);
  assert (sat_result == BOOLECTOR_UNSAT);
  assert (boolector_failed (g_btor, eq_index));
  sat_result = boolector_sat (g_btor);
  assert (sat_result == BOOLECTOR_SAT);
  boolector_release (g_btor, array);
  boolector_release (g_btor, index1);
  boolector_release (g_btor, index2);
  boolector_release (g_btor, read1);
  boolector_release (g_btor, read2);
  boolector_release (g_btor, eq_index);
  boolector_release (g_btor, ne_read);

  finish_inc_test ();
}

static void
test_inc_lemmas_on_demand_1 ()
{
  int sat_result;
  BoolectorNode *array, *index1, *index2, *read1, *read2, *eq, *ne;

  init_inc_test ();
  boolector_enable_inc_usage (g_btor);
  boolector_set_rewrite_level (g_btor, 0);
  array  = boolector_array (g_btor, 1, 1, "array1");
  index1 = boolector_var (g_btor, 1, "index1");
  index2 = boolector_var (g_btor, 1, "index2");
  read1  = boolector_read (g_btor, array, index1);
  read2  = boolector_read (g_btor, array, index2);
  eq     = boolector_eq (g_btor, index1, index2);
  ne     = boolector_ne (g_btor, read1, read2);
  boolector_assert (g_btor, eq);
  boolector_assume (g_btor, ne);
  sat_result = boolector_sat (g_btor);
  assert (sat_result == BOOLECTOR_UNSAT);
  assert (boolector_failed (g_btor, ne));
  sat_result = boolector_sat (g_btor);
  assert (sat_result == BOOLECTOR_SAT);
  boolector_release (g_btor, array);
  boolector_release (g_btor, index1);
  boolector_release (g_btor, index2);
  boolector_release (g_btor, read1);
  boolector_release (g_btor, read2);
  boolector_release (g_btor, eq);
  boolector_release (g_btor, ne);
  boolector_delete (g_btor);
}

void
init_inc_tests (void)
{
}

void
run_inc_tests (int argc, char **argv)
{
  BTOR_RUN_TEST (inc_true_false);
  BTOR_RUN_TEST (inc_count1);
  BTOR_RUN_TEST (inc_count2);
  BTOR_RUN_TEST (inc_count3);
  BTOR_RUN_TEST (inc_count4);
  BTOR_RUN_TEST (inc_count8);
  BTOR_RUN_TEST (inc_count1nondet);
  BTOR_RUN_TEST (inc_count2nondet);
  BTOR_RUN_TEST (inc_count3nondet);
  BTOR_RUN_TEST (inc_count4nondet);
  BTOR_RUN_TEST (inc_count8nondet);
  BTOR_RUN_TEST (inc_lt1);
  BTOR_RUN_TEST (inc_lt2);
  BTOR_RUN_TEST (inc_lt3);
  BTOR_RUN_TEST (inc_lt4);
  BTOR_RUN_TEST (inc_lt8);
  BTOR_RUN_TEST (inc_assume_assert1);
  BTOR_RUN_TEST (inc_lemmas_on_demand_1);
}

void
finish_inc_tests (void)
{
}
