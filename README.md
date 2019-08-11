# Requirements:

AIDEL is first trained with sklearn, then we optimize AIDEL with C++ to get high performance. 
The machine should install sklearn, support avx instructions.

# Run:
1.compile: 
	make

2.gen data:
	./script/gen_data.sh

3.train the models:
	./script/train_aidel.sh
	./script/train_learnedindex.sh

4.evaluate static search:
	./script/run_search.sh

5.evaluate insertion performance when sample 10% of the data:
	./script/run_insert_sample_0.1.sh

6.evaluate insertion performance when sample 1% of the data:
	./script/run_insert_sample_0.01.sh
