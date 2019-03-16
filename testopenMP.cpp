#include <iostream>
#include <array>
#include <ctime>
#include <random>
#include <chrono>

using namespace std;
using namespace std::chrono;

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

    
    // for (auto i = 0; i < n ; i++){
    //     for(auto j = 0; j < n ; j++){
    //         cout << A->at(i).at(j) << " ";
    //     }
    //     cout <<endl;
    // }
}

template<class T, int n, int blockSize>
void blockTranspose(array<array<T, n>,n>* A){
    cout << "starting" << endl;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    #pragma omp parallel for
    for(auto i = 0; i < n; i=i+blockSize){
        for(auto l = i; l < n; l=l+blockSize){
            #pragma omp parallel for
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
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

    std::cout << "The OpenMP Block threading operation took: " << time_span.count() << " seconds.";
}

#define BLOCK_S 32
int main(){

	cout << "Matrix size: " << 128 << endl; 
    auto A = new array<array<int32_t, 128>, 128>;
    Generate2DArray<int32_t,128>(A);
    blockTranspose<int32_t, 128, BLOCK_S>(A);
    cout << endl;
    delete A;
    
	cout << "Matrix size: " << 1024 << endl; 
    auto B = new array<array<int32_t, 1024>, 1024>;
    Generate2DArray<int32_t,1024>(B);
    blockTranspose<int32_t, 1024, BLOCK_S>(B);
    cout << endl;
    delete B;

	cout << "Matrix size: " << 2048 << endl; 
    auto C = new array<array<int32_t, 2048>, 2048>;
    Generate2DArray<int32_t, 2048>(C);
    blockTranspose<int32_t, 2048, BLOCK_S>(C);
    cout << endl;
    delete C;

	cout << "Matrix size: " << 4096 << endl; 
    auto D = new array<array<int32_t, 4096>, 4096>;
    Generate2DArray<int32_t,4096>(D);
    blockTranspose<int32_t, 4096, BLOCK_S>(D);
    cout << endl;
    delete D;

	cout << "Matrix size: " << 16384 << endl; 
    auto E = new array<array<int32_t, 16384>, 16384>;
    Generate2DArray<int32_t, 16384>(E);
    blockTranspose<int32_t, 16384, BLOCK_S>(E);
    cout << endl;
    delete E;

    return 0;
}