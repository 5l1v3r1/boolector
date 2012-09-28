#include <stdio.h>
#include <stdlib.h>
#include "../../boolector.h"
#include "../../btorutil.h"

int
main (int argc, char **argv)
{
  int num_bits, num_bits_index, num_elements, i;
  Btor *btor;
  BtorNode **indices, *array, *ugt, *temp, *read, *formula, *max, *index;
  if (argc != 3)
  {
    printf ("Usage: ./max <num-bits> <num-elements>\n");
    return 1;
  }
  num_bits = atoi (argv[1]);
  if (num_bits <= 0)
  {
    printf ("Number of bits must be greater than zero\n");
    return 1;
  }
  num_elements = atoi (argv[2]);
  if (num_elements <= 1)
  {
    printf ("Number of elements must be greater than one\n");
    return 1;
  }
  if (!btor_is_power_of_2_util (num_elements))
  {
    printf ("Number of elements must be a power of two\n");
    return 1;
  }
  num_bits_index = btor_log_2_util (num_elements);
  btor           = boolector_new ();
  boolector_set_rewrite_level (btor, 0);
  indices = (BtorNode **) malloc (sizeof (BtorNode *) * num_elements);
  for (i = 0; i < num_elements; i++)
    indices[i] = boolector_int (btor, i, num_bits_index);
  array = boolector_array (btor, num_bits, num_bits_index, "array");
  /* current maximum is first element of array */
  max = boolector_read (btor, array, indices[0]);
  /* compute maximum of array */
  for (i = 1; i < num_elements; i++)
  {
    read = boolector_read (btor, array, indices[i]);
    ugt  = boolector_ugt (btor, read, max);
    temp = boolector_cond (btor, ugt, read, max);
    boolector_release (btor, max);
    max = temp;
    boolector_release (btor, read);
    boolector_release (btor, ugt);
  }
  /* show that maximum is really the maximum */
  index = boolector_var (btor, num_bits_index, "index");
  read  = boolector_read (btor, array, index);
  /* there is no arbitrary read value which is greater than the maximum */
  formula = boolector_ult (btor, max, read);
  boolector_dump_btor (btor, stdout, formula);
  /* clean up */
  for (i = 0; i < num_elements; i++) boolector_release (btor, indices[i]);
  boolector_release (btor, formula);
  boolector_release (btor, read);
  boolector_release (btor, index);
  boolector_release (btor, max);
  boolector_release (btor, array);
  boolector_delete (btor);
  free (indices);
  return 0;
}
