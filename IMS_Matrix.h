#pragma once

/**
 * \brief generalized matrix helper class
 * useful for where the Orbiter API stops
 * \todo figure out what this stuff does and document it.
 * \author meson800
 */
template <typename T> class IMS_Matrix
{
public:
    IMS_Matrix(unsigned int n, unsigned int m);
    IMS_Matrix(unsigned int n, unsigned int m, std::initializer_list<T> init);
    unsigned int numRows() const;
    unsigned int numCols() const;
    std::string toString() const;
    
    //operations-both linear algebra stuff and straight up operator overloads

    //operations
    const IMS_Matrix<T> makePositive() const;
    const IMS_Matrix<T> transpose() const;
    const IMS_Matrix<T> augment_right(const IMS_Matrix<T> &rhs) const;
    const IMS_Matrix<T> gaussianEliminate() const;
    IMS_Matrix<T> & swapRowOp(unsigned int firstRow, unsigned int secondRow);
    IMS_Matrix<T> & replacementRowOp(unsigned int firstRow, unsigned int secondRow, T scaleFactor);
    IMS_Matrix<T> & scaleRowOp(unsigned int row, T scaleFactor);

    //operators
    IMS_Matrix<T> & operator=(const IMS_Matrix<T> &rhs);
    IMS_Matrix<T> & operator+=(const IMS_Matrix<T> &rhs);
    IMS_Matrix<T> & operator-=(const IMS_Matrix<T> &rhs);
    IMS_Matrix<T> & operator*=(const IMS_Matrix<T> &rhs);
    const IMS_Matrix<T> operator*(const IMS_Matrix<T> &rhs)  const;
    bool operator==(const IMS_Matrix<T> &other) const;
    bool operator!=(const IMS_Matrix<T> &other) const;
    T operator()(unsigned int x, unsigned int y) const;
    T & operator()(unsigned int x, unsigned int y);

    //exceptions
    class MatrixException : public std::runtime_error
    {
    public:
        MatrixException(const char* arg) : runtime_error(arg){}
    };
    class MatrixArithmeticException : public std::runtime_error
    {
    public:
        MatrixArithmeticException(const char* arg) : runtime_error(arg){}
    };
    class MatrixEliminationException : public std::runtime_error
    {
    public:
        MatrixEliminationException(const char* arg) : runtime_error(arg){}
    };
private:
    unsigned int rows, cols;
    std::vector<T> data;
};

