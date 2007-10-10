#include "../../boolector.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
die (const char* fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);
  fprintf (stderr, "*** genmemcpy: ");
  vfprintf (stderr, fmt, ap);
  va_end (ap);
  fputc ('\n', stderr);
  exit (1);
}

static int
isint (const char* str)
{
  char ch;

  if ((ch = *str++) == '-') ch = *str++;

  if (!isdigit (ch)) return 0;

  while (isdigit ((ch = *str++)))
    ;

  return !ch;
}

int
main (int argc, char** argv)
{
  BtorExp *src, *dst, *eos, *eod, *p, *q, *tmp, *n, *j, *z, *done;
  BtorExp *mem, *assumption, *alternative, *cmp, *root;
  int i, len, havelen;
  BtorExpMgr* mgr;

  len     = 0;
  havelen = 0;

  for (i = 1; i < argc; i++)
  {
    if (!strcmp (argv[i], "-h"))
    {
      fprintf (stderr, "usage: genmemcpy [-h] <len>\n");
      exit (1);
    }
    else if (!argv[i][0] == '-')
      die ("invalid command line option '%s'", argv[i]);
    else if (!isint (argv[i]))
      die ("expected integer but got '%s'", argv[i]);
    else if (havelen)
      die ("multiple integer arguments");
    else
    {
      havelen = 1;
      len     = atoi (argv[i]);
    }
  }
  mgr = btor_new_exp_mgr (2, 0, 2, 0);
  mem = btor_array_exp (mgr, 8, 32);
  src = btor_var_exp (mgr, 32, "src");
  dst = btor_var_exp (mgr, 32, "dst");

  n = btor_int_to_exp (mgr, len, 32);

  eos = btor_add_exp (mgr, src, n);
  eod = btor_add_exp (mgr, dst, n);

  cmp        = btor_ulte_exp (mgr, src, eos);
  assumption = cmp;

  cmp = btor_ulte_exp (mgr, dst, eod);
  tmp = btor_and_exp (mgr, assumption, cmp);
  btor_release_exp (mgr, assumption);
  btor_release_exp (mgr, cmp);
  assumption = tmp;

  cmp         = btor_ulte_exp (mgr, eos, dst);
  alternative = cmp;

  cmp = btor_ulte_exp (mgr, eod, src);
  tmp = btor_or_exp (mgr, alternative, cmp);
  btor_release_exp (mgr, alternative);
  btor_release_exp (mgr, cmp);
  alternative = tmp;

  tmp = btor_and_exp (mgr, assumption, alternative);
  btor_release_exp (mgr, assumption);
  btor_release_exp (mgr, alternative);
  assumption = tmp;

  root = assumption;

  btor_dump_exp (mgr, stdout, assumption);

  btor_release_exp (mgr, root);
  btor_release_exp (mgr, eos);
  btor_release_exp (mgr, eod);
  btor_release_exp (mgr, n);
  btor_release_exp (mgr, dst);
  btor_release_exp (mgr, src);
  btor_release_exp (mgr, mem);
  btor_delete_exp_mgr (mgr);

  return 0;
}
