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
    int blockSize;
    int j;
    int k;
    bool swap;
};

template<class T, int n>
void *swap(void *args){
    struct newData<T,n> *data;
    data = (struct newData<T,n> *) args;
    auto i = data -> i;
    auto l = data -> l;
    auto j = data -> j;
    auto k = data -> k;
    auto A = data -> A;
    auto swap = data -> swap;
    auto blockSize = data -> blockSize;
    if((i == l && swap) || (i != l)){
        auto temp = A->at(i+j).at(l+k);
        A->at(i+j).at(l+k) = A->at(l+k).at(i+j);
        A->at(l+k).at(i+j) = temp;
    }
}

template<class T, int n>
void *Transpose(void *args){
    struct thread_data<T,n> *data;
    data = (struct thread_data<T,n> *) args;
    auto i = data -> i;
    auto l = data -> l;
    auto A = data -> A;
    auto blockSize = data -> blockSize;
    pthread_t threads[blockSize];
    struct newData<T,n> newData[blockSize];
    void *status;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for(auto q = 0; q < blockSize; q++){
		newData[q].A = A;
		newData[q].i = i;
        newData[q].l = l;
	}
    auto iter = 0;
    for(auto j = 0; j < (blockSize); j++){
        for(auto k = 0; k < (blockSize); k++){
            newData[iter].j = j;
            newData[iter].k = k;
            newData[iter].swap = true;
            if(i == l && (iter%2 == 0)){
                newData[iter].swap = false;
                cout << "swapping" << endl;
            }
            pthread_create(&threads[iter],NULL, swap<T,n>, (void*)&newData[iter]);
            iter++;
        }
    }
    pthread_attr_destroy(&attr);
    for(auto i = 0; i < blockSize; i++){
	    pthread_join(threads[i], &status);
    }
}

template<class T, int n, int blockSize>
void blockTranspose(array<array<T, n>,n>* A){
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