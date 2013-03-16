#include <gsl/gsl_randist.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <complex>

using namespace std;

const int numberOfWords = 1000;
const int wordLenght = 100;
bool bit = false;

complex<double> **sPrediction;
complex<double> **s;
complex<double> **r;
int M;
int signalLevelPrediction;
int numberOfSymbolsErrorsOverall;
gsl_rng *pRNG = gsl_rng_alloc(gsl_rng_mt19937);

int **sourceMLevel;

unsigned short binaryToGray(unsigned short num)
{
    return (num>>1) ^ num;
}

unsigned short grayToBinary(unsigned short gray)
{
    unsigned short result = gray & 64;
    result |= (gray ^ (result >> 1)) & 32;
    result |= (gray ^ (result >> 1)) & 16;
    result |= (gray ^ (result >> 1)) & 8;
    result |= (gray ^ (result >> 1)) & 4;
    result |= (gray ^ (result >> 1)) & 2;
    result |= (gray ^ (result >> 1)) & 1;
    return result;
}

int NumOf1Bits(unsigned ch)
{
    int r = 1;
    int summ = 0; // сумма битов
    for(int i=0;i<log2(M); i++)
    {
        if((ch & r)  > 0)
        {
            summ++;
        }
        r <<= 1;
    }
    return summ;
}

void message(){
    double m;
    unsigned binaryVector;
    for (int j = 0; j < numberOfWords; j++){
        for (int i = 0; i < wordLenght; i++){
            if (bit) {
                binaryVector = 0;
                for (int k=0; k<log2(M); k++) {
                    binaryVector += (unsigned) gsl_rng_uniform_int(pRNG,2);
                    binaryVector <<= 1;
                }
                binaryVector >>= 1;
//                cout << "  binaryVector = " << binaryVector << "\n" ;
                m = grayToBinary(binaryVector);
//                cout << "  m = " << m << "\n" ;
            }
            else{
                
                m = gsl_rng_uniform_int(pRNG,M);
            }
//            s[j][i] (cos(2 * M_PI * i/ (double)M), sin(2 * M_PI * i/ (double)M));
            s[j][i].real(cos(2 * M_PI * m/ (double)M));
            s[j][i].imag(sin(2 * M_PI * m/ (double)M));
            sourceMLevel[j][i] = m;
//            cout << "s = " << s[j][i] << "\n";
        }
    }
}

void noize(double SNR){
	double sigma = sqrt(pow(10.0, -SNR/10.0) / (2.0 * log2(M) ));
//    cout << "sigma = " << sigma << "\n";
    for (int j = 0; j < numberOfWords; j++){
        for (int i = 0; i < wordLenght; i++){
            r[j][i].real(s[j][i].real() + gsl_ran_gaussian(pRNG,sigma));
            r[j][i].imag(s[j][i].imag() + gsl_ran_gaussian(pRNG,sigma));
            
//            cout << "gsl_ran_gaussian(pRNG,sigma) = " << gsl_ran_gaussian(pRNG,sigma) << "\n";
//            cout << "r[j][i] = " << r[j][i] << "\n";
        }
	}
}

double errorProbabilityPerSymbol (){
    for (int wordNumber = 0; wordNumber < numberOfWords; wordNumber++){
        for (int numberOfSymbolInWord=0; numberOfSymbolInWord < wordLenght ; numberOfSymbolInWord++){
            //            cout << "\n\nr = " << r[wordNumber][numberOfSymbolInWord];
//            double arctgR = atan(r[wordNumber][numberOfSymbolInWord].real() / r[wordNumber][numberOfSymbolInWord].imag());
            double minDist = 100;
            for (int i = 0; i < M; i++) {
                complex<double> a (cos(2 * M_PI * i/ (double)M), sin(2 * M_PI * i/ (double)M));
                double dist = pow (r[wordNumber][numberOfSymbolInWord].real() - a.real(),2) + pow (r[wordNumber][numberOfSymbolInWord].imag() - a.imag(),2);
                if (dist < minDist){
                    minDist = dist;
                    sPrediction[wordNumber][numberOfSymbolInWord] = a;
                    signalLevelPrediction = i;
                }
            }
            if (bit) {
//                int mSource = acos( (s[wordNumber][numberOfSymbolInWord].real() * (double)M) / 2*M_PI);
//    //            cout << "\na = " << a;
//                cout << "\nsignalLevelPrediction = " << signalLevelPrediction << " mSource = " << mSource;
                unsigned XOR = binaryToGray(signalLevelPrediction) ^ binaryToGray(sourceMLevel[wordNumber][numberOfSymbolInWord]);
                if (XOR > 0){
//                    cout << "\nXOR = " << XOR;
//                    cout << "sPrediction = " << sPrediction[wordNumber][numberOfSymbolInWord] << "  s = " << s[wordNumber][numberOfSymbolInWord] << "\n";
//                    
//                    cout << "\nsignalLevelPrediction = " << signalLevelPrediction << "  mSource = " <<  mSource;
//
    //                cout << "\nb = " << b;
                    numberOfSymbolsErrorsOverall += NumOf1Bits(XOR);
//                    cout <<  "\nnumOfBits = " << NumOf1Bits(XOR);
//                    numberOfSymbolsErrorsOverall += 1;
                }
            }else{
//                cout << "\nsPrediction = " << sPrediction[wordNumber][numberOfSymbolInWord];
                if (sPrediction[wordNumber][numberOfSymbolInWord]!=s[wordNumber][numberOfSymbolInWord] ) {
//                if (abs(sPrediction[wordNumber][numberOfSymbolInWord] - s[wordNumber][numberOfSymbolInWord] ) < 0.000001) {
                    numberOfSymbolsErrorsOverall += 1;
//                    cout << "sPrediction = " << sPrediction[wordNumber][numberOfSymbolInWord] << "  s = " << s[wordNumber][numberOfSymbolInWord] << "\n";
                }else{
//                    cout << "sPrediction = " << sPrediction[wordNumber][numberOfSymbolInWord] << "  s = " << s[wordNumber][numberOfSymbolInWord] << "\n";
                }
            }

        }
    }
    if (bit) {
        return (double)numberOfSymbolsErrorsOverall/ (double)(log2(M)*numberOfWords*wordLenght);
    }else{
        return (double)numberOfSymbolsErrorsOverall/ (double)(numberOfWords*wordLenght);
    }
}


int main(int argc, const char * argv[])
{
    ofstream outfile2 ("/Users/mike/Documents/Study/8semestr/Telecom/lab2/lab1/example.txt");
    
    sPrediction = (complex<double> **)malloc(numberOfWords * sizeof(complex<double>*));
    s = (complex<double>**)malloc(numberOfWords * sizeof(complex<double>*));
    r = (complex<double>**)malloc(numberOfWords * sizeof(complex<double>*));
    sourceMLevel = (int**)malloc(numberOfWords * sizeof(int*));
    
//    sPrediction = (double **)malloc(numberOfWords * wordLenght * sizeof(double));
//    s = (double**)malloc(numberOfWords * wordLenght * sizeof(double));
//    r = (double**)malloc(numberOfWords * wordLenght * sizeof(double));
    
    for (int wordNumber = 0; wordNumber < numberOfWords ; wordNumber++){
        sPrediction[wordNumber] = (complex<double>*)malloc(wordLenght*sizeof(complex<double>));
        s[wordNumber] = (complex<double>*)malloc(wordLenght*sizeof(complex<double>));
        r[wordNumber] = (complex<double>*)malloc(wordLenght*sizeof(complex<double>));
        sourceMLevel[wordNumber] = (int*)malloc(wordLenght*sizeof(int));
    }
    
    
    
    for (double SNR = -5; SNR <= 20; SNR += 0.5) {
            outfile2 << SNR;
        for (M=2; M<=16; M*=2) {
//            cout << "\n\nM = " << M;
            numberOfSymbolsErrorsOverall = 0;
            
            message();
            noize(SNR);
            
            outfile2 << " " << errorProbabilityPerSymbol();
//            cout << "numberOfSymbolsErrorsOverall = " << numberOfSymbolsErrorsOverall << " numberOfSymbols = " << (numberOfWords*wordLenght)<<"\n";
        }
        outfile2 << "\n";
        cout << "\n\n" << SNR << "\n";
    }
    
    return 0;
}