#include <gsl/gsl_randist.h>
#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;

const int numberOfWords = 10000;
const int wordLenght = 10 ;
double **sPrediction;
double **s;
double **r;
int M;
double d;
int numberOfSymbolsErrorsOverall;
gsl_rng *pRNG = gsl_rng_alloc(gsl_rng_mt19937);

void message(){
    
    for (int j = 0; j < numberOfWords; j++){
        for (int i = 0; i < wordLenght; i++){
            s[j][i] = (2 * ( gsl_rng_uniform_int(pRNG,M) ) + 1 - M) * d;
//            cout << "s[j][i]" << s[j][i] << "\n";
        }
    }
}

void noize(double SNR){
	double sigma = sqrt(pow(10.0, -SNR/10.0) / (2.0 * log2(M) ));
//    cout << "sigma = " << sigma << "\n";
    for (int j = 0; j < numberOfWords; j++){
        for (int i = 0; i < wordLenght; i++){
            r[j][i] = s[j][i] + gsl_ran_gaussian(pRNG,sigma);
//            cout << "gsl_ran_gaussian(pRNG,sigma) = " << gsl_ran_gaussian(pRNG,sigma) << "\n";
//            cout << "r[j][i] = " << r[j][i] << "\n";
        }
	}
}

double errorProbabilityPerSymbol (){
    double *t = (double*)malloc((M+1) * sizeof(double));
    cout << "\nM = " <<M;
    for (int m = 0; m <= M ; m++) {
        t[m] = (2*m - M) * d;
//        t[m] = 2*m - M;
        cout << "\nt[m] = " << t[m];
    }
    for (int wordNumber = 0; wordNumber < numberOfWords; wordNumber++){
        for (int numberOfSymbolInWord=0; numberOfSymbolInWord < wordLenght ; numberOfSymbolInWord++){
//            cout << "\n\nr = " << r[wordNumber][numberOfSymbolInWord];
            if (r[wordNumber][numberOfSymbolInWord] >= t[M]) {
                sPrediction[wordNumber][numberOfSymbolInWord] = t[M] - d;
                break;
            }else{
                if (r[wordNumber][numberOfSymbolInWord] < t[0]) {
                    sPrediction[wordNumber][numberOfSymbolInWord] = t[0] + d;
                    break;
                }else{
                    for (int m = 0; m < M ; m++) {
                        if (r[wordNumber][numberOfSymbolInWord] >= t[m] && r[wordNumber][numberOfSymbolInWord] < t[m+1]) {
                            sPrediction[wordNumber][numberOfSymbolInWord] = t[m] + d;
//                            cout << "\nm = " << m;
//                            cout << "\nt1 = " << t[m];
//                            cout << "\nt2 = " << t[m+1];
                            break;
                        }
                    }
                }
            }
//            cout << "\nsPrediction = " << sPrediction[wordNumber][numberOfSymbolInWord];
            if (sPrediction[wordNumber][numberOfSymbolInWord]!=s[wordNumber][numberOfSymbolInWord] ) {
                numberOfSymbolsErrorsOverall += 1;
//                cout << "sPrediction = " << sPrediction[wordNumber][numberOfSymbolInWord] << "  s = " << s[wordNumber][numberOfSymbolInWord] << "\n";
            }

        }
    }
    return (double)numberOfSymbolsErrorsOverall/ (double)(numberOfWords*wordLenght);

}


int main(int argc, const char * argv[])
{
    ofstream outfile2 ("/Users/mike/Documents/Study/8semestr/Telecom/lab1/lab1/example.txt");
    
    sPrediction = (double **)malloc(numberOfWords * sizeof(double*));
    s = (double**)malloc(numberOfWords * sizeof(double*));
    r = (double**)malloc(numberOfWords * sizeof(double*));
    
    for (int wordNumber = 0; wordNumber < numberOfWords ; wordNumber++){
        sPrediction[wordNumber] = (double*)malloc(wordLenght*sizeof(double));
        s[wordNumber] = (double*)malloc(wordLenght*sizeof(double));
        r[wordNumber] = (double*)malloc(wordLenght*sizeof(double));
    }
    
    for (double SNR = -5; SNR <= 20; SNR += 0.5) {
            outfile2 << SNR;
        for (M=2; M<=16; M*=2) {
            d = sqrt(3 / (pow(M,2) - 1));
            cout << "d = " << d << "\n";
            numberOfSymbolsErrorsOverall = 0;
            
            message();
            noize(SNR);
            
//            cout << "numberOfSymbolsErrorsOverall = " << numberOfSymbolsErrorsOverall << "numberOfSymbols = " << (numberOfWords*wordLenght)<<"\n";
            outfile2 << " " << errorProbabilityPerSymbol();
        }
        outfile2 << "\n";
        cout << "\n\n" << SNR << "\n";
    }
    
    return 0;
}