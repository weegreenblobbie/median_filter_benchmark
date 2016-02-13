1D Moving Median Benchmark
==========================


What is this?
-------------

This is a stand-alone C++11 program that times various techniques for performing a 1D moving median filter (sometimes called rolling median, or streaming median).

It currently only uses a signle thread.


How to build
------------

.. code-block:: bash

    g++ -o main main.cpp -std=c++11 -O3


How to execute benchmark
------------------------

.. code-block:: bash

    ./main


Current Results
---------------

.. code-block::

    All times are milliseconds
    T = float
    Filtering random std::vector<float>
        vector.size() = 128000
        n iterations  = 15
    TABLE:
          Window          NthElement      LowerBoundDeque  LowerBoundVector
             3                 4                 7                 4
             5                 7                10                 6
             9                15                14                 8
            17                30                18                10
            33                52                23                13
            65                87                27                17
            129               159               32                21
            257               307               38                25
            513               649               47                31


All Stats Plotted
~~~~~~~~~~~~~~~~~

.. image:: stats_all.png


Ingoring Nth Element
~~~~~~~~~~~~~~~~~~~~

.. image:: stats_zoom.png
