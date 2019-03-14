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
    //displayMatrix(A);
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

void transposeBlock(shared_ptr<vector<shared_ptr<vector<int32_t>>>> A){
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	int bSize = 2;

	#pragma omp parallel for
	for(auto i = 0; i < A -> size(); i+=bSize){
		for(auto j = (i + bSize); j < A -> size(); j+=bSize){
			#pragma omp parallel for collapse(2)
			for(auto a = 0; a < bSize; a++){
				for(auto b = 0; b < bSize; b++){
					A -> at(i+a) -> at(j+b) = A -> at(i+a) -> at(j+b) + A -> at(j+a) -> at(i+b);
					A -> at(j+a) -> at(i+b) = A -> at(i+a) -> at(j+b) - A -> at(j+a) -> at(i+b);
					A -> at(i+a) -> at(j+b) = A -> at(i+a) -> at(j+b) - A -> at(j+a) -> at(i+b);
				}
			}
            //Transpose inner blocks after they have been moved
			#pragma omp parallel for
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

    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    std::cout << "The OpenMP Block threading operation took: " << time_span.count() << " seconds.";
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
		//displayMatrix(A);

		transposeDiagonal(A);
		cout<<endl;
		//displayMatrix(A);

		transposeBlock(A);
		cout<<endl;
		//displayMatrix(A);

		count++;
	}
    return 0;
}