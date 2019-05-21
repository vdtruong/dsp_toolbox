/*
DSPRelated.com Logo

    Home
    Forums
    Code Snippets
    Blogs
    Books
    Jobs	
    Papers
    Links
    Contact Us

Sign in
Not a member? | Forgot your Password?
Search code
Search tips
Free PDF Downloads

A Quadrature Signals Tutorial: Complex, But Not Complicated

Understanding the 'Phasing Method' of Single Sideband Demodulation

Complex Digital Signal Processing in Telecommunications

Introduction to Sound Processing

C++ Tutorial

Introduction of C Programming for DSP Applications

Fixed-Point Arithmetic: An Introduction

Cascaded Integrator-Comb (CIC) Filter Introduction
FFT Spectral Analysis Software
See Also
Embedded SystemsFPGA
DSP Code Sharing > Fixed-point Math Library

Fixed-point Math Library

Language: C

Processor: Not Relevant

Submitted by Jordan Rhee on Nov 30 2010

Licensed under a Creative Commons Attribution 3.0 Unported License
Fixed-point Math Library

 

We provide a set of macros and routines that perform 
fixed-point arithmetic, including basic arithmetic operators, 
conversions between fixed-point and floating point types, 
sines and cosines, exponentiation, and logarithms. 
This code has been used on an embedded ARM system without an FPU, 
and achieves much better performance than floating point arithmetic. 
This code can be used in conjunction with our fixed-point filtering 
library: http://ieee.ucsd.edu/wiki/tutorials:fixed_point_filtering_library.

qmath.h contains all the basic arithmetic operations as macros or inline 
functions, while qmath.c contains the more involved routines such as sine 
and cosine. */
 
/* 
Rate this code snippet:
Rating: 5 | Votes: 1
 
   
 
posted by Jordan Rhee	
Jordan Rhee received his B.S. in Electrical Engineering from 
UC San Diego in 2010. 
He is currently pursing an M.S. in Circuits and Systems. 
His areas of interest are circuits, embedded systems, and DSP.


Comments	

No comments yet for this code


Add a Comment
You need to login before you can post a comment (best way to prevent spam). ( Not a member? )

    DSP Related Home
    Discussion Groups
    Code Snippets
    Blogs
    Articles
    DSP Books
    privacy policy
    Contact

Loading...
*/
 
/* **********************************************************************
 *
 * Fixed Point Math Library
 *
 * **********************************************************************
 *
 * qmath.h
 *
 * Alex Forencich
 * alex@alexelectronics.com
 *
 * Jordan Rhee
 * rhee.jordan@gmail.com
 *
 * IEEE UCSD
 * http://ieee.ucsd.edu
 *
 * **********************************************************************/

#ifndef _QMATH_H_
#define _QMATH_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef INLINE
#ifdef _MSC_VER
#define INLINE __inline
#else
#define INLINE inline
#endif /* _MSC_VER */
#endif /* INLINE */

/*
 * Default fractional bits. This precision is used in the routines
 * and macros without a leading underscore.
 * For example, if you are mostly working with values that come from
 * a 10-bit A/D converter, you may want to choose 21. This leaves 11
 * bits for the whole part, which will help avoid overflow in addition.
 * On ARM, bit shifts require a single cycle, so all fracbits
 * require the same amount of time to compute and there is no advantage
 * to selecting fracbits that are a multiple of 8.
 */
#define FIXED_FRACBITS 24

#define FIXED_RESOLUTION (1 << FIXED_FRACBITS)
#define FIXED_INT_MASK (0xffffffffL << FIXED_FRACBITS)
#define FIXED_FRAC_MASK (~FIXED_INT_MASK)

// square roots
#define FIXED_SQRT_ERR 1 << (FIXED_FRACBITS - 10)

// fixedp2a
#define FIXED_DECIMALDIGITS 6

typedef long fixedp;

// conversions for arbitrary fracbits
#define _short2q(x, fb)                 ((fixedp)((x) << (fb)))
#define _int2q(x, fb)                   ((fixedp)((x) << (fb)))
#define _long2q(x, fb)                  ((fixedp)((x) << (fb)))
#define _float2q(x, fb)                 ((fixedp)((x) * (1 << (fb))))
#define _double2q(x, fb)                ((fixedp)((x) * (1 << (fb))))

// conversions for default fracbits
#define short2q(x)                      _short2q(x, FIXED_FRACBITS)
#define int2q(x)                        _int2q(x, FIXED_FRACBITS)
#define long2q(x)                       _long2q(x, FIXED_FRACBITS)
#define float2q(x)                      _float2q(x, FIXED_FRACBITS)
#define double2q(x)                     _double2q(x, FIXED_FRACBITS)

// conversions for arbitrary fracbits
#define _q2short(x, fb)         ((short)((x) >> (fb)))
#define _q2int(x, fb)           ((int)((x) >> (fb)))
#define _q2long(x, fb)          ((long)((x) >> (fb)))
#define _q2float(x, fb)         ((float)(x) / (1 << (fb)))
#define _q2double(x, fb)        ((double)(x) / (1 << (fb)))

// conversions for default fracbits
#define q2short(x)                      _q2short(x, FIXED_FRACBITS)
#define q2int(x)                        _q2int(x, FIXED_FRACBITS)
#define q2long(x)                       _q2long(x, FIXED_FRACBITS)
#define q2float(x)                      _q2float(x, FIXED_FRACBITS)
#define q2double(x)                     _q2double(x, FIXED_FRACBITS)

// evaluates to the whole (integer) part of x
#define qipart(x)                       q2long(x)

// evaluates to the fractional part of x
#define qfpart(x)                       ((x) & FIXED_FRAC_MASK)

/*
 * Constants
 */
#define _QPI      3.1415926535897932384626433832795
#define QPI      double2q(_QPI)
#define _Q2PI     6.283185307179586476925286766559
#define Q2PI     double2q(_Q2PI)
#define _QPIO2    1.5707963267948966192313216916398
#define QPIO2    double2q(_QPIO2)
#define _QPIO4    0.78539816339744830961566084581988
#define QPIO4    double2q(_QPIO4)
#define _QLN_E    2.71828182845904523536
#define QLN_E    double2q(_QLN_E)
#define _QLN_10   2.30258509299404568402
#define QLN_10   double2q(_QLN_10)
#define _Q1OLN_10 0.43429448190325182765
#define Q1OLN_10 double2q(_Q1OLN_10)


// Both operands in addition and subtraction must have the same fracbits.
// If you need to add or subtract fixed point numbers with different
// fracbits, then use q2q to convert each operand beforehand.
#define qadd(a, b) ((a) + (b))
#define qsub(a, b) ((a) - (b))

/**
 * q2q - convert one fixed point type to another
 * x - the fixed point number to convert
 * xFb - source fracbits
 * yFb - destination fracbits
 */
static INLINE fixedp q2q(fixedp x, int xFb, int yFb)
{
        if(xFb == yFb) {
                return x;
        } else if(xFb < yFb) {
                return x << (yFb - xFb);
        } else {
                return x >> (xFb - yFb);
        }
}

/**
 * Multiply two fixed point numbers with arbitrary fracbits
 * x - left operand
 * y - right operand
 * xFb - number of fracbits for X
 * yFb - number of fracbits for Y
 * resFb - number of fracbits for the result
 *
 */
#define _qmul(x, y, xFb, yFb, resFb) ((fixedp)(((long long)(x) * (long long)(y)) >> ((xFb) + (yFb) - (resFb))))

/**
 * Fixed point multiply for default fracbits.
 */
#define qmul(x, y) _qmul(x, y, FIXED_FRACBITS, FIXED_FRACBITS, FIXED_FRACBITS)

/**
 * divide
 * shift into 64 bits and divide, then truncate
 */
#define _qdiv(x, y, xFb, yFb, resFb) ((fixedp)((((long long)x) << ((xFb) + (yFb) - (resFb))) / y))

/**
 * Fixed point divide for default fracbbits.
 */
#define qdiv(x, y) _qdiv(x, y, FIXED_FRACBITS, FIXED_FRACBITS, FIXED_FRACBITS)

/**
 * Invert a number (x^-1) for arbitrary fracbits
 */
#define _qinv(x, xFb, resFb) ((fixedp)((((long long)1) << (xFb + resFb)) / x))

/**
 * Invert a number with default fracbits.
 */
#define qinv(x) _qinv(x, FIXED_FRACBITS, FIXED_FRACBITS);

/**
 * Modulus. Modulus is only defined for two numbers of the same fracbits
 */
#define qmod(x, y) ((x) % (y))

/**
 * Absolute value. Works for any fracbits.
 */
#define qabs(x) (((x) < 0) ? (-x) : (x))

/**
 * Floor for arbitrary fracbits
 */
#define _qfloor(x, fb) ((x) & (0xffffffff << (fb)))

/**
 * Floor for default fracbits
 */
#define qfloor(x) _qfloor(x, FIXED_FRACBITS)

/**
 * ceil for arbitrary fracbits.
 */
static INLINE fixedp _qceil(fixedp x, int fb)
{
        // masks off fraction bits and adds one if there were some fractional bits
        fixedp f = _qfloor(x, fb);
        if (f != x) return qadd(f, _int2q(1, fb));
        return x;
}

/**
 * ceil for default fracbits
 */
#define qceil(x) _qceil(x, FIXED_FRACBITS)


/**
 * square root for default fracbits
 */
fixedp qsqrt(fixedp p_Square);

/**
 * log (base e) for default fracbits
 */
fixedp qlog( fixedp p_Base );

/**
 * log base 10 for default fracbits
 */
fixedp qlog10( fixedp p_Base );

/**
 * exp (e to the x) for default fracbits
 */
fixedp qexp(fixedp p_Base);

/**
 * pow for default fracbits
 */
fixedp qpow( fixedp p_Base, fixedp p_Power );

/**
 * sine for default fracbits
 */
fixedp qsin(fixedp theta);

/**
 * cosine for default fracbits
 */
fixedp qcos(fixedp theta);

/**
 * tangent for default fracbits
 */
fixedp qtan(fixedp theta);

/**
 * fixedp2a - converts a fixed point number with default fracbits to a string
 */
char *q2a(char *buf, fixedp n);

#ifdef __cplusplus
}       // extern C
#endif


#endif /* _QMATH_H_ */


/* **********************************************************************
 *
 * Fixed Point Math Library
 *
 * **********************************************************************
 *
 * qmath.c
 *
 * Alex Forencich
 * alex@alexelectronics.com
 *
 * Jordan Rhee
 * rhee.jordan@gmail.com
 *
 * IEEE UCSD
 * http://ieee.ucsd.edu
 *
 * **********************************************************************/

#include "qmath.h"

/**
 * square root
 */
fixedp qsqrt(fixedp p_Square)
{
        fixedp   res;
        fixedp   delta;
        fixedp   maxError;

        if ( p_Square <= 0 )
                return 0;

        /* start at half */
        res = (p_Square >> 1);

        /* determine allowable error */
        maxError =  qmul(p_Square, FIXED_SQRT_ERR);

        do
        {
                delta =  (qmul( res, res ) - p_Square);
                res -=  qdiv(delta, ( res << 1 ));
        }
        while ( delta > maxError || delta < -maxError );

        return res;
}

/**
 * log (base e)
 */
fixedp qlog( fixedp p_Base )
{
    fixedp w = 0;
        fixedp y = 0;
        fixedp z = 0;
        fixedp num = int2q(1);
        fixedp dec = 0;

        if ( p_Base == int2q(1) )
                return 0;

        if ( p_Base == 0 )
                return 0xffffffff;

        for ( dec=0 ; qabs( p_Base ) >= int2q(2) ; dec += int2q(1) )
                p_Base = qdiv(p_Base, QLN_E);

        p_Base -= int2q(1);
        z = p_Base;
        y = p_Base;
        w = int2q(1);

        while ( y != y + w )
        {
                z = 0 - qmul( z , p_Base );
                num += int2q(1);
                w = qdiv( z , num );
                y += w;
        }

        return y + dec;
}

/**
 * log base 10
 */
fixedp qlog10( fixedp p_Base )
{
    // ln(p_Base)/ln(10)
    // more accurately, ln(p_Base) * (1/ln(10))
    return qmul(qlog(p_Base), Q1OLN_10);
}

/**
 * exp (e to the x)
 */
fixedp qexp(fixedp p_Base)
{
        fixedp w;
        fixedp y;
        fixedp num;

        for ( w=int2q(1), y=int2q(1), num=int2q(1) ; y != y+w ; num += int2q(1) )
        {
                w = qmul(w, qdiv(p_Base, num));
                y += w;
        }

        return y;
}

/**
 * pow
 */
fixedp qpow( fixedp p_Base, fixedp p_Power )
{
        if ( p_Base < 0 && qmod(p_Power, int2q(2)) != 0 )
                return - qexp( qmul(p_Power, qlog( -p_Base )) );
        else
                return qexp( qmul(p_Power, qlog(qabs( p_Base ))) );
}

/**
 * sinx, internal sine function
 */
static fixedp sinx(fixedp x)
{
        fixedp xpwr;
        long xftl;
        fixedp xresult;
        short positive;
        int i;

        xresult = 0;
        xpwr = x;
        xftl = 1;
        positive = -1;

        // Note: 12! largest for long
        for (i = 1; i < 7; i+=2)
        {
                if ( positive )
                        xresult += qdiv(xpwr, long2q(xftl));
                else
                        xresult -= qdiv(xpwr, long2q(xftl));

                xpwr = qmul(x, xpwr);
                xpwr = qmul(x, xpwr);
                xftl *= i+1;
                xftl *= i+2;
                positive = !positive;
        }

        return xresult;
}

/**
 * sine
 */
fixedp qsin(fixedp theta)
{
        fixedp xresult;
        short bBottom = 0;
        //static fixed xPI = XPI;
        //static fixed x2PI = X2PI;
        //static fixed xPIO2 = XPIO2;

        fixedp x = qmod(theta, Q2PI);
        if ( x < 0 )
                x += Q2PI;

        if ( x > QPI )
        {
                bBottom = -1;
                x -= QPI;
        }

        if ( x <= QPIO2 )
                xresult = sinx(x);
        else
                xresult = sinx(QPIO2-(x-QPIO2));

        if ( bBottom )
                return -xresult;

        return xresult;
}

/**
 * cosine
 */
fixedp qcos(fixedp theta)
{
        return qsin(theta + QPIO2);
}

/**
 * tangent
 */
fixedp qtan(fixedp theta)
{
        return qdiv(qsin(theta), qcos(theta));
}

/**
 * q2a - converts a fixed point number to a string
 */
char *q2a(char *buf, fixedp n)
{
        long ipart = qipart(n);
        long fpart = qfpart(n);
        long intdigits = 0;

        int i = 0;
        int j = 0;
        int d = 0;

        int offset = 0;

        long v;
        long t;
        long st = 0;

        if (n != 0)
        {
                intdigits = qipart(qceil(qlog10(qabs(n))));
        }

        if (intdigits < 1) intdigits = 1;

        offset = intdigits - 1;

        if (n < 0)
        {
                buf[0] = '-';
                offset++;
                n = -n;
                ipart = -ipart;
                fpart = -fpart;
        }

        // integer portion

        for (i = 0; i < intdigits; i++)
        {
                j = offset - i;
                d = ipart % 10;
                ipart = ipart / 10;
                buf[j] = '0' + d;
        }

        // decimal point

        buf[offset + 1] = '.';

        // fractional portion

        v = 5;
        t = FIXED_RESOLUTION >> 1;

        for (i = 0; i < FIXED_DECIMALDIGITS - 1; i++)
        {
                v = (v << 1) + (v << 3);
        }

        for (i = 0; i < FIXED_FRACBITS; i++)
        {
                if (fpart & t)
                {
                        st += v;
                }
                v = v >> 1;
                t = t >> 1;
        }

        offset += FIXED_DECIMALDIGITS + 1;

        for (i = 0; i < FIXED_DECIMALDIGITS; i++)
        {
                j = offset - i;
                d = st % 10;
                st = st / 10;
                buf[j] = '0' + d;
        }

        // ending zero
        buf[offset + 1] = '\0';

        return buf;
}		
		
		
