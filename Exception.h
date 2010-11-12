/*
 *  Exception.h
 *  SimpleMatrix
 *
 *  Created by Jamie Cho on 11/3/07.
 *  Copyright 2007 Jamie Cho. All rights reserved.
 *
 */

#ifndef _JCHO_EXCEPTION_H
#define _JCHO_EXCEPTION_H

#include <string>


namespace jcho {
	// Class for representing exceptions thrown from the jcho namespace
	class Exception {
	public:
		// Creates an exception with the given message
		Exception(const std::string &message = "") : _message(message) { }
		
		// Returns the Exception's message
		const std::string &message() const { return _message; }
		
	private:
		// Exception's message
		std::string _message;
	};
}

#endif
