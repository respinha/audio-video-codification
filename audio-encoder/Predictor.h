#include <cstdlib>
#include <string>



using namespace std;

class Predictor {
	public:
		Predictor(int m, string encodedFilename, string decodedFilename);
		void simple_predict(int* sequence, int length);		
		void predict(int* sequence, int length);

	private:
};
