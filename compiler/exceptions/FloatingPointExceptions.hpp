/*******************************************************************************
 *
 * (c) Copyright IBM Corp. 2000, 2016
 *
 *  This program and the accompanying materials are made available
 *  under the terms of the Eclipse Public License v1.0 and
 *  Apache License v2.0 which accompanies this distribution.
 *
 *      The Eclipse Public License is available at
 *      http://www.eclipse.org/legal/epl-v10.html
 *
 *      The Apache License v2.0 is available at
 *      http://www.opensource.org/licenses/apache2.0.php
 *
 * Contributors:
 *    Multiple authors (IBM Corp.) - initial implementation and documentation
 ******************************************************************************/

#ifndef FLOATINGPOINTEXCEPTIONS_HPP
#define FLOATINGPOINTEXCEPTIONS_HPP

#include "exceptions/CompilationException.hpp"

namespace TR {

struct FloatingPointException : public virtual TR::CompilationException
   {
   virtual const char* what() const throw() { return "Floating point exception"; }
   };

struct FloatingPointOverflowException : public virtual FloatingPointException
   {
   virtual const char* what() const throw() { return "Floating point overflow"; }
   };

struct FloatingPointUnderflowException : public virtual FloatingPointException
   {
   virtual const char* what() const throw() { return "Floating point underflow"; }
   };

struct FloatingPointDivisionByZeroException : public virtual FloatingPointException
   {
   virtual const char* what() const throw() { return "Floating point division by zero"; }
   };

}

#endif // FLOATINGPOINTEXCEPTIONS_HPP
