///////////////////////////////////////////////////////////////////////////////
//
//  Moving Median Filter Benchmark
//
//  Copyright (c) 2016 to present Nick Hilton
//
//  weegreenblobbie_at_yahoo_com
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Library General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////

// C++ headers
#include <algorithm>
#include <cassert>
#include <chrono>
#include <deque>
#include <exception>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>


unsigned keep_odd(unsigned n)
{
    if(n % 2 == 0) return n + 1;

    return n;
}


template <class T>
class FilterInterface
{

public:
    virtual std::vector<T> filter(const std::vector<T> & in) = 0;
};



template <class T>
class NthElement : public FilterInterface<T>
{
    std::vector<T> _history;
    std::vector<T> _pool;
    unsigned       _median;

public:

    NthElement(unsigned window_size)
        :
        _history(keep_odd(window_size), T()),
        _pool(_history),
        _median(window_size / 2 + 1)
    {
        assert(window_size >= 3);
    }


    std::vector<T> filter(const std::vector<T> & in)
    {
        assert(in.size() > 0);

        //---------------------------------------------------------------------
        // init state

        unsigned hist_ptr = 0;

        std::fill(_history.begin(), _history.end(), in[0]);

        //---------------------------------------------------------------------
        // filter input

        std::vector<T> out;
        out.reserve(in.size());

        for(auto x : in)
        {
            // step 1, write to history pool

            _history[hist_ptr] = x;

            hist_ptr = (hist_ptr + 1) % _history.size();

            // step 2, copy history to pool

            std::copy(_history.begin(), _history.end(), _pool.begin());

            // step 3, compute median on pool

            auto first = _pool.begin();
            auto last  = _pool.end();
            auto middle = first + (last - first) / 2;

            std::nth_element(first, middle, last);

            out.push_back(*middle);
        }

        return out;
    }
};


template <class T>
class LowerBoundVector : public FilterInterface<T>
{
    std::vector<T> _history;
    std::vector<T> _pool;
    unsigned       _median;

public:

    LowerBoundVector(unsigned window_size)
        :
        _history(keep_odd(window_size), T()),
        _pool(_history),
        _median(window_size / 2 + 1)
    {
        assert(window_size >= 3);
    }

    std::vector<T> filter(const std::vector<T> & in)
    {
        assert(in.size() > 0);

        //---------------------------------------------------------------------
        // init state

        unsigned hist_ptr = 0;

        std::fill(_history.begin(), _history.end(), in[0]);
        std::fill(_pool.begin(), _pool.end(), in[0]);

        // pool is keep sorted

        //---------------------------------------------------------------------
        // filter input

        std::vector<T> out;
        out.reserve(in.size());

        for(auto x : in)
        {
            // step 1, remove oldest value from the pool.

            auto old_index = (hist_ptr + 1) % _pool.size();

            auto pool_index = std::lower_bound(_pool.begin(), _pool.end(), _history[old_index]);

            _pool.erase(pool_index);

            // step 2, insert new value into pool

            auto insert_pos = std::lower_bound(_pool.begin(), _pool.end(), x);

            _pool.insert(insert_pos, x);

            // step 3, write input value into history.

            _history[hist_ptr] = x;

            hist_ptr = old_index;

            // median is always the middle of the pool

            out.push_back(_pool[_median]);
        }

        return out;
    }
};


template <class T>
class LowerBoundDeque : public FilterInterface<T>
{
    std::vector<T> _history;
    std::deque<T>  _pool;
    unsigned       _median;

public:

    LowerBoundDeque(unsigned window_size)
        :
        _history(keep_odd(window_size), T()),
        _pool(keep_odd(window_size), T()),
        _median(window_size / 2 + 1)
    {
        assert(window_size >= 3);
    }

    std::vector<T> filter(const std::vector<T> & in)
    {
        assert(in.size() > 0);

        //---------------------------------------------------------------------
        // init state

        unsigned hist_ptr = 0;

        std::fill(_history.begin(), _history.end(), in[0]);
        std::fill(_pool.begin(), _pool.end(), in[0]);

        // pool is keep sorted

        //---------------------------------------------------------------------
        // filter input

        std::vector<T> out;
        out.reserve(in.size());

        for(auto x : in)
        {
            // step 1, remove oldest value from the pool.

            auto old_index = (hist_ptr + 1) % _pool.size();

            auto pool_index = std::lower_bound(_pool.begin(), _pool.end(), _history[old_index]);

            _pool.erase(pool_index);

            // step 2, insert new value into pool

            auto insert_pos = std::lower_bound(_pool.begin(), _pool.end(), x);

            _pool.insert(insert_pos, x);

            // step 3, write input value into history.

            _history[hist_ptr] = x;

            hist_ptr = old_index;

            // median is always the middle of the pool

            out.push_back(*(_pool.begin() +_median));
        }

        return out;
    }
};


//-----------------------------------------------------------------------------
// types & random vectors


template <class T>
std::string type_string();

template <> std::string type_string<short>()     { return "short"; }
template <> std::string type_string<int>()       { return "int"; }
template <> std::string type_string<long long>() { return "long long"; }
template <> std::string type_string<float>()     { return "float"; }
template <> std::string type_string<double>()    { return "double"; }


template <class T>
std::vector<T> get_random_vector(unsigned n_elements);


template <class T>
std::vector<T> get_random_integer_vector(unsigned n_elements)
{
    std::vector<T> out;

    std::mt19937_64 rng;

    union
    {
        std::uint_fast64_t from;
        T                  to;
    };

    for(unsigned i = 0; i < n_elements; ++i)
    {
        from = rng();

        out.push_back(to);
    }

    return out;
}


template <>
std::vector<short> get_random_vector(unsigned n_elements)
{
    return get_random_integer_vector<short>(n_elements);
}


template <>
std::vector<int> get_random_vector(unsigned n_elements)
{
    return get_random_integer_vector<int>(n_elements);
}


template <>
std::vector<long long> get_random_vector(unsigned n_elements)
{
    return get_random_integer_vector<long long>(n_elements);
}


template <class T>
std::vector<T> get_random_float_vector(unsigned n_elements)
{
    std::vector<T> out;

    std::mt19937_64 rng;

    auto top = std::numeric_limits<std::uint_fast64_t>::max();
    auto mid = top / 2;

    T middle = static_cast<T>(mid);
    T scale = 1.0 / middle;

    for(unsigned i = 0; i < n_elements; ++i)
    {
        auto r = rng();

        auto rrr = static_cast<T>(r) - middle;

        out.push_back(scale * rrr);
    }

    return out;
}


template <>
std::vector<float> get_random_vector(unsigned n_elements)
{
    return get_random_float_vector<float>(n_elements);
}


template <>
std::vector<double> get_random_vector(unsigned n_elements)
{
    return get_random_float_vector<double>(n_elements);
}


//-----------------------------------------------------------------------------
// timing utils

using TimePoint = std::chrono::time_point<std::chrono::system_clock>;


TimePoint now()
{
    return std::chrono::system_clock::now();
}



template <class T>
float delta(const T & d)
{
    return static_cast<float>(
        std::chrono::duration_cast<std::chrono::milliseconds>(d).count()
    );
}


const unsigned vector_size = 128000;
const unsigned iterations = 15;


template <class T>
float run_timing_test(FilterInterface<T> & f)
{
    std::vector<float> timings;

    auto input = get_random_vector<T>(vector_size);

    for(unsigned i = 0; i < iterations; ++i)
    {
        auto t0 = now();

        auto out = f.filter(input);

        auto t1 = now();

        timings.push_back(delta(t1 - t0));
    }

    // compute average of timing values

    float avg = 0.0f;

    for(auto t : timings) avg += t;

    avg /= static_cast<float>(timings.size());

    return avg;
}


enum Alignment { left, right, center };


std::string print(const std::string & in, unsigned width, Alignment align = left)
{
    std::stringstream ss;

    switch(align)
    {
        case left:
        {
            ss << in;

            int delta = width - in.size();

            while(delta > 0)
            {
                ss << " ";
                --delta;
            }

            break;
        }

        case right:
        {
            int delta = width - in.size();

            while(delta > 0)
            {
                ss << " ";
                --delta;
            }

            ss << in;

            break;
        }

        case center:
        {
            int delta = width - in.size();

            int d = delta / 2 + delta % 2;

            while(d > 0)
            {
                ss << " ";
                --d;
            }

            ss << in;

            d = delta / 2;

            while(d > 0)
            {
                ss << " ";
                --d;
            }
        }
    }

    return ss.str();
}



int main(int argc, char ** argv)
{
    std::vector<unsigned> window_sizes = {3, 5, 9, 17, 33, 65, 129, 257, 513};

    using std::cout;

    using Type = float;

    auto tname = type_string<Type>();

    cout
        << "All times are milliseconds\n"
        << "T = " << tname << "\n"
        << "Filtering random std::vector<" << tname << ">\n"
        << "    vector.size() = " << vector_size << "\n"
        << "    n iterations  = " << iterations << "\n"
        << "TABLE:\n";

    int width = 18;

    cout
        << print("Window", width, center)
        << print("NthElement", width, center)
        << print("LowerBoundDeque", width, center)
        << print("LowerBoundVector", width, center)
        << "\n";

    typedef std::shared_ptr<FilterInterface<Type>> FilterPtr;

    for(auto w : window_sizes)
    {
        cout << print(std::to_string(w), width, center);

        std::vector<FilterPtr> filters = {
            std::make_shared<NthElement<Type>>(w),
            std::make_shared<LowerBoundDeque<Type>>(w),
            std::make_shared<LowerBoundVector<Type>>(w),
        };

        for(auto & fptr : filters)
        {
            auto t = run_timing_test(*fptr);

            cout << print(std::to_string(static_cast<int>(t)), width, center);
        }

        cout << "\n";
    }

    return 0;
}