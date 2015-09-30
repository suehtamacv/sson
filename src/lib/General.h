#ifndef GENERAL_H
#define GENERAL_H

#include <assert.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>

using namespace std;

/**
 * @brief The General class implements general classes to general activities.
 */
class General {
  public:
    /**
     * @brief General::ABS evaluates the absolute value of x.
     * @return the absolute value of x.
     */
    template <class T> static const T &ABS(const T &x);

    /**
     * @brief General::exponential generates a number extracted from a exponential distribution with parameter L.
     * @param L is the parameter of the exponential distribution.
     * @return a number extracted from the exponential distribution.
     */
    static long double exponential(long double L);

    /**
     * @brief General::MAX evaluates and returns the biggest between a and b.
     * @return the biggest between a and b.
     */
    template <class T> static const T &MAX(const T &a, const T &b);

    /**
     * @brief General::MIN evaluates and returns the smallest between a and b.
     * @return the smallest between a and b.
     */
    template <class T> static const T &MIN(const T &a, const T &b);

    /**
     * @brief General::printAnsiVector prints in a single line all the values of the vector.
     */
    template <class T> static void printAnsiVector(vector<T> &);

    /**
     * @brief General::setVector sets a vector with size values, each one initialized to value.
     * @param V is a pointer to the set vector.
     * @param size is the size of the vector.
     * @param value is the value that initializes each value of the vector.
     */
    template <class T> static void setVector(T *V, int size, T value);

    /**
     * @brief General::dBtoLin converts a number in dB to its conterpart in linear unities.
     * @param x is a value measured in dB.
     * @return the value of x converted to linear unities.
     */
    template <class T> static long double dBtoLin(T x) {
        return pow(10, 0.1 * x);
    }

    /**
     * @brief General::LintodB converts a number in linear unities to its conterpart in dB.
     * @param x is a value measured in linear unities.
     * @return the value of x converted to dB.
     */
    template <class T> static long double LintodB(T x) {
        return 10.0 * (log10(x));
    }

    /**
     * @brief General::uniform generates a number extracted from a uniform distribution ranging from xmin to xmax.
     * @param xmin is the minimum value of the uniform distribution.
     * @param xmax is the maximum value of the uniform distribution.
     * @return a number extracted from the uniform distribution.
     */
    static long double uniform(long double xmin, long double xmax);
};

template <class T> const T &General::ABS(const T &x) {
    return (x > 0) ? x : -x;
}

template <class T> const T &General::MAX(const T &a, const T &b) {
    return (a > b) ? a : b;
}

template <class T> const T &General::MIN(const T &a, const T &b) {
    return (a < b) ? a : b;
}

template <class T> void General::printAnsiVector(vector<T> &v) {
    for (int i = 0; i < v.size(); i++) {
        std::cout << v.at(i) << " ";
    }
}

template <class T> void General::setVector(T *V, int size, T value) {
    for (int i = 0; i < size; i++) {
        V[i] = value;
    }
}

#endif /* GENERAL_H */
