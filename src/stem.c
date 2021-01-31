/*    Pre-Pair
**    Word-based Pre-processor for Re-Pair
**    Copyright (C) 2003, 2007 by Raymond Wan (rwan@kuicr.kyoto-u.ac.jp)
**
**    Version 1.0.1 -- 2007/04/02
**
**    This file is part of the Pre-Pair software.
**
**    Pre-Pair is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
**    Pre-Pair is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License along
**    with Pre-Pair; if not, write to the Free Software Foundation, Inc.,
**    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/*
   Implementation of the Porter stemming algorithm.
   Written by Raymond Wan, 2002/10/30
 
   Original algorithm described in:
 
     Porter, M. F., 1980, An algorithm for suffix stripping,
       Program, 14(3) : 130-137.  Reprinted in Sparck Jones, Karen,
       and Peter Willet, 1997, Readings in Information Retrieval,
       San Francisco: Morgan Kaufmann, ISBN 1-55860-454-4.
 
     The official Porter Stemming Algorithm home page:
       http://www.tartarus.org/~martin/PorterStemmer/index.html
 
    This implementation of the Porter stemming algorithm is 
    reversible.  Changes to a word result in a 23-bit modifier
    which can be used to losslessly reproduce the original word.
    Words can be up to 256 characters in length, but are assumed to 
    have been case folded.
 
    The 23-bit modifier is divided as follows, starting from the most
    significant bit:
      step 1a:  3 bits
      step 1b:  2 bits
      step 1c:  3 bits
      step 1d:  1 bit
      step 2:   4 bits
      step 3:   3 bits
      step 4:   5 bits
      step 5a:  1 bit
      step 5b:  1 bit
 
    Note that the bits do not indicate which rule was performed, but
    only what to do to recreate the word.  Therefore, several rules
    in the original paper may be undone using the same method.
    Of the 5 steps mentioned in the original paper, the following 
    have not been done:
      1)  In Step 5a, according to the FAQ at the above URL, only one
          of the rules would be performed because S1 (e) matches to
          both rules.
 
    From the Porter Stemming Algorithm home page, the following
    additions have been made:
      1)  New rule for Step 2:  (m > 0) logi -> log
      2)  Updated rule for Step 2:  (m > 0) abli -> able replaced
          by (m > 0) bli -> ble.
      3)  Words of 1 or 2 characters in length are not processed.
 
    #define the symbol BRITISH_SPELL at compile time in order
    to accept only British spelling of -ize (-ise) words.

    Update 2003/03/06
      1)  Words must be longer than 3 characters in order to be
          stemmed.
      2)  Steps 1a, 1b, and 1c have been renamed to steps 1b, 1c, and
          1d, respectively.
      3)  Step 1a is not part of the Porter stemming algorithm and
          removes 'd, 'm, 's, 't, 'll, 're, 've from a word.

*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#include "common-def.h"
#include "ustring.h"
#include "stem.h"

static R_UINT *findM (R_UCHAR *wrd, R_UINT len, R_UINT *m_array);

static R_UINT step1a (R_UCHAR *wrd, R_UINT *len, R_UINT *m);
static R_UINT step1b (R_UCHAR *wrd, R_UINT *len, R_UINT *m);
static R_UINT step1c (R_UCHAR *wrd, R_UINT *len, R_UINT *m);
static R_UINT step1d (R_UCHAR *wrd, R_UINT *len, R_UINT *m);
static R_UINT step2 (R_UCHAR *wrd, R_UINT *len, R_UINT *m);
static R_UINT step3 (R_UCHAR *wrd, R_UINT *len, R_UINT *m);
static R_UINT step4 (R_UCHAR *wrd, R_UINT *len, R_UINT *m);
static R_UINT step5a (R_UCHAR *wrd, R_UINT *len, R_UINT *m);
static R_UINT step5b (R_UCHAR *wrd, R_UINT *len, R_UINT *m);

static void unstep1a (R_UCHAR *wrd, R_UINT *len, enum S_STEM1a result);
static void unstep1b (R_UCHAR *wrd, R_UINT *len, enum S_STEM1b result);
static void unstep1c (R_UCHAR *wrd, R_UINT *len, enum S_STEM1c result);
static void unstep1d (R_UCHAR *wrd, R_UINT *len, enum S_STEM1d result);
static void unstep2 (R_UCHAR *wrd, R_UINT *len, enum S_STEM2 result);
static void unstep3 (R_UCHAR *wrd, R_UINT *len, enum S_STEM3 result);
static void unstep4 (R_UCHAR *wrd, R_UINT *len, enum S_STEM4 result);
static void unstep5a (R_UCHAR *wrd, R_UINT *len, enum S_STEM5a result);
static void unstep5b (R_UCHAR *wrd, R_UINT *len, enum S_STEM5b result);

static R_UINT packModifier (enum S_STEM1a result1a, enum S_STEM1b result1b, enum S_STEM1c result1c, enum S_STEM1d result1d, enum S_STEM2 result2, enum S_STEM3 result3, enum S_STEM4 result4, enum S_STEM5a result5a, enum S_STEM5b result5b);
static void unpackModifier (R_UINT modifier, enum S_STEM1a *result1a, enum S_STEM1b *result1b, enum S_STEM1c *result1c, enum S_STEM1d *result1d, enum S_STEM2 *result2, enum S_STEM3 *result3, enum S_STEM4 *result4, enum S_STEM5a *result5a, enum S_STEM5b *result5b);


static R_UINT *findM (R_UCHAR *wrd, R_UINT len, R_UINT *m_array) {
  R_UINT i = 0;
  R_UINT m = 0;
  enum CV status = UNKNOWN;
  enum CV prev_status = UNKNOWN;

  for (i = 0; i < len; i++) {
    status = CLASSIFYCHAR (wrd, i);
    if ((prev_status == VOWEL) && (status == CONSONANT)) {
      m++;
    }
    prev_status = status;
    m_array[i] = m;
  }

  return (m_array);
}


static R_UINT step1a (R_UCHAR *wrd, R_UINT *len, R_UINT *m) {
  enum S_STEM1a result = NONE_1a;
  R_BOOLEAN match_result = R_FALSE;

  CHOP (wrd, *len, "'d", 2, 2, D_1a, m);
  CHOP (wrd, *len, "'m", 2, 2, M_1a, m);
  CHOP (wrd, *len, "'s", 2, 2, S_1a, m);
  CHOP (wrd, *len, "'t", 2, 2, T_1a, m);
  CHOP (wrd, *len, "'ll", 3, 3, LL_1a, m);
  CHOP (wrd, *len, "'re", 3, 3, RE_1a, m);
  CHOP (wrd, *len, "'ve", 3, 3, VE_1a, m);

  return (result);
}


static R_UINT step1b (R_UCHAR *wrd, R_UINT *len, R_UINT *m) {
  enum S_STEM1b result = NONE_1b;
  R_BOOLEAN match_result = R_FALSE;

  CHOP (wrd, *len, "sses", 4, 2, ES_1b, m);
  CHOP (wrd, *len, "ies", 3, 2, ES_1b, m);
  if (((*len) > 2) && (wrd[(*len) - 2] != (R_UCHAR) 's')) {
    CHOP (wrd, *len, "s", 1, 1, S_1b, m);
  }

  return (result);
}


static R_UINT step1c (R_UCHAR *wrd, R_UINT *len, R_UINT *m) {
  enum S_STEM1c result = NONE_1c;
  R_BOOLEAN match_result = R_FALSE;
  enum CV second_last = UNKNOWN;
  enum CV third_last = UNKNOWN;
  enum CV fourth_last = UNKNOWN;
  R_UINT i = 0;

  CHOP_M (wrd, *len, "eed", 3, 1, E_ED_1c, m, 0);

  if (((*len) > 3) && (memcmp (wrd + (*len) - 2, "ed", 2) == 0)) {
    match_result = R_TRUE;
    for (i = 0; i < (*len) - 2; i++) {
      if (CLASSIFYCHAR (wrd, i) == VOWEL) {
        (*len) -= 2;
        result = ED_1c;
        m = findM (wrd, *len, m);
        break;
      }
    }
  }
  else if (((*len) > 4) && (memcmp (wrd + (*len) - 3, "ing", 3) == 0)) {
    match_result = R_TRUE;
    for (i = 0; i < (*len) - 3; i++) {
      if (CLASSIFYCHAR (wrd, i) == VOWEL) {
        (*len) -= 3;
        result = ING_1c;
        m = findM (wrd, *len, m);
        break;
      }
    }
  }

  if ((result == ED_1c) || (result == ING_1c)) {
    if ((memcmp (wrd + (*len) - 2, "at", 2) == 0) ||
        (memcmp (wrd + (*len) - 2, "bl", 2) == 0) ||
        (memcmp (wrd + (*len) - 2, EN_IZ, 2) == 0)) {
      wrd[*len] = (R_UCHAR) 'e';
      (*len)++;
      result++;
      m = findM (wrd, *len, m);
      return (result);
    }

    if ((*len > 2) && (wrd[(*len) - 1] == wrd[(*len) - 2]) && (CLASSIFYCHAR (wrd, (*len) - 1) == CONSONANT) && ((wrd[(*len) - 1] != (R_UCHAR) 'l') && (wrd[(*len) - 1] != (R_UCHAR) 's') && (wrd[(*len) - 1] != (R_UCHAR) 'z'))) {
      (*len)--;
      result = result + 2;
      m = findM (wrd, *len, m);
      return (result);
    }

    if (((*len) > 2) && (m[(*len) - 1] == 1)) {
      second_last = CLASSIFYCHAR (wrd, (*len) - 1);
      third_last = CLASSIFYCHAR (wrd, (*len) - 2);
      if (*len == 2) {
        fourth_last = VOWEL;
      }
      else {
        fourth_last = CLASSIFYCHAR (wrd, (*len) - 3);
      }
      if ((second_last == CONSONANT) && (third_last == VOWEL) && (fourth_last == CONSONANT)) {
        if ((wrd[(*len) - 1] != (R_UCHAR) 'w') && (wrd[(*len) - 1] != (R_UCHAR) 'x') && (wrd[(*len) - 1] != (R_UCHAR) 'y')) {
          wrd[*len] = (R_UCHAR) 'e';
          (*len)++;
          result++;
         }
      }
    }
  }

  m = findM (wrd, *len, m);
  return (result);
}


static R_UINT step1d (R_UCHAR *wrd, R_UINT *len, R_UINT *m) {
  enum S_STEM1d result = NONE_1d;
  R_UINT i = 0;

  if (wrd[(*len) - 1] == (R_UCHAR) 'y') {
    for (i = 0; i < (*len) - 1; i++) {
      if (CLASSIFYCHAR (wrd, i) == VOWEL) {
        wrd[(*len) - 1] = (R_UCHAR) 'i';
        result = Y_I_1d;
        return (result);
      }
    }
  }

  return (result);
}


static R_UINT step2 (R_UCHAR *wrd, R_UINT *len, R_UINT *m) {
  enum S_STEM2 result = NONE_2;
  R_BOOLEAN match_result = R_FALSE;

  MOD_CHOP_M (wrd, *len, "ational", 7, 5, "e", 1, IONAL_E_2, m, 0);
  CHOP_M (wrd, *len, "tional", 6, 2, AL_2, m, 0);
  MOD_CHOP_M (wrd, *len, "enci", 4, 1, "e", 1, I_E_2, m, 0);
  MOD_CHOP_M (wrd, *len, "anci", 4, 1, "e", 1, I_E_2, m, 0);
  CHOP_M (wrd, *len, EN_IZER, 4, 1, R_2, m, 0);
  /*  Next one modified from the one in the original paper -- mentioned
  **  on Porter's web site.  */
  MOD_CHOP_M (wrd, *len, "bli", 3, 1, "e", 1, I_E_2, m, 0);
  /*  MOD_CHOP_M (wrd, *len, "abli", 4, 1, "e", 1, I_E_2, m, 0);*/
  CHOP_M (wrd, *len, "alli", 4, 2, LI_2, m, 0);
  CHOP_M (wrd, *len, "entli", 5, 2, LI_2, m, 0);
  CHOP_M (wrd, *len, "eli", 3, 2, LI_2, m, 0);
  CHOP_M (wrd, *len, "ousli", 5, 2, LI_2, m, 0);
  MOD_CHOP_M (wrd, *len, EN_IZATION, 7, 5, "e", 1, IZATION_IZE_2, m, 0);
  MOD_CHOP_M (wrd, *len, "ation", 5, 3, "e", 1, ATION_ATE_2, m, 0);
  MOD_CHOP_M (wrd, *len, "ator", 4, 2, "e", 1, ATOR_ATE_2, m, 0);
  CHOP_M (wrd, *len, "alism", 5, 3, ISM_2, m, 0);
  CHOP_M (wrd, *len, "iveness", 7, 4, NESS_2, m, 0);
  CHOP_M (wrd, *len, "fulness", 7, 4, NESS_2, m, 0);
  CHOP_M (wrd, *len, "ousness", 7, 4, NESS_2, m, 0);
  CHOP_M (wrd, *len, "aliti", 5, 3, ITI_2, m, 0);
  MOD_CHOP_M (wrd, *len, "iviti", 5, 3, "e", 1, ITI_E_2, m, 0);
  MOD_CHOP_M (wrd, *len, "biliti", 6, 5, "le", 2, BILITI_BLE_2, m, 0);
  /*  Next one is new, but mentioned on Porter's web site  */
  CHOP_M (wrd, *len, "logi", 4, 1, I_2, m, 0);

  return (result);
}


static R_UINT step3 (R_UCHAR *wrd, R_UINT *len, R_UINT *m) {
  enum S_STEM3 result = NONE_3;
  R_BOOLEAN match_result = R_FALSE;

  /*  Step 3  */
  CHOP_M (wrd, *len, "icate", 5, 3, ATE_3, m, 0);
  CHOP_M (wrd, *len, "ative", 5, 5, ATIVE_3, m, 0);
  CHOP_M (wrd, *len, EN_ALIZE, 5, 3, IZE_3, m, 0);
  CHOP_M (wrd, *len, "iciti", 5, 3, ITI_3, m, 0);
  CHOP_M (wrd, *len, "ical", 4, 2, AL_3, m, 0);
  CHOP_M (wrd, *len, "ful", 3, 3, FUL_3, m, 0);
  CHOP_M (wrd, *len, "ness", 4, 4, NESS_3, m, 0);

  return (result);
}


static R_UINT step4 (R_UCHAR *wrd, R_UINT *len, R_UINT *m) {
  enum S_STEM4 result = NONE_4;
  R_BOOLEAN match_result = R_FALSE;

  /*  Step 4  */
  CHOP_M (wrd, *len, "al", 2, 2, AL_4, m, 1);
  CHOP_M (wrd, *len, "ance", 4, 4, ANCE_4, m, 1);
  CHOP_M (wrd, *len, "ence", 4, 4, ENCE_4, m, 1);
  CHOP_M (wrd, *len, "er", 2, 2, ER_4, m, 1);
  CHOP_M (wrd, *len, "ic", 2, 2, IC_4, m, 1);
  CHOP_M (wrd, *len, "able", 4, 4, ABLE_4, m, 1);
  CHOP_M (wrd, *len, "ible", 4, 4, IBLE_4, m, 1);
  CHOP_M (wrd, *len, "ant", 3, 3, ANT_4, m, 1);
  CHOP_M (wrd, *len, "ement", 5, 5, EMENT_4, m, 1);
  CHOP_M (wrd, *len, "ment", 4, 4, MENT_4, m, 1);
  CHOP_M (wrd, *len, "ent", 3, 3, ENT_4, m, 1);

  if ((*len > 4) && ((wrd[(*len) - 4] == (R_UCHAR) 's') || (wrd[(*len) - 4] == (R_UCHAR) 't'))) {
    CHOP_M (wrd, *len, "ion", 3, 3, ION_4, m, 1);
  }

  CHOP_M (wrd, *len, "ou", 2, 2, OU_4, m, 1);
  CHOP_M (wrd, *len, "ism", 3, 3, ISM_4, m, 1);
  CHOP_M (wrd, *len, "ate", 3, 3, ATE_4, m, 1);
  CHOP_M (wrd, *len, "iti", 3, 3, ITI_4, m, 1);
  CHOP_M (wrd, *len, "ous", 3, 3, OUS_4, m, 1);
  CHOP_M (wrd, *len, "ive", 3, 3, IVE_4, m, 1);
  CHOP_M (wrd, *len, EN_IZE, 3, 3, IZE_4, m, 1);

  return (result);
}


static R_UINT step5a (R_UCHAR *wrd, R_UINT *len, R_UINT *m) {
  enum S_STEM5a result = NONE_5a;
  R_BOOLEAN match_result = R_FALSE;
  enum CV second_last = UNKNOWN;
  enum CV third_last = UNKNOWN;
  enum CV fourth_last = UNKNOWN;

  /*  Step 5a  */
  CHOP_M_NORETURN (wrd, *len, "e", 1, 1, E_5a, m, 1);

  /*  Deviation from Porter algorithm  */
  match_result = R_FALSE;

  if ((match_result == R_FALSE) && ((*len) > 2)) {
    if ((wrd[(*len) - 1] == (R_UCHAR) 'e') && (m[(*len) - 2] == 1)) {
      second_last = CLASSIFYCHAR (wrd, (*len) - 2);
      third_last = CLASSIFYCHAR (wrd, (*len) - 3);
      if ((*len) == 3) {
        fourth_last = VOWEL;
      }
      else {
        fourth_last = CLASSIFYCHAR (wrd, (*len) - 4);
      }

      if ((second_last == VOWEL) || (third_last == CONSONANT) || 
          (fourth_last == VOWEL) || ((wrd[(*len) - 2] == (R_UCHAR) 'w') ||
          (wrd[(*len) - 2] == (R_UCHAR) 'x') || (wrd[(*len) - 2] == (R_UCHAR) 'y'))) {
        CHOP (wrd, *len, "e", 1, 1, E_5a, m);
      }
    }
  }

  return (result);
}


static R_UINT step5b (R_UCHAR *wrd, R_UINT *len, R_UINT *m) {
  enum S_STEM5b result = NONE_5b;

  /*  Step 5b  */
  /*  No check for consonant, since a check for 'l' already done.  */
  if ((*len > 2) && (m[(*len) - 2] > 1) && (wrd[(*len) - 1] == wrd[(*len) - 2]) && (wrd[(*len) - 1] == (R_UCHAR) 'l')) {
    (*len)--;
    result = DOUBLE_5b;
  }

  return (result);
}      


static void unstep1a (R_UCHAR *wrd, R_UINT *len, enum S_STEM1a result) {
  UNCHOP (wrd, *len, "'d", 2, 0, D_1a);
  UNCHOP (wrd, *len, "'m", 2, 0, M_1a);
  UNCHOP (wrd, *len, "'s", 2, 0, S_1a);
  UNCHOP (wrd, *len, "'t", 2, 0, T_1a);
  UNCHOP (wrd, *len, "'ll", 3, 0, LL_1a);
  UNCHOP (wrd, *len, "'re", 3, 0, RE_1a);
  UNCHOP (wrd, *len, "'ve", 3, 0, VE_1a);

  return;
}


static void unstep1b (R_UCHAR *wrd, R_UINT *len, enum S_STEM1b result) {
  UNCHOP (wrd, *len, "es", 2, 0, ES_1b);
  UNCHOP (wrd, *len, "s", 1, 0, S_1b);

  return;
}

static void unstep1c (R_UCHAR *wrd, R_UINT *len, enum S_STEM1c result) {
  if ((result == DOUBLE_ED_1c) || (result == DOUBLE_ING_1c)) {
    wrd[(*len)] = wrd[(*len) - 1];
    (*len)++;
    result -= 2;
  }
  else if ((result == E_ED_1c) || (result == E_ING_1c)) {
    (*len)--;
    result -= 1;
  }

  UNCHOP (wrd, *len, "ing", 3, 0, ING_1c);
  UNCHOP (wrd, *len, "ed", 2, 0, ED_1c);

  return;
}


static void unstep1d (R_UCHAR *wrd, R_UINT *len, enum S_STEM1d result) {
  UNCHOP (wrd, *len, "y", 1, 1, Y_I_1d);

  return;
}


static void unstep2 (R_UCHAR *wrd, R_UINT *len, enum S_STEM2 result) {
  UNCHOP (wrd, *len, "ional", 5, 1, IONAL_E_2);
  UNCHOP (wrd, *len, "al", 2, 0, AL_2);
  UNCHOP (wrd, *len, "i", 1, 1, I_E_2);
  UNCHOP (wrd, *len, "r", 1, 0, R_2);
  UNCHOP (wrd, *len, "li", 2, 0, LI_2);
  UNCHOP (wrd, *len, "ation", 5, 1, IZATION_IZE_2);
  UNCHOP (wrd, *len, "ion", 3, 1, ATION_ATE_2);
  UNCHOP (wrd, *len, "or", 2, 1, ATOR_ATE_2);
  UNCHOP (wrd, *len, "ism", 3, 0, ISM_2);
  UNCHOP (wrd, *len, "ness", 4, 0, NESS_2);
  UNCHOP (wrd, *len, "iti", 3, 0, ITI_2);
  UNCHOP (wrd, *len, "iti", 3, 1, ITI_E_2);
  UNCHOP (wrd, *len, "iliti", 5, 2, BILITI_BLE_2);
  /*  Next one is new, but mentioned on Porter's web site  */
  UNCHOP (wrd, *len, "i", 1, 0, I_2);

  return;
}


static void unstep3 (R_UCHAR *wrd, R_UINT *len, enum S_STEM3 result) {
  UNCHOP (wrd, *len, "ate", 3, 0, ATE_3);
  UNCHOP (wrd, *len, "ative", 5, 0, ATIVE_3);
  UNCHOP (wrd, *len, "ize", 3, 0, IZE_3);
  UNCHOP (wrd, *len, "iti", 3, 0, ITI_3);
  UNCHOP (wrd, *len, "al", 2, 0, AL_3);
  UNCHOP (wrd, *len, "ful", 3, 0, FUL_3);
  UNCHOP (wrd, *len, "ness", 4, 0, NESS_3);

  return;
}


static void unstep4 (R_UCHAR *wrd, R_UINT *len, enum S_STEM4 result) {
  UNCHOP (wrd, *len, "al", 2, 0, AL_4);
  UNCHOP (wrd, *len, "ance", 4, 0, ANCE_4);
  UNCHOP (wrd, *len, "ence", 4, 0, ENCE_4);
  UNCHOP (wrd, *len, "er", 2, 0, ER_4);
  UNCHOP (wrd, *len, "ic", 2, 0, IC_4);
  UNCHOP (wrd, *len, "able", 4, 0, ABLE_4);
  UNCHOP (wrd, *len, "ible", 4, 0, IBLE_4);
  UNCHOP (wrd, *len, "ant", 3, 0, ANT_4);
  UNCHOP (wrd, *len, "ement", 5, 0, EMENT_4);
  UNCHOP (wrd, *len, "ment", 4, 0, MENT_4);
  UNCHOP (wrd, *len, "ent", 3, 0, ENT_4);
  UNCHOP (wrd, *len, "ion", 3, 0, ION_4);
  UNCHOP (wrd, *len, "ou", 2, 0, OU_4);
  UNCHOP (wrd, *len, "ism", 3, 0, ISM_4);
  UNCHOP (wrd, *len, "ate", 3, 0, ATE_4);
  UNCHOP (wrd, *len, "iti", 3, 0, ITI_4);
  UNCHOP (wrd, *len, "ous", 3, 0, OUS_4);
  UNCHOP (wrd, *len, "ive", 3, 0, IVE_4);
  UNCHOP (wrd, *len, "ize", 3, 0, IZE_4);

  return;
}


static void unstep5a (R_UCHAR *wrd, R_UINT *len, enum S_STEM5a result) {

  UNCHOP (wrd, *len, "e", 1, 0, E_5a);

  return;
}


static void unstep5b (R_UCHAR *wrd, R_UINT *len, enum S_STEM5b result) {

  if (result == DOUBLE_5b) {
    wrd[(*len)] = wrd[(*len) - 1];
    (*len)++;
  }

  return;
}


R_UINT stem (R_UCHAR *wrd, R_UINT *wrd_len, R_UINT *m) {
  R_UINT modifier = 0;
  R_UINT len = *wrd_len;
  enum S_STEM1a result1a = NONE_1a;
  enum S_STEM1b result1b = NONE_1b;
  enum S_STEM1c result1c = NONE_1c;
  enum S_STEM1d result1d = NONE_1d;
  enum S_STEM2 result2 = NONE_2;
  enum S_STEM3 result3 = NONE_3;
  enum S_STEM4 result4 = NONE_4;
  enum S_STEM5a result5a = NONE_5a;
  enum S_STEM5b result5b = NONE_5b;

  if (len > 16) {
    fprintf (stderr, "Original length greater: %u\n", len);
    exit (EXIT_FAILURE);
  }
  /*  Do not bother stemming words if they are 3 characters or shorter
  **  or are a tag.  */
  if ((len <= 3) || (wrd[len - 1] == (R_UCHAR) '>')) {
    return (0);
  }

  m = findM (wrd, len, m);
  result1a = step1a (wrd, &len, m);
  result1b = step1b (wrd, &len, m);
  result1c = step1c (wrd, &len, m);
  result1d = step1d (wrd, &len, m);
  result2 = step2 (wrd, &len, m);
  result3 = step3 (wrd, &len, m);
  result4 = step4 (wrd, &len, m);
  result5a = step5a (wrd, &len, m);
  result5b = step5b (wrd, &len, m);

  modifier = packModifier (result1a, result1b, result1c, result1d, result2, result3, result4, result5a, result5b);

  /*
  fprintf (stderr, "(%u %u %u %u, %u, %u, %u, %u %u)\t", result1a, result1b, result1c, result1d, result2, result3, result4, result5a, result5b);
  */

  *wrd_len = len;
  return (modifier);
}


R_UINT unstem (R_UCHAR *wrd, R_UINT wrd_len, R_UINT modifier) {
  enum S_STEM1a result1a = NONE_1a;
  enum S_STEM1b result1b = NONE_1b;
  enum S_STEM1c result1c = NONE_1c;
  enum S_STEM1d result1d = NONE_1d;
  enum S_STEM2 result2 = NONE_2;
  enum S_STEM3 result3 = NONE_3;
  enum S_STEM4 result4 = NONE_4;
  enum S_STEM5a result5a = NONE_5a;
  enum S_STEM5b result5b = NONE_5b;
  R_UINT len = wrd_len;

  if ((modifier == 0) || (wrd[wrd_len - 1] == (R_UCHAR) '>')) {
    return (len);
  }

  unpackModifier (modifier, &result1a, &result1b, &result1c, &result1d, &result2, &result3, &result4, &result5a, &result5b);

  /*
  fprintf (stderr, "[%u, %u, %u, %u, %u]\n", result1, result2, result3, result4, result5);
  */

  if (result5b != NONE_5b) {
    unstep5b (wrd, &len, result5b);
  }
  if (result5a != NONE_5a) {
    unstep5a (wrd, &len, result5a);
  }
  if (result4 != NONE_4) {
    unstep4 (wrd, &len, result4);
  }
  if (result3 != NONE_3) {
    unstep3 (wrd, &len, result3);
  }
  if (result2 != NONE_2) {
    unstep2 (wrd, &len, result2);
  }
  if (result1d != NONE_1d) {
    unstep1d (wrd, &len, result1d);
  }
  if (result1c != NONE_1c) {
    unstep1c (wrd, &len, result1c);
  }
  if (result1b != NONE_1b) {
    unstep1b (wrd, &len, result1b);
  }
  if (result1a != NONE_1a) {
    unstep1a (wrd, &len, result1a);
  }

  return (len);
}


static R_UINT packModifier (enum S_STEM1a result1a, enum S_STEM1b result1b, enum S_STEM1c result1c, enum S_STEM1d result1d, enum S_STEM2 result2, enum S_STEM3 result3, enum S_STEM4 result4, enum S_STEM5a result5a, enum S_STEM5b result5b) {
  R_UINT modifier = 0;

  modifier = (R_UINT) result1a;

  modifier = modifier << 2;
  modifier |= (R_UINT) result1b;

  modifier = modifier << 3;
  modifier |= (R_UINT) result1c;

  modifier = modifier << 1;
  modifier |= (R_UINT) result1d;

  modifier = modifier << 4;
  modifier |= (R_UINT) result2;

  modifier = modifier << 3;
  modifier |= (R_UINT) result3;

  modifier = modifier << 5;
  modifier |= (R_UINT) result4;

  modifier = modifier << 1;
  modifier |= (R_UINT) result5a;

  modifier = modifier << 1;
  modifier |= (R_UINT) result5b;

  return (modifier);
}


static void unpackModifier (R_UINT modifier, enum S_STEM1a *result1a, enum S_STEM1b *result1b, enum S_STEM1c *result1c, enum S_STEM1d *result1d, enum S_STEM2 *result2, enum S_STEM3 *result3, enum S_STEM4 *result4, enum S_STEM5a *result5a, enum S_STEM5b *result5b) {

  *result5b = (enum S_STEM5b) modifier & 0x1;
  modifier = modifier >> 1;

  *result5a = (enum S_STEM5a) modifier & 0x1;
  modifier = modifier >> 1;

  *result4 = (enum S_STEM4) modifier & 0x1F;
  modifier = modifier >> 5;

  *result3 = (enum S_STEM3) modifier & 0x7;
  modifier = modifier >> 3;

  *result2 = (enum S_STEM2) modifier & 0xF;
  modifier = modifier >> 4;

  *result1d = (enum S_STEM1d) modifier & 0x1;
  modifier = modifier >> 1;

  *result1c = (enum S_STEM1c) modifier & 0x7;
  modifier = modifier >> 3;

  *result1b = (enum S_STEM1b) modifier & 0x3;
  modifier = modifier >> 2;

  *result1a = (enum S_STEM1a) modifier;

  /*
  fprintf (stderr, "%u, (%u, %u, %u), %u, %u, %u, (%u, %u)\n", *result1a, *result1b, *result1c, *result1d, *result2, *result3, *result4, *result5a, *result5b);
  */

  return;
}

