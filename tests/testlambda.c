/*  Boolector: Satisfiablity Modulo Theories (SMT) solver.
 *  Copyright (C) 2012 Aina Niemetz, Mathias Preiner
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

#include "btorexp.h"
#include "btorutil.h"
#include "testexp.h"
#include "testrunner.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <stdio.h>

static Btor *g_btor   = NULL;
static int g_index_bw = 32;
static int g_elem_bw  = 16;

void
init_lambda_test (void)
{
  g_btor                 = btor_new_btor ();
  g_btor->rewrite_writes = g_rwwrites;
  g_btor->rewrite_reads  = g_rwreads;
}

void
finish_lambda_test (void)
{
  btor_delete_btor (g_btor);
}

void
init_lambda_tests (void)
{
}

static void
assert_parameterized (int argc, ...)
{
  int i;
  va_list ap;
  BtorNode *e;

  va_start (ap, argc);
  for (i = 0; i < argc; i++)
  {
    e = va_arg (ap, BtorNode *);
    assert (BTOR_REAL_ADDR_NODE (e)->parameterized);
  }
  va_end (ap);
}

static void
assert_not_parameterized (int argc, ...)
{
  int i;
  va_list ap;
  BtorNode *e;

  va_start (ap, argc);
  for (i = 0; i < argc; i++)
  {
    e = va_arg (ap, BtorNode *);
    assert (!BTOR_REAL_ADDR_NODE (e)->parameterized);
  }
  va_end (ap);
}

/*---------------------------------------------------------------------------
 * constant lambda tests
 *---------------------------------------------------------------------------*/

static void
test_lambda_const_lambda_const (void)
{
  init_lambda_test ();
  BtorNode *result;
  BtorNode *x      = btor_param_exp (g_btor, g_index_bw, "x");
  BtorNode *c      = btor_zero_exp (g_btor, g_elem_bw);
  BtorNode *i      = btor_var_exp (g_btor, g_index_bw, "i");
  BtorNode *lambda = btor_lambda_exp (g_btor, x, c);

  /* (lambda x . 0) (i) */
  result = btor_apply (g_btor, 1, &i, lambda);
  assert (result == c);
  assert_not_parameterized (1, result);
  btor_release_exp (g_btor, result);

  /* (lambda x . 0) () */
  result = btor_apply (g_btor, 0, 0, lambda);
  assert (result == c);
  assert_parameterized (1, x);
  assert_not_parameterized (4, result, c, i, lambda);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, i);
  btor_release_exp (g_btor, c);
  btor_release_exp (g_btor, x);
  finish_lambda_test ();
}

static void
test_lambda_const_lambda_var (void)
{
  init_lambda_test ();
  BtorNode *result;
  BtorNode *x      = btor_param_exp (g_btor, g_index_bw, "x");
  BtorNode *a      = btor_var_exp (g_btor, g_elem_bw, "a");
  BtorNode *i      = btor_var_exp (g_btor, g_index_bw, "i");
  BtorNode *lambda = btor_lambda_exp (g_btor, x, a);

  /* (lambda x . a) (i) */
  result = btor_apply (g_btor, 1, &i, lambda);
  assert (result == a);
  assert_not_parameterized (1, result);
  btor_release_exp (g_btor, result);

  /* (lambda x . a) () */
  result = btor_apply (g_btor, 0, 0, lambda);
  assert (result == a);
  assert_parameterized (1, x);
  assert_not_parameterized (4, result, lambda, i, a);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, a);
  btor_release_exp (g_btor, x);
  btor_release_exp (g_btor, i);
  finish_lambda_test ();
}

static void
test_lambda_const_lambda_param (void)
{
  init_lambda_test ();
  BtorNode *result;
  BtorNode *x      = btor_param_exp (g_btor, g_elem_bw, "x");
  BtorNode *a      = btor_var_exp (g_btor, g_elem_bw, "a");
  BtorNode *lambda = btor_lambda_exp (g_btor, x, x);

  /* (lambda x . x) (a) */
  result = btor_apply (g_btor, 1, &a, lambda);
  assert (result == a);
  assert_not_parameterized (1, result);
  btor_release_exp (g_btor, result);

  /* (lambda x . x) () */
  result = btor_apply (g_btor, 0, 0, lambda);
  assert (result == lambda);
  assert_parameterized (1, x);
  assert_not_parameterized (3, result, lambda, a);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, a);
  btor_release_exp (g_btor, x);
  finish_lambda_test ();
}

static void
test_lambda_const_lambda_negated (void)
{
  init_lambda_test ();
  BtorNode *result;
  BtorNode *a      = btor_var_exp (g_btor, g_elem_bw, "a");
  BtorNode *not_a  = btor_not_exp (g_btor, a);
  BtorNode *x      = btor_param_exp (g_btor, g_elem_bw, "x");
  BtorNode *not_x  = btor_not_exp (g_btor, x);
  BtorNode *lambda = btor_lambda_exp (g_btor, x, not_x);

  /* (lambda x . not (x)) (not (a)) */
  result = btor_apply (g_btor, 1, &not_a, lambda);
  assert (result == a);
  assert_not_parameterized (1, result);
  btor_release_exp (g_btor, result);

  /* (lambda x . not (x)) () */
  result = btor_apply (g_btor, 0, 0, lambda);
  assert (result == lambda);
  assert_parameterized (2, x, not_x);
  assert_not_parameterized (4, result, lambda, not_a, a);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, not_x);
  btor_release_exp (g_btor, x);
  btor_release_exp (g_btor, not_a);
  btor_release_exp (g_btor, a);
  finish_lambda_test ();
}

/* (lambda x . a) () */
static void
test_lambda_unassigned_param (void)
{
  init_lambda_test ();
  BtorNode *x      = btor_param_exp (g_btor, g_index_bw, "x");
  BtorNode *a      = btor_var_exp (g_btor, g_elem_bw, "a");
  BtorNode *lambda = btor_lambda_exp (g_btor, x, a);
  BtorNode *result = btor_apply (g_btor, 0, 0, lambda);

  assert (result == a);
  assert_parameterized (1, x);
  assert_not_parameterized (3, result, lambda, a);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, a);
  btor_release_exp (g_btor, x);
  finish_lambda_test ();
}

/*---------------------------------------------------------------------------
 * unary expression tests
 *---------------------------------------------------------------------------*/

static void
unary_param_exp_test (BtorNode *(*func) (Btor *, BtorNode *) )
{
  init_lambda_test ();
  int lambda_index_bw = g_elem_bw;

  BtorNode *var       = btor_var_exp (g_btor, g_elem_bw, "v1");
  BtorNode *expected  = func (g_btor, var);
  BtorNode *param     = btor_param_exp (g_btor, lambda_index_bw, "p1");
  BtorNode *param_exp = func (g_btor, param);
  BtorNode *lambda    = btor_lambda_exp (g_btor, param, param_exp);
  BtorNode *result    = btor_apply (g_btor, 1, &var, lambda);

  assert (result == expected);
  assert_parameterized (2, param, param_exp);
  assert_not_parameterized (4, var, expected, lambda, result);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, param_exp);
  btor_release_exp (g_btor, param);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, var);
  finish_lambda_test ();
}

static void
test_lambda_param_not (void)
{
  unary_param_exp_test (btor_not_exp);
}

static void
test_lambda_param_neg (void)
{
  unary_param_exp_test (btor_neg_exp);
}

static void
test_lambda_param_redor (void)
{
  unary_param_exp_test (btor_redor_exp);
}

static void
test_lambda_param_redxor (void)
{
  unary_param_exp_test (btor_redxor_exp);
}

static void
test_lambda_param_redand (void)
{
  unary_param_exp_test (btor_redand_exp);
}

static void
test_lambda_param_slice (void)
{
  init_lambda_test ();
  int lower          = g_elem_bw / 2 + 1;
  int upper          = g_elem_bw - 1;
  BtorNode *var      = btor_var_exp (g_btor, g_elem_bw, "v1");
  BtorNode *param    = btor_param_exp (g_btor, g_elem_bw, "p1");
  BtorNode *expected = btor_slice_exp (g_btor, var, upper, lower);
  BtorNode *slice    = btor_slice_exp (g_btor, param, upper, lower);
  BtorNode *lambda   = btor_lambda_exp (g_btor, param, slice);
  BtorNode *result   = btor_apply (g_btor, 1, &var, lambda);

  assert (result == expected);
  assert_parameterized (2, param, slice);
  assert_not_parameterized (4, var, expected, lambda, result);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, slice);
  btor_release_exp (g_btor, param);
  btor_release_exp (g_btor, var);
  finish_lambda_test ();
}

static void
param_extension_test (BtorNode *(*func) (Btor *, BtorNode *, int) )
{
  init_lambda_test ();
  int lower           = g_elem_bw / 2 + 1;
  int upper           = g_elem_bw - 1;
  BtorNode *var       = btor_var_exp (g_btor, lower, "v1");
  BtorNode *param     = btor_param_exp (g_btor, lower, "p1");
  BtorNode *expected  = func (g_btor, var, upper);
  BtorNode *param_exp = func (g_btor, param, upper);
  BtorNode *lambda    = btor_lambda_exp (g_btor, param, param_exp);
  BtorNode *result    = btor_apply (g_btor, 1, &var, lambda);

  assert (result == expected);
  assert_parameterized (2, param, param_exp);
  assert_not_parameterized (4, var, expected, lambda, result);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, param_exp);
  btor_release_exp (g_btor, param);
  btor_release_exp (g_btor, var);
  finish_lambda_test ();
}

static void
test_lambda_param_uext (void)
{
  param_extension_test (btor_uext_exp);
}

static void
test_lambda_param_sext (void)
{
  param_extension_test (btor_sext_exp);
}

/*---------------------------------------------------------------------------
 * binary expression tests
 *---------------------------------------------------------------------------*/

/* (lambda x . bin_exp (x, v2)) (v1) or (lambda x . bin_exp (v1, x)) (v2) */
static void
binary_param_exp_test (int param_pos,
                       BtorNode *(*func) (Btor *, BtorNode *, BtorNode *) )
{
  assert (param_pos == 0 || param_pos == 1);

  init_lambda_test ();
  int x_bw  = g_elem_bw;
  int v1_bw = g_elem_bw;
  int v2_bw = g_elem_bw;
  BtorNode *result;

  if (func == btor_implies_exp || func == btor_iff_exp)
  {
    v1_bw = 1;
    v2_bw = 1;
  }
  else if (func == btor_sll_exp || func == btor_srl_exp || func == btor_sra_exp
           || func == btor_rol_exp || func == btor_ror_exp)
  {
    v2_bw = btor_log_2_util (v1_bw);
  }

  x_bw = (param_pos == 0) ? v1_bw : v2_bw;

  BtorNode *param_exp;
  BtorNode *v1       = btor_var_exp (g_btor, v1_bw, "v1");
  BtorNode *v2       = btor_var_exp (g_btor, v2_bw, "v2");
  BtorNode *expected = func (g_btor, v1, v2);
  BtorNode *x        = btor_param_exp (g_btor, x_bw, "x");

  if (param_pos == 0)
    param_exp = func (g_btor, x, v2);
  else
    param_exp = func (g_btor, v1, x);

  BtorNode *lambda = btor_lambda_exp (g_btor, x, param_exp);

  if (param_pos == 0)
    result = btor_apply (g_btor, 1, &v1, lambda);
  else
    result = btor_apply (g_btor, 1, &v2, lambda);

  assert (result == expected);
  assert_parameterized (2, x, param_exp);
  assert_not_parameterized (5, v1, v2, expected, lambda, result);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, param_exp);
  btor_release_exp (g_btor, x);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, v2);
  btor_release_exp (g_btor, v1);
  finish_lambda_test ();
}

static void
test_lambda_param_implies (void)
{
  binary_param_exp_test (0, btor_implies_exp);
  binary_param_exp_test (1, btor_implies_exp);
}

static void
test_lambda_param_iff (void)
{
  binary_param_exp_test (0, btor_iff_exp);
  binary_param_exp_test (1, btor_iff_exp);
}

static void
test_lambda_param_xor (void)
{
  binary_param_exp_test (0, btor_xor_exp);
  binary_param_exp_test (1, btor_xor_exp);
}

static void
test_lambda_param_xnor (void)
{
  binary_param_exp_test (0, btor_xnor_exp);
  binary_param_exp_test (1, btor_xnor_exp);
}

static void
test_lambda_param_and (void)
{
  binary_param_exp_test (0, btor_and_exp);
  binary_param_exp_test (1, btor_and_exp);
}

static void
test_lambda_param_nand (void)
{
  binary_param_exp_test (0, btor_nand_exp);
  binary_param_exp_test (1, btor_nand_exp);
}

static void
test_lambda_param_or (void)
{
  binary_param_exp_test (0, btor_or_exp);
  binary_param_exp_test (1, btor_or_exp);
}

static void
test_lambda_param_nor (void)
{
  binary_param_exp_test (0, btor_nor_exp);
  binary_param_exp_test (1, btor_nor_exp);
}

static void
test_lambda_param_eq (void)
{
  binary_param_exp_test (0, btor_eq_exp);
  binary_param_exp_test (1, btor_eq_exp);
}

static void
test_lambda_param_ne (void)
{
  binary_param_exp_test (0, btor_ne_exp);
  binary_param_exp_test (1, btor_ne_exp);
}

static void
test_lambda_param_add (void)
{
  binary_param_exp_test (0, btor_add_exp);
  binary_param_exp_test (1, btor_add_exp);
}

static void
test_lambda_param_uaddo (void)
{
  binary_param_exp_test (0, btor_uaddo_exp);
  binary_param_exp_test (1, btor_uaddo_exp);
}

static void
test_lambda_param_saddo (void)
{
  binary_param_exp_test (0, btor_saddo_exp);
  binary_param_exp_test (1, btor_saddo_exp);
}

static void
test_lambda_param_mul (void)
{
  binary_param_exp_test (0, btor_mul_exp);
  binary_param_exp_test (1, btor_mul_exp);
}

static void
test_lambda_param_umulo (void)
{
  binary_param_exp_test (0, btor_umulo_exp);
  binary_param_exp_test (1, btor_umulo_exp);
}

static void
test_lambda_param_smulo (void)
{
  binary_param_exp_test (0, btor_smulo_exp);
  binary_param_exp_test (1, btor_smulo_exp);
}

static void
test_lambda_param_ult (void)
{
  binary_param_exp_test (0, btor_ult_exp);
  binary_param_exp_test (1, btor_ult_exp);
}

static void
test_lambda_param_slt (void)
{
  binary_param_exp_test (0, btor_slt_exp);
  binary_param_exp_test (1, btor_slt_exp);
}

static void
test_lambda_param_ulte (void)
{
  binary_param_exp_test (0, btor_ulte_exp);
  binary_param_exp_test (1, btor_ulte_exp);
}

static void
test_lambda_param_slte (void)
{
  binary_param_exp_test (0, btor_slte_exp);
  binary_param_exp_test (1, btor_slte_exp);
}

static void
test_lambda_param_ugt (void)
{
  binary_param_exp_test (0, btor_ugt_exp);
  binary_param_exp_test (1, btor_ugt_exp);
}

static void
test_lambda_param_sgt (void)
{
  binary_param_exp_test (0, btor_sgt_exp);
  binary_param_exp_test (1, btor_sgt_exp);
}

static void
test_lambda_param_ugte (void)
{
  binary_param_exp_test (0, btor_ugte_exp);
  binary_param_exp_test (1, btor_ugte_exp);
}

static void
test_lambda_param_sgte (void)
{
  binary_param_exp_test (0, btor_sgte_exp);
  binary_param_exp_test (1, btor_sgte_exp);
}

static void
test_lambda_param_sll (void)
{
  binary_param_exp_test (0, btor_sll_exp);
  binary_param_exp_test (1, btor_sll_exp);
}

static void
test_lambda_param_srl (void)
{
  binary_param_exp_test (0, btor_srl_exp);
  binary_param_exp_test (1, btor_srl_exp);
}

static void
test_lambda_param_sra (void)
{
  binary_param_exp_test (0, btor_sra_exp);
  binary_param_exp_test (1, btor_sra_exp);
}

static void
test_lambda_param_rol (void)
{
  binary_param_exp_test (0, btor_rol_exp);
  binary_param_exp_test (1, btor_rol_exp);
}

static void
test_lambda_param_ror (void)
{
  binary_param_exp_test (0, btor_ror_exp);
  binary_param_exp_test (1, btor_ror_exp);
}

static void
test_lambda_param_sub (void)
{
  binary_param_exp_test (0, btor_sub_exp);
  binary_param_exp_test (1, btor_sub_exp);
}

static void
test_lambda_param_usubo (void)
{
  binary_param_exp_test (0, btor_usubo_exp);
  binary_param_exp_test (1, btor_usubo_exp);
}

static void
test_lambda_param_ssubo (void)
{
  binary_param_exp_test (0, btor_ssubo_exp);
  binary_param_exp_test (1, btor_ssubo_exp);
}

static void
test_lambda_param_udiv (void)
{
  binary_param_exp_test (0, btor_udiv_exp);
  binary_param_exp_test (1, btor_udiv_exp);
}

static void
test_lambda_param_sdiv (void)
{
  binary_param_exp_test (0, btor_sdiv_exp);
  binary_param_exp_test (1, btor_sdiv_exp);
}

static void
test_lambda_param_sdivo (void)
{
  binary_param_exp_test (0, btor_sdivo_exp);
  binary_param_exp_test (1, btor_sdivo_exp);
}

static void
test_lambda_param_urem (void)
{
  binary_param_exp_test (0, btor_urem_exp);
  binary_param_exp_test (1, btor_urem_exp);
}

static void
test_lambda_param_srem (void)
{
  binary_param_exp_test (0, btor_srem_exp);
  binary_param_exp_test (1, btor_srem_exp);
}

static void
test_lambda_param_smod (void)
{
  binary_param_exp_test (0, btor_smod_exp);
  binary_param_exp_test (1, btor_smod_exp);
}

static void
test_lambda_param_concat (void)
{
  binary_param_exp_test (0, btor_concat_exp);
  binary_param_exp_test (1, btor_concat_exp);
}

/* (lambda x . read(a, x)) (i) */
static void
test_lambda_param_read (void)
{
  init_lambda_test ();
  BtorNode *x        = btor_param_exp (g_btor, g_index_bw, "x");
  BtorNode *i        = btor_var_exp (g_btor, g_index_bw, "i");
  BtorNode *a        = btor_array_exp (g_btor, g_elem_bw, g_index_bw, "a");
  BtorNode *expected = btor_read_exp (g_btor, a, i);
  BtorNode *read     = btor_read_exp (g_btor, a, x);
  BtorNode *lambda   = btor_lambda_exp (g_btor, x, read);
  BtorNode *result   = btor_apply (g_btor, 1, &i, lambda);

  assert (result == expected);
  assert_parameterized (2, x, read);
  assert_not_parameterized (5, result, lambda, expected, a, i);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, read);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, a);
  btor_release_exp (g_btor, i);
  btor_release_exp (g_btor, x);
  finish_lambda_test ();
}

/*---------------------------------------------------------------------------
 * ternary expression tests
 *---------------------------------------------------------------------------*/

/* (lambda x . write (a, i, x)) (e) */
static void
test_lambda_param_write1 (void)
{
  init_lambda_test ();
  int index_bw        = g_elem_bw;
  int elem_bw         = g_elem_bw;
  BtorNode *i         = btor_var_exp (g_btor, index_bw, "i");
  BtorNode *e         = btor_var_exp (g_btor, elem_bw, "e");
  BtorNode *a         = btor_array_exp (g_btor, elem_bw, index_bw, "a");
  BtorNode *expected  = btor_write_exp (g_btor, a, i, e);
  BtorNode *x         = btor_param_exp (g_btor, elem_bw, "x");
  BtorNode *param_exp = btor_write_exp (g_btor, a, i, x);
  BtorNode *lambda    = btor_lambda_exp (g_btor, x, param_exp);
  BtorNode *result    = btor_apply (g_btor, 1, &e, lambda);

  assert (result == expected);
  assert_parameterized (2, x, param_exp);
  assert_not_parameterized (6, result, lambda, expected, a, e, i);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, param_exp);
  btor_release_exp (g_btor, x);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, a);
  btor_release_exp (g_btor, e);
  btor_release_exp (g_btor, i);
  finish_lambda_test ();
}

/* (lambda x . write (a, x, e)) (i) */
static void
test_lambda_param_write2 (void)
{
  init_lambda_test ();
  BtorNode *i         = btor_var_exp (g_btor, g_index_bw, "i");
  BtorNode *e         = btor_var_exp (g_btor, g_elem_bw, "e");
  BtorNode *a         = btor_array_exp (g_btor, g_elem_bw, g_index_bw, "a");
  BtorNode *expected  = btor_write_exp (g_btor, a, i, e);
  BtorNode *x         = btor_param_exp (g_btor, g_index_bw, "p");
  BtorNode *param_exp = btor_write_exp (g_btor, a, x, e);
  BtorNode *lambda    = btor_lambda_exp (g_btor, x, param_exp);
  BtorNode *result    = btor_apply (g_btor, 1, &i, lambda);

  assert (result == expected);
  assert_parameterized (2, x, param_exp);
  assert_not_parameterized (6, result, lambda, expected, a, e, i);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, param_exp);
  btor_release_exp (g_btor, x);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, a);
  btor_release_exp (g_btor, e);
  btor_release_exp (g_btor, i);
  finish_lambda_test ();
}

/* (lambda x . x ? v2 : v3) (v1) */
static void
test_lambda_param_bcond1 (void)
{
  init_lambda_test ();
  BtorNode *v1        = btor_var_exp (g_btor, 1, "v1");
  BtorNode *x         = btor_param_exp (g_btor, 1, "x");
  BtorNode *v2        = btor_var_exp (g_btor, g_elem_bw, "v2");
  BtorNode *v3        = btor_var_exp (g_btor, g_elem_bw, "v3");
  BtorNode *expected  = btor_cond_exp (g_btor, v1, v2, v3);
  BtorNode *param_exp = btor_cond_exp (g_btor, x, v2, v3);
  BtorNode *lambda    = btor_lambda_exp (g_btor, x, param_exp);
  BtorNode *result    = btor_apply (g_btor, 1, &v1, lambda);

  assert (result == expected);
  assert_parameterized (2, x, param_exp);
  assert_not_parameterized (6, result, lambda, expected, v3, v2, v1);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, param_exp);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, v3);
  btor_release_exp (g_btor, v2);
  btor_release_exp (g_btor, x);
  btor_release_exp (g_btor, v1);
  finish_lambda_test ();
}

/* (lambda x . v1 ? x : v3) (v2) */
static void
test_lambda_param_bcond2 (void)
{
  init_lambda_test ();
  BtorNode *v1        = btor_var_exp (g_btor, 1, "v1");
  BtorNode *x         = btor_param_exp (g_btor, g_elem_bw, "x");
  BtorNode *v2        = btor_var_exp (g_btor, g_elem_bw, "v2");
  BtorNode *v3        = btor_var_exp (g_btor, g_elem_bw, "v3");
  BtorNode *expected  = btor_cond_exp (g_btor, v1, v2, v3);
  BtorNode *param_exp = btor_cond_exp (g_btor, v1, x, v3);
  BtorNode *lambda    = btor_lambda_exp (g_btor, x, param_exp);
  BtorNode *result    = btor_apply (g_btor, 1, &v2, lambda);

  assert (result == expected);
  assert_parameterized (2, x, param_exp);
  assert_not_parameterized (6, result, lambda, expected, v3, v2, v1);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, param_exp);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, v3);
  btor_release_exp (g_btor, v2);
  btor_release_exp (g_btor, x);
  btor_release_exp (g_btor, v1);
  finish_lambda_test ();
}

/* (lambda x . v1 ? v2 : x) (v3) */
static void
test_lambda_param_bcond3 (void)
{
  init_lambda_test ();
  BtorNode *v1        = btor_var_exp (g_btor, 1, "v1");
  BtorNode *x         = btor_param_exp (g_btor, g_elem_bw, "x");
  BtorNode *v2        = btor_var_exp (g_btor, g_elem_bw, "v2");
  BtorNode *v3        = btor_var_exp (g_btor, g_elem_bw, "v3");
  BtorNode *expected  = btor_cond_exp (g_btor, v1, v2, v3);
  BtorNode *param_exp = btor_cond_exp (g_btor, v1, v2, x);
  BtorNode *lambda    = btor_lambda_exp (g_btor, x, param_exp);
  BtorNode *result    = btor_apply (g_btor, 1, &v3, lambda);

  assert (result == expected);
  assert_parameterized (2, x, param_exp);
  assert_not_parameterized (6, result, lambda, expected, v3, v2, v1);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, param_exp);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, v3);
  btor_release_exp (g_btor, v2);
  btor_release_exp (g_btor, x);
  btor_release_exp (g_btor, v1);
  finish_lambda_test ();
}

/* NOTE: right now, we have to use a read on the array condition as lambdas
 * return bit-vectors only. */
static void
test_lambda_param_acond (void)
{
  init_lambda_test ();
  BtorNode *var           = btor_var_exp (g_btor, g_index_bw, "v1");
  BtorNode *index         = btor_var_exp (g_btor, g_index_bw, "i");
  BtorNode *expected_cond = btor_eq_exp (g_btor, var, index);
  BtorNode *e_if   = btor_array_exp (g_btor, g_elem_bw, g_index_bw, "a1");
  BtorNode *e_else = btor_array_exp (g_btor, g_elem_bw, g_index_bw, "a2");
  BtorNode *expected_acond =
      btor_cond_exp (g_btor, expected_cond, e_if, e_else);
  BtorNode *expected = btor_read_exp (g_btor, expected_acond, var);

  BtorNode *param       = btor_param_exp (g_btor, g_index_bw, "p");
  BtorNode *param_cond  = btor_eq_exp (g_btor, param, index);
  BtorNode *param_acond = btor_cond_exp (g_btor, param_cond, e_if, e_else);
  BtorNode *param_exp   = btor_read_exp (g_btor, param_acond, param);
  BtorNode *lambda      = btor_lambda_exp (g_btor, param, param_exp);
  BtorNode *result      = btor_apply (g_btor, 1, &var, lambda);

  assert (result == expected);
  assert_parameterized (4, param, param_cond, param_acond, param_exp);
  assert_not_parameterized (4, result, lambda, expected, expected_acond);
  assert_not_parameterized (5, e_else, e_if, expected_cond, index, var);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, param_exp);
  btor_release_exp (g_btor, param_acond);
  btor_release_exp (g_btor, param_cond);
  btor_release_exp (g_btor, param);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, expected_acond);
  btor_release_exp (g_btor, e_else);
  btor_release_exp (g_btor, e_if);
  btor_release_exp (g_btor, expected_cond);
  btor_release_exp (g_btor, index);
  btor_release_exp (g_btor, var);
  finish_lambda_test ();
}

// TODO: arrays as arguments (not possible yet)

/*---------------------------------------------------------------------------
 * reduction tests (with reduced expressions)
 *---------------------------------------------------------------------------*/

static void
test_lambda_reduce_write1 (void)
{
  init_lambda_test ();
  BtorNode *a      = btor_array_exp (g_btor, g_elem_bw, g_index_bw, "a");
  BtorNode *i      = btor_var_exp (g_btor, g_index_bw, "i");
  BtorNode *e      = btor_var_exp (g_btor, g_elem_bw, "e");
  BtorNode *param  = btor_param_exp (g_btor, g_index_bw, "p");
  BtorNode *read   = btor_read_exp (g_btor, a, param);
  BtorNode *eq     = btor_eq_exp (g_btor, param, i);
  BtorNode *cond   = btor_cond_exp (g_btor, eq, e, read);
  BtorNode *lambda = btor_lambda_exp (g_btor, param, cond);
  BtorNode *result = btor_apply (g_btor, 1, &i, lambda);

  assert (result == e);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, cond);
  btor_release_exp (g_btor, eq);
  btor_release_exp (g_btor, read);
  btor_release_exp (g_btor, param);
  btor_release_exp (g_btor, e);
  btor_release_exp (g_btor, i);
  btor_release_exp (g_btor, a);
  finish_lambda_test ();
}

static void
test_lambda_reduce_write2 (void)
{
  init_lambda_test ();
  BtorNode *a        = btor_array_exp (g_btor, g_elem_bw, g_index_bw, "a");
  BtorNode *i        = btor_var_exp (g_btor, g_index_bw, "i");
  BtorNode *e        = btor_var_exp (g_btor, g_elem_bw, "e");
  BtorNode *param    = btor_param_exp (g_btor, g_index_bw, "p");
  BtorNode *read     = btor_read_exp (g_btor, a, param);
  BtorNode *expected = btor_read_exp (g_btor, a, i);
  BtorNode *eq       = btor_ne_exp (g_btor, param, i);
  BtorNode *cond     = btor_cond_exp (g_btor, eq, e, read);
  BtorNode *lambda   = btor_lambda_exp (g_btor, param, cond);
  BtorNode *result   = btor_apply (g_btor, 1, &i, lambda);

  assert (result == expected);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda);
  btor_release_exp (g_btor, cond);
  btor_release_exp (g_btor, eq);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, read);
  btor_release_exp (g_btor, param);
  btor_release_exp (g_btor, e);
  btor_release_exp (g_btor, i);
  btor_release_exp (g_btor, a);
  finish_lambda_test ();
}

static void
test_lambda_reduce_nested_writes (void)
{
  init_lambda_test ();
  BtorNode *i = btor_var_exp (g_btor, g_index_bw, "i");
  /* w1 = write (a, i, e2) */
  BtorNode *a       = btor_array_exp (g_btor, g_elem_bw, g_index_bw, "a");
  BtorNode *e2      = btor_var_exp (g_btor, g_elem_bw, "e2");
  BtorNode *param2  = btor_param_exp (g_btor, g_index_bw, "p2");
  BtorNode *read2   = btor_read_exp (g_btor, a, param2);
  BtorNode *eq2     = btor_eq_exp (g_btor, param2, i);
  BtorNode *cond2   = btor_cond_exp (g_btor, eq2, e2, read2);
  BtorNode *lambda2 = btor_lambda_exp (g_btor, param2, cond2);
  /* w2 = write (w1, not i, e1) */
  BtorNode *e1      = btor_var_exp (g_btor, g_elem_bw, "e1");
  BtorNode *param1  = btor_param_exp (g_btor, g_index_bw, "p1");
  BtorNode *read1   = btor_read_exp (g_btor, lambda2, param1);
  BtorNode *eq1     = btor_ne_exp (g_btor, param1, i);
  BtorNode *cond1   = btor_cond_exp (g_btor, eq1, e1, read1);
  BtorNode *lambda1 = btor_lambda_exp (g_btor, param1, cond1);
  BtorNode *result  = btor_apply (g_btor, 1, &i, lambda1);

  assert (result == e2);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda1);
  btor_release_exp (g_btor, cond1);
  btor_release_exp (g_btor, eq1);
  btor_release_exp (g_btor, read1);
  btor_release_exp (g_btor, param1);
  btor_release_exp (g_btor, e1);
  btor_release_exp (g_btor, lambda2);
  btor_release_exp (g_btor, cond2);
  btor_release_exp (g_btor, eq2);
  btor_release_exp (g_btor, read2);
  btor_release_exp (g_btor, param2);
  btor_release_exp (g_btor, e2);
  btor_release_exp (g_btor, a);
  btor_release_exp (g_btor, i);
  finish_lambda_test ();
}

/* (lambda x . (lambda y . (x + y))) (a b) */
static void
test_lambda_reduce_nested_lambdas_add1 (void)
{
  init_lambda_test ();
  BtorNode *a        = btor_var_exp (g_btor, g_elem_bw, "a");
  BtorNode *b        = btor_var_exp (g_btor, g_elem_bw, "b");
  BtorNode *expected = btor_add_exp (g_btor, a, b);
  BtorNode *x        = btor_param_exp (g_btor, g_elem_bw, "x");
  BtorNode *y        = btor_param_exp (g_btor, g_elem_bw, "y");
  BtorNode *add      = btor_add_exp (g_btor, x, y);
  BtorNode *lambda2  = btor_lambda_exp (g_btor, y, add);
  BtorNode *lambda1  = btor_lambda_exp (g_btor, x, lambda2);
  BtorNode *params[2];

  params[0]        = a;
  params[1]        = b;
  BtorNode *result = btor_apply (g_btor, 2, params, lambda1);

  assert (result == expected);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda1);
  btor_release_exp (g_btor, lambda2);
  btor_release_exp (g_btor, add);
  btor_release_exp (g_btor, y);
  btor_release_exp (g_btor, x);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, b);
  btor_release_exp (g_btor, a);
  finish_lambda_test ();
}

/* (lambda x . (x + read(lambda y . y, b))) (a) */
static void
test_lambda_reduce_nested_lambdas_add2 (void)
{
  init_lambda_test ();
  int lambda_index_bw = g_elem_bw;
  BtorNode *a         = btor_var_exp (g_btor, g_elem_bw, "a");
  BtorNode *b         = btor_var_exp (g_btor, g_elem_bw, "b");
  BtorNode *expected  = btor_add_exp (g_btor, a, b);
  BtorNode *x         = btor_param_exp (g_btor, lambda_index_bw, "x");
  BtorNode *y         = btor_param_exp (g_btor, lambda_index_bw, "y");
  BtorNode *lambda2   = btor_lambda_exp (g_btor, y, y);
  BtorNode *read      = btor_read_exp (g_btor, lambda2, b);
  BtorNode *add       = btor_add_exp (g_btor, x, read);
  BtorNode *lambda1   = btor_lambda_exp (g_btor, x, add);
  BtorNode *result    = btor_apply (g_btor, 1, &a, lambda1);

  assert (result == expected);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda1);
  btor_release_exp (g_btor, add);
  btor_release_exp (g_btor, read);
  btor_release_exp (g_btor, lambda2);
  btor_release_exp (g_btor, y);
  btor_release_exp (g_btor, x);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, b);
  btor_release_exp (g_btor, a);
  finish_lambda_test ();
}

/* (lambda x . not (read (lambda y . y, x + var))) (a) */
static void
test_lambda_reduce_nested_lambdas_read (void)
{
  init_lambda_test ();
  BtorNode *var     = btor_var_exp (g_btor, g_elem_bw, "var");
  BtorNode *y       = btor_param_exp (g_btor, g_elem_bw, "y");
  BtorNode *lambda2 = btor_lambda_exp (g_btor, y, y);
  BtorNode *x       = btor_param_exp (g_btor, g_elem_bw, "x");
  BtorNode *add     = btor_add_exp (g_btor, x, var);
  BtorNode *read    = btor_read_exp (g_btor, lambda2, add);
  BtorNode *nread   = btor_not_exp (g_btor, read);
  BtorNode *lambda1 = btor_lambda_exp (g_btor, x, nread);
  BtorNode *a       = btor_var_exp (g_btor, g_elem_bw, "a");
  /* exptected not (a + var) */
  BtorNode *expected_add = btor_add_exp (g_btor, a, var);
  BtorNode *expected     = btor_not_exp (g_btor, expected_add);
  BtorNode *result       = btor_apply (g_btor, 1, &a, lambda1);

  assert (result == expected);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, expected);
  btor_release_exp (g_btor, expected_add);
  btor_release_exp (g_btor, a);
  btor_release_exp (g_btor, lambda1);
  btor_release_exp (g_btor, nread);
  btor_release_exp (g_btor, read);
  btor_release_exp (g_btor, add);
  btor_release_exp (g_btor, x);
  btor_release_exp (g_btor, lambda2);
  btor_release_exp (g_btor, y);
  btor_release_exp (g_btor, var);
  finish_lambda_test ();
}

static void
test_lambda_reduce_nested_lambdas_const_n1000 (void)
{
  init_lambda_test ();
  int i;
  int nesting_lvl = 1000;
  size_t size     = nesting_lvl * sizeof (BtorNode *);
  BtorNode *exp, *result;
  BtorNode **lambdas;
  BtorNode **params;
  BtorNode **indices;
  BtorNode *var = btor_var_exp (g_btor, g_elem_bw, "");

  lambdas = btor_malloc (g_btor->mm, size);
  params  = btor_malloc (g_btor->mm, size);
  indices = btor_malloc (g_btor->mm, size);

  for (i = nesting_lvl - 1; i >= 0; i--)
  {
    indices[i] = btor_var_exp (g_btor, g_index_bw, "");
    params[i]  = btor_param_exp (g_btor, g_index_bw, "");

    if (i == nesting_lvl - 1)
      exp = var;
    else
      exp = lambdas[i + 1];

    lambdas[i] = btor_lambda_exp (g_btor, params[i], exp);
  }

  result = btor_apply (g_btor, nesting_lvl, indices, lambdas[0]);
  assert (result == var);

  for (i = 0; i < nesting_lvl; i++)
  {
    btor_release_exp (g_btor, lambdas[i]);
    btor_release_exp (g_btor, params[i]);
    btor_release_exp (g_btor, indices[i]);
  }

  btor_free (g_btor->mm, lambdas, size);
  btor_free (g_btor->mm, params, size);
  btor_free (g_btor->mm, indices, size);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, var);
  finish_lambda_test ();
}

/* (lambda x . (lambda y . (x + y))) (a) */
static void
test_lambda_partial_reduce_nested_lambdas_add1 (void)
{
  init_lambda_test ();
  BtorNode *a       = btor_var_exp (g_btor, g_elem_bw, "a");
  BtorNode *x       = btor_param_exp (g_btor, g_elem_bw, "x");
  BtorNode *y       = btor_param_exp (g_btor, g_elem_bw, "y");
  BtorNode *add     = btor_add_exp (g_btor, x, y);
  BtorNode *lambda2 = btor_lambda_exp (g_btor, y, add);
  BtorNode *lambda1 = btor_lambda_exp (g_btor, x, lambda2);
  BtorNode *result  = btor_apply (g_btor, 1, &a, lambda1);

  /* expected: lambda y' . (a + y') */
  assert (BTOR_IS_LAMBDA_NODE (result));
  assert (result != lambda2);
  assert (result->e[0] != lambda2->e[0]);
  assert (BTOR_REAL_ADDR_NODE (result->e[1])->kind == BTOR_ADD_NODE);
  assert (BTOR_REAL_ADDR_NODE (result->e[1])->e[0] == a);
  assert (BTOR_REAL_ADDR_NODE (result->e[1])->e[1] == result->e[0]);

  btor_release_exp (g_btor, result);
  btor_release_exp (g_btor, lambda1);
  btor_release_exp (g_btor, lambda2);
  btor_release_exp (g_btor, add);
  btor_release_exp (g_btor, y);
  btor_release_exp (g_btor, x);
  btor_release_exp (g_btor, a);
  finish_lambda_test ();
}

/*---------------------------------------------------------------------------
 * additional tests
 *---------------------------------------------------------------------------*/

static void
test_lambda_define_fun (void)
{
  init_lambda_test ();

  int i;
  int nesting_lvl = 1000;
  size_t size;
  BtorNode **params, **lambdas, **ands, *left, *right, *expected, *result;

  size    = nesting_lvl * sizeof (BtorNode *);
  params  = btor_malloc (g_btor->mm, size);
  lambdas = btor_malloc (g_btor->mm, size);
  ands    = btor_malloc (g_btor->mm, size - sizeof (BtorNode *));

  for (i = 0; i < nesting_lvl; i++)
    params[i] = btor_param_exp (g_btor, g_elem_bw, "");

  assert (nesting_lvl > 1);
  left  = params[0];
  right = params[1];
  for (i = 0; i < nesting_lvl - 1; i++)
  {
    ands[i] = btor_and_exp (g_btor, left, right);

    if (i + 2 < nesting_lvl)
    {
      left  = params[i + 2];
      right = ands[i];
    }
  }

  /* build expected */
  expected = ands[nesting_lvl - 2];
  for (i = nesting_lvl - 1; i >= 0; i--)
  {
    assert (expected);
    lambdas[i] = btor_lambda_exp (g_btor, params[i], expected);
    expected   = lambdas[i];
  }

  result = btor_fun_exp (g_btor, nesting_lvl, params, ands[nesting_lvl - 2]);

  assert (result == expected);

  for (i = 0; i < nesting_lvl; i++)
  {
    btor_release_exp (g_btor, lambdas[i]);
    btor_release_exp (g_btor, params[i]);

    if (i < nesting_lvl - 1) btor_release_exp (g_btor, ands[i]);
  }

  btor_free (g_btor->mm, params, size);
  btor_free (g_btor->mm, lambdas, size);
  btor_free (g_btor->mm, ands, size - sizeof (BtorNode *));
  btor_release_exp (g_btor, result);
  finish_lambda_test ();
}

void
run_lambda_tests (int argc, char **argv)
{
  /* constant lambda tests */
  BTOR_RUN_TEST (lambda_const_lambda_const);
  BTOR_RUN_TEST (lambda_const_lambda_var);
  BTOR_RUN_TEST (lambda_const_lambda_param);
  BTOR_RUN_TEST (lambda_unassigned_param);
  /* unary exp tests */
  BTOR_RUN_TEST (lambda_param_not);
  BTOR_RUN_TEST (lambda_param_neg);
  BTOR_RUN_TEST (lambda_param_redor);
  BTOR_RUN_TEST (lambda_param_redxor);
  BTOR_RUN_TEST (lambda_param_redand);
  BTOR_RUN_TEST (lambda_param_slice);
  BTOR_RUN_TEST (lambda_param_uext);
  BTOR_RUN_TEST (lambda_param_sext);
  /* binary exp tests */
  BTOR_RUN_TEST (lambda_param_implies);
  BTOR_RUN_TEST (lambda_param_iff);
  BTOR_RUN_TEST (lambda_param_xor);
  BTOR_RUN_TEST (lambda_param_xnor);
  BTOR_RUN_TEST (lambda_param_and);
  BTOR_RUN_TEST (lambda_param_nand);
  BTOR_RUN_TEST (lambda_param_or);
  BTOR_RUN_TEST (lambda_param_nor);
  BTOR_RUN_TEST (lambda_param_eq);
  BTOR_RUN_TEST (lambda_param_ne);
  BTOR_RUN_TEST (lambda_param_add);
  BTOR_RUN_TEST (lambda_param_uaddo);
  BTOR_RUN_TEST (lambda_param_saddo);
  BTOR_RUN_TEST (lambda_param_mul);
  BTOR_RUN_TEST (lambda_param_umulo);
  BTOR_RUN_TEST (lambda_param_smulo);
  BTOR_RUN_TEST (lambda_param_ult);
  BTOR_RUN_TEST (lambda_param_slt);
  BTOR_RUN_TEST (lambda_param_ulte);
  BTOR_RUN_TEST (lambda_param_slte);
  BTOR_RUN_TEST (lambda_param_ugt);
  BTOR_RUN_TEST (lambda_param_sgt);
  BTOR_RUN_TEST (lambda_param_ugte);
  BTOR_RUN_TEST (lambda_param_sgte);
  BTOR_RUN_TEST (lambda_param_sll);
  BTOR_RUN_TEST (lambda_param_srl);
  BTOR_RUN_TEST (lambda_param_sra);
  BTOR_RUN_TEST (lambda_param_rol);
  BTOR_RUN_TEST (lambda_param_ror);
  BTOR_RUN_TEST (lambda_param_sub);
  BTOR_RUN_TEST (lambda_param_usubo);
  BTOR_RUN_TEST (lambda_param_ssubo);
  BTOR_RUN_TEST (lambda_param_udiv);
  BTOR_RUN_TEST (lambda_param_sdiv);
  BTOR_RUN_TEST (lambda_param_sdivo);
  BTOR_RUN_TEST (lambda_param_urem);
  BTOR_RUN_TEST (lambda_param_srem);
  BTOR_RUN_TEST (lambda_param_smod);
  BTOR_RUN_TEST (lambda_param_concat);
  BTOR_RUN_TEST (lambda_param_read);
  /* ternary exp tests */
  BTOR_RUN_TEST (lambda_param_write1);
  BTOR_RUN_TEST (lambda_param_write2);
  BTOR_RUN_TEST (lambda_param_bcond1);
  BTOR_RUN_TEST (lambda_param_bcond2);
  BTOR_RUN_TEST (lambda_param_bcond3);
  BTOR_RUN_TEST (lambda_param_acond);

  /* full reduction tests (with reduced expressions) */
  BTOR_RUN_TEST (lambda_reduce_write1);
  BTOR_RUN_TEST (lambda_reduce_write2);
  BTOR_RUN_TEST (lambda_reduce_nested_writes);
  BTOR_RUN_TEST (lambda_reduce_nested_lambdas_add1);
  BTOR_RUN_TEST (lambda_reduce_nested_lambdas_add2);
  BTOR_RUN_TEST (lambda_reduce_nested_lambdas_read);
  BTOR_RUN_TEST (lambda_reduce_nested_lambdas_const_n1000);

  /* partial reduction tests */
  BTOR_RUN_TEST (lambda_partial_reduce_nested_lambdas_add1);

  /* additional tests */
  BTOR_RUN_TEST (lambda_define_fun);
}

void
finish_lambda_tests (void)
{
}
