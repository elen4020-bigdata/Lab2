#include <iostream>
#include <array>
#include <ctime>
#include <random>
#include <chrono>
#include <memory>

using namespace std;

class DimensionsHaveToBeNonZeroPositiveIntegersException{};

template<class T, int n>
void Generate2DArray(array<array<T, n>,n>* A){
    if(n <= 0){
        throw DimensionsHaveToBeNonZeroPositiveIntegersException{};
    }


    default_random_engine generator(chrono::high_resolution_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> distribution(0,n);
    distribution(generator);

    //auto A = make_shared<vector<shared_ptr<vector<int32_t>>>>();

    for(auto i = 0; i < n; i++){
        for(auto j = 0; j < n; j++)
        {
            A->at(i).at(j) = distribution(generator);
        }
        
    }

    
    for (auto i = 0; i < n ; i++){
        for(auto j = 0; j < n ; j++){
            cout << A->at(i).at(j) << " ";
        }
        cout <<endl;
    }
}

template<class T, int n, int blockSize>
void blockTranspose(array<array<T, n>,n>* A){
    for(auto i = 0; i < n; i=i+blockSize){
        for(auto l = i; l < n; l=l+blockSize){
            for(auto j = 0; j < (blockSize); j++){
                for(auto k = 0; k < (blockSize); k++){
                    auto temp = A->at(i+j).at(l+k);
                    A->at(i+j).at(l+k) = A->at(l+k).at(i+j);
                    A->at(l+k).at(i+j) = temp;
                    if(i == l){
                        break;
                    }
                }
            }
        }
    }
}


int main(){
    auto A = new array<array<int32_t, 4>,4>;
    Generate2DArray<int32_t,4>(A);
    blockTranspose<int32_t, 4, 2>(A);
    cout << endl;
    for (auto i = 0; i < 4 ; i++){
        for(auto j = 0; j < 4 ; j++){
            cout << A->at(i).at(j) << " ";
        }
        cout <<endl;
    }
    delete A;
    return 0;
}