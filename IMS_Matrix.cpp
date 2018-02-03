#include <initializer_list>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include "IMS_Matrix.h"
#include "Orbitersdk.h"
#include "Helpers.h"

template <typename T>
IMS_Matrix<T>::IMS_Matrix(unsigned int n, unsigned int m) : rows(n), cols(m)
{
    //init matrix with default value
    for (unsigned int i = 0; i < n * m; ++i)
        data.push_back(T());
}

template <typename T>
IMS_Matrix<T>::IMS_Matrix(unsigned int n, unsigned int m, std::initializer_list<T> init) : rows(n), cols(m)
{
    if (init.size() != n*m)
    {
        throw MatrixException("Can't initalize a matrix unless given N*M elements");
    }
    //insert values into list
    data.insert(data.end(), init.begin(), init.end());
}

template <typename T>
unsigned int IMS_Matrix<T>::numRows() const
{
    return rows;
}

template <typename T>
unsigned int IMS_Matrix<T>::numCols() const
{
    return cols;
}

template <typename T>
std::string IMS_Matrix<T>::toString() const
{
    std::ostringstream builder;
    builder << "(" << rows << "x" << cols << ")[\n";
    for (unsigned int row = 0; row < rows; ++row)
    {
        //we can't concot all elements in loop, we'd get an ending comma
        builder << "[" << (*this)(row, 0); 
        for (unsigned int col = 1; col < cols; ++col)
        {
            builder << "," << (*this)(row, col);
        }
        builder << "]\n";
    }
    builder << "]\n";
    return builder.str();
}

template <typename T>
const IMS_Matrix<T> IMS_Matrix<T>::transpose() const
{
    //straightforward transpose-switch row and col
    IMS_Matrix<T> result = IMS_Matrix<T>(numCols(), numRows());
    for (unsigned int row = 0; row < numRows(); ++row)
    {
        for (unsigned int col = 0; col < numCols(); ++col)
        {
            result(col, row) = (*this)(row, col);
        }
    }
    return result;
}

template <typename T>
const IMS_Matrix<T> IMS_Matrix<T>::augment_right(const IMS_Matrix<T> &rhs) const
{
    //add this and rhs, placing RHS on the right of this
    //only works when they have the same number of rows
    if (numRows() != rhs.numRows())
    {
        throw MatrixArithmeticException("Can't right augment unless operands have same number of rows");
    }
    IMS_Matrix<T> result = IMS_Matrix<T>(numRows(), numCols() + rhs.numCols());
    //copy lhs
    for (unsigned int row = 0; row < numRows(); ++row)
    {
        for (unsigned int col = 0; col < (numCols() + rhs.numCols()); ++col)
        {
            if (col < numCols())
            {
                //we're in lhs still
                result(row, col) = (*this)(row, col);
            }
            else
            {
                //copy rhs value
                result(row, col) = rhs(row, col - numCols());
            }

        }
    }
    return result;
}

template <typename T>
const IMS_Matrix<T> IMS_Matrix<T>::makePositive() const
{
    IMS_Matrix<T> result = *this;
    //find pivot columns and nonpivot columns
    std::vector<unsigned int> pivotCols;
    std::vector<unsigned int> freeCols;
    std::map<unsigned int, bool> impossibleRows;
    std::map<unsigned int, T> positiveIndex;
    for (unsigned int i = 0; i < numRows(); ++i)
    {
        if (result(i, i) != 0)
        {
            pivotCols.push_back(i);
        }
        else
        {
            freeCols.push_back(i);
        }
    }
    //Create ones for the nonpivot columns
    for (auto it = freeCols.begin(); it != freeCols.end(); ++it)
    {
        result(*it, *it) = 1;
        //generate 'positivity' index 
        for (unsigned int row = 0; row < numRows(); ++row)
        {
            positiveIndex[*it] += result(row, *it);
        }
    }

    //loop at least numRows times
    for (unsigned int i = 0; i < numRows(); ++i)
    {
        //find the most negative coefficent on the right
        T mostNegativeValue = 0;
        unsigned int mostNegativeRow = 0;
        for (unsigned int row = 0; row < numRows(); ++row)
        {
            //ignore if this row is marked as impossible
            //impossible rows can't be made positive :(
            if (impossibleRows.count(row))
                continue;
            if (result(row, numCols() - 1) < mostNegativeValue)
            {
                mostNegativeRow = row;
                mostNegativeValue = result(row, numCols() - 1);
            }
        }
        if (mostNegativeValue == 0)
        {
            //we're done!  No negative rows!
            return result;
        }
        //otherwise pick most positive nonpivot that we have
        //whose value is positive in that row!
        //USE POSITIVITY INDEX IN SECOND PART OF IF BELOW
        unsigned int bestFreeRow = 0;
        T bestFreeValue = -1000000000;
        for (auto it = freeCols.begin(); it != freeCols.end(); ++it)
        {
            if (result(mostNegativeRow, *it) > 0 && positiveIndex[*it] > bestFreeValue)
            {
                bestFreeValue = positiveIndex[*it];
                bestFreeRow = *it;
            }
        }

        if (bestFreeValue == -1000000000)
        {
            //set this row to impossible, we didn't find a best row
            impossibleRows[mostNegativeRow] = true;
            continue;
        }

        //now add that sucker to everything
        //the coefficent will make the most negative row zero
        T coeff = abs(result(mostNegativeRow, numCols() - 1)) / result(mostNegativeRow, bestFreeRow);

        //and add it to all columns that have it (this well set this thruster directly because of the one
        for (unsigned int row = 0; row < numRows(); ++row)
        {
            result(row, numCols() - 1) += result(row, bestFreeRow) * coeff;
        }


    }
    
    return result;
}

template <typename T>
//swaps firstRow and secondRow
IMS_Matrix<T> & IMS_Matrix<T>::swapRowOp(unsigned int firstRow, unsigned int secondRow)
{
    if (firstRow >= numRows() || secondRow >= numRows())
    {
        throw MatrixArithmeticException("Can't swap rows, rows out of bounds");
    }
    for (unsigned int col = 0; col < numCols(); ++col)
    {
        T temp = (*this)(firstRow, col);
        (*this)(firstRow, col) = (*this)(secondRow, col);
        (*this)(secondRow, col) = temp;
    }
    return *this;
}

template <typename T>
//does the operation firstRow = firstRow + scaleFactor * secondRow
IMS_Matrix<T> & IMS_Matrix<T>::replacementRowOp(unsigned int firstRow, unsigned int secondRow, T scaleFactor)
{
    if (firstRow >= numRows() || secondRow >= numRows())
    {
        throw MatrixArithmeticException("Can't replace rows, rows out of bounds");
    }
    for (unsigned int col = 0; col < numCols(); ++col)
    {
        (*this)(firstRow, col) += scaleFactor * (*this)(secondRow, col);
    }
    return *this;
}

template <typename T>
//row = row * scaleFactor
IMS_Matrix<T> & IMS_Matrix<T>::scaleRowOp(unsigned int row, T scaleFactor)
{
    if (row >= numRows())
    {
        throw MatrixArithmeticException("Can't scale row, row out of bounds");
    }
    for (unsigned int col = 0; col < numCols(); ++col)
    {
        (*this)(row, col) *= scaleFactor;
    }
    return *this;
}

template <typename T>
const IMS_Matrix<T> IMS_Matrix<T>::gaussianEliminate() const
{
    //we have to have at least two columns to eliminate
    if (numCols() < 2)
    {
        throw MatrixArithmeticException("Can't Gaussian eliminate matrix with only one column");
    }
    //check to see if there are at least as many rows as columns eliminated
    if (numRows() < (numCols() - 1))
    {
        throw MatrixArithmeticException("Can't Gaussian eliminate if there are less rows than columns (not counting augmented column)");
    }
    IMS_Matrix<T> result = *this;

    //start by forward substituting
    for (unsigned int row = 0; row < numRows(); ++row)
    {
        //pick the row with the largest absolute value, and swap it to this row
        //This partial pivoting reduces errors
        unsigned int bestRow = row;
        T bestValue = abs(result(row, row));
        for (unsigned int testRow = row + 1; testRow < numRows(); ++testRow)
        {
            double value = abs(result(testRow, row));
            if (value > bestValue)
            {
                bestValue = value;
                bestRow = testRow;
            }
        }
        if (bestRow != row)
        {
            result.swapRowOp(row, bestRow);
        }
        
        //check if value (col,col) is not zero, if it is we fail
        if (result(row, row) == 0)
        {
            continue;
            throw MatrixEliminationException("Can't fully eliminate matrix, there is at least one free variable (multiple solutions)");
        }

        //create zeros beneath pivot (row,row)
        for (unsigned int replaceRow = row + 1; replaceRow < numRows(); ++replaceRow)
        {
            if (result(replaceRow, row) == 0)
                continue;
            result.replacementRowOp(replaceRow, row, -1 * result(replaceRow, row) / result(row, row));
        }
    }

    //and now backward substitute
    for (int row = numRows() - 1; row >= 0; --row)
    {
        if (result(row, row) == 0)
        {
            continue;
        }
        //scale this row based on the pivot back to zero
        result.scaleRowOp(row, 1 / result(row, row));

        //now eliminate upward
        for (int replaceRow = row - 1; replaceRow >= 0; replaceRow--)
        {
            result.replacementRowOp(replaceRow, row, result(replaceRow, row));
        }
    }
    return result;
}

template <typename T>
IMS_Matrix<T> & IMS_Matrix<T>::operator=(const IMS_Matrix<T> &rhs)
{
    //check for self assignment, this will explode if we don't check for it and try to do this
    if (this != &rhs)
    {
        //only actually do the assignment if this and rhs are different objects
        //check to see if matrixes are the same size
        if (numRows() != rhs.numRows() || numCols() != rhs.numCols())
        {
            throw MatrixArithmeticException("Can't assign matrixes of different sizes.");
        }
        //otherwise just set it, element by element
        for (unsigned int row = 0; row < numRows(); ++row)
        {
            for (unsigned int col = 0; col < numCols(); ++col)
            {
                (*this)(row, col) = rhs(row, col);
            }
        }
    }
    return *this;
}

template <typename T>
IMS_Matrix<T> & IMS_Matrix<T>::operator+=(const IMS_Matrix<T> &rhs)
{
    //check to see if matrixes are the same size
    if (numRows() != rhs.numRows() || numCols() != rhs.numCols())
    {
        throw MatrixArithmeticException("Can't add matrixes of different sizes.");
    }
    //otherwise just add element by element
    for (unsigned int row = 0; row < numRows(); ++row)
    {
        for (unsigned int col = 0; col < numCols(); ++col)
        {
            (*this)(row, col) += rhs(row, col);
        }
    }

    return *this;
}

template <typename T>
IMS_Matrix<T> & IMS_Matrix<T>::operator-=(const IMS_Matrix<T> &rhs)
{
    //check to see if matrixes are the same size
    if (numRows() != rhs.numRows() || numCols() != rhs.numCols())
    {
        throw MatrixArithmeticException("Can't subtract matrixes of different sizes.");
    }
    //otherwise just add element by element
    for (unsigned int row = 0; row < numRows(); ++row)
    {
        for (unsigned int col = 0; col < numCols(); ++col)
        {
            (*this)(row, col) -= rhs(row, col);
        }
    }
    return *this;
}

template <typename T>
IMS_Matrix<T> & IMS_Matrix<T>::operator*=(const IMS_Matrix<T> &rhs)
{
    (*this) = (*this) * rhs;
    return *this;
}

template <typename T>
const IMS_Matrix<T> IMS_Matrix<T>::operator*(const IMS_Matrix<T> &rhs)  const
{
    //check to see if the two matrixes can be multiplied
    //lhs is NxR, rhs is RxM, where R must be the same value
    if (numCols() != rhs.numRows())
    {
        throw MatrixArithmeticException("Can't multiply matrixes where left hand columns doesn't equal right hand rows");
    }
    unsigned int numSumTerms = numCols(); //how many multiplications we have to do to get each result value
    unsigned int N = numRows();
    unsigned int M = rhs.numCols();
    IMS_Matrix<T> result = IMS_Matrix<T>(N,M);
    for (unsigned int row = 0; row < N; ++row)
    {
        for (unsigned int col = 0; col < M; ++col)
        {
            //do the multiplications, across the row (increment col) for lhs, down column (increment row) for rhs
            for (unsigned int subTerm = 0; subTerm < numSumTerms; ++subTerm)
            {
                result(row, col) += ((*this)(row, subTerm) * rhs(subTerm, col));
            }
        }
    }

    return result;
}

template <typename T>
bool IMS_Matrix<T>::operator==(const IMS_Matrix<T> &other) const
{
    if (numRows() != other.numRows() || numCols() != other.numCols())
    {
        throw MatrixArithmeticException("Can't compare matrixes of different sizes.");
    }
    for (unsigned int row = 0; row < numRows(); ++row)
    {
        for (unsigned int col = 0; col < numCols(); ++col)
        {
            //if an element is not the same, return false
            if ((*this)(row, col) != other(row, col))
            {
                return false;
            }
        }
    }
    return true;
}

template <typename T>
bool IMS_Matrix<T>::operator!=(const IMS_Matrix<T> &other) const
{
    return !(*this == other);
}

//we need both of these to be able to assign and access with the []
template <typename T>
T IMS_Matrix<T>::operator()(unsigned int row, unsigned int col) const
{
    if (row >= numRows() || col >= numCols())
    {
        throw MatrixArithmeticException("Matrix subscript out of bounds");
    }
    return data[cols* row + col];
}

template <typename T>
T & IMS_Matrix<T>::operator()(unsigned int row, unsigned int col)
{
    if (row >= numRows() || col >= numCols())
    {
        throw MatrixArithmeticException("Matrix subscript out of bounds");
    }
    return data[cols * row + col];
}


//explicit instantiations
template class IMS_Matrix < double > ;