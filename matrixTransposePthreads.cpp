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
    int row;
	shared_ptr<vector<shared_ptr<vector<int32_t>>>> A;
};

void *diagonal(void *threadarg){
    struct thread_data *data;
    data = (struct thread_data *) threadarg;
    //cout << "i in function = " << data -> i << endl;
    auto k = data -> row;
    for(auto j = (k + 1); j < data -> A -> size(); j++){
		data -> A -> at(k) -> at(j) = data ->A -> at(k) -> at(j) + data ->A -> at(j) -> at(k);
		data ->A -> at(j) -> at(k) = data ->A -> at(k) -> at(j) - data ->A -> at(j) -> at(k);
		data ->A -> at(k) -> at(j) = data ->A -> at(k) -> at(j) - data ->A -> at(j) -> at(k);
	}
    //cout << "i after it is used = " << data -> i << endl;
}

void transpose(shared_ptr<vector<shared_ptr<vector<int32_t>>>> A){
    	int sizeMatrix = A -> size();
    	pthread_t threads[sizeMatrix];
    	int rc;
    	struct thread_data data[sizeMatrix];
    	void *status;
    	pthread_attr_t attr;
    	pthread_attr_init(&attr);
    	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    	for(auto i = 0; i < sizeMatrix; i++){
		data[i].A = A;
		data[i].row = i;
	}
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for(auto i = 0; i < A -> size(); i++){
        //cout << "i in loop = " << i << endl;
		rc = pthread_create(&threads[i], NULL, diagonal, (void*)&data[i]);
        //data.i = data.i + 1;
	}
    	pthread_attr_destroy(&attr);
    	for(auto i = 0; i < A -> size(); i++){
	    	rc = pthread_join(threads[i], &status);
    	}
    //cout << "completed transpose" << endl;
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

    std::cout << "The PThreads Diagonal operation took: " << time_span.count() << " seconds.";
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
    return A;
}

int main(){

    int sizes[5] = {128, 1024, 2048, 4096, 16384};
	auto count = 0;
	while(count < 5){
		auto n = sizes[count];
		cout << "Matrix size: " << n << endl; 
		auto A = Generate2DArray(n);

		transpose(A);
		cout << endl;
	    count++;
	}
    
    pthread_exit(NULL);
    return 0;
}
