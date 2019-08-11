#include "insert_aidel.h"
#include "aidel.h"
#include "learned_index.h"
#include "levelbtree.h"

#define QUERIES_PER_TRIAL (50 * 1000 * 1000)


int main(int argc, char** argv) {
    if (argc < 7) {
        std::cerr << "Usage: " << argv[0] << " data_path model_path maxError lea_models_path lea_maxError insert_x" << std::endl;
        exit(1);
    }

    int num_trials = 1;

    int insert_x = std::atoi(argv[6]);
    printf("============ >> insert %dx data << ==============\n", insert_x);

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
    //std::vector<int> keys = read_data("./data/lognormal.select100.1.9M");
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

    // Construct learned index
    int lea_maxError = std::atoi(argv[5]);
    std::vector<learned_node> lea_models = read_learned_model<learned_node>(argv[4]);
    LearnedIndex<int, learned_node> learnedindex(lea_models, keys, lea_maxError);



    uint32_t seed = std::random_device()();
    std::mt19937 rng;
    std::uniform_int_distribution<> dist_total(0, total - 1);
    std::uniform_int_distribution<> dist(0, n - 1);
    std::vector<int> queries(QUERIES_PER_TRIAL);


    std::vector<double> times_bs;  // 2-level index
    std::vector<double> times_ai_db;  // aidel with delta_buffer
    //std::vector<double> times_ai_sl;  // aidel with sorted_list
    std::vector<double> times_ai_lb;  // aidel with level_bin
    std::vector<double> times_li;  // aidel with sorted_list

    double time_bs = 0.0;
    double time_ai_db = 0.0;
    //double time_ai_sl = 0.0;
    double time_ai_lb = 0.0;
    double time_li = 0.0;

    int a=0, b=0, c=0, d=0, e=0;


    //========================check the correctness of aidel=====================
    printf("checking the correctness before insert: ");
    for (int i=0; i<n; i++){
        int key = keys[i];
        a = aidel.findInsert2(key);
        b = aidel.findInsert(key);
        c = aidel.findInsert_level(key);
        d = learnedindex.findInsert(key);
        if(a!=key || b!=key || c!=key || d!=key){
            printf("wrong!\n");
            printf("key[%d]: %d, a: %d, b: %d, c: %d, d: %d\n", i,key, a, b,c,d);
            exit(1);
        }
    }
    printf("correct!\n");



    //===========================search before insert==============================
    // btree
    printf("Running btree\n");
    rng.seed(seed);
    for (int t = 0; t < num_trials; t++) {
        for (int &query : queries) {
            query = keys[dist(rng)];
        }

        auto start = clock();
        for (const int& key : queries) {
            //int pos = btree[key];
            btree.find(key);
        }
        double elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        times_bs.push_back(elapsed);
    }

    // aidel using delta_buffer
    printf("Running aidel with delta_buffer\n");
    rng.seed(seed);
    for (int t = 0; t < num_trials; t++) {
        for (int &query : queries) {
            query = keys[dist(rng)];
        }

        auto start = clock();
        for (const int& key : queries) {
            int pos = aidel.findInsert2(key);
        }
        double elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        times_ai_db.push_back(elapsed);
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
        times_ai_lb.push_back(elapsed);
    }


    // learned index
    printf("Running learned index\n");
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


    time_bs = 1e+9 * std::accumulate(times_bs.begin(), times_bs.end(), 0.) / (num_trials * QUERIES_PER_TRIAL);
    time_ai_db = 1e+9 * std::accumulate(times_ai_db.begin(), times_ai_db.end(), 0.) / (num_trials * QUERIES_PER_TRIAL);
    time_ai_lb = 1e+9 * std::accumulate(times_ai_lb.begin(), times_ai_lb.end(), 0.) / (num_trials * QUERIES_PER_TRIAL);
    time_li = 1e+9 * std::accumulate(times_li.begin(), times_li.end(), 0.) / (num_trials * QUERIES_PER_TRIAL);
 

    printf("Mean time per query\n");
    printf("%8.1lf ns : %.40s\n", time_bs, "btree");
    printf("%8.1lf ns : %.40s\n", time_ai_db, "aidel with delta_buffer");
    printf("%8.1lf ns : %.40s\n", time_ai_lb, "aidel with level_bin");
    printf("%8.1lf ns : %.40s\n", time_li, "learnedindex");


    
    // =========================insert the data using deltabuffer===============================
    times_bs.clear();
    times_ai_db.clear();
    times_ai_lb.clear();
    times_li.clear();

    int insert_number = (n-1)*insert_x;

    // btree
    printf("Inserting btree\n");
    for (int t = 0; t < num_trials; t++) {
        auto start = clock();
        //for (const int& key : keys_ooo) {
        for(int i = 0; i<insert_number; i++) {
            int key = keys_ooo[i];
            btree.insert2(key, key);
        }
        double elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        times_bs.push_back(elapsed);
    }

    // aidel with delta_buffer
    printf("Inserting aidel delta_buffer\n");
    for (int t = 0; t < num_trials; t++) {
        auto start = clock();
        //for (const int& key : keys_ooo) {
        for(int i = 0; i<insert_number; i++) {
            int key = keys_ooo[i];
            aidel.insert2(key);
        }
        double elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        times_ai_db.push_back(elapsed);
    }


    // aidel with level_bin
    printf("Inserting aidel level_bin\n");
    for (int t = 0; t < num_trials; t++) {
        auto start = clock();
       //for (const int& key : keys_ooo) {
        for(int i = 0; i<insert_number; i++) {
            int key = keys_ooo[i];
            aidel.insert_level(key);
        }
        double elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        times_ai_lb.push_back(elapsed);
    }

    // learnedindex
    printf("Inserting learned index\n");
    for (int t = 0; t < num_trials; t++) {
        auto start = clock();
       //for (const int& key : keys_ooo) {
        for(int i = 0; i<insert_number; i++) {
            int key = keys_ooo[i];
            learnedindex.insert(key);
        }
        double elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        times_li.push_back(elapsed);
    }


    time_bs = 1e+9 * std::accumulate(times_bs.begin(), times_bs.end(), 0.) / (num_trials * insert_number);
    time_ai_db = 1e+9 * std::accumulate(times_ai_db.begin(), times_ai_db.end(), 0.) / (num_trials * insert_number);
    time_ai_lb = 1e+9 * std::accumulate(times_ai_lb.begin(), times_ai_lb.end(), 0.) / (num_trials * insert_number);
    time_li = 1e+9 * std::accumulate(times_li.begin(), times_li.end(), 0.) / (num_trials * insert_number);
   
    printf("Mean time per insert\n");
    printf("%8.1lf ns : %.40s\n", time_bs, "btree");
    printf("%8.1lf ns : %.40s\n", time_ai_db, "aidel with delta_buffer");
    printf("%8.1lf ns : %.40s\n", time_ai_lb, "aidel with level_bin");
    printf("%8.1lf ns : %.40s\n", time_li, "learnedindex");


    //===========================search after insert==============================
    times_bs.clear();
    times_ai_db.clear();
    times_ai_lb.clear();
    times_li.clear();

    // btree
    printf("Running btree\n");
    rng.seed(seed);
    for (int t = 0; t < num_trials; t++) {
        for (int &query : queries) {
            query = keys_total[dist_total(rng)];
        }

        auto start = clock();
        for (const int& key : queries) {
            //int pos = btree[key];
            btree.find(key);
        }
        double elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        times_bs.push_back(elapsed);
    }

    // aidel with delta_buffer
    printf("Running aidel delta_buffer\n");
    rng.seed(seed);
    for (int t = 0; t < num_trials; t++) {
        for (int &query : queries) {
            query = keys_total[dist_total(rng)];
        }

        auto start = clock();
        for (const int& key : queries) {
            int pos = aidel.findInsert2(key);
        }
        double elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        times_ai_db.push_back(elapsed);
    }


    // aidel with level_bin
    printf("Running aidel level_bin\n");
    rng.seed(seed);
    for (int t = 0; t < num_trials; t++) {
        for (int &query : queries) {
            query = keys_total[dist_total(rng)];
        }

        auto start = clock();
        for (const int& key : queries) {
            int pos = aidel.findInsert_level(key);
        }
        double elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        times_ai_lb.push_back(elapsed);
    }


    // learned index
    printf("Running learned index\n");
    rng.seed(seed);
    for (int t = 0; t < num_trials; t++) {
        for (int &query : queries) {
            query = keys_total[dist_total(rng)];
        }

        auto start = clock();
        for (const int& key : queries) {
            int pos = learnedindex.findInsert(key);
        }
        double elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        times_li.push_back(elapsed);
    }


    time_bs = 1e+9 * std::accumulate(times_bs.begin(), times_bs.end(), 0.) / (num_trials * QUERIES_PER_TRIAL);
    time_ai_db = 1e+9 * std::accumulate(times_ai_db.begin(), times_ai_db.end(), 0.) / (num_trials * QUERIES_PER_TRIAL);
    time_ai_lb = 1e+9 * std::accumulate(times_ai_lb.begin(), times_ai_lb.end(), 0.) / (num_trials * QUERIES_PER_TRIAL);
    time_li = 1e+9 * std::accumulate(times_li.begin(), times_li.end(), 0.) / (num_trials * QUERIES_PER_TRIAL);
   
    
    printf("Mean time per query\n");
    printf("%8.1lf ns : %.40s\n", time_bs, "btree");
    printf("%8.1lf ns : %.40s\n", time_ai_db, "aidel with delta_buffer");
    printf("%8.1lf ns : %.40s\n", time_ai_lb, "aidel with level_bin");
    printf("%8.1lf ns : %.40s\n", time_li, "learned index");


    //========================check the correctness of aidel after insert=============
    printf("check the correctness of aidel after insert: ");
    for (int i=0; i<total; i++){
        int key = keys_total[i];
        a = btree[key];
        b = aidel.findInsert2(key);
        d = aidel.findInsert_level(key);
        e = learnedindex.findInsert(key);
        if (a!=b || a!=d || a!=e) {
            printf("wrong!\n");
            printf("keys[%d]: %d, a: %d, b: %d, c: %d, d: %d, e: %d\n", i,key,a,b, c,d,e);
            exit(1);
        }
    }
    printf("correct!\n");

    printf(">>>> Note: aidel with sorted list can't hold as many as 100x data! <<<<\n");
    printf(">>>> Note: The results of sorted list is valid only if the insert data < 10x <<<<\n");

    return 0;
}