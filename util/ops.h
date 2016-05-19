/* ops.h
 *
 * Commonly used operations. Macro naming convention:
 * ME_ expands to an expression
 * MS_ expands to a statement (or compound statement)
 * author: Seth Rader
 */
#ifndef UTIL_OPS_H_
#define UTIL_OPS_H_

#define ME_MAX(a, b) \
    ((a) > (b) ? (a) : (b))

#define ME_MIN(a, b) \
    ((a) < (b) ? (a) : (b))

#define MS_SWAP(a, b) {\
    (a) ^= (b);\
    (b) ^= (a);\
    (a) ^= (b);\
}

#endif
