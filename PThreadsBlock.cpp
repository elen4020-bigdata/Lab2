#include <iostream>
#include <array>
#include <ctime>
#include <random>
#include <chrono>
#include <memory>

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

    for(auto i = 0; i < n; i++){
        for(auto j = 0; j < n; j++)
        {
            A->at(i).at(j) = distribution(generator);
        }
        
    }
}

template<class T, int n>
struct thread_data{
    array<array<T, n>,n>* A;
    int i;
    int l;
    int blockSize;
};

template<class T, int n>
struct newData{
    array<array<T, n>,n>* A;
    int i;
    int l;
    int j;
    int k;
    bool swap;
};

template<class T, int n>
void *Transpose(void *args){
    struct thread_data<T,n> *data;
    data = (struct thread_data<T,n> *) args;
    auto i = data -> i;
    auto l = data -> l;
    auto A = data -> A;
    auto blockSize = data -> blockSize;
    for(auto j = 0; j < (blockSize); j++){
        for(auto k = 0; k < (blockSize); k++){
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

template<class T, int n, int blockSize>
void blockTranspose(array<array<T, n>,n>* A){
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    int numThreads = ((n/blockSize)*(n/blockSize)/2)+((n/blockSize)/2);
    pthread_t threads[numThreads];
    struct thread_data<T, n> data[numThreads];
    void *status;
  	pthread_attr_t attr;
	pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for(auto i = 0; i < numThreads; i++){
        data[i].A = A;
        data[i].blockSize = blockSize;
    }
    auto iter = 0;
    for(auto i = 0; i < n; i=i+blockSize){
        for(auto l = i; l < n; l=l+blockSize){
            data[iter].i = i;
            data[iter].l = l;
            pthread_create(&threads[iter], NULL, Transpose<T,n>, (void*)&data[iter]);
            iter++;
        }
    }
    pthread_attr_destroy(&attr);
    for(auto i = 0; i < numThreads; i++){
    	pthread_join(threads[i], &status);
    }
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    std::cout << "The PThreads Block operation took: " << time_span.count() << " seconds.";
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

    return 0;
}