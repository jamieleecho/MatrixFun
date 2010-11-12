/*
 *  Functions.h
 *  SimpleMatrix
 *
 *  Created by Jamie Cho on 10/28/07.
 *  Copyright 2007 Jamie Cho. All rights reserved.
 *
 */

#ifndef _JCHO_FUNCTIONS_H
#define _JCHO_FUNCTIONS_H

namespace jcho {
	// Returns the square of x
	template<class T>
	inline T sq(T x) { return x * x; }
}

#endif