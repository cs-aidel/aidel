# Descriptions

AIDEL is a `scalale learned index structure`, which is motivated from the paper `'The Case for Learned Index structures'`.<br>

> Kraska T, Beutel A, Chi E H, et al. The case for learned index structures[C]//Proceedings of the 2018 International Conference on Management of Data. ACM, 2018: 489-504. <br>

We compare AIDEL with the state-of-the-art index structures, including [`B+-tree`](http://panthema.net/2007/stx-btree/), [`FAST`](http://kaldewey.com/pubs/FAST__SIGMOD10.pdf), [`spline interpolation`](http://databasearchitects.blogspot.com/2017/12/the-case-for-b-tree-index-structures.html), and [`the learned indexes`](https://arxiv.org/abs/1712.01208).

# The implementations
### The learned indexes and aidel
We implement the learned indexes following the design and configurations of [the original paper](https://arxiv.org/abs/1712.01208). We build a `delta-buffer` for the learned indexes to handle inserts, where the `delta-buffer` uses the implementation of [`stx::btree`](http://panthema.net/2007/stx-btree/).<br>
<br>AIDEL is optimized with SIMD by using the code from [`linear search`](https://dirtyhandscoding.wordpress.com/2017/08/25/performance-comparison-linear-search-vs-binary-search/).<br>
<br>We first train all the models in a widely used deep learning framework, called [`sklearn`](https://scikit-learn.org/stable/), then we optimize the inference phase in C++ to obtain high performance.<br>

### index baselines
The B+-tree use a popular implementation, called `stx::btree`.<br>
> [http://panthema.net/2007/stx-btree/](http://panthema.net/2007/stx-btree/)<br>

[`FAST`](http://kaldewey.com/pubs/FAST__SIGMOD10.pdf) and [`spline interpolation`](http://databasearchitects.blogspot.com/2017/12/the-case-for-b-tree-index-structures.html) are the cache- and SIMD-conscious variants of B+-tree, where [`FAST`](http://kaldewey.com/pubs/FAST__SIGMOD10.pdf) is the state-of-the-art SIMD optimize structure and the [`spline interpolation`](http://databasearchitects.blogspot.com/2017/12/the-case-for-b-tree-index-structures.html) is a fixed-height B+-tree. For the two structures, we use the implementations from [`Standford Future Data System`](https://github.com/stanford-futuredata/index-baselines).
> [https://github.com/stanford-futuredata/index-baselines](https://github.com/stanford-futuredata/index-baselines)



# How to run:
The machine should install [`sklearn`](https://scikit-learn.org/stable/), support `avx` instructions.<br>
1.compile: <br>
```
~$ mkdir build
~$ make
```

> We compile all implementations under director `"./build/"` with -Ofast option.

2.gen data:<br>

```
~$ ./script/gen_data.sh
```

> First generate the dataset `'lognormal.sorted.190M'`, then respectively sample `10%` and `1%` data, finally `disorder` the data.

3.train the models:<br>
```
~$ ./script/train_aidel.sh
~$ ./script/train_learnedindex.sh
```

> All models are trained in [`sklearn`](https://scikit-learn.org/stable/), and are stored under the directory `"./data/"` to be accessed by C++ to obtian high performance.

4.evaluate static search:<br>
```
~$ ./script/run_search.sh
```

> Compare the lookup performance without insertions.

5.evaluate insertion performance when sample 10% of the data:<br>
```
~$ ./script/run_insert_sample_0.1.sh
```

> First sample 10% of the data for training and then test the insertion performance by inserting the data from 1x to 10x than the training data.

6.evaluate insertion performance when sample 1% of the data:<br>
```
~$ ./script/run_insert_sample_0.01.sh
```

> First sample 1% of the data for training and then test the insertion performance by inserting the data from 10x to 100x than the training data.
