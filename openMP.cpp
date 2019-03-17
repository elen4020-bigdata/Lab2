#include <vector>
#include <ctime>
#include <random>
#include <chrono>
#include <memory>
#include <iostream>

using namespace std;
using namespace std::chrono;

class DimensionsHaveToBeNonZeroPositiveIntegersException{};

void displayMatrix(shared_ptr<vector<shared_ptr<vector<int32_t>>>> A){
    for(auto i = 0; i < A->size(); i++){
		for(auto j = 0; j < A ->size(); j++){
			cout << A -> at(i) -> at(j) << " ";
		}
	cout << endl;
	}
}

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
        for(auto j = 0; j < n; j++){
            A->at(i)->push_back(distribution(generator));
        } 
    }
    return A;
}

void transposeNaive(shared_ptr<vector<shared_ptr<vector<int32_t>>>> A){
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
	#pragma omp parallel for
	for(auto i = 0; i < A -> size(); i++){
		#pragma omp parallel for
		for(auto j = (i + 1); j < A -> size(); j++){
			A -> at(i) -> at(j) = A -> at(i) -> at(j) + A -> at(j) -> at(i);
			A -> at(j) -> at(i) = A -> at(i) -> at(j) - A -> at(j) -> at(i);
			A -> at(i) -> at(j) = A -> at(i) -> at(j) - A -> at(j) -> at(i);
		}
	}
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

    std::cout << "The OpenMP Naive threading operation took: " << time_span.count() << " seconds.";
}

void transposeDiagonal(shared_ptr<vector<shared_ptr<vector<int32_t>>>> A){
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
	#pragma omp parallel for
	for(auto i = 0; i < A -> size(); i++){
		for(auto j = (i + 1); j < A -> size(); j++){
			A -> at(i) -> at(j) = A -> at(i) -> at(j) + A -> at(j) -> at(i);
			A -> at(j) -> at(i) = A -> at(i) -> at(j) - A -> at(j) -> at(i);
			A -> at(i) -> at(j) = A -> at(i) -> at(j) - A -> at(j) -> at(i);
		}
	}
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

    std::cout << "The OpenMP Diagonal threading operation took: " << time_span.count() << " seconds.";
}

#define NUM_SIZES 5
int main(){

	int sizes[NUM_SIZES] = {128, 1024, 2048, 4096, 16384};
	auto count = 0;
	while(count < NUM_SIZES){
		auto n = sizes[count];
		cout << "Matrix size: " << n << endl; 
		auto A = Generate2DArray(n);
		transposeNaive(A);
		cout << endl;
		transposeDiagonal(A);
		cout<<endl;
		count++;
	}
    return 0;
}