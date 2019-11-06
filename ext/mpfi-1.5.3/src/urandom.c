/* urandom.c -- Random element in the interval.

Copyright 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2010,
                     Spaces project, Inria Lorraine
                     and Salsa project, INRIA Rocquencourt,
                     and Arenaire project, Inria Rhone-Alpes, France
                     and Lab. ANO, USTL (Univ. of Lille),  France

This file is part of the MPFI Library.

The MPFI Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPFI Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPFI Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
MA 02110-1301, USA. */

#include "mpfi-impl.h"

/* Picks randomly a point m in y */
void
mpfi_urandom (mpfr_ptr m, mpfi_srcptr y, gmp_randstate_t state)
{
  mp_prec_t prec, tmp_prec;
  mpfr_t diam, fact;


  if (MPFI_NAN_P(y)) {
    mpfr_set_nan (m);
    return;
  }

  if (mpfr_equal_p (&(y->left), &(y->right))) {
    mpfr_set (m, &(y->left), MPFI_RNDD);
    return;
  }

  prec = mpfr_get_prec (m);
  tmp_prec = mpfi_get_prec(y);
  if (tmp_prec > prec)
    {
    prec = tmp_prec;
    }
  mpfr_init2 (diam, prec);
  mpfr_init2 (fact, prec);

  mpfi_diam_abs (diam, y);
  mpfr_urandomb (fact, state); /* fact lies between 0 and 1 */

  if (mpfr_cmp_ui (diam, 1) <= 0) {
    /* the picked point lies at a relative distance "fact" of the left
       endpoint: m = inf + (sup - inf) * fact  */
    mpfr_mul (fact, fact, diam, MPFI_RNDD);
    /* FIXME: because of possible cancelation, the random distribution is
       not uniform among the floating-point numbers in y */
    mpfr_add (m, &(y->left), fact, MPFI_RNDD);
  }
  else {
    mp_exp_t e;
    if (mpfr_cmp_abs (&(y->left), &(y->right)) < 0) {
      e = mpfr_inf_p (&(y->right)) ? mpfr_get_emax ()
        : mpfr_get_exp (&(y->right));
    }
    else {
      e = mpfr_inf_p (&(y->left)) ? mpfr_get_emax ()
        : mpfr_get_exp (&(y->left));
    }
    e += 1;
    /* resize fact in [0, 2^e] where e = 1 + max{exp(left), exp(right)} */
    mpfr_mul_2exp (fact, fact, e, MPFI_RNDD);
    mpfr_set (m, &(y->left), MPFI_RNDD);
    if (mpfr_inf_p (m)) {
      mpfr_nextabove (m);
    }
    /* m may be outside y */
    mpfr_add (m, m, fact, MPFI_RNDD);
  }
  mpfr_clear (fact);
  mpfr_clear (diam);

  /* Ensure that m belongs to y (if the precision is sufficient) */
  if (mpfr_cmp (m, &(y->left)) < 0)
    mpfr_set (m, &(y->left), MPFI_RNDU);

  if (mpfr_cmp (&(y->right), m) < 0)
    mpfr_set (m, &(y->right), MPFI_RNDD);
}
