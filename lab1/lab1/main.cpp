#include <gsl/gsl_randist.h>
#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;

const int numberOfWords = 1000;
const int wordLenght = 100;
double **sPrediction;
double **s;
double **r;
int M;
double d;
bool bit = true;
int signalLevelPrediction;
int numberOfSymbolsErrorsOverall;
gsl_rng *pRNG = gsl_rng_alloc(gsl_rng_mt19937);

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
            
            s[j][i] = (2 * m + 1 - M) * d;
//            cout << "s/d = " << s[j][i]/d << "\n";
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
//    cout << "\nM = " <<M;
    for (int m = 0; m <= M ; m++) {
        t[m] = (2*m - M) * d;
//        t[m] = 2*m - M;
//        cout << "\nt[m] = " << t[m];
    }
    for (int wordNumber = 0; wordNumber < numberOfWords; wordNumber++){
        for (int numberOfSymbolInWord=0; numberOfSymbolInWord < wordLenght ; numberOfSymbolInWord++){
//            cout << "\n\nr = " << r[wordNumber][numberOfSymbolInWord];
            if (r[wordNumber][numberOfSymbolInWord] >= t[M]) {
                signalLevelPrediction = M;
                sPrediction[wordNumber][numberOfSymbolInWord] = t[M] - d;
            }else{
                if (r[wordNumber][numberOfSymbolInWord] < t[0]) {
                    signalLevelPrediction = 0;
                    sPrediction[wordNumber][numberOfSymbolInWord] = t[0] + d;
                }else{
                    for (int m = 0; m < M ; m++) {
                        if (r[wordNumber][numberOfSymbolInWord] >= t[m] && r[wordNumber][numberOfSymbolInWord] < t[m+1]) {
                            signalLevelPrediction = m;
                            sPrediction[wordNumber][numberOfSymbolInWord] = t[m] + d;
//                            cout << "\nm = " << m;
//                            cout << "\nt1 = " << t[m];
//                            cout << "\nt2 = " << t[m+1];
                            break;
                        }
                    }
                }
            }
            if (bit) {
//                спросить почему когда 1 - дабл приводиш к инту получается 0
                int mSource = 0.0001+ fabs( (s[wordNumber][numberOfSymbolInWord] / d + (double)M - 1.0) / 2.0 );
                

                unsigned XOR = binaryToGray(signalLevelPrediction) ^ binaryToGray(mSource);
                if (XOR > 0){
//                    cout << "\ns[wordNumber][numberOfSymbolInWord] / d + (double)M = " << fabs((s[wordNumber][numberOfSymbolInWord] / d + (double)M - 1.0) / 2.0);
//                    cout << "sPrediction = " << sPrediction[wordNumber][numberOfSymbolInWord] << "  s = " << s[wordNumber][numberOfSymbolInWord] << "\n";

//                    cout << "\nsignalLevelPrediction = " << signalLevelPrediction << "  mSource = " <<  mSource;
                    numberOfSymbolsErrorsOverall += NumOf1Bits(XOR);
//                    cout <<  "\nnumOfBits = " << NumOfBits(XOR);
//                    numberOfSymbolsErrorsOverall += 1;
                }
            }else{
//                cout << "\nsPrediction = " << sPrediction[wordNumber][numberOfSymbolInWord];
                if (sPrediction[wordNumber][numberOfSymbolInWord]!=s[wordNumber][numberOfSymbolInWord] ) {
                    numberOfSymbolsErrorsOverall += 1;
    //                cout << "sPrediction = " << sPrediction[wordNumber][numberOfSymbolInWord] << "  s = " << s[wordNumber][numberOfSymbolInWord] << "\n";
                }else{
    //                cout << "sPrediction = " << sPrediction[wordNumber][numberOfSymbolInWord] << "  s = " << s[wordNumber][numberOfSymbolInWord] << "\n";
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
    ofstream outfile2 ("/Users/mike/Documents/Study/8semestr/Telecom/lab1/lab1/example.txt");
    
    sPrediction = (double **)malloc(numberOfWords * sizeof(double*));
    s = (double**)malloc(numberOfWords * sizeof(double*));
    r = (double**)malloc(numberOfWords * sizeof(double*));
//    sPrediction = (double **)malloc(numberOfWords * wordLenght * sizeof(double));
//    s = (double**)malloc(numberOfWords * wordLenght * sizeof(double));
//    r = (double**)malloc(numberOfWords * wordLenght * sizeof(double));
    
    for (int wordNumber = 0; wordNumber < numberOfWords ; wordNumber++){
        sPrediction[wordNumber] = (double*)malloc(wordLenght*sizeof(double));
        s[wordNumber] = (double*)malloc(wordLenght*sizeof(double));
        r[wordNumber] = (double*)malloc(wordLenght*sizeof(double));
    }
    
    for (double SNR = -5; SNR <= 20; SNR += 0.5) {
            outfile2 << SNR;
        for (M=2; M<=16; M*=2) {
            d = sqrt(3 / (pow(M,2) - 1));
            
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