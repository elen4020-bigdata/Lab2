#include <vector>
#include <ctime>
#include <random>
#include <chrono>
#include <memory>
#include <iostream>
#include <pthread.h>

using namespace std;

#define NUM_THREADS 4

struct thread_data{
	shared_ptr<vector<shared_ptr<vector<int32_t>>>> A;
};

void *transpose(void *threadarg){
	struct thread_data *data;
	data = (struct thread_data *) threadarg;
	//auto A = data -> A;
	for(auto i = 0; i < data ->A -> size(); i++){
		for(auto j = (i + 1); j < data -> A -> size(); j++){
			data -> A -> at(i) -> at(j) = data ->A -> at(i) -> at(j) + data ->A -> at(j) -> at(i);
			data ->A -> at(j) -> at(i) = data ->A -> at(i) -> at(j) - data ->A -> at(j) -> at(i);
			data ->A -> at(i) -> at(j) = data ->A -> at(i) -> at(j) - data ->A -> at(j) -> at(i);
		}
	}
    pthread_exit(NULL);
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

    
    for (auto i = 0; i < n ; i++){
        for(auto j = 0; j < n ; j++){
            cout << A->at(i)->at(j) << " ";
        }
        cout <<endl;
    }
    return A;
}

int main(){

    auto A = Generate2DArray(4);

    pthread_t threads[NUM_THREADS];
    int rc;
    struct thread_data data;
    void *status;
    pthread_attr_t attr;
    data.A = A;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(auto i = 0; i < NUM_THREADS; i++){
	    rc = pthread_create(&threads[i], NULL, transpose, (void*)&data);
    }
    //matrixTranspose(A);
    pthread_attr_destroy(&attr);
    for(auto i = 0; i < NUM_THREADS; i++){
	    rc = pthread_join(threads[i], &status);
    }
    cout << endl;
    for (auto i = 0; i < 4 ; i++){
        for(auto j = 0; j < 4 ; j++){
            cout << data.A->at(i)->at(j) << " ";
        }
        cout <<endl;
    }
    pthread_exit(NULL);
    
    return 0;
}
