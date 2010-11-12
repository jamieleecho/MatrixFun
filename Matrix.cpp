/*
 *  Matrix.cpp
 *  SimpleMatrix
 *
 *  Created by Jamie Cho on 10/4/07.
 *  Copyright 2007 Jamie Cho. All rights reserved.
 *
 */

#ifndef _JCHO_MATRIX_CPP
#define _JCHO_MATRIX_CPP

#include <string.h>
#include <sstream>

#include "Matrix.h"
#include "Functions.h"


template<class T>
jcho::Matrix<T>::Matrix(int m, int n, MatrixInit_t t) : _storage(NULL), _m(m), _n(n), _size(m * n) {
	// Validate inputs
	if ((m < 1) || (n < 1))
		throw Exception("Attempt to create Matrix with non-positive dimensions.");
	if (_size < 1)
		throw Exception("Attempt to create an absurdly large Matrix.");
	if ((m != n) && (t == Identity))
		throw Exception("Attempt to create a non-square Identity Matrix.");
		
	// Create the space required by the matrix
	_storage = new double[_size];

	// Fill the matrix contents
	try { initialize(t); 
	} catch(...) {
		delete [] _storage;
		throw;
	}
}


template<class T>
jcho::Matrix<T>::Matrix(const Matrix &a) : _storage(NULL), _m(a.m()), _n(a.n()), _size(a.size()) {
	_storage = new double[_size];
	memcpy(_storage, a._storage, _size * sizeof(double));
}


template<class T>
jcho::Matrix<T>::Matrix(const std::string &str) : _storage(NULL), _m(0), _n(0), _size(0) {
  // Determine whether or not the Matrix is a scalar (1x1 matrix)
  std::istringstream inputStream(str);
  int firstChar = inputStream.peek();
  if (firstChar != '[') {
    double val;
    inputStream >> val;
    _m = 1;
    _n = 1;
    _size = 1;
    _storage = new double[_size];
    _storage[0] = val;
    return;    
  }

  // Determine that the Matrix is well formed
  if (firstChar != '[')
    throw Exception("Matrix::(const std::string &str): Error parsing matrix. Expected '[' or numeric literal.");
  inputStream.get();
  
  // We have a matrix. Fill in a temporary vector of rows and columns.
  std::vector<std::vector <double> > rowsAndColumns;
  int numCols = 0;
  int originalNumCols = -1;
  while(true) {
    int c = inputStream.peek();
    if (c == EOF) throw Exception("Matrix::(const std::string &str): Unexpected end of stream.");

    // If we see a ']', then we are done parsing
    if (c == ']') {
      if (originalNumCols < 0)
        originalNumCols = numCols;
      if (numCols != originalNumCols)
        throw Exception("Matrix::(const std::string &str): Matrix has a row with a different number of columns.");
      break;
    }

    // If we see a ';', then we must start a new row
    if (c == ';') {
      inputStream.get();
      if (originalNumCols == 0)
        throw Exception("Matrix::(const std::string &str): Matrix with multiple rows but zero columns.");
      if (originalNumCols < 0)
        originalNumCols = numCols;
      if (numCols != originalNumCols)
        throw Exception("Matrix::(const std::string &str): Matrix has a row with a different number of columns.");
      numCols = 0;
      rowsAndColumns.push_back(std::vector<double>());        
    }
    
    // Otherwise we expect a numeric literal
    double val;
    inputStream >> val;
    if (rowsAndColumns.size() < 1)
      rowsAndColumns.push_back(std::vector<double>());
    rowsAndColumns[rowsAndColumns.size() - 1].push_back(val);
    numCols++;
  }

  // Copy the temporary buffer to the final location
  _m = rowsAndColumns.size();
  _n = (originalNumCols < 0) ? 0 : originalNumCols;
  _size = (originalNumCols < 0) ? 1 : (_m * _n);
  _storage = new double[_size];
  for(int ii=0; ii<_m; ii++)
    for(int jj=0; jj<_n; jj++)
      set(ii, jj, rowsAndColumns[ii][jj]);
}

template<class T>
void jcho::Matrix<T>::initialize(MatrixInit_t t) {
	// Initialize the Matrix as needed
	switch(t) {
	case Zeros:
		for(int ii=0; ii<_size; ii++)
			_storage[ii] = 0.0;
		break;

	case Identity: 
		{
			int kk=0;
			for(int ii=0; ii<_n; ii++)
				for(int jj=0; jj<_m; jj++, kk++)
					_storage[kk] = (ii == jj) ? 1.0 : 0.0;
			break;
		}

	case Uninitialized:
		break;

	default:
		delete [] _storage;
		throw Exception("Attempt to create Matrix with an unknown MatrixInit_t value.");
	}
}


template<class T>
jcho::Matrix<T> &jcho::Matrix<T>::operator =(const Matrix &a) {
	if (&a == this) return *this;
	delete [] _storage;
	_m = a.m();
	_n = a.n();
	_size = a.size();
	_storage = new double[size()];
	memcpy(_storage, a._storage, size() * sizeof(double));
	return *this;	
}


template<class T>
jcho::Matrix<T> jcho::Matrix<T>::operator +(const Matrix &a) const {
	AssertSameDimensions(a);
	Matrix retval(_m, _n, Uninitialized);
	for(int ii=0; ii<_size; ii++)
		retval._storage[ii] = _storage[ii] + a._storage[ii];	
	return retval;	
}


template<class T>
jcho::Matrix<T> &jcho::Matrix<T>::operator +=(const Matrix &a) {
	AssertSameDimensions(a);
	for(int ii=0; ii<_size; ii++)
		_storage[ii] += a._storage[ii];
	return *this;	
}


template<class T>
jcho::Matrix<T> jcho::Matrix<T>::operator -(const Matrix &a) const {
	AssertSameDimensions(a);
	Matrix retval(_m, _n, Uninitialized);
	for(int ii=0; ii<_size; ii++)
		retval._storage[ii] = _storage[ii] - a._storage[ii];	
	return retval;	
}


template<class T>
jcho::Matrix<T> &jcho::Matrix<T>::operator -=(const Matrix &a) {
	AssertSameDimensions(a);
	for(int ii=0; ii<_size; ii++)
		_storage[ii] -= a._storage[ii];
	return *this;
}


template<class T>
jcho::Matrix<T> &jcho::Matrix<T>::operator *=(const Matrix<T> &a) {
	AssertMultiplyCompatible(a);
	*this = *this * a;
	return *this;	
}


template<class T>
jcho::Matrix<T> jcho::Matrix<T>::operator *(T k) const {
	Matrix<T> retval(_m, _n, Uninitialized);
	for(int ii=0; ii<_size; ii++)
		retval._storage[ii] = _storage[ii] * k;
	return retval;	
}


template<class T>
jcho::Matrix<T> &jcho::Matrix<T>::operator *=(T k) {
	for(int ii=0; ii<_size; ii++)
		_storage[ii] *= k;
	return *this;	
}


template<class T>
jcho::Matrix<T> jcho::Matrix<T>::operator /(T k) const {
	Matrix retval(_m, _n, Uninitialized);
	for(int ii=0; ii<_size; ii++)
		retval._storage[ii] = _storage[ii] / k;
	return retval;	
}


template<class T>
jcho::Matrix<T> &jcho::Matrix<T>::operator /=(T k) {
	for(int ii=0; ii<_size; ii++)
		_storage[ii] /= k;
	return *this;	
}


template<class T>
jcho::Matrix<T> &jcho::Matrix<T>::operator /=(const Matrix &a) {
	AssertSquare();
	*this = *this / a;
	return *this;
}


template<class T>
jcho::Matrix<T> jcho::Matrix<T>::inversion() const {
	AssertSquare();
	Matrix eye(m(), n(), Identity);
	return eye / *this;
}


template<class T>
jcho::Matrix<T> jcho::Matrix<T>::transpose() const {
	Matrix retval(n(), m(), Uninitialized);
	for(int ii=0; ii<m(); ii++)
		for(int jj=0; jj<n(); jj++)
			retval.set(jj, ii, get(ii, jj));
	return retval;
}


template<class T>
T jcho::Matrix<T>::dot(const Matrix<T> &v) const {
	AssertSameDimensions(v);
	AssertIsVector();
	double sum = 0;
	if (m() == 1)
		for(int jj=0; jj<n(); jj++)
			sum += (get(0, jj) * v.get(0, jj));
	else
		for(int ii=0; ii<m(); ii++)
			sum += (get(ii, 0) * v.get(ii, 0));
	return sum;
}


template<class T>
jcho::Matrix<T> jcho::Matrix<T>::cross(const Matrix &v) const {
	AssertSameDimensions(v);
	AssertIsVector();
	if ((m() != 3) && (n() != 3))
		throw Exception("Matrix::cross(const Matrix &v): This operation can only be applied to 3x1 or 1x3 matrices");

	const double a1 = get(0, 0);
	const double b1 = v.get(0, 0);
	Matrix retval(m(), n(), Uninitialized);
	if (m() == 3) {
		const double a2 = get(1, 0);
		const double a3 = get(2, 0);
		const double b2 = v.get(1, 0);
		const double b3 = v.get(2, 0);
		retval.set(0, 0, a2*b3 - a3*b2);
		retval.set(1, 0, a3*b1 - a1*b3);
		retval.set(2, 0, a1*b2 - a2*b1);
	} else {
		const double a2 = get(0, 1);
		const double a3 = get(0, 2);
		const double b2 = v.get(0, 1);
		const double b3 = v.get(0, 2);
		retval.set(0, 0, a2*b3 - a3*b2);
		retval.set(0, 1, a3*b1 - a1*b3);
		retval.set(0, 2, a1*b2 - a2*b1);
	}

	return retval;
}


template<class T>
T jcho::Matrix<T>::magnitude() const {
	AssertIsVector();
	T sum = 0;
	for(int ii=0; ii<m(); ii++)
		for(int jj=0; jj<n(); jj++)
			sum += sq(this->get(ii, jj));
	return (T)sqrt(sum);
}


template<class T>
jcho::Matrix<T> jcho::Matrix<T>::unit() const {
	return *this/magnitude();
}


template<class T>
jcho::Matrix<T> jcho::Matrix<T>::wedge(const std::vector<Matrix<T> *> &vs) {
	// Validate the inputs
	const int sz = (int)vs.size();
	if (sz < 1)
		throw Exception("Matrix::wedge(const vector<Matrix *>&): Vector must contain at least 1 or more pointers to Matrix instances");
	Matrix &v1 = *vs[0];
	v1.AssertIsVector();
	const int sz1 = sz + 1;
	if ((v1.m() != (sz1)) && (v1.n() != (sz1)))
		throw Exception("Matrix::wedge(const vector<Matrix *>&): This operation can only be applied to (sz+1)x1 or 1x(sz+1) matrices");
	for(typename std::vector<Matrix< T> *>::const_iterator iter = vs.begin(); iter != vs.end(); iter++)
		v1.AssertSameDimensions(*(*iter));
			
	// First create a working matrix from all of the vectors
	Matrix tmp(sz1, sz1, Zeros);
	int jj = 0;
	for(typename std::vector<Matrix< T> *>::const_iterator iter = vs.begin(); iter != vs.end(); iter++) {
		Matrix &v = *(*iter);
		for(int ii=0; ii<sz1; ii++)
			tmp.set(jj, ii, v1.is_row_vector() ? v.get(ii, 0) : v.get(0, ii));
	}
	for(int ii=0; ii<sz1; ii++) // the last row is our unit vector row
		tmp.set(jj, ii, 1);

	// Perform the wedge and transpose the vector as needed
	Matrix retval = tmp.wedge_internal();
	return (v1.n() == 1) ? retval.transpose() : retval;
}


template<class T>
T jcho::Matrix<T>::determinent() const {
	AssertSquare();
	
	// Base cases
	if (m() < 1)
		throw Exception("Matrix::determinent() const: This operation can only be applied to square matrices with at least one element");
	if (m() == 1)
		return get(0, 0);
	if (m() == 2)
		return (get(0, 0) * get(1, 1)) - (get(0, 1) * get(1, 0));
	
	// Recursive case
	double sum = 0;
	for(int ii=0; ii<m(); ii++) {
		Matrix m(*this, ii);
		sum -= (((ii & 1) == 1) ? -1 : 1) * get(0, ii) * m.determinent();
	}
	return sum;
}


template<class T>
std::string jcho::Matrix<T>::to_string() const {
	std::stringstream out;
  out << "[";
	for(int ii=0; ii<m(); ii++) {
		for(int jj=0; jj<n(); jj++)
			out << get(ii, jj) << ((jj != n() - 1) ? " " : ((ii != m() - 1) ? ";" : ""));
	}
  out << "]";
	return out.str();
}


template<class T>
jcho::Matrix<T>::Matrix(const Matrix<T> &a, int m, int n) : _storage(NULL), _m(m), _n(n), _size(m * n) {
	_storage = new double[size()];
	int aOffset = 0, thisOffset = 0;
	int n_ = (a.n() < n) ? a.n() : n;
	int m_ = (a.m() < m) ? a.m() : m;
	for(int jj=0; jj<n_; jj++) {
		memcpy(_storage + thisOffset, a._storage + aOffset, m_ * sizeof(double));
		aOffset += a.m();
		thisOffset += m;
	}
}


template<class T>
jcho::Matrix<T>::Matrix(const Matrix<T> &a, int n) : _storage(NULL), _m(a.m()-1), _n(a.n()-1), _size((a.m()-1) * a.n()-1) {
	// We can only copy a if this matrix is going to be larger than it
	if ((a.m() <= 1) || (a.n() <= n))
		throw Exception("Matrix(const Matrix &a, int n): a.m() > 1 && a.n() > n to be compatble with this operation");
	_storage = new double[size()];
	int aOffset = 1, thisOffset = 0;
	for(int jj=0; jj<a.n(); jj++) {
		if (jj != n) {
			memcpy(_storage + thisOffset, a._storage + aOffset, m() * sizeof(double));
			thisOffset += m();
		}
		aOffset += a.m();
	}
}


template<class T>
template<void (*GEMM)(const enum CBLAS_ORDER, const enum CBLAS_TRANSPOSE, const enum CBLAS_TRANSPOSE, const int, const int, const int, const T, const T *, const int, const T *, const int, const T, T *, const int)>
jcho::Matrix<T> jcho::Matrix<T>::multiply(const Matrix<T> &a) const {
	AssertMultiplyCompatible(a);
	Matrix retval(_m, a._n, Uninitialized);
	GEMM(CblasColMajor, CblasNoTrans, CblasNoTrans, _m, a._n, _n, 1.0, _storage, _m, a._storage, _n, 0.0, retval._storage, retval._m);
	return retval;	
}

	
template<class T>
template<int (*GESV)(__CLPK_integer *, __CLPK_integer *, T *, __CLPK_integer *, __CLPK_integer *, T *, __CLPK_integer *, __CLPK_integer *)>
jcho::Matrix<T> jcho::Matrix<T>::divide(const Matrix<T> &a) const {
	a.AssertSquare();

	// Perform the PLU factorization
	__CLPK_integer m_ = m(), a_n = a.n(), a_m = a.m(), n_ = n();
	Matrix retval(*this), lu(a, m(), a.n());
	__CLPK_integer *pivots = new __CLPK_integer[a_m], info;
	GESV(&a_n, &n_, lu._storage, &a_m,  pivots, retval._storage, &m_, &info);
	delete [] pivots;
	if (info != 0)
		throw Exception("Matrix::operator /(const Matrix &a): ?gesv_ returned a non-zero error code");

	return retval;
}


template<class T>
template<int (*GELS)(char *, __CLPK_integer *, __CLPK_integer *, __CLPK_integer *, T *, __CLPK_integer *, T *, __CLPK_integer *, T *, __CLPK_integer *, __CLPK_integer *)>
jcho::Matrix<T> jcho::Matrix<T>::least_squares(const Matrix<T> &a) const {
	//ax = b (note that b = this)
	//x = b/a

	//x.m = a.n
	//x.n = b.n
	//b.m = a.m
	
	// Get the best workspace size
	__CLPK_integer m_ = a.m(), n_ = a.n(), nrhs = n();
	__CLPK_integer ldb = (m_ > n_) ? m_ : n_;
	ldb = (ldb > 1) ? ldb : 1;
	Matrix aa(a, (m_ > 1) ? m_ : 1, (n_ > 1) ? n_ : 1);
	__CLPK_integer lda = aa.m();
	Matrix b(*this, ldb, nrhs);
	T tlwork;
	__CLPK_integer info;
	char tmp_trans = MatrixNoTranspose[0];
	__CLPK_integer lwork = -1;
	GELS(&tmp_trans, &m_, &n_, &nrhs, aa._storage, &lda, b._storage, &ldb, &tlwork, &lwork, &info);
	//if (info != 0)
	//	throw Exception("Matrix::operator /(const Matrix &a): ?gels_ returned a non-zero error code");
	lwork = (__CLPK_integer)tlwork;

	// Perform the factorization 
	T *work = new T[lwork];	
	GELS(&tmp_trans, &m_, &n_, &nrhs, aa._storage, &lda, b._storage, &ldb, work, &lwork, &info);
	delete [] work;
	if (info != 0)
		throw Exception("Matrix::operator /(const Matrix &a): ?gels_ returned a non-zero error code");

	return Matrix(b, n_, nrhs);
}


template<class T>
jcho::Matrix<T> jcho::Matrix<T>::wedge_internal() const {
	AssertSquare();
	if (m() < 2)
		throw Exception("Matrix::wedge() const: This operation can only be applied to square matrices with at least four elements");

	// Base case
	if (m() == 2) {
		Matrix retval(1, n(), Uninitialized);
		retval.set(0, 0, -(get(0, 1) * get(1, 0)));
		retval.set(0, 1, get(0, 0) * get(1, 1));
		return retval;		
	}
	
	// Recursive case
	Matrix retval(1, n(), Zeros);
	for(int ii=0; ii<m(); ii++) {
		Matrix m(*this, ii);
		Matrix tmp = m.wedge_internal() * (((ii & 1) == 1) ? -1 : 1) * get(0, ii);
		for(int jj=0, kk=0; jj<n(); jj++) {
			if (jj == ii) continue;
			retval.set(0, jj, retval.get(0, jj) + tmp.get(0, kk));
			kk++;
		}
	}
	
	return retval;
}

#endif
