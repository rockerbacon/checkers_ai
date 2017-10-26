#ifndef _LAB309_ADT_MATRIX_H_
#define _LAB309_ADT_MATRIX_H_

#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <math.h>

#define COORDINATE_X 0
#define COORDINATE_Y 1

namespace lab309 {

	template<typename dataType> class Vector;

	template<typename dataType>
	class Matrix {
		template<typename any> friend class Vector;
		private:
			dataType *data;
			size_t lines;
			size_t colums;
			size_t columSize;
			size_t beginningOffset;
			bool freeData;
			
		public:
			//CONSTRUCTORS
			Matrix (void);
			Matrix (const Matrix<dataType> &matrix);
			Matrix (Matrix<dataType> &&matrix);
			Matrix (size_t lines, size_t colums);
			Matrix (const std::initializer_list<std::initializer_list<dataType>> &matrix);
			~Matrix (void);
			
			//GETTERS
			size_t getLines (void) const;
			size_t getColums (void) const;
			
			//METHODS
			Matrix<dataType> slice (size_t beginLine, size_t beginColum, size_t endLine, size_t endColum);
			
			//OPERATORS
			template<typename any> explicit operator Matrix<any> (void) const;
			
			Matrix<dataType> operator+ (const Matrix<dataType> &matrix) const;
			Matrix<dataType> operator- (const Matrix<dataType> &matrix) const;
			template<typename any> Matrix<dataType> operator* (any scalar) const;
			
			dataType* operator[](size_t line);
			const dataType* operator[] (size_t line) const;
			inline dataType& operator[](const Matrix<size_t> &point) {
				return this->operator[](point[0][0])[point[1][0]];
			}
			inline const dataType& operator[](const Matrix<size_t> &point) const {
				return this->operator[](point[0][0])[point[1][0]];
			}
			
			Matrix<dataType>& operator= (const Matrix<dataType> &matrix);
			Matrix<dataType>& operator= (Matrix<dataType> &&matrix);
			
	};
	
	template<typename dataType, typename any>
	inline Matrix<dataType> operator* (any scalar, const Matrix<dataType> &matrix) {
		return matrix*scalar;
	}
	template<typename dataType>
	Matrix<dataType> normalize (const Matrix<dataType> &matrix);
	
	template<typename dataType>
	class Vector : public Matrix<dataType> {
		public:
			Vector (void) = default;
			Vector (const Vector<dataType> &vector) : Matrix<dataType>(vector) {}
			Vector (Vector<dataType> &&vector);
			Vector (size_t lines) : Matrix<dataType>(lines, 1) {}
			Vector (const Matrix<dataType> &matrix) throw (std::invalid_argument);
			Vector (Matrix<dataType> &&matrix) throw (std::invalid_argument);
			Vector (const std::initializer_list<dataType> &array);
			
			inline Vector<dataType> slice (size_t begin, size_t end) {
				return this->slice(begin, 0, end, 0);
			}
			
			inline dataType& operator[](size_t line) {
				return *Matrix<dataType>::operator[](line);
			}
			
			inline const dataType& operator[](size_t line) const {
				return *Matrix<dataType>::operator[](line);
			}
			
			template<typename any> operator Matrix<any> (void) const;
			
			Vector<dataType>& operator= (const Vector<dataType> &vector);
			Vector<dataType>& operator= (Vector<dataType> &&vector);
	};
	
	template<typename dataType>
	double manhattanDistance (const Vector<dataType> &a, const Vector<dataType> &b);

}

#include "Lab309_ADT_MatrixImplementation.h"

#endif
