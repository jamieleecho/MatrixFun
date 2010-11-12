/*
 *  SimpleMatrixExample.cpp
 *  MatrixFun
 *
 *  Created by Jamie Cho on 2010-11-12.
 *  Copyright 2010 CS2 Technologies LLC. All rights reserved.
 *
 */

#include <iostream>
#include "SimpleMatrixExample.h"
#include "Matrix.h"

using namespace std;
using namespace jcho;

extern "C" void simpleMatrixExample() {
  // Define some simple matrices
  Matrix<double> identity("[1 0 0; 0 1 0; 0 0 1]");
  Matrix<double> m_2("[.5 0 0; 0 .5 0; 0 0 1]");
  Matrix<double> translate_1x("[1 0 1; 0 1 0; 0 0 1]");
  Matrix<double> translate_1y("[1 0 0; 0 1 1; 0 0 1]");
  Matrix<double> rotate90("[0 1 0; 1 0 0; 0 0 1]");
  Matrix<double> v0("[0; 0; 1]");

  // should output [1 0 0; 0 1 0; 0 0 1] and [.5 0 0; 0 .5 0; 0 0 1]
  cout << identity.to_string() << endl;
  cout << m_2.to_string() << endl;

  // should output [0; 0; 1]
  Matrix<double> v00 = identity * v0;
  cout << v00.to_string() << endl << endl;
  
  // Should output [1; 0; 1] [1; 1; 1] [1; 2; 1] [1; 3; 1] [3; 1; 1] [4; 1; 1] [4; 2; 1] [2; 1; 1]
  Matrix<double> v10 = translate_1x * v0;  // move in x direction 1 unit
  Matrix<double> v11 = translate_1y * v10; // move in y direction 1 unit
  Matrix<double> v12 = translate_1y * v11; // move in y direction 1 unit
  Matrix<double> v13 = translate_1y * v12; // move in y direction 1 unit
  Matrix<double> v31 = rotate90 * v13;     // rotate 90 degrees
  Matrix<double> v41 = translate_1x * v31; // move in x direction 1 unit
  Matrix<double> v42 = translate_1y * v41; // move in y direction 1 unit
  Matrix<double> v21 = m_2 * v42;          // scale everything in half
  cout << v10.to_string() << endl;
  cout << v11.to_string() << endl;
  cout << v12.to_string() << endl;
  cout << v13.to_string() << endl;
  cout << v31.to_string() << endl;
  cout << v41.to_string() << endl;
  cout << v42.to_string() << endl;
  cout << v21.to_string() << endl << endl;
  
  // If there are lots of points, then for performance and convenience, we can combine all matrices into matrix. Note that the order of operations goes RIGHT TO LEFT.
  Matrix<double> master = m_2 * translate_1y * translate_1x * rotate90 * translate_1y * translate_1y * translate_1y * translate_1x;
  cout << master.to_string() << endl;
  v21 = master * v00;
  cout << v21.to_string() << endl; // should be [2; 1; 1]
}
