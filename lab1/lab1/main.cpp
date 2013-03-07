#include <gsl/gsl_randist.h>
#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;

const int numberOfWords = 5000;
const int wordLenght = 50;
int ***sPrediction;
int ***s;
double ***r;
int numberOfBits;
int numberOfSymbolsErrorsOverall;
gsl_rng *pRNG = gsl_rng_alloc(gsl_rng_mt19937);

void message(){
    
	for (int i = 0; i < wordLenght; i++){
        for (int j = 0; j < numberOfWords; j++){
            for (int c = 0; c < numberOfBits; c++) {
                s[j][i][c] = gsl_rng_uniform_int(pRNG,numberOfBits*2 +1);
                if (s[j][i][c] == 0) {
                    s[j][i][c] = -1;
                }
            }
            
        }
        
	}
}

void noize(double SNR){
	double sigma = sqrt(pow(10, -SNR/10) / (2 * log(numberOfBits) ));
	for (int i = 0; i < wordLenght; i++){
        for (int j = 0; j < numberOfWords; j++){
            for (int c = 0; c < numberOfBits; c++) {
                r[j][i][c] = (double)s[j][i][c] + gsl_ran_gaussian(pRNG,sigma);
            }
        }
	}
}

double errorProbabilityPerSymbol (){
    for (int numberOfSymbolInWord=0; numberOfSymbolInWord < wordLenght ; numberOfSymbolInWord++){
        for (int wordNumber = 0; wordNumber < numberOfWords; wordNumber++){
            for (int bitPosition = 0; bitPosition < numberOfBits;  bitPosition++) {
                if (r[wordNumber][numberOfSymbolInWord][bitPosition] >= 0)
                    sPrediction[wordNumber][numberOfSymbolInWord][bitPosition] = 1;
                else
                    sPrediction[wordNumber][numberOfSymbolInWord][bitPosition] = -1;
                
                if (sPrediction[wordNumber][numberOfSymbolInWord][bitPosition]!=s[wordNumber][numberOfSymbolInWord][bitPosition] ) {
                    numberOfSymbolsErrorsOverall += 1;
                    break;
                }
            }
        }
    }
    return (double)numberOfSymbolsErrorsOverall/ (double)(numberOfWords*wordLenght);

}


int main(int argc, const char * argv[])
{
    ofstream outfile2 ("/Users/mike/Documents/Study/8semestr/Telecom/lab1/lab1/example.txt");
    
    for (double SNR = -5; SNR <= 20; SNR += 0.1) {
        outfile2 << SNR;
        for (numberOfBits = 1; numberOfBits <= 4; numberOfBits++) {
            
            numberOfSymbolsErrorsOverall = 0;
            
            sPrediction = (int***)malloc(numberOfWords * wordLenght * sizeof(int*));
            s = (int***)malloc(numberOfWords * wordLenght * sizeof(int*));
            r = (double***)malloc(numberOfWords * wordLenght * sizeof(double*));
            
            for (int wordNumber = 0; wordNumber < numberOfWords ; wordNumber++){
                sPrediction[wordNumber] = (int**)malloc(numberOfWords * wordLenght*sizeof(int));
                s[wordNumber] = (int**)malloc(numberOfWords * wordLenght*sizeof(int));
                r[wordNumber] = (double**)malloc(numberOfWords * wordLenght*sizeof(double));
                for (int numberOfSymbolInWord = 0; numberOfSymbolInWord < wordLenght ; numberOfSymbolInWord++){
                    sPrediction[wordNumber][numberOfSymbolInWord] = (int*)malloc(numberOfBits*sizeof(int));
                    s[wordNumber][numberOfSymbolInWord] = (int*)malloc(numberOfBits*sizeof(int));
                    r[wordNumber][numberOfSymbolInWord] = (double*)malloc(numberOfBits*sizeof(double));
                }
            }
            
            message();
            noize(SNR);
            errorProbabilityPerSymbol();
                    
            outfile2 << " " << errorProbabilityPerSymbol();
        }
        outfile2 << "\n";
        cout << SNR << "\n";
    }
    
    return 0;
}