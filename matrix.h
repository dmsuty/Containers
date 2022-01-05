/*
как делать так, чтобы деления для непростых давало CE
*/

#include <iostream>
#include <vector>
#include "biginteger.h"


template<size_t N>
class Residue {
private:
   size_t value=0;

public:
   Residue& operator+= (const Residue& other) {
       value += other.value;
       value %= N;
   }
 
   Residue& operator-= (const Residue& other) {
       value += N - other.value;
       value %= N;
   } 

   Residue& operator*= (const Residue& other) {
       value *= other.value;
       value %= N;
   }    

   Residue& operator/= (const Residue& other) {

   }
};


template<size_t Height, size_t Width, typename Field=Rational>
class Matrix {
private:
    Field matrix[Height][Width];

public:
    
};
