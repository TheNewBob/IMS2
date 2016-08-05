#pragma once
/**
 * \file IMS_Algs.h
 */

/**
 * This namespace is meant to contain any specialised algorithms that might be needed.
 * implement simple wrapper-tasks for STL-algorithms as functions, create classes for more complex stuff
 */
namespace IMS_Algs
{
	/**
	 * \brief Creates a vector containing the difference between two other vectors
	 * \param v1 reference vector
	 * \param v2 the vector to be subtracted from v1
	 * \return a vector containing all elements that are in v1 AND NOT in v2
	 * \note Inputs are passed by value rather than by reference as to not affect orignial data while sorting
	 */
	template <class T>
	vector<T> Vector_Difference(vector<T> v1, vector<T> v2)
	{
		//sort inputs
		sort(v1.begin(), v1.end());
		sort(v2.begin(), v2.end());
		//set size of result to size of v1 to save a bit of time when allocating
		vector<T> result(v1.size());
		//get the difference with the stl algorithm
		vector<T>::iterator it = set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), result.begin());
		//resize the result to its actual size
		result.resize(it - result.begin());
		return result;
	}


	
	/**
	 * \brief Appends all values in a map to a vector, ignoring keys
	 * \param _map The map from which to copy the values
	 * \param _vector The vector to which the values should be appended
	 */
	template <typename M, typename V>
	void MapToVec(const  M &_map, V &_vector) 
	{
		//resize vector first, it's faster this way
		UINT i = _vector.size();
		_vector.resize(i + _map.size());
		for (typename M::const_iterator it = _map.begin(); it != _map.end(); ++it, ++i) 
		{
			_vector[i] = it->second;
		}
	}
}

