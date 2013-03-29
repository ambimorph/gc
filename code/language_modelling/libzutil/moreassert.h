/**
 * copyright 2002-2007 Bryce "Zooko" Wilcox-O'Hearn
 * mailto:zooko@zooko.com
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this work to deal in this work without
 * restriction (including the rights to use, modify, distribute,
 * sublicense, and/or sell copies).
 */

#ifndef __INCL_moreassert_h
#define __INCL_moreassert_h

/* implementation stuff that you needn't see in order to use the library */
#include "moreassertimp.h"

/**
 * A verbose abort message contains the file name, line number, function name, 
 * and a description string, like this:
 * test.c:238: test_minmax_strict: This string describes what went wrong.
 *
 * Use verbose_abort as though it is defined like this (although in reality it 
 * is a macro instead of a function):
 *
 * void verbose_abort(const char* msg);
 * void verbose_abort2(const char* msg1, const char* msg2);
 * void verbose_abort3(const char* msg1, const char* msg2, const char* msg3);
 * ... etc up to verbose_abort8()
 *
 * runtime_assert2 processes the msg arguments in the following simple way: 
 * first it treats any NULL pointers as empty strings, second it concatenates 
 * all the msg arguments together.  It does not do any other interpolation or 
 * processing of their contents.
 */

/**
 * runtime_assert() gets checked even if NDEBUG is set.
 *
 * A typical use of runtime_assert() is to test input and exit if the input 
 * isn't correct.  You shouldn't use normal assert() for this, because you 
 * ought to test the input even if the program was compiled with the NDEBUG 
 * flag.
 *
 * Use runtime_assert as though it is defined like this (although in reality it 
 * is a macro instead of a function):
 * bool runtime_assert(int condition, const char* msg);
 * bool runtime_assert2(int condition, const char* msg1, const char* msg2);
 * bool runtime_assert3(int condition, const char* msg1, const char* msg2, const char* msg3);
 * bool runtime_assert4(int condition, const char* msg1, const char* msg2, const char* msg3, const char* msg4);
 *
 * (It evaluates to true, if it doesn't exit with an exception msg.)
 *
 * Sometimes it can be useful to have two error messages, especially if one of 
 * them is generated by lower-level code, like this:
 *
 * retval = somelib_frob(whatsit);
 * runtime_assert2(retval == 0, "non-zero return value from somelib_frob(). somelib error message: ", somelib_geterrmsg());
 *
 * runtime_assert2 processes the msg arguments in the following simple way: 
 * first it treats any NULL pointers as empty strings, second it concatenates 
 * all the msg arguments together, inserting a separator string ("; ") between 
 * each successive pair of msgs.  It does not do any other interpolation or 
 * processing of their contents.
 */

#endif /* #ifndef __INCL_moreassert_h */