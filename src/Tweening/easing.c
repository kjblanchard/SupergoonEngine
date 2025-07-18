//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
//
// For more information, please refer to <https://unlicense.org>
//

// For windows
#define _USE_MATH_DEFINES

#include <Supergoon/Tweening/easing.h>
#include <math.h>

// Modeled after the line y = x
AHFloat geLinearInterpolation(AHFloat p) {
	return p;
}

// Modeled after the parabola y = x^2
AHFloat geQuadraticEaseIn(AHFloat p) {
	return p * p;
}

// Modeled after the parabola y = -x^2 + 2x
AHFloat geQuadraticEaseOut(AHFloat p) {
	return -(p * (p - 2));
}

// Modeled after the piecewise quadratic
// y = (1/2)((2x)^2)             ; [0, 0.5)
// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
AHFloat geQuadraticEaseInOut(AHFloat p) {
	if (p < 0.5) {
		return 2 * p * p;
	} else {
		return (-2 * p * p) + (4 * p) - 1;
	}
}

// Modeled after the cubic y = x^3
AHFloat geCubicEaseIn(AHFloat p) {
	return p * p * p;
}

// Modeled after the cubic y = (x - 1)^3 + 1
AHFloat geCubicEaseOut(AHFloat p) {
	AHFloat f = (p - 1);
	return f * f * f + 1;
}

// Modeled after the piecewise cubic
// y = (1/2)((2x)^3)       ; [0, 0.5)
// y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
AHFloat geCubicEaseInOut(AHFloat p) {
	if (p < 0.5) {
		return 4 * p * p * p;
	} else {
		AHFloat f = ((2 * p) - 2);
		return 0.5 * f * f * f + 1;
	}
}

// Modeled after the quartic x^4
AHFloat geQuarticEaseIn(AHFloat p) {
	return p * p * p * p;
}

// Modeled after the quartic y = 1 - (x - 1)^4
AHFloat geQuarticEaseOut(AHFloat p) {
	AHFloat f = (p - 1);
	return f * f * f * (1 - p) + 1;
}

// Modeled after the piecewise quartic
// y = (1/2)((2x)^4)        ; [0, 0.5)
// y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
AHFloat geQuarticEaseInOut(AHFloat p) {
	if (p < 0.5) {
		return 8 * p * p * p * p;
	} else {
		AHFloat f = (p - 1);
		return -8 * f * f * f * f + 1;
	}
}

// Modeled after the quintic y = x^5
AHFloat geQuinticEaseIn(AHFloat p) {
	return p * p * p * p * p;
}

// Modeled after the quintic y = (x - 1)^5 + 1
AHFloat geQuinticEaseOut(AHFloat p) {
	AHFloat f = (p - 1);
	return f * f * f * f * f + 1;
}

// Modeled after the piecewise quintic
// y = (1/2)((2x)^5)       ; [0, 0.5)
// y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
AHFloat geQuinticEaseInOut(AHFloat p) {
	if (p < 0.5) {
		return 16 * p * p * p * p * p;
	} else {
		AHFloat f = ((2 * p) - 2);
		return 0.5 * f * f * f * f * f + 1;
	}
}

// Modeled after quarter-cycle of sine wave
AHFloat geSineEaseIn(AHFloat p) {
	return sin((p - 1) * M_PI_2) + 1;
}

// Modeled after quarter-cycle of sine wave (different phase)
AHFloat geSineEaseOut(AHFloat p) {
	return sin(p * M_PI_2);
}

// Modeled after half sine wave
AHFloat geSineEaseInOut(AHFloat p) {
	return 0.5 * (1 - cos(p * M_PI));
}

// Modeled after shifted quadrant IV of unit circle
AHFloat geCircularEaseIn(AHFloat p) {
	return 1 - sqrt(1 - (p * p));
}

// Modeled after shifted quadrant II of unit circle
AHFloat geCircularEaseOut(AHFloat p) {
	return sqrt((2 - p) * p);
}

// Modeled after the piecewise circular function
// y = (1/2)(1 - sqrt(1 - 4x^2))           ; [0, 0.5)
// y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
AHFloat geCircularEaseInOut(AHFloat p) {
	if (p < 0.5) {
		return 0.5 * (1 - sqrt(1 - 4 * (p * p)));
	} else {
		return 0.5 * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
	}
}

// Modeled after the exponential function y = 2^(10(x - 1))
AHFloat geExponentialEaseIn(AHFloat p) {
	return (p == 0.0) ? p : pow(2, 10 * (p - 1));
}

// Modeled after the exponential function y = -2^(-10x) + 1
AHFloat geExponentialEaseOut(AHFloat p) {
	return (p == 1.0) ? p : 1 - pow(2, -10 * p);
}

// Modeled after the piecewise exponential
// y = (1/2)2^(10(2x - 1))         ; [0,0.5)
// y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
AHFloat geExponentialEaseInOut(AHFloat p) {
	if (p == 0.0 || p == 1.0) return p;

	if (p < 0.5) {
		return 0.5 * pow(2, (20 * p) - 10);
	} else {
		return -0.5 * pow(2, (-20 * p) + 10) + 1;
	}
}

// Modeled after the damped sine wave y = sin(13pi/2*x)*pow(2, 10 * (x - 1))
AHFloat geElasticEaseIn(AHFloat p) {
	return sin(13 * M_PI_2 * p) * pow(2, 10 * (p - 1));
}

// Modeled after the damped sine wave y = sin(-13pi/2*(x + 1))*pow(2, -10x) + 1
AHFloat geElasticEaseOut(AHFloat p) {
	return sin(-13 * M_PI_2 * (p + 1)) * pow(2, -10 * p) + 1;
}

// Modeled after the piecewise exponentially-damped sine wave:
// y = (1/2)*sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))      ; [0,0.5)
// y = (1/2)*(sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
AHFloat geElasticEaseInOut(AHFloat p) {
	if (p < 0.5) {
		return 0.5 * sin(13 * M_PI_2 * (2 * p)) * pow(2, 10 * ((2 * p) - 1));
	} else {
		return 0.5 * (sin(-13 * M_PI_2 * ((2 * p - 1) + 1)) * pow(2, -10 * (2 * p - 1)) + 2);
	}
}

// Modeled after the overshooting cubic y = x^3-x*sin(x*pi)
AHFloat geBackEaseIn(AHFloat p) {
	return p * p * p - p * sin(p * M_PI);
}

// Modeled after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi))
AHFloat geBackEaseOut(AHFloat p) {
	AHFloat f = (1 - p);
	return 1 - (f * f * f - f * sin(f * M_PI));
}

// Modeled after the piecewise overshooting cubic function:
// y = (1/2)*((2x)^3-(2x)*sin(2*x*pi))           ; [0, 0.5)
// y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) ; [0.5, 1]
AHFloat geBackEaseInOut(AHFloat p) {
	if (p < 0.5) {
		AHFloat f = 2 * p;
		return 0.5 * (f * f * f - f * sin(f * M_PI));
	} else {
		AHFloat f = (1 - (2 * p - 1));
		return 0.5 * (1 - (f * f * f - f * sin(f * M_PI))) + 0.5;
	}
}

AHFloat geBounceEaseIn(AHFloat p) {
	return 1 - geBounceEaseOut(1 - p);
}

AHFloat geBounceEaseOut(AHFloat p) {
	if (p < 4 / 11.0) {
		return (121 * p * p) / 16.0;
	} else if (p < 8 / 11.0) {
		return (363 / 40.0 * p * p) - (99 / 10.0 * p) + 17 / 5.0;
	} else if (p < 9 / 10.0) {
		return (4356 / 361.0 * p * p) - (35442 / 1805.0 * p) + 16061 / 1805.0;
	} else {
		return (54 / 5.0 * p * p) - (513 / 25.0 * p) + 268 / 25.0;
	}
}

AHFloat geBounceEaseInOut(AHFloat p) {
	if (p < 0.5) {
		return 0.5 * geBounceEaseIn(p * 2);
	} else {
		return 0.5 * geBounceEaseOut(p * 2 - 1) + 0.5;
	}
}
