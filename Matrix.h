/*
 *  Matrix.h
 *  SimpleMatrix
 *
 *  Created by Jamie Cho on 10/4/07.
 *  Copyright 2007 Jamie Cho. All rights reserved.
 *
 */

#ifndef _JCHO_MATRIX_H
#define _JCHO_MATRIX_H

#include <Accelerate/Accelerate.h>

#include <vector>
#include <string>

#include "Exception.h"


namespace jcho {
	// Different types of matrix initializations
	enum MatrixInit_t {
		Zeros         = 0,
		Identity      = 1,
		Uninitialized = 2
	};

	// Indicates that the matrix should be transposed before the operation
	extern const char *MatrixTranspose;

	// Indicates that the matrix should NOT be transposed before the operation
	extern const char *MatrixNoTranspose;

	// A simple wrapper around BLAS and LINPACK to make matrix manipulation possible by mere mortals. Matrix dimensions are specified as M x N where M is the number of rows and N is the number of columns. The row is always specified first.
	template<class T>
	class Matrix {
	public:
		// Creates an M x N matrix
		Matrix(int m, int n = 1, MatrixInit_t t = Zeros);

		// Creates a copy of a
		Matrix(const Matrix<T> &a);

		// Creates a matrix from str
		Matrix(const std::string &str);

		virtual ~Matrix() { delete [] _storage; }

		// Returns number of rows
		int m() const { return _m; }

		// Returns number of columns
		int n() const { return _n; }

		// Returns m() * n()
		int size() const { return _size; }
		
		// Returns whether or not this is square
		bool is_square() const { return _m == _n; }

		// Returns whether or not this Matrix is a vector
		bool is_vector() const { return is_column_vector() || is_row_vector(); }

		// Returns whether or not this Matrix is a column vector
		bool is_column_vector() const { return  (_m == 1) && (_n > 0); }

		// Returns whether or not this Matrix is a row vector
		bool is_row_vector() const { return  (_n == 1) && (_m > 0); }

		// Returns a pointer to column n
		const T *operator[](int n) const { return _storage + (_m * n); }

		// Returns a pointer to column n
		T  *operator[](int n) { return _storage + (_m * n); }

		// Returns (0, n)
		T get(int n) const { return *((*this)[n]); }

		// Returns (m, n)
		T get(int m, int n) const { return *((*this)[n] + m); }

		// Sets (0, n) to v
		void set(int n, T v) { *((*this)[n]) = v; }

		// Sets (m, n) to v
		void set(int m, int n, T v) { *((*this)[n] + m) = v; }

		// Initializes the matrix to t
		void initialize(MatrixInit_t t);

		// Mutates this to be a copy of a.
		Matrix<T> &operator =(const Matrix<T> &a);

		// Adds a to this matrix and returns the result
		Matrix<T> operator +(const Matrix<T> &a) const;
		// Mutates this matrix by adding a and returning this.
		Matrix<T> &operator +=(const Matrix<T> &a);

		// Subtracts a from this matrix and returns the result
		Matrix<T> operator -(const Matrix<T> &a) const;
		// Mutates this matrix by subtracting a and returning this.
		Matrix<T> &operator -=(const Matrix<T> &a);

		// Multiplies this matrix by a and returns the result
		Matrix<T> operator *(const Matrix<T> &a) const;
		// Mutates this matrix by multiplying it by a and returning this.
		Matrix<T> &operator *=(const Matrix<T> &a);
		// Multiplies this matrix by k and returns the result
		Matrix<T> operator *(T k) const;
		// Mutates this matrix by multiplying it by k and returning this.
		Matrix<T> &operator *=(T k);

		// Solves for x and returns x: this x = a
		Matrix<T> operator /(const Matrix<T> &a) const;
		// Solves for x and mutates this such that this_new = this_old * a
		Matrix<T> &operator /=(const Matrix<T> &a);
		// Divides this matrix by k and returns the result
		Matrix<T> operator /(T k) const;
		// Mutates this matrix by dividing it by k and returning the result
		Matrix<T> &operator /=(T k);

		// Returns the linear least squares solution  for: this x = a and returns x.
		Matrix<T> linear_least_squares(const Matrix<T> &a) const;

		// Returns the inversion of this matrix.
		Matrix<T> inversion() const;

		// Returns the transpose of this matrix
		Matrix<T> transpose() const;

		// Returns the dot product of this and v, both of which must be vectors with the same dimensions.
		T dot(const Matrix<T> &v) const;

		// Returns this x v. this and v must both have the same dimensions and be 1x3 or 3x1 matrices.
		Matrix<T> cross(const Matrix<T> &v) const;

		// Returns the magnitude of the vector.
		T magnitude() const;

		// Returns the unit vector of the vector.
		Matrix<T> unit() const;

		// Given a vector<Matrix *> of pointers to sz vectors that are all either (sz+1, 1) or (1, sz+1),
		// where sz >= 2, returns the wedge
		static Matrix<T> wedge(const std::vector<Matrix<T> *> &vs);

		// Returns the determinent of this matrix.
		T determinent() const;

		// Returns a string version of this matrix
		std::string to_string() const;

		// Throws an Exception iff this() has different dimensions than a.
		void AssertSameDimensions(const Matrix<T> &a) const {
			if ((m() != a.m()) || (n() != a.n()))
				throw Exception("Matrices have different dimensions and are incompatible with this operation.");
		}

		// Throws an Exception iff the number of columns in this() is different than the number of rows in a.
		void AssertMultiplyCompatible(const Matrix<T> &a) const {
			if (n() != a.m())
				throw Exception("The number of columns in this matrix are different than the number of columns in a so the Matrices are incompatible with this operation");
		}

		// Throws an exception iff this is not square.
		void AssertSquare() const {
			if (!is_square())
				throw Exception("This matrix is not square and is incompatible with this operation.");
		}
		
		// Throws an exception iff this is not a vector.
		void AssertIsVector() const {
			if (!is_vector())
				throw Exception("This operation requires a vector.");
		}

	private:
		// Creates a copy of a except the size of the matrix will be m x n.
		Matrix(const Matrix<T> &a, int m, int n);

		// Creates a sub matrix of a from ([1, m()], [0, n()] - n)
		Matrix(const Matrix<T> &a, int n);

		// Performs the multiply operation using the given CBLAS function to perform the multiplication.
		template<void (*GEMM)(const enum CBLAS_ORDER, const enum CBLAS_TRANSPOSE, const enum CBLAS_TRANSPOSE, const int, const int, const int, const T, const T *, const int, const T *, const int, const T, T *, const int)>
		Matrix<T> multiply(const Matrix<T> &a) const;
	
		// Performs the divide operation using the given CLAPACK function to perform the division.
		template<int (*GESV)(__CLPK_integer *, __CLPK_integer *, T *, __CLPK_integer *, __CLPK_integer *, T *, __CLPK_integer *, __CLPK_integer *)>
		Matrix<T> divide(const Matrix<T> &a) const;
	
		// Performs the least_squares operation using the given CLAPACK function to perform the operation
		template<int (*GELS)(char *, __CLPK_integer *, __CLPK_integer *, __CLPK_integer *, T *, __CLPK_integer *, T *, __CLPK_integer *, T *, __CLPK_integer *, __CLPK_integer *)>
		Matrix<T> least_squares(const Matrix<T> &a) const;

		// Performs the least square operation for the given type
		static int gels(const char *trans, __CLPK_integer *m, __CLPK_integer *n, __CLPK_integer *nrhs, T *a, __CLPK_integer *lda, T *b, __CLPK_integer *ldb, T *work, __CLPK_integer *lwork, __CLPK_integer *info);

		// Given a square matrix with at least 4 elements, returns the wedge of the row vectors [0, m()-1] as a row vector. The last row must only contain 1s.
		Matrix<T> wedge_internal() const;

		T *_storage;
		int _m, _n, _size;
	};
}

#include "Matrix.cpp"

#endif
