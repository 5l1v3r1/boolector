# Boolector: Satisfiablity Modulo Theories (SMT) solver.
#
# Copyright (C) 2013 Mathias Preiner.
#
# All rights reserved.
#
# This file is part of Boolector.
# See COPYING for more information on using this software.
#

from libc.stdio cimport FILE

cdef extern from "boolector.h":
    ctypedef struct BoolectorNode:
        pass
    ctypedef struct Btor:
        pass

    #void boolector_set_trapi (Btor * btor, FILE * apitrace)
    #FILE *boolector_get_trapi (Btor * btor)

    Btor *boolector_new ()

    Btor *boolector_clone (Btor * btor)

    void boolector_enable_model_gen (Btor * btor)

    void boolector_generate_model_for_all_reads (Btor * btor)

    void boolector_enable_inc_usage (Btor * btor)

    int boolector_set_sat_solver (Btor * btor, const char * solver)

    void boolector_set_rewrite_level (Btor * btor, int rewrite_level)

    int boolector_get_refs (Btor * btor)

    void boolector_delete (Btor * btor)

    int boolector_simplify (Btor * btor)

    void boolector_enable_beta_reduce_all (Btor * btor)

    BoolectorNode *boolector_const (Btor * btor, const char *bits)

    BoolectorNode *boolector_zero (Btor * btor, int width)

    BoolectorNode *boolector_false (Btor * btor)

    BoolectorNode *boolector_ones (Btor * btor, int width)

    BoolectorNode *boolector_true (Btor * btor)

    BoolectorNode *boolector_one (Btor * btor, int width)

    BoolectorNode *boolector_unsigned_int (Btor * btor, unsigned u, int width)

    BoolectorNode *boolector_int (Btor * btor, int i, int width)

    BoolectorNode *boolector_var (Btor * btor, int width, const char *symbol)

    BoolectorNode *boolector_array (Btor * btor, 
                                   int elem_width, 
                                   int index_width, 
                                   const char *symbol)

    BoolectorNode *boolector_not (Btor * btor, BoolectorNode * node)

    BoolectorNode *boolector_neg (Btor * btor, BoolectorNode * node)

    BoolectorNode *boolector_redor (Btor * btor, BoolectorNode * node)

    BoolectorNode *boolector_redxor (Btor * btor, BoolectorNode * node)

    BoolectorNode *boolector_redand (Btor * btor, BoolectorNode * node)

    BoolectorNode *boolector_slice (
        Btor * btor, BoolectorNode * node, int upper, int lower)

    BoolectorNode *boolector_uext (Btor * btor, BoolectorNode * node, int width)

    BoolectorNode *boolector_sext (Btor * btor, BoolectorNode * node, int width)

    BoolectorNode *boolector_implies (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_iff (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_xor (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_xnor (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_and (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_nand (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_or (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_nor (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_eq (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_ne (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_add (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_uaddo (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_saddo (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_mul (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_umulo (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_smulo (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_ult (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_slt (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_ulte (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_slte (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_ugt (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_sgt (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_ugte (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_sgte (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_sll (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_srl (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_sra (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_rol (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_ror (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_sub (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_usubo (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_ssubo (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_udiv (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_sdiv (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_sdivo (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_urem (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_srem (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_smod (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_concat (
        Btor * btor, BoolectorNode * n0, BoolectorNode * n1)

    BoolectorNode *boolector_read (
        Btor * btor, BoolectorNode * n_array, BoolectorNode * n_index)

    BoolectorNode *boolector_write (Btor * btor, 
                                    BoolectorNode * n_array, 
                                    BoolectorNode * n_index, 
                                    BoolectorNode * n_value)

    BoolectorNode *boolector_cond (Btor * btor, 
                                   BoolectorNode * n_cond, 
                                   BoolectorNode * n_if, 
                                   BoolectorNode * n_else)

    BoolectorNode *boolector_param (Btor * btor, int width, const char * symbol) 

    BoolectorNode *boolector_fun (Btor * btor, 
                                  int paramc, 
                                  BoolectorNode ** param_nodes, 
                                  BoolectorNode * node) 

# NOTE: not required
#    BoolectorNode *boolector_args (
#        Btor * btor, int argc, BoolectorNode ** arg_nodes)

    BoolectorNode *boolector_apply (
        Btor * btor, int argc, BoolectorNode ** arg_nodes,
        BoolectorNode * n_fun)

# NOTE: not required
#    BoolectorNode *boolector_apply_args (
#        Btor * btor, BoolectorNode * n_args, BoolectorNode * n_fun)

    BoolectorNode *boolector_inc (Btor * btor, BoolectorNode *node)

    BoolectorNode *boolector_dec (Btor * btor, BoolectorNode *node)

    int boolector_is_array (Btor * btor, BoolectorNode * node)

    int boolector_is_array_var (Btor * btor, BoolectorNode * node)

    int boolector_is_param (Btor * btor, BoolectorNode * node)

    int boolector_is_bound_param (Btor * btor, BoolectorNode * node)

    int boolector_is_fun (Btor * btor, BoolectorNode * node)

    int boolector_get_fun_arity (Btor * btor, BoolectorNode * node)

# NOTE: not required
#    int boolector_is_args (Btor * btor, BoolectorNode * node)
#
#    int boolector_get_args_arity (Btor * btor, BoolectorNode * node)

    int boolector_get_width (Btor * btor, BoolectorNode * node)

    int boolector_get_index_width (Btor * btor, BoolectorNode * n_array)

    #int boolector_fun_sort_check (
    #    Btor * btor, int argc, BoolectorNode ** arg_nodes, BoolectorNode * n_fun)

    const char *boolector_get_symbol_of_var (Btor * btor, BoolectorNode * var)

    BoolectorNode *boolector_copy (Btor * btor, BoolectorNode * node)

    void boolector_release (Btor * btor, BoolectorNode * node)

    void boolector_dump_btor (Btor * btor, FILE * file, BoolectorNode * node)

    void boolector_dump_btor_all (Btor * btor, FILE * file)

    #void boolector_dump_smt (Btor * btor, FILE * file, BoolectorNode * node)
    #
    #void boolector_dump_smt2 (Btor * btor, FILE * file, BoolectorNode * node)

    void boolector_assert (Btor * btor, BoolectorNode * node)

    void boolector_assume (Btor * btor, BoolectorNode * node)

    int boolector_sat (Btor * btor)

    const char *boolector_bv_assignment (Btor * btor, BoolectorNode * node)

    void boolector_free_bv_assignment (Btor * btor, const char * assignment)

    void boolector_array_assignment (Btor * btor, 
                                     BoolectorNode * n_array, 
                                     char ***indices, 
                                     char ***values, 
                                     int *size)

    void boolector_free_array_assignment (
        Btor * btor, char ** indices, char ** values, int size)
