/*
 * This header provides optimization for matrix operations where the matrix stores primitive types
 * The optimization is done using AVX registers
 *
 */

/*MATRIX*/
/*CONSTRUCTORS*/
template<typename dataType>
lab309::Matrix<dataType>::Matrix (void) {
	this->freeData = false;
}

template<typename dataType>
lab309::Matrix<dataType>::Matrix (const Matrix<dataType> &matrix) : Matrix(matrix.lines, matrix.colums) {
	for (size_t i = 0; i < matrix.lines; i++) {
		for (size_t j = 0; j < matrix.colums; j++) {
			(*this)[i][j] = matrix[i][j];
		}
	}
}

template<typename dataType>
lab309::Matrix<dataType>::Matrix (Matrix<dataType> &&matrix) {

	this->data = matrix.data;
	this->lines = matrix.lines;
	this->colums = matrix.colums;
	this->columSize = matrix.columSize;
	this->beginningOffset = matrix.beginningOffset;
	
	matrix.freeData = false;
}

template<typename dataType>
lab309::Matrix<dataType>::Matrix (size_t lines, size_t colums) {

	this->data = (dataType*)malloc(lines*colums*sizeof(dataType));

	this->lines = lines;
	this->colums = colums;
	this->columSize = colums;
	this->beginningOffset = 0;
	this->freeData = true;
}

template<typename dataType>
lab309::Matrix<dataType>::Matrix (const std::initializer_list<std::initializer_list<dataType>> &matrix) : Matrix(matrix.size(), matrix.begin()->size()) {
	const std::initializer_list<dataType> *i;
	const dataType *j;
	size_t k;

	k = 0;
	for (i = matrix.begin(); i != matrix.end(); i++) {
		for (j = i->begin(); j != i->end(); j++) {
			(*this)[k] = *j;
			k++;
		}
	}
}

template<typename dataType>
lab309::Matrix<dataType>::~Matrix (void) {
	if (this->freeData) {
		free (this->data);
	}
}

/*GETTERS*/
template<typename dataType>
size_t lab309::Matrix<dataType>::getLines (void) const {
	return this->lines;
}

template<typename dataType>
size_t lab309::Matrix<dataType>::getColums (void) const {
	return this->colums;
}


/*METHODS*/
template<typename dataType>
lab309::Matrix<dataType> lab309::Matrix<dataType>::slice (size_t beginLine, size_t beginColum, size_t endLine, size_t endColum) {
	Matrix<dataType> slice;
	
	slice.data = this->data;
	slice.lines = endLine - beginLine;
	slice.colums = endColum - beginColum;
	slice.columSize = this->columSize;
	slice.beginningOffset = beginLine*this->columSize + beginColum;
	slice.freeData = false;
	
}

/*OPERATORS*/
template<typename dataType> template<typename any>
lab309::Matrix<dataType>::operator Matrix<any> (void) const {
	Matrix<any> copy(this->lines, this->colums);
	for (size_t i = 0; i < this->lines; i++) {
		for (size_t j = 0; j < this->colums; j++) {
			copy[i][j] = (*this)[i][j];
		}
	}
	return copy;
}

template<typename dataType>
lab309::Matrix<dataType> lab309::Matrix<dataType>::operator+ (const Matrix<dataType> &matrix) const {
	Matrix<dataType> result(this->lines, this->colums);
	for (size_t i = 0; i < this->lines; i++) {
		for (size_t j = 0; j < this->colums; j++) {
			result[i][j] = (*this)[i][j] + matrix[i][j];
		}
	}
	return result;
}

template<typename dataType>
lab309::Matrix<dataType> lab309::Matrix<dataType>::operator- (const Matrix<dataType> &matrix) const {
	Matrix<dataType> result(this->lines, this->colums);
	for (size_t i = 0; i < this->lines; i++) {
		for (size_t j = 0; j < this->colums; j++) {
			result[i][j] = (*this)[i][j] - matrix[i][j];
		}
	}
	return result;
}

template<typename dataType> template<typename any>
lab309::Matrix<dataType> lab309::Matrix<dataType>::operator* (any scalar) const {
	Matrix<dataType> result(this->lines, this->colums);
	for (size_t i = 0; i < this->lines; i++) {
		for (size_t j = 0; j < this->colums; j++) {
			result[i][j] = (*this)[i][j]*scalar;
		}
	}
	return result;
}
			
template<typename dataType>
dataType* lab309::Matrix<dataType>::operator[] (size_t line) {
	return this->data + line*this->columSize + this->beginningOffset;
}

template<typename dataType>
const dataType* lab309::Matrix<dataType>::operator[] (size_t line) const {
	return this->data + line*this->columSize + this->beginningOffset;
}

template<typename dataType>
lab309::Matrix<dataType>& lab309::Matrix<dataType>::operator= (const Matrix<dataType> &matrix) {
	Matrix<dataType> copy(matrix);
	dataType *aux;
	
	aux = this->data;
	this->data = copy.data;
	copy.data = aux;
	
	copy.freeData = this->freeData;
	
	this->lines = copy.lines;
	this->colums = copy.colums;
	this->columSize = copy.columSize;
	this->beginningOffset = 0;
	this->freeData = true;
	
	return *this;
}

template<typename dataType>
lab309::Matrix<dataType>& lab309::Matrix<dataType>::operator= (Matrix<dataType> &&matrix) {
	dataType *aux;
	bool auxB;
	
	aux = this->data;
	this->data = matrix.data;
	matrix.data = aux;
	
	auxB = matrix.freeData;
	matrix.freeData = this->freeData;
	this->freeData = auxB;
	
	this->lines = matrix.lines;
	this->colums = matrix.colums;
	this->columSize = matrix.columSize;
	this->beginningOffset = matrix.beginningOffset;
	
	return *this;
}

template<typename dataType>
lab309::Matrix<dataType> lab309::normalize (const Matrix<dataType> &matrix) {
	Matrix<dataType> normalized(matrix.getLines(), matrix.getColums());
	dataType norm = 0;
	for (size_t i = 0; i < matrix.getLines(); i++) {
		for (size_t j = 0; j < matrix.getColums(); j++) {
			norm += matrix[i][j]*matrix[i][j];
		}
	}
	norm = sqrt(norm);
	
	for (size_t i = 0; i < normalized.getLines(); i++) {
		for (size_t j = 0; j < normalized.getColums(); j++) {
			normalized[i][j] = matrix[i][j]/norm;
		}
	}
	
	return normalized;
}

/*VECTOR*/
/*CONSTRUCTORS*/
template<typename dataType>
lab309::Vector<dataType>::Vector (const Matrix<dataType> &matrix) throw (std::invalid_argument) : Matrix<dataType>(matrix.lines, 1) {
	if (matrix.getColums > 1) {
		throw std::invalid_argument("The matrix has more than one colum, hence it's not a vector");
	}
	
	for (size_t i = 0; i < matrix.lines; i++) {
		(*this)[i] = *(matrix[i]);
	}
}

template<typename dataType>
lab309::Vector<dataType>::Vector (Vector<dataType> &&matrix) {

	this->data = matrix.data;
	this->lines = matrix.lines;
	this->colums = matrix.colums;
	this->columSize = matrix.columSize;
	this->beginningOffset = matrix.beginningOffset;
	
	matrix.freeData = false;
}

template<typename dataType>
lab309::Vector<dataType>::Vector (const std::initializer_list<dataType> &array) : Matrix<dataType>(array.size(), 1) {
	const dataType *i;
	size_t j;

	j = 0;
	for (i = array.begin(); i != array.end(); i++) {
		(*this)[j] = *i;
		j++;
	}
}
	
template<typename dataType>
lab309::Vector<dataType>::Vector (Matrix<dataType> &&matrix) throw (std::invalid_argument) {
	if (matrix.getColums() > 1) {
		throw std::invalid_argument("The matrix has more than one colum, hence it's not a vector");
	}
	
	this->data = matrix.data;
	this->lines = matrix.lines;
	this->colums = 1;
	this->columSize = matrix.columSize;
	this->beginningOffset = matrix.beginningOffset;
	this->freeData = matrix.freeData;
	
	matrix.freeData = false;
}

/*OPERATORS*/
template<typename dataType> template<typename any>
lab309::Vector<dataType>::operator Matrix<any> (void) const {
	Matrix<any> copy(this->lines, 1);
	for (size_t i = 0; i < this->lines; i++) {
			copy[i][0] = (*this)[i];
	}
	return copy;
}

template<typename dataType>
lab309::Vector<dataType>& lab309::Vector<dataType>::operator= (const Vector<dataType> &matrix) {
	Vector<dataType> copy(matrix);
	dataType *aux;
	
	aux = this->data;
	this->data = copy.data;
	copy.data = aux;
	
	copy.freeData = this->freeData;
	
	this->lines = copy.lines;
	this->colums = copy.colums;
	this->columSize = copy.columSize;
	this->beginningOffset = 0;
	this->freeData = true;
	
	return *this;
}

template<typename dataType>
lab309::Vector<dataType>& lab309::Vector<dataType>::operator= (Vector<dataType> &&matrix) {
	dataType *aux;
	bool auxB;
	
	aux = this->data;
	this->data = matrix.data;
	matrix.data = aux;
	
	auxB = matrix.freeData;
	matrix.freeData = this->freeData;
	this->freeData = auxB;
	
	this->lines = matrix.lines;
	this->colums = matrix.colums;
	this->columSize = matrix.columSize;
	this->beginningOffset = matrix.beginningOffset;
	
	return *this;
}

template<typename dataType>
double lab309::manhattanDistance (const Vector<dataType> &a, const Vector<dataType> &b) {
	double result = 0;
	for (size_t i = 0; i < a.getLines(); i++) {
		result = fabs(a[i] - b[i]);
	}
	return result;
}
