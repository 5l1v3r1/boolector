#ifndef BTORAIGVEC_H_INCLUDED
#define BTORAIGVEC_H_INCLUDED

#include "btoraig.h"
#include "btormem.h"

/*------------------------------------------------------------------------*/
/* PRIVATE INTERFACE                                                      */
/*------------------------------------------------------------------------*/

struct BtorAIGVec
{
  BtorAIG **aigs; /* vector of AIGs */
  int len;        /* length of the AIG vector */
};

typedef struct BtorAIGVec BtorAIGVec;

typedef struct BtorAIGVecMgr BtorAIGVecMgr;

/* Creates new AIG vector manager. An AIG vector manager is used by nearly
 * all functions of the AIG vector layer.
 */
BtorAIGVecMgr *btor_new_aigvec_mgr (BtorMemMgr *mm, int verbosity);

/* Deletes AIG vector manager from memory. */
void btor_delete_aigvec_mgr (BtorAIGVecMgr *avmgr);

/* Implicit precondition of all functions taking AIG vectors as inputs:
 * The length of all input AIG vectors have to be greater than zero.
 */

/* Returns AIG manager of the AIG vector manager. */
BtorAIGMgr *btor_get_aig_mgr_aigvec_mgr (BtorAIGVecMgr *avmgr);

/* Creates new AIG vector representing a constant specified by bits.
 * len(result) = strlen(bits)
 */
BtorAIGVec *btor_const_aigvec (BtorAIGVecMgr *avmgr, const char *bits);

/* Creates new AIG vector representing a variable.
 * len > 0
 * len(result) = len
 */
BtorAIGVec *btor_var_aigvec (BtorAIGVecMgr *avmgr, int len);

/* Creates new AIG vector representing ones's complement of av.
 * len(result) = len(av)
 */
BtorAIGVec *btor_not_aigvec (BtorAIGVecMgr *avmgr, BtorAIGVec *av);

/* Creates new AIG vector representing a slice of av.
 * upper < len(av)
 * lower >= 0
 * upper >= lower
 * len(result) = upper - lower + 1
 */
BtorAIGVec *btor_slice_aigvec (BtorAIGVecMgr *avmgr,
                               BtorAIGVec *av,
                               int upper,
                               int lower);

/* Creates new AIG vector representing av1 AND av2.
 * len(av1) = len(av2)
 * len(result) = len(av1) = len(av2)
 */
BtorAIGVec *btor_and_aigvec (BtorAIGVecMgr *avmgr,
                             BtorAIGVec *av1,
                             BtorAIGVec *av2);

/* Creates new AIG vector representing av1 less than av2 (unsigned).
 * len(av1) = len(av2)
 * len(result) = 1
 */
BtorAIGVec *btor_ult_aigvec (BtorAIGVecMgr *avmgr,
                             BtorAIGVec *av1,
                             BtorAIGVec *av2);

/* Creates new AIG vector representing av1 equal av2.
 * len(av1) = len(av2)
 * len(result) = 1
 */
BtorAIGVec *btor_eq_aigvec (BtorAIGVecMgr *avmgr,
                            BtorAIGVec *av1,
                            BtorAIGVec *av2);

/* Creates new AIG vector representing av1 + av2.
 * len(av1) = len(av2)
 * len(result) = len(av1) = len(av2)
 */
BtorAIGVec *btor_add_aigvec (BtorAIGVecMgr *avmgr,
                             BtorAIGVec *av1,
                             BtorAIGVec *av2);

/* Creates new AIG vector representing av1 shift left logical by av2.
 * is_power_of_2(len(av1))
 * len(av2) = log2(len(av1))
 * len(result) = len(av1)
 */
BtorAIGVec *btor_sll_aigvec (BtorAIGVecMgr *avmgr,
                             BtorAIGVec *av1,
                             BtorAIGVec *av2);
/* Creates new AIG vector representing av1 shift right logical by av2.
 * is_power_of_2(len(av1))
 * len(av2) = log2(len(av1))
 * len(result) = len(av1)
 */
BtorAIGVec *btor_srl_aigvec (BtorAIGVecMgr *avmgr,
                             BtorAIGVec *av1,
                             BtorAIGVec *av2);

/* Creates new AIG vector representing av1 * av2.
 * len(av1) = len(av2)
 * len(result) = len(av1) = len(av2)
 */
BtorAIGVec *btor_mul_aigvec (BtorAIGVecMgr *avmgr,
                             BtorAIGVec *av1,
                             BtorAIGVec *av2);

/* Creates new AIG vector representing av1 / av2 (unsigned).
 * len(av1) = len(av2)
 * len(result) = len(av1) = len(av2)
 */
BtorAIGVec *btor_udiv_aigvec (BtorAIGVecMgr *avmgr,
                              BtorAIGVec *av1,
                              BtorAIGVec *av2);

/* Creates new AIG vector representing av1 % av2 (unsigned).
 * len(av1) = len(av2)
 * len(result) = len(av1) = len(av2)
 */
BtorAIGVec *btor_urem_aigvec (BtorAIGVecMgr *avmgr,
                              BtorAIGVec *av1,
                              BtorAIGVec *av2);

/* Creates new AIG vector representing the concatenation av1.av2.
 * len(result) = len(av1) + len(av2)
 */
BtorAIGVec *btor_concat_aigvec (BtorAIGVecMgr *avmgr,
                                BtorAIGVec *av1,
                                BtorAIGVec *av2);

/* Creates new AIG vector representing av_cond ? av_if : av_else.
 * len(av_cond) = 1
 * len(av_if) = len(av_else)
 * len(result) = len(av_if) = len(av_else)
 */
BtorAIGVec *btor_cond_aigvec (BtorAIGVecMgr *avmgr,
                              BtorAIGVec *av_cond,
                              BtorAIGVec *av_if,
                              BtorAIGVec *av_else);

/* Creates new AIG vector representing a copy of av.
 * len(result) = len(av)
 */
BtorAIGVec *btor_copy_aigvec (BtorAIGVecMgr *avmgr, BtorAIGVec *av);

/* Determines if av is a constant AIG vector or not. */
int btor_is_const_aigvec (BtorAIGVecMgr *avmgr, BtorAIGVec *av);

/* Determines if two AIG vectors contain the same AIGs or not. */
int btor_is_equal_aigvec (BtorAIGVecMgr *avmgr,
                          BtorAIGVec *av1,
                          BtorAIGVec *av2);

/* Release all AIGs of the AIG vector and delete AIG vector from memory. */
void btor_release_delete_aigvec (BtorAIGVecMgr *avmgr, BtorAIGVec *av);

/* Gets AIG manager of AIG vector manager. */
BtorAIGMgr *btor_get_aig_mgr_aigvec_mgr (BtorAIGVecMgr *avmgr);

/* Gets current assignemt of AIG vector. */
char *btor_get_assignment_aigvec (BtorAIGVecMgr *avmgr, BtorAIGVec *av);

#endif
