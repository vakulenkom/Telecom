#include <gsl/gsl_randist.h>
#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;

const int numberOfWords = 10000;
const int wordLenght = 100;
int **s_ver;
int **s;
double **r;
int numberOfSymbolsErrorsOverall;

void message(int **s,int i,int j, gsl_rng *pRNG){
    
	for (int count1 = 0; count1 < j; count1++){
        for (int count = 0; count < i; count++){
            s[count][count1] = gsl_rng_uniform_int(pRNG,2);
            if (s[count][count1] == 0) {
                s[count][count1] = -1;
            }
        }
        
	}
}

void noize(double **r, int **s, double SNR, gsl_rng *pRNG,int i, int j){
	double sigma = sqrt(pow(10,-SNR/10)/2);
	for (int count1 = 0; count1 <j; count1++){
        for (int count = 0; count < i; count++){
            r[count][count1] = (double)s[count][count1] + gsl_ran_gaussian(pRNG,sigma);
        }
	}
}

double errorProbabilityPerSymbol (){
    for (int numberOfSymbolInWord=0; numberOfSymbolInWord < wordLenght ; numberOfSymbolInWord++){
        for (int wordNumber = 0; wordNumber < numberOfWords; wordNumber++){
            
            if (r[wordNumber][numberOfSymbolInWord] >= 0)
                s_ver[wordNumber][numberOfSymbolInWord] = 1;
            else
                s_ver[wordNumber][numberOfSymbolInWord] = -1;
            
            if (s_ver[wordNumber][numberOfSymbolInWord]!=s[wordNumber][numberOfSymbolInWord]) {
                numberOfSymbolsErrorsOverall += 1;
            }
        }
    }
    return (double)numberOfSymbolsErrorsOverall/ (double)(numberOfWords*wordLenght);

}


int main(int argc, const char * argv[])
{
    ofstream outfile2 ("/Users/mike/Documents/Study/8semestr/telekom/lab1/lab1/example.txt");
    
    for (double SNR = -5; SNR <= 20; SNR += 0.2) {
        int numberOfSymbolsErrorsOverall = 0;
        
        s_ver = (int**)malloc(numberOfWords*sizeof(int*));
        s = (int**)malloc(numberOfWords*sizeof(int*));
        r = (double**)malloc(numberOfWords*sizeof(double*));
        
        for (int masindex = 0; masindex <numberOfWords;masindex++){
            s_ver[masindex] = (int*)malloc(wordLenght*sizeof(int));
            s[masindex] = (int*)malloc(wordLenght*sizeof(int));
            r[masindex] = (double*)malloc(wordLenght*sizeof(double));
        }
        gsl_rng *pRNG = gsl_rng_alloc(gsl_rng_mt19937);
        
        message(s,numberOfWords,wordLenght,pRNG);
        noize(r,s,SNR,pRNG,numberOfWords,wordLenght);
        errorProbabilityPerSymbol();
                
        outfile2 << SNR << " " << errorProbabilityPerSymbol() << "\n";
        
        cout << SNR << "\n";
    }
    
    return 0;
}