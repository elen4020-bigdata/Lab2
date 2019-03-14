#include <vector>
#include <ctime>
#include <random>
#include <chrono>
#include <memory>
#include <iostream>

using namespace std;
using namespace std::chrono;

void blockThreading(shared_ptr<vector<shared_ptr<vector<int32_t>>>> A){
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
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
     high_resolution_clock::time_point t2 = high_resolution_clock::now();
     duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
     std::cout << "The operation took: " << time_span.count() << " seconds.";

}

void transpose(shared_ptr<vector<shared_ptr<vector<int32_t>>>> A) {
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for(auto i = 0; i < A -> size(); i++){
		for(auto j = (i + 1); j < A -> size(); j++){
			A -> at(i) -> at(j) = A -> at(i) -> at(j) + A -> at(j) -> at(i);
			A -> at(j) -> at(i) = A -> at(i) -> at(j) - A -> at(j) -> at(i);
			A -> at(i) -> at(j) = A -> at(i) -> at(j) - A -> at(j) -> at(i);
		}
	}
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    std::cout << "The Non-threaded Naive operation took: " << time_span.count() << " seconds.";
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
            //cout << A->at(i)->at(j) << " ";
        }
        //cout <<endl;
    }
    return A;
}

int main(){
	int sizes[4] = {128, 1024, 2048, 4096};
	auto count = 0;
	while(count < 4){
		auto n = sizes[count];
		cout << "Matrix size: " << n << endl; 
		auto A = Generate2DArray(n);

		transpose(A);
		cout << endl;
	    count++;
	}
    return 0;
}