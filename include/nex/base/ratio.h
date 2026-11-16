/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/attributes.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/limits.h"

NEX_NAMESPACE_BEGIN

NEX_SUBNAMESPACE_BEGIN(ratio_impl)

// Compute the greatest common divisor (GCD) of two integers at compile time.

template <intmax XVal, intmax YVal>
inline const intmax staticGCD = staticGCD<YVal, XVal % YVal>;

template <intmax XVal>
inline const intmax staticGCD<XVal, 0> = XVal;

template <>
inline const intmax staticGCD<0, 0> = 1;

// Compute the least common multiple (LCM) of two integers at compile time.
template <intmax XVal, intmax YVal>
inline const intmax staticLCM = XVal / staticGCD<XVal, YVal> * YVal;

// Compute the absolute value of an integer at compile time.
template <intmax XVal>
inline const intmax staticAbs = XVal < 0 ? -XVal : XVal;

// Compute the sign of an integer at compile time.
template <intmax XVal>
inline const intmax staticSign = XVal == 0 ? 0 : (XVal < 0 ? -1 : 1);

template <
    intmax XVal, intmax YVal,
    intmax = staticSign<YVal>
>
class LLAdd;

/**
 * @brief Compile-time addition of two integers with overflow checking.
 *
 * @tparam XVal The first integer operand.
 * @tparam YVal The second integer operand.
 * @tparam Sign The sign of the second operand (default: staticSign<YVal>).
 *
 * @note
 * This class performs compile-time addition with overflow checking based on the sign of the second operand.
 * It ensures that the result of the addition does not exceed the representable range of `intmax`.
 */
template <intmax XVal, intmax YVal>
class LLAdd<XVal, YVal, 1> {
    static const intmax min = (1LL << (sizeof(intmax) * NEX_CHAR_BIT - 1)) + 1;
    static const intmax max = -min;

    static_assert(XVal <= max - YVal, "Error: Overflow in LLAdd");

public:
    static const intmax value = XVal + YVal;
};

// Compile-time addition of two integers with overflow checking for the case when the second operand is zero.
// No overflow check is needed since adding zero does not change the value.
template <intmax XVal, intmax YVal>
class LLAdd<XVal, YVal, 0> {
public:
    static const intmax value = XVal;
};

// Compile-time addition of two integers with overflow checking for the case when the second operand is negative.
template <intmax XVal, intmax YVal>
class LLAdd<XVal, YVal, -1> {
    static const intmax min = (1LL << (sizeof(intmax) * NEX_CHAR_BIT - 1)) + 1;
    static const intmax max = -min;

    static_assert(min - YVal <= XVal, "Error: Overflow in LLAdd");

public:
    static const intmax value = XVal + YVal;
};

template <intmax XVal, intmax YVal, intmax = staticSign<YVal> >
class LLSub;

/**
 * @brief Compile-time subtraction of two integers with overflow checking.
 *
 * @tparam XVal The first integer operand.
 * @tparam YVal The second integer operand.
 * @tparam Sign The sign of the second operand (default: staticSign<YVal>).
 *
 * @note
 * This class performs compile-time subtraction with overflow checking based on the sign of the second operand.
 * It ensures that the result of the subtraction does not exceed the representable range of `intmax`.
 */
template <intmax XVal, intmax YVal>
class LLSub<XVal, YVal, 1> {
    static const intmax min = (1LL << (sizeof(intmax) * NEX_CHAR_BIT - 1)) + 1;
    static const intmax max = -min;

    static_assert(min + YVal <= XVal, "Error: Overflow in LLSub");

public:
    static const intmax value = XVal - YVal;
};

// Compile-time subtraction of two integers with overflow checking for the case when the second operand is zero.
// No overflow check is needed since subtracting zero does not change the value.
template <intmax XVal, intmax YVal>
class LLSub<XVal, YVal, 0> {
public:
    static const intmax value = XVal;
};

// Compile-time subtraction of two integers with overflow checking for the case when the second operand is negative.
template <intmax XVal, intmax YVal>
class LLSub<XVal, YVal, -1> {
    static const intmax min = (1LL << (sizeof(intmax) * NEX_CHAR_BIT - 1)) + 1;
    static const intmax max = -min;

    static_assert(XVal <= max + YVal, "Error: Overflow in LLSub");

public:
    static const intmax value = XVal - YVal;
};

/**
 * @brief Compile-time multiplication of two integers with overflow checking.
 *
 * @tparam XVal The first integer operand.
 * @tparam YVal The second integer operand.
 *
 * @note
 * This class performs compile-time multiplication with overflow checking.
 * It ensures that the result of the multiplication does not exceed the representable range of `intmax`.
 */
template <intmax XVal, intmax YVal>
class LLMul {
    static const intmax nan   = (1LL << (sizeof(intmax) * NEX_CHAR_BIT - 1));
    static const intmax min   = nan + 1;
    static const intmax max   = -min;
    static const intmax aX = staticAbs<XVal>;
    static const intmax aY = staticAbs<YVal>;

    static_assert(XVal != nan && YVal != nan && aX <= max / aY, "Error: Overflow in LLMul");

public:
    static const intmax value = XVal * YVal;
};

// Compile-time multiplication of two integers with overflow checking for the case when the first operand is zero.
template <intmax YVal>
class LLMul<0, YVal> {
public:
    static const intmax value = 0;
};

// Compile-time multiplication of two integers with overflow checking for the case when the second operand is zero.
template <intmax XVal>
class LLMul<XVal, 0> {
public:
    static const intmax value = 0;
};

// Compile-time multiplication of two integers with overflow checking for the case when both operands are zero.
template <>
class LLMul<0, 0> {
public:
    static const intmax value = 0;
};

// Not actually used but left here in case needed in future maintenance
template <intmax XVal, intmax YVal>
class LLDiv {
    static const intmax nan = (1LL << (sizeof(intmax) * NEX_CHAR_BIT - 1));
    static const intmax min = nan + 1;
    static const intmax max = -min;

    static_assert(XVal != nan && YVal != nan && YVal != 0, "Error: Overflow in LLDiv");

public:
    static const intmax value = XVal / YVal;
};

NEX_SUBNAMESPACE_END(ratio_impl)

/**
 * @brief Compile-time representation of a rational number.
 *
 * @tparam Num The numerator of the ratio.
 * @tparam Den The denominator of the ratio.
 *
 * @note
 * This class represents a rational number as a compile-time constant ratio of two integers.
 * It performs compile-time checks to ensure the numerator and denominator are within valid ranges
 * and that the denominator is not zero. The ratio is automatically reduced to its simplest form.
 */
template <intmax Num, intmax Den = 1>
class Ratio {
    static_assert(ratio_impl::staticAbs<Num> >= 0,  "Error: Ratio numerator is out of range");
    static_assert(Den != 0,                         "Error: Ratio divide by 0");
    static_assert(ratio_impl::staticAbs<Den> > 0,   "Error: Ratio denominator is out of range");

    static constexpr const intmax na  = ratio_impl::staticAbs<Num>;
    static constexpr const intmax da  = ratio_impl::staticAbs<Den>;
    static constexpr const intmax s   = ratio_impl::staticSign<Num> * ratio_impl::staticSign<Den>;
    static constexpr const intmax gcd = ratio_impl::staticGCD<na, da>;

public:
    static inline constexpr const intmax num = s * na / gcd;
    static inline constexpr const intmax den = da / gcd;

    using type NEX_NODEBUG = Ratio<num, den>;
};

// Trait to check if a type is a specialization of the Ratio template.
template <class Type>
inline const bool IsRatioV = false;

// Specialization of the IsRatioV trait for the Ratio template.
template <intmax Num, intmax Den>
inline const bool IsRatioV<Ratio<Num, Den> > = true;

/**
 * @brief Commonly used SI prefixes as Ratio types.
 *
 * @details
 * This section defines type aliases for commonly used SI prefixes as Ratio types.
 * Each alias represents the corresponding ratio as a compile-time constant using the Ratio template.
 */

using atto      = Ratio<1LL, 1000000000000000000LL>;        // 1/10^18
using femto     = Ratio<1LL, 1000000000000000LL>;           // 1/10^15
using pico      = Ratio<1LL, 1000000000000LL>;              // 1/10^12
using nano      = Ratio<1LL, 1000000000LL>;                 // 1/10^9
using micro     = Ratio<1LL, 1000000LL>;                    // 1/10^6
using milli     = Ratio<1LL, 1000LL>;                       // 1/10^3
using centi     = Ratio<1LL, 100LL>;                        // 1/10^2
using deci      = Ratio<1LL, 10LL>;                         // 1/10^1
using deca      = Ratio<10LL, 1LL>;                         // 10^1
using hecto     = Ratio<100LL, 1LL>;                        // 10^2
using kilo      = Ratio<1000LL, 1LL>;                       // 10^3
using mega      = Ratio<1000000LL, 1LL>;                    // 10^6
using giga      = Ratio<1000000000LL, 1LL>;                 // 10^9
using tera      = Ratio<1000000000000LL, 1LL>;              // 10^12
using peta      = Ratio<1000000000000000LL, 1LL>;           // 10^15
using exa       = Ratio<1000000000000000000LL, 1LL>;        // 10^18

NEX_SUBNAMESPACE_BEGIN(ratio_impl)

/**
 * @brief Multiplies two Ratio types.
 * 
 * @details
 * This template calculates the product of two Ratio types, Ratio1 and Ratio2,
 * and provides the result as a nested type named 'type'.
 * The calculation uses the greatest common divisor (GCD) to simplify the result
 * and prevent overflow during multiplication.
 * 
 * @tparam Ratio1 The first Ratio type.
 * @tparam Ratio2 The second Ratio type.
 */
template <class Ratio1, class Ratio2>
struct RatioMultiply {
private:
  static const intmax gcdN1D2 = staticGCD<Ratio1::num, Ratio2::den>;
  static const intmax gcdD1N2 = staticGCD<Ratio1::den, Ratio2::num>;

  static_assert(IsRatioV<Ratio1>, "[ratio.general]/2 requires R1 to be a specialisation of the ratio template");
  static_assert(IsRatioV<Ratio2>, "[ratio.general]/2 requires R2 to be a specialisation of the ratio template");

public:
  using type NEX_NODEBUG =
    typename Ratio<
        LLMul<Ratio1::num / gcdN1D2, Ratio2::num / gcdD1N2>::value,
        LLMul<Ratio2::den / gcdN1D2, Ratio1::den / gcdD1N2>::value
    >::type;
};

// Helper alias template for RatioMultiply.
template <class Ratio1, class Ratio2>
using RatioMultiplyT NEX_NODEBUG = typename RatioMultiply<Ratio1, Ratio2>::type;

/**
 * @brief Divides two Ratio types.
 * 
 * @details
 * This template calculates the quotient of two Ratio types, Ratio1 and Ratio2,
 * and provides the result as a nested type named 'type'.
 * The calculation uses the greatest common divisor (GCD) to simplify the result
 * and prevent overflow during division.
 * 
 * @tparam Ratio1 The first Ratio type.
 * @tparam Ratio2 The second Ratio type.
 */
template <class Ratio1, class Ratio2>
struct RatioDivide {
private:
  static const intmax gcdN1N2 = staticGCD<Ratio1::num, Ratio2::num>;
  static const intmax gcdD1D2 = staticGCD<Ratio1::den, Ratio2::den>;

  static_assert(IsRatioV<Ratio1>, "[ratio.general]/2 requires R1 to be a specialisation of the ratio template");
  static_assert(IsRatioV<Ratio2>, "[ratio.general]/2 requires R2 to be a specialisation of the ratio template");

public:
  using type NEX_NODEBUG =
    typename Ratio<
        LLMul<Ratio1::num / gcdN1N2, Ratio2::den / gcdD1D2>::value,
        LLMul<Ratio2::num / gcdN1N2, Ratio1::den / gcdD1D2>::value
    >::type;
};

// Helper alias template for RatioDivide.
template <class Ratio1, class Ratio2>
using RatioDivideT NEX_NODEBUG = typename RatioDivide<Ratio1, Ratio2>::type;

/**
 * @brief Adds two Ratio types.
 * 
 * @details
 * This template calculates the sum of two Ratio types, Ratio1 and Ratio2,
 * and provides the result as a nested type named 'type'.
 * The calculation uses the greatest common divisor (GCD) to simplify the result
 * and prevent overflow during addition.
 * 
 * @tparam Ratio1 The first Ratio type.
 * @tparam Ratio2 The second Ratio type.
 */
template <class Ratio1, class Ratio2>
struct RatioAdd {
private:
  static const intmax gcdN1N2 = staticGCD<Ratio1::num, Ratio2::num>;
  static const intmax gcdD1D2 = staticGCD<Ratio1::den, Ratio2::den>;

  static_assert(IsRatioV<Ratio1>, "[ratio.general]/2 requires R1 to be a specialisation of the ratio template");
  static_assert(IsRatioV<Ratio2>, "[ratio.general]/2 requires R2 to be a specialisation of the ratio template");

public:
  using type NEX_NODEBUG =
    typename RatioMultiply<
        Ratio<gcdN1N2, Ratio1::den / gcdD1D2>,
        Ratio<
            LLAdd<
                LLMul<Ratio1::num / gcdN1N2, Ratio2::den / gcdD1D2>::value,
                LLMul<Ratio2::num / gcdN1N2, Ratio1::den / gcdD1D2>::value
            >::value,
            Ratio2::den
        >
    >::type;
};

// Helper alias template for RatioAdd.
template <class Ratio1, class Ratio2>
using RatioAddT NEX_NODEBUG = typename RatioAdd<Ratio1, Ratio2>::type;

/**
 * @brief Subtracts two Ratio types.
 * 
 * @details
 * This template calculates the difference of two Ratio types, Ratio1 and Ratio2,
 * and provides the result as a nested type named 'type'.
 * The calculation uses the greatest common divisor (GCD) to simplify the result
 * and prevent overflow during subtraction.
 * 
 * @tparam Ratio1 The first Ratio type.
 * @tparam Ratio2 The second Ratio type.
 */
template <class Ratio1, class Ratio2>
struct RatioSubtract {
private:
  static const intmax gcdN1N2 = staticGCD<Ratio1::num, Ratio2::num>;
  static const intmax gcdD1D2 = staticGCD<Ratio1::den, Ratio2::den>;

  static_assert(IsRatioV<Ratio1>, "[ratio.general]/2 requires R1 to be a specialisation of the ratio template");
  static_assert(IsRatioV<Ratio2>, "[ratio.general]/2 requires R2 to be a specialisation of the ratio template");

public:
  using type NEX_NODEBUG =
    typename RatioMultiply<
        Ratio<gcdN1N2, Ratio1::den / gcdD1D2>,
        Ratio<
            LLSub<
                LLMul<Ratio1::num / gcdN1N2, Ratio2::den / gcdD1D2>::value,
                LLMul<Ratio2::num / gcdN1N2, Ratio1::den / gcdD1D2>::value
            >::value,
            Ratio2::den
        >
    >::type;
};

// Helper alias template for RatioSubtract.
template <class Ratio1, class Ratio2>
using RatioSubtractT NEX_NODEBUG = typename RatioSubtract<Ratio1, Ratio2>::type;

/**
 * @brief Calculates the greatest common divisor (GCD) of two Ratio types.
 * 
 * @details
 * This template calculates the GCD of the numerators of Ratio1 and Ratio2,
 * and the least common multiple (LCM) of their denominators, providing the result
 * as a Ratio type.
 * 
 * @tparam Ratio1 The first Ratio type.
 * @tparam Ratio2 The second Ratio type.
 */
template <class Ratio1, class Ratio2>
using RatioGCD NEX_NODEBUG =
    Ratio<
        staticGCD<Ratio1::num, Ratio2::num>,
        staticLCM<Ratio1::den, Ratio2::den>
    >;

NEX_SUBNAMESPACE_END(ratio_impl)

// Compile-time addition of two Ratio types.
template <class Ratio1, class Ratio2>
using RatioAdd NEX_NODEBUG = ratio_impl::RatioAddT<Ratio1, Ratio2>;

// Compile-time subtraction of two Ratio types.
template <class Ratio1, class Ratio2>
using RatioSubtract NEX_NODEBUG = ratio_impl::RatioSubtractT<Ratio1, Ratio2>;

// Compile-time multiplication of two Ratio types.
template <class Ratio1, class Ratio2>
using RatioMultiply NEX_NODEBUG = ratio_impl::RatioMultiplyT<Ratio1, Ratio2>;

// Compile-time division of two Ratio types.
template <class Ratio1, class Ratio2>
using RatioDivide NEX_NODEBUG = ratio_impl::RatioDivideT<Ratio1, Ratio2>;

// Compile-time equality comparison of two Ratio types.
template <class Ratio1, class Ratio2>
struct RatioEqual : meta::BoolConstant<(Ratio1::num == Ratio2::num && Ratio1::den == Ratio2::den)> {
    static_assert(IsRatioV<Ratio1>, "[ratio.general]/2 requires R1 to be a specialisation of the ratio template");
    static_assert(IsRatioV<Ratio2>, "[ratio.general]/2 requires R2 to be a specialisation of the ratio template");
};

// Compile-time inequality comparison of two Ratio types.
template <class Ratio1, class Ratio2>
struct RatioNotEqual : meta::BoolConstant<!RatioEqual<Ratio1, Ratio2>::value> {
    static_assert(IsRatioV<Ratio1>, "[ratio.general]/2 requires R1 to be a specialisation of the ratio template");
    static_assert(IsRatioV<Ratio2>, "[ratio.general]/2 requires R2 to be a specialisation of the ratio template");
};

// Compile-time equality comparison of two Ratio types as a boolean value.
template <class Ratio1, class Ratio2>
inline constexpr bool RatioEqualV = RatioEqual<Ratio1, Ratio2>::value;

// Compile-time inequality comparison of two Ratio types as a boolean value.
template <class Ratio1, class Ratio2>
inline constexpr bool RatioNotEqualV = !RatioEqual<Ratio1, Ratio2>::value;

NEX_SUBNAMESPACE_BEGIN(ratio_impl)

// Implementation details for compile-time less-than comparison of two Ratio types.
template <
    class Ratio1,
    class Ratio2,
    bool IsOdd = false,
    intmax Q1  = Ratio1::num / Ratio1::den,
    intmax M1  = Ratio1::num % Ratio1::den,
    intmax Q2  = Ratio2::num / Ratio2::den,
    intmax M2  = Ratio2::num % Ratio2::den
>
struct RatioLessImpl {
    static const bool value = IsOdd ? Q2 < Q1 : Q1 < Q2;
};

// Specialization for the case when both ratios have zero remainders.
// In this case, the ratios are exactly equal, so the less-than comparison is false.
template <
    class Ratio1,
    class Ratio2,
    bool IsOdd,
    intmax Qp
>
struct RatioLessImpl<Ratio1, Ratio2, IsOdd, Qp, 0, Qp, 0> {
    static const bool value = false;
};

// Specialization for the case when the first ratio has zero remainder and the second does not.
template <
    class Ratio1,
    class Ratio2,
    bool IsOdd,
    intmax Qp,
    intmax M2
>
struct RatioLessImpl<Ratio1, Ratio2, IsOdd, Qp, 0, Qp, M2> {
    static const bool value = !IsOdd;
};

// Specialization for the case when the second ratio has zero remainder and the first does not.
// In this case, the less-than comparison depends on the parity of the comparison level.
template <class Ratio1, class Ratio2, bool IsOdd, intmax Qp, intmax M1>
struct RatioLessImpl<Ratio1, Ratio2, IsOdd, Qp, M1, Qp, 0> {
    static const bool value = IsOdd;
};

// General case for the less-than comparison when both ratios have non-zero remainders.
// The comparison is performed recursively on the remainders, with the parity flag toggled.
template <
    class Ratio1,
    class Ratio2,
    bool IsOdd,
    intmax Qp,
    intmax M1,
    intmax M2
>
struct RatioLessImpl<Ratio1, Ratio2, IsOdd, Qp, M1, Qp, M2> {
    static const bool value =
        RatioLessImpl<
            Ratio<Ratio1::den, M1>,
            Ratio<Ratio2::den, M2>,
            !IsOdd
        >::value;
};

// Primary template for compile-time less-than comparison of two Ratio types.
// This handles the cases where the signs of the ratios are different.
// Specializations for the case when both ratios are positive or both are negative are provided below.
// These specializations delegate to the RatioLessImpl structure for the actual comparison logic.
template <
    class Ratio1,
    class Ratio2,
    intmax S1 = staticSign<Ratio1::num>,
    intmax S2 = staticSign<Ratio2::num>
>
struct RatioLess {
    static const bool value = S1 < S2;
};

// Specialization for the case when both ratios are positive.
template <class Ratio1, class Ratio2>
struct RatioLess<Ratio1, Ratio2, 1LL, 1LL> {
    static const bool value = RatioLessImpl<Ratio1, Ratio2>::value;
};

// Specialization for the case when both ratios are negative.
// The comparison is performed by negating both ratios and reversing the order,
// effectively comparing their absolute values.
template <class Ratio1, class Ratio2>
struct RatioLess<Ratio1, Ratio2, -1LL, -1LL> {
    static const bool value =
        RatioLessImpl<
            Ratio<-Ratio2::num, Ratio2::den>,
            Ratio<-Ratio1::num, Ratio1::den>
        >::value;
};

NEX_SUBNAMESPACE_END(ratio_impl)

// Compile-time less-than comparison of two Ratio types.
template <class Ratio1, class Ratio2>
struct RatioLess : meta::BoolConstant<ratio_impl::RatioLess<Ratio1, Ratio2>::value> {
    static_assert(IsRatioV<Ratio1>, "[ratio.general]/2 requires R1 to be a specialisation of the ratio template");
    static_assert(IsRatioV<Ratio2>, "[ratio.general]/2 requires R2 to be a specialisation of the ratio template");
};

// Compile-time less-than-or-equal comparison of two Ratio types.
template <class Ratio1, class Ratio2>
struct RatioLessEqual : meta::BoolConstant<!RatioLess<Ratio2, Ratio1>::value> {
    static_assert(IsRatioV<Ratio1>, "[ratio.general]/2 requires R1 to be a specialisation of the ratio template");
    static_assert(IsRatioV<Ratio2>, "[ratio.general]/2 requires R2 to be a specialisation of the ratio template");
};

// Compile-time greater-than comparison of two Ratio types.
template <class Ratio1, class Ratio2>
struct RatioGreater : meta::BoolConstant<RatioLess<Ratio2, Ratio1>::value> {
    static_assert(IsRatioV<Ratio1>, "[ratio.general]/2 requires R1 to be a specialisation of the ratio template");
    static_assert(IsRatioV<Ratio2>, "[ratio.general]/2 requires R2 to be a specialisation of the ratio template");
};

// Compile-time greater-than-or-equal comparison of two Ratio types.
template <class Ratio1, class Ratio2>
struct RatioGreaterEqual : meta::BoolConstant<!RatioLess<Ratio1, Ratio2>::value> {
    static_assert(IsRatioV<Ratio1>, "[ratio.general]/2 requires R1 to be a specialisation of the ratio template");
    static_assert(IsRatioV<Ratio2>, "[ratio.general]/2 requires R2 to be a specialisation of the ratio template");
};

// Variable template for compile-time less-than comparison of two Ratio types.
template <class Ratio1, class Ratio2>
inline constexpr bool RatioLessV = RatioLess<Ratio1, Ratio2>::value;

// Variable template for compile-time less-than-or-equal comparison of two Ratio types.
template <class Ratio1, class Ratio2>
inline constexpr bool RatioLessEqualV = RatioLessEqual<Ratio1, Ratio2>::value;

// Variable template for compile-time greater-than comparison of two Ratio types.
template <class Ratio1, class Ratio2>
inline constexpr bool RatioGreaterV = RatioGreater<Ratio1, Ratio2>::value;

// Variable template for compile-time greater-than-or-equal comparison of two Ratio types.
template <class Ratio1, class Ratio2>
inline constexpr bool RatioGreaterEqualV = RatioGreaterEqual<Ratio1, Ratio2>::value;

NEX_NAMESPACE_END
