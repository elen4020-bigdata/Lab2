#include <vector>
#include <ctime>
#include <random>
#include <chrono>
#include <memory>
#include <iostream>
#include <pthread.h>

using namespace std;
using namespace std::chrono;

struct thread_data{
    int i;
    int j;
    int c;
    shared_ptr<vector<shared_ptr<vector<int32_t>>>> A;
};

void *PTransposeBlock(void *threadArgTranspose){
    struct thread_data *data = (struct thread_data *) threadArgTranspose;
    auto i = data -> i;
    auto j = data -> j;
    auto c = data -> c;
    auto A = data -> A;

    A -> at((j+1)*c-i*(c-1))->at((c-1)*(-j-1)+c*i) =  A -> at((j+1)*c-i*(c-1))->at((c-1)*(-j-1)+c*i) 
        + A->at(c*(j-1)+1-i*(c-1))->at(c*i+1-(c-1)*(j-1));

    A -> at(c*(j-1)+1-i*(c-1))->at(c*i+1-(c-1)*(j-1)) =  A -> at((j+1)*c-i*(c-1))->at((c-1)*(-j-1)+c*i) 
        - A->at(c*(j-1)+1-i*(c-1))->at(c*i+1-(c-1)*(j-1));

    A -> at((j+1)*c-i*(c-1))->at((c-1)*(-j-1)+c*i) =  A -> at((j+1)*c-i*(c-1))->at((c-1)*(-j-1)+c*i) 
        - A->at(c*(j-1)+1-i*(c-1))->at(c*i+1-(c-1)*(j-1));

}

void *PBlockSwap(void *threadargBlock){
    struct thread_data *data = (struct thread_data *) threadargBlock;
    auto i = data ->i;
    auto j = data -> j;
    auto A = data -> A;
    for(auto a = 0; a < 2; a++){
		for(auto b = 0; b < 2; b++){
			A -> at(i+a) -> at(j+b) = A -> at(i+a) -> at(j+b) + A -> at(j+a) -> at(i+b);
			A -> at(j+a) -> at(i+b) = A -> at(i+a) -> at(j+b) - A -> at(j+a) -> at(i+b);
			A -> at(i+a) -> at(j+b) = A -> at(i+a) -> at(j+b) - A -> at(j+a) -> at(i+b);
		}
	}

    pthread_t thread1, thread2;
    thread_data thread_data_1 = {i, j, 0, A};
    thread_data thread_data_2 = {i, j, 1, A};

    auto iret1 = pthread_create( &thread1, NULL, PTransposeBlock, (void*)&thread_data_1);
    auto iret2 = pthread_create( &thread2, NULL, PTransposeBlock, (void*)&thread_data_2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

}


void PBlockThreading(shared_ptr<vector<shared_ptr<vector<int32_t>>>> A){
    int looper = 0;
    int numthreads = 4;
    //while(looper < A->size()){
      //  numthreads += (A->size() - looper)/2;
        //looper+=2;
    //}
    pthread_t threads[numthreads];
    int rc;
    struct thread_data data[numthreads+1];
    void *status;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int threadCounter = 0;
    int dataCounter = 0;
    
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    for(auto i = 0; i < A -> size(); i+=2){
        //cout<<i<<endl;
		for(auto j = (i + 2); j < A -> size(); j+=2){
            //cout<<"TOp of loop"<<endl;
            data[dataCounter].A = A;
            data[dataCounter].i = i;
            data[dataCounter].j = j;
            rc = pthread_create(&threads[threadCounter], NULL, PBlockSwap, (void*)&data[dataCounter]);
            //cout<<threadCounter<<endl;
            if(threadCounter == numthreads - 1){
                for(auto c = 0; c < numthreads; c++){
		            rc = pthread_join(threads[c], &status);
                    threadCounter = -1;
                    dataCounter = -1;
                }
            }
            threadCounter++;
            dataCounter++;
        }
        //diagonals here
        data[dataCounter].A = A;
        data[dataCounter].i = i;
        data[dataCounter].j = i;
        data[dataCounter].c = 0;
        rc = pthread_create(&threads[threadCounter], NULL, PTransposeBlock, (void*)&data[dataCounter]);
        threadCounter++;
        dataCounter++;
        if(threadCounter == 4){
            for(auto c = 0; c < numthreads; c++){
		        rc = pthread_join(threads[c], &status);
                threadCounter = -1;
                dataCounter = -1;
            }
            threadCounter = 0;
            dataCounter = 0;
        }
    }
    pthread_attr_destroy(&attr);

    
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    std::cout << "The operation took: " << time_span.count() << " seconds.";

}

class DimensionsHaveToBeNonZeroPositiveIntegersException{};

shared_ptr<vector<shared_ptr<vector<int32_t>>>> Generate2DArray(int32_t n){
    if(n <= 0){
        throw DimensionsHaveToBeNonZeroPositiveIntegersException{};
    }


    default_random_engine generator(chrono::high_resolution_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> distribution(0,n);
    distribution(generator);

    auto A = make_shared<vector<shared_ptr<vector<int32_t>>>>();
    for(auto i = 0; i < n; i++){
        A->push_back(make_shared<vector<int32_t>>());
        for(auto j = 0; j < n; j++)
        {
            A->at(i)->push_back(distribution(generator));
        }
        
    }

    
    //for (auto i = 0; i < n ; i++){
    //    for(auto j = 0; j < n ; j++){
    //        cout << A->at(i)->at(j) << " ";
    //    }
    //    cout <<endl;
    //}
    return A;
}

#define NUM_SIZES 5
int main(){
	int sizes[NUM_SIZES] = {128, 1024, 2048, 4096, 16384};
	auto count = 0;
	while(count < NUM_SIZES){
		auto n = sizes[count];
		cout << "Matrix size: " << n << endl; 
		auto A = Generate2DArray(n);

		PBlockThreading(A);
		cout << endl;

		count++;
	}
    return 0;
}