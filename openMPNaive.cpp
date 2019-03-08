#include <vector>
#include <ctime>
#include <random>
#include <chrono>
#include <memory>
#include <iostream>

using namespace std;


void transpose(shared_ptr<vector<shared_ptr<vector<int32_t>>>> A){
	#pragma omp parallel for
	for(auto i = 0; i < A -> size(); i++){
		#pragma omp parallel for
		for(auto j = (i + 1); j < A -> size(); j++){
			A -> at(i) -> at(j) = A -> at(i) -> at(j) + A -> at(j) -> at(i);
			A -> at(j) -> at(i) = A -> at(i) -> at(j) - A -> at(j) -> at(i);
			A -> at(i) -> at(j) = A -> at(i) -> at(j) - A -> at(j) -> at(i);
		}
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

    	auto A = Generate2DArray(10);
    	transpose(A);

	cout << endl;

	for(auto i = 0; i < A->size(); i++){
		for(auto j = 0; j < A ->size(); j++){
			cout << A -> at(i) -> at(j) << " ";
		}
		cout << endl;
	}

    	return 0;
}
