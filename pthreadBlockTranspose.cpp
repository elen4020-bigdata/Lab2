#include <vector>
#include <ctime>
#include <random>
#include <chrono>
#include <memory>
#include <iostream>
#include <pthread.h>

using namespace std;


/*void blockThreading(shared_ptr<vector<shared_ptr<vector<int32_t>>>> A){
	
	int bSize = 2;

	for(auto i = 0; i < A -> size(); i+=bSize){
		for(auto j = (i + bSize); j < A -> size(); j+=bSize){
			for(auto a = 0; a < bSize; a++){
				for(auto b = 0; b < bSize; b++){
					A -> at(i+a) -> at(j+b) = A -> at(i+a) -> at(j+b) + A -> at(j+a) -> at(i+b);
					A -> at(j+a) -> at(i+b) = A -> at(i+a) -> at(j+b) - A -> at(j+a) -> at(i+b);
					A -> at(i+a) -> at(j+b) = A -> at(i+a) -> at(j+b) - A -> at(j+a) -> at(i+b);
				}
			}
            //Transpose inner blocks after they have been moved
            for(auto c = 0; c < bSize; c++){
                A -> at((j+1)*c-i*(c-1))->at((c-1)*(-j-1)+c*i) =  A -> at((j+1)*c-i*(c-1))->at((c-1)*(-j-1)+c*i) 
                + A->at(c*(j-1)+1-i*(c-1))->at(c*i+1-(c-1)*(j-1));

                A -> at(c*(j-1)+1-i*(c-1))->at(c*i+1-(c-1)*(j-1)) =  A -> at((j+1)*c-i*(c-1))->at((c-1)*(-j-1)+c*i) 
                - A->at(c*(j-1)+1-i*(c-1))->at(c*i+1-(c-1)*(j-1));

                A -> at((j+1)*c-i*(c-1))->at((c-1)*(-j-1)+c*i) =  A -> at((j+1)*c-i*(c-1))->at((c-1)*(-j-1)+c*i) 
                - A->at(c*(j-1)+1-i*(c-1))->at(c*i+1-(c-1)*(j-1));
            }
		}
        //Transpose the diagonal
        A -> at(i)->at(i+1) = A -> at(i)->at(i+1) + A -> at(i+1)->at(i);
        A -> at(i+1)->at(i) = A -> at(i)->at(i+1) - A -> at(i+1)->at(i);
        A -> at(i)->at(i+1) = A -> at(i)->at(i+1) - A -> at(i+1)->at(i);
	 }
}


void transpose(shared_ptr<vector<shared_ptr<vector<int32_t>>>> A) {
	for(auto i = 0; i < A -> size(); i++){
		for(auto j = (i + 1); j < A -> size(); j++){
			A -> at(i) -> at(j) = A -> at(i) -> at(j) + A -> at(j) -> at(i);
			A -> at(j) -> at(i) = A -> at(i) -> at(j) - A -> at(j) -> at(i);
			A -> at(i) -> at(j) = A -> at(i) -> at(j) - A -> at(j) -> at(i);
		}
	}
}*/


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
    auto A = data->A;
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
    printf("Created Transpose Thread\n");
    auto iret2 = pthread_create( &thread2, NULL, PTransposeBlock, (void*)&thread_data_2);
    printf("Created Transpose Thread\n");

    pthread_join(thread1, NULL);
    printf("Joined Transpose Thread\n");
    pthread_join(thread2, NULL);
    printf("Joined Transpose Thread\n");

}


void PBlockThreading(shared_ptr<vector<shared_ptr<vector<int32_t>>>> A){
    int looper = 0;
    int numthreads = 0;
    while(looper < A->size()){
        numthreads += (A->size() - looper)/2;
        looper+=2;
    }

    pthread_t threads[numthreads];
    int rc;
    struct thread_data data[numthreads];
    void *status;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int threadCounter = 0;

    for(auto i = 0; i < A -> size(); i+=2){
		for(auto j = (i + 2); j < A -> size(); j+=2){
            data[threadCounter].A = A;
            data[threadCounter].i = i;
            data[threadCounter].j = j;
            rc = pthread_create(&threads[threadCounter], NULL, PBlockSwap, (void*)&data[threadCounter]);
            printf("Created Swap Thread\n");
            threadCounter++;
        }
        //diagonals here
        data[threadCounter].A = A;
        data[threadCounter].i = i;
        data[threadCounter].j = i;
        data[threadCounter].c = 0;
        rc = pthread_create(&threads[threadCounter], NULL, PTransposeBlock, (void*)&data[threadCounter]);
        printf("Created Diagonal Thread\n");
        threadCounter++;
    }

    pthread_attr_destroy(&attr);
    for(auto i = 0; i < numthreads; i++){
		rc = pthread_join(threads[i], &status);
        printf("Joined Swap/Diagonal Thread\n");
    }
    


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
	auto n = 32;
    auto A = Generate2DArray(n);
	//blockThreading(A);
    /*transpose(A);*/ 

    PBlockThreading(A);

	cout<<endl;

	for (auto i = 0; i < n ; i++){
        for(auto j = 0; j < n ; j++){
            cout << A->at(i)->at(j) << " ";
        }
        cout <<endl;
    }

    return 0;
}