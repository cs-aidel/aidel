//
// Generates 190M values drawn from a
//

#include <iostream>
#include <random>
#include <algorithm>
#include <fstream>
#include <climits>
#include <set>
#include <vector>

#include "readdata.h"

void ooo_lognormal() {
    std::vector<int> keys_total = read_data("./data/lognormal.sorted.190M");
    int total = (int) keys_total.size();
    printf("num elements: %d\n", total);

    //====================select step is 10=============================
    /*std::vector<int> keys;
    int selectStep = 10;
    for (int i = 0;i<total; i+=selectStep){
        keys.push_back(keys_total[i]);
    }
    int n = keys.size();
    cout << "select data: " << n << endl;

    FILE *fout = fopen("./data/lognormal.select10.19M", "wb");
    for (int x : keys) {
        fwrite(&x, sizeof(x), 1, fout);
    }
    fclose(fout);*/


    //====================select step is 100=============================
    /*std::vector<int> keys;
    int selectStep = 100;
    for (int i = 0;i<total; i+=selectStep){
        keys.push_back(keys_total[i]);
    }
    int n = keys.size();
    cout << "select data : " << n << endl;

    FILE *fout = fopen("./data/lognormal.select100.1.9M", "wb");
    for (int x : keys) {
        fwrite(&x, sizeof(x), 1, fout);
    }
    fclose(fout);*/

    //====================random shuffle=============================
    printf("before shuffle\n");
    for(int i=0;i<10;i++) printf("%d ", keys_total[i]);
    printf("\n");
    
    std::random_shuffle(keys_total.begin(), keys_total.end());
    int n = keys_total.size();
    cout << "random data : " << n << endl;
    printf("after shuffle\n");
    for(int i=0;i<10;i++) printf("%d ", keys_total[i]);
    printf("\n");

    FILE *fout = fopen("./data/lognormal.ooo.190M", "wb");
    for (int x : keys_total) {
        fwrite(&x, sizeof(x), 1, fout);
    }
    fclose(fout);
}

void select_10_lognormal() {
    std::vector<int> keys_total = read_data("./data/lognormal.sorted.190M");
    int total = (int) keys_total.size();
    printf("num elements: %d\n", total);

    //====================select step is 10=============================
    std::vector<int> keys;
    int selectStep = 10;
    for (int i = 0;i<total; i+=selectStep){
        keys.push_back(keys_total[i]);
    }
    int n = keys.size();
    cout << "select data: " << n << endl;

    FILE *fout = fopen("./data/lognormal.select10.19M", "wb");
    for (int x : keys) {
        fwrite(&x, sizeof(x), 1, fout);
    }
    fclose(fout);

}

void select_100_lognormal() {
    std::vector<int> keys_total = read_data("./data/lognormal.sorted.190M");
    int total = (int) keys_total.size();
    printf("num elements: %d\n", total);

    //====================select step is 100=============================
    std::vector<int> keys;
    int selectStep = 100;
    for (int i = 0;i<total; i+=selectStep){
        keys.push_back(keys_total[i]);
    }
    int n = keys.size();
    cout << "select data : " << n << endl;

    FILE *fout = fopen("./data/lognormal.select100.1.9M", "wb");
    for (int x : keys) {
        fwrite(&x, sizeof(x), 1, fout);
    }
    fclose(fout);

}



int main()
{

    ooo_lognormal();
    select_10_lognormal();
    select_100_lognormal();

    return 0;
}
