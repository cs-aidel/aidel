# Requirements:

AIDEL is first trained with sklearn, then we optimize AIDEL with C++ to get high performance.<br> 
The machine should install [`sklearn`](https://scikit-learn.org/stable/), support `avx` instructions.

# Run:
1.compile: <br>
		mkdir build<br>
		make

2.gen data:<br>
		./script/gen_data.sh

3.train the models:<br>
	./script/train_aidel.sh<br>
	./script/train_learnedindex.sh

4.evaluate static search:<br>
	./script/run_search.sh

5.evaluate insertion performance when sample 10% of the data:<br>
	./script/run_insert_sample_0.1.sh

6.evaluate insertion performance when sample 1% of the data:<br>
	./script/run_insert_sample_0.01.sh
