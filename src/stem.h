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

#ifndef STEM_H
#define STEM_H

#define MAXSTEMLEN 32

enum CV { UNKNOWN = 0, CONSONANT = 1, VOWEL = 2 };

enum S_STEM1a { NONE_1a = 0, D_1a = 1, M_1a = 2, S_1a = 3, T_1a = 4, LL_1a = 5, RE_1a = 6, VE_1a = 7};

enum S_STEM1b { NONE_1b = 0, ES_1b = 1, S_1b = 2 };

enum S_STEM1c { NONE_1c = 0, ED_1c = 1, E_ED_1c = 2, DOUBLE_ED_1c = 3, ING_1c = 4, E_ING_1c = 5, DOUBLE_ING_1c = 6 };

enum S_STEM1d { NONE_1d = 0, Y_I_1d = 1 };

enum S_STEM2 { NONE_2 = 0, IONAL_E_2 = 1, AL_2 = 2, I_E_2 = 3, R_2 = 4, LI_2 = 5, IZATION_IZE_2 = 6, ATION_ATE_2 = 7, ATOR_ATE_2 = 8, ISM_2 = 9, NESS_2 = 10, ITI_2 = 11, ITI_E_2 = 12, BILITI_BLE_2 = 13, I_2 = 14 };

enum S_STEM3 { NONE_3 = 0, ATE_3 = 1, ATIVE_3 = 2, IZE_3 = 3, ITI_3 = 4, AL_3 = 5, FUL_3 = 6, NESS_3 = 7};

enum S_STEM4 { NONE_4 = 0, AL_4 = 1, ANCE_4 = 2, ENCE_4 = 3, ER_4 = 4, IC_4 = 5, ABLE_4 = 6, IBLE_4 = 7, ANT_4 = 8, EMENT_4 = 9, MENT_4 = 10, ENT_4 = 11, ION_4 = 12, OU_4 = 13, ISM_4 = 14, ATE_4 = 15, ITI_4 = 16, OUS_4 = 17, IVE_4 = 18, IZE_4 = 19 };

enum S_STEM5a { NONE_5a = 0, E_5a = 1 };

enum S_STEM5b { NONE_5b = 0, DOUBLE_5b = 1 };

#ifdef BRITISH_SPELL
#define EN_IZ "is"
#define EN_IZER "iser"
#define EN_IZATION "isation"
#define EN_ALIZE "alise"
#define EN_IZE "ise"
#else
#define EN_IZ "iz"
#define EN_IZER "izer"
#define EN_IZATION "ization"
#define EN_ALIZE "alize"
#define EN_IZE "ize"
#endif


#define CLASSIFYCHAR(STR,POS)                                      \
((STR[POS] == (R_UCHAR) 'a' || STR[POS] == (R_UCHAR) 'e' ||        \
STR[POS] == (R_UCHAR) 'i' || STR[POS] == (R_UCHAR) 'o' ||          \
STR[POS] == (R_UCHAR) 'u' || ((POS != 0) && (STR[POS] ==           \
(R_UCHAR) 'y') && (STR[POS - 1] != (R_UCHAR) 'a') && (STR[POS - 1] \
!= (R_UCHAR) 'e') && (STR[POS - 1] != (R_UCHAR) 'i') &&            \
(STR[POS - 1] != (R_UCHAR) 'o') && (STR[POS - 1] !=                \
(R_UCHAR) 'u'))) ? VOWEL : CONSONANT)


/*  Check if STR, of length LEN, is longer than SUFLEN, the length of the 
**  suffix.  If so, then check if SUF is a suffix of STR.  If a match 
**  occurs, then decrement the length of the string by CUTLEN (where 
**  CUTLEN <= SUFLEN) and set FLAG.  Use the M array to check MVAL, if 
**  necessary.  The datatypes are:
**    STR (R_UCHAR)
**    LEN (R_UINT)
**    SUF (R_UCHAR)
**    SUFLEN (R_UINT)
**    CUTLEN (R_UINT)
**    FLAG (some enumerated type)
**    M (R_UINT*)
**    MVAL (R_UINT)
** 
**  The M array needs to be updated after any change.   CHOP does the 
**  same, but without checking M.  MOD_CHOP_M is used for cases when
**  cutting a suffix alone is not sufficient and a change in the 
**  string is required.  In this case, NEWSUF of length NEWSUFLEN is
**  added to STR after CUTLEN is removed from SUF.  The functions
**  return when complete, except for the ones which end in NORETURN.
*/
#define CHOP_M(STR,LEN,SUF,SUFLEN,CUTLEN,FLAG,M,MVAL)        \
if ((match_result == R_FALSE) && ((LEN) > SUFLEN)) {         \
  if (memcmp (STR+(LEN)-SUFLEN, SUF, SUFLEN) == 0) {         \
    match_result = R_TRUE;                                   \
    if (M[(LEN)-SUFLEN-1] > MVAL) {                          \
      (LEN) -= CUTLEN;                                       \
      result = FLAG;                                         \
      M = findM (STR, LEN, M);                               \
    }                                                        \
    return (result);                                         \
  }                                                          \
}


#define CHOP_M_NORETURN(STR,LEN,SUF,SUFLEN,CUTLEN,FLAG,M,MVAL)\
if ((match_result == R_FALSE) && ((LEN) > SUFLEN)) {         \
  if (memcmp (STR+(LEN)-SUFLEN, SUF, SUFLEN) == 0) {         \
    match_result = R_TRUE;                                   \
    if (M[(LEN)-SUFLEN-1] > MVAL) {                          \
      (LEN) -= CUTLEN;                                       \
      result = FLAG;                                         \
      M = findM (STR, LEN, M);                               \
    }                                                        \
  }                                                          \
}


#define MOD_CHOP_M(STR,LEN,SUF,SUFLEN,CUTLEN,NEWSUF,NEWSUFLEN,FLAG,M,MVAL)     \
if ((match_result == R_FALSE) && ((LEN) > SUFLEN)) {         \
  if (memcmp (STR+(LEN)-SUFLEN, SUF, SUFLEN) == 0) {         \
    match_result = R_TRUE;                                   \
    if (M[(LEN)-SUFLEN-1] > MVAL) {                          \
      (LEN) -= CUTLEN;                                       \
      memcpy (STR+(LEN),NEWSUF,NEWSUFLEN);                   \
      (LEN) += NEWSUFLEN;                                    \
      result = FLAG;                                         \
      M = findM (STR, LEN, M);                               \
    }                                                        \
    return (result);                                         \
  }                                                          \
}


#define CHOP(STR,LEN,SUF,SUFLEN,CUTLEN,FLAG,M)               \
if ((match_result == R_FALSE) && ((LEN) > SUFLEN)) {         \
  if (memcmp (STR+(LEN)-SUFLEN, SUF, SUFLEN) == 0) {         \
    match_result = R_TRUE;                                   \
    (LEN) -= CUTLEN;                                         \
    result = FLAG;                                           \
    M = findM (STR, LEN, M);                                 \
    return (result);                                         \
  }                                                          \
}


/*  Add SUF back to the end of STR.  STR is of length LEN and the
**  suffix is of length SUFLEN.  SUF is added after CUTLEN is 
**  removed from STR.  If CUTLEN = 0, then SUF is appended on to
**  STR.  All of this is done only if result is equal to FLAG.
**  The datatypes are:
**    STR (R_UCHAR)
**    LEN (R_UINT)
**    SUF (R_UCHAR)
**    SUFLEN (R_UINT)
**    CUTLEN (R_UINT)
**    FLAG (some enumerated type)
*/
#define UNCHOP(STR,LEN,SUF,SUFLEN,CUTLEN,FLAG)               \
if (result == FLAG) {                                        \
  (LEN) -= CUTLEN;                                           \
  memcpy (STR+(LEN), SUF, SUFLEN);                           \
  (LEN) += SUFLEN;                                           \
  return;                                                    \
}

R_UINT stem (R_UCHAR *wrd, R_UINT *wrd_len, R_UINT *m);
R_UINT unstem (R_UCHAR *wrd, R_UINT len, R_UINT modifier);


#endif

