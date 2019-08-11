# Descriptions

AIDEL is a `scalale learned index structure`, which is motivated from the paper `'The Case for Learned Index structures'`.<br>

> Kraska T, Beutel A, Chi E H, et al. The case for learned index structures[C]//Proceedings of the 2018 International Conference on Management of Data. ACM, 2018: 489-504. <br>

We compare AIDEL with the state-of-the-art index structures, including [`B+-tree`](http://panthema.net/2007/stx-btree/), [`FAST`](http://kaldewey.com/pubs/FAST__SIGMOD10.pdf), [`spline interpolation`](http://databasearchitects.blogspot.com/2017/12/the-case-for-b-tree-index-structures.html), and [`the learned indexes`](https://arxiv.org/abs/1712.01208).

# The implementations


# Requirements:

The machine should install [`sklearn`](https://scikit-learn.org/stable/), support `avx` instructions.

# Run:
1.compile: <br>
> mkdir build<br>
> make

2.gen data:<br>
> ./script/gen_data.sh

3.train the models:<br>
> ./script/train_aidel.sh<br>
> ./script/train_learnedindex.sh

4.evaluate static search:<br>
> ./script/run_search.sh

5.evaluate insertion performance when sample 10% of the data:<br>
> ./script/run_insert_sample_0.1.sh

6.evaluate insertion performance when sample 1% of the data:<br>
> ./script/run_insert_sample_0.01.sh
