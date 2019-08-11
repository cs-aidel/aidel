#include "insert_aidel.h"
#include "aidel.h"
#include "levelbtree.h"
#include "learned_index.h"

#define QUERIES_PER_TRIAL (50 * 1000 * 1000)


int main(int argc, char** argv) {
    if (argc < 6) {
        std::cerr << "Usage: " << argv[0] << " data_path model_path maxError lea_models_path lea_maxError" << std::endl;
        exit(1);
    }

    int num_trials = 1;

    // ==============read the models and construct the index for each model ==================
    int maxError = std::atoi(argv[3]);
    std::vector<iNode> models = read_model<int, iNode>(argv[2]);
    int modelsize_ = models.size();
    std::vector<int> index;
    for(int i = 0; i<modelsize_; i++)
        index.push_back(models[i].index);

    int page_size_ = 64;
    int padding_ = modelsize_ % page_size_;
    if (padding_ > 0) {
        for (int i = 0; i < page_size_ - padding_; i++) {
            index.push_back(INT_MAX);
        }
    }
    padding_ = index.size();
    printf("padding models: %d\n", padding_);

    // ====================read the data (total, insert, query)========================
  	std::vector<int> keys_total = read_data("./data/lognormal.sorted.190M");
    int total = (int) keys_total.size();
    printf("num elements: %d\n", total);

    // read out-of-order data
    std::vector<int> keys_ooo = read_data("./data/lognormal.ooo.190M");
    int ooo_n = (int) keys_ooo.size();
    printf("ooo elements: %d\n", ooo_n);

    //select data
    //std::vector<int> keys = read_data("./data/lognormal.select10.19M");
    std::vector<int> keys = read_data(argv[1]);
    keys.push_back(INT_MAX);
    int n = keys.size();
    cout << "select data : " << n << endl;



    //===========================construct different models=====================
    // Construct B-Tree
    std::vector<std::pair<int, int>> pairs;
    for (int i = 0; i < n; i++) {
        pairs.emplace_back(keys[i], keys[i]);
    }
    stx::btree_map<int, int> btree(pairs.begin(), pairs.end());

  	// Construct aidel
  	AIDEL<int, iNode, iinsertRoot, iinsertNode> aidel(models, index, keys, maxError);


    // Construct learned indexes
    int lea_maxError = std::atoi(argv[5]);
    std::vector<learned_node> lea_models = read_learned_model<learned_node>(argv[4]);
    LearnedIndex<int, learned_node> learnedindex(lea_models, keys, lea_maxError);



  	uint32_t seed = std::random_device()();
    std::mt19937 rng;
    std::uniform_int_distribution<> dist(0, n - 1);
    std::vector<int> queries(QUERIES_PER_TRIAL);


    std::vector<double> times_bt;  // 2-level index
  	std::vector<double> times_ai;  // aidel with level-bin
    std::vector<double> times_li;  // aidel with level-bin

    double time_bt = 0.0;
    double time_ai = 0.0;
    double time_li = 0.0;

    int a=0, b=0, c=0;



    //===========================search ==============================
  	// btree
    printf("Running btree\n");
    rng.seed(seed);
    for (int t = 0; t < num_trials; t++) {
        for (int &query : queries) {
            query = keys[dist(rng)];
        }

        auto start = clock();
        for (const int& key : queries) {
            btree.find(key);
        }
        double elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        times_bt.push_back(elapsed);
    }

    // aidel using level_bin
    printf("Running aidel with level_bin\n");
    rng.seed(seed);
    for (int t = 0; t < num_trials; t++) {
       for (int &query : queries) {
            query = keys[dist(rng)];
        }

        auto start = clock();
        for (const int& key : queries) {
            int pos = aidel.findInsert_level(key);
        }
        double elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        times_ai.push_back(elapsed);
    }


    // aidel using level_bin
    printf("Running aidel with learned indexes\n");
    rng.seed(seed);
    for (int t = 0; t < num_trials; t++) {
        for (int &query : queries) {
            query = keys[dist(rng)];
        }

        auto start = clock();
        for (const int& key : queries) {
            int pos = learnedindex.findInsert(key);
        }
        double elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        times_li.push_back(elapsed);
    }





  	time_bt = 1e+9 * std::accumulate(times_bt.begin(), times_bt.end(), 0.) / (num_trials * QUERIES_PER_TRIAL);
    time_ai = 1e+9 * std::accumulate(times_ai.begin(), times_ai.end(), 0.) / (num_trials * QUERIES_PER_TRIAL);
  	time_li = 1e+9 * std::accumulate(times_li.begin(), times_li.end(), 0.) / (num_trials * QUERIES_PER_TRIAL);
    

  	printf("Mean time per query\n");
  	printf("%8.1lf ns : %.40s\n", time_bt, "btree");
    printf("%8.1lf ns : %.40s\n", time_ai, "aidel with level_bin");
  	printf("%8.1lf ns : %.40s\n", time_li, "learned indexes");


  
    //========================check the correctness of aidel after insert=============
    printf("check the correctness of aidel after search: ");
    for (int i=0; i<total; i++){
        int key = keys_total[i];
        typename stx::btree_map<int, int>::iterator it = btree.find(key);
        a = it==btree.end()? 0:it.data();
        b = aidel.findInsert_level(key);
        c = learnedindex.findInsert(key);
        if (a!=b || a!=c) {
            printf("wrong!\n");
            printf("keys[%d]: %d, a: %d, b: %d, c: %d\n", i,key,a,b,c);
            exit(1);
        }
    }
    printf("correct!\n");


  	return 0;
}