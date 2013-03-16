#include <gsl/gsl_randist.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <complex>

using namespace std;

const int numberOfWords = 1000;
const int wordLenght = 500;
complex<double> **sPrediction;
complex<double> **s;
complex<double> **r;
int M;
double d;
bool bit = false;
int signalLevelPrediction[2];
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
    double m[2];
    unsigned binaryVector;
    for (int j = 0; j < numberOfWords; j++){
        for (int i = 0; i < wordLenght; i++){
            if (bit) {
                for (int k=0; k<2; k++) {
                    binaryVector = 0;
                    for (int k=0; k<log2(M); k++) {
                        binaryVector += (unsigned) gsl_rng_uniform_int(pRNG,2);
                        binaryVector <<= 1;
                    }
                    binaryVector >>= 1;
//                    cout << "  binaryVector = " << binaryVector << "\n" ;
                    m[k] = grayToBinary(binaryVector);
//                    cout << "  m = " << m << "\n" ;
                }
                
            }
            else{
                m[0] = gsl_rng_uniform_int(pRNG,M);
                m[1] = gsl_rng_uniform_int(pRNG,M);
            }
            
            s[j][i].real( (2 * m[0] + 1 - M) * d );
            s[j][i].imag( (2 * m[1] + 1 - M) * d );
//            cout << "s/d = " << s[j][i]/d << "\n";
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
            if (r[wordNumber][numberOfSymbolInWord].real() >= t[M]) {
                signalLevelPrediction[0] = M;
                sPrediction[wordNumber][numberOfSymbolInWord].real(t[M] - d);
            }else{
                if (r[wordNumber][numberOfSymbolInWord].real() < t[0]) {
                    signalLevelPrediction[0] = 0;
                    sPrediction[wordNumber][numberOfSymbolInWord].real(t[0] + d);
                }else{
                    for (int m = 0; m < M ; m++) {
                        if (r[wordNumber][numberOfSymbolInWord].real() >= t[m] && r[wordNumber][numberOfSymbolInWord].real() < t[m+1]) {
                            signalLevelPrediction[0] = m;
                            sPrediction[wordNumber][numberOfSymbolInWord].real(t[m] + d);
//                            cout << "\nm = " << m;
//                            cout << "\nt1 = " << t[m];
//                            cout << "\nt2 = " << t[m+1];
                            break;
                        }
                    }
                }
            }
            
            if (r[wordNumber][numberOfSymbolInWord].imag() >= t[M]) {
                signalLevelPrediction[1] = M;
                sPrediction[wordNumber][numberOfSymbolInWord].imag(t[M] - d);
            }else{
                if (r[wordNumber][numberOfSymbolInWord].imag() < t[0]) {
                    signalLevelPrediction[1] = 0;
                    sPrediction[wordNumber][numberOfSymbolInWord].imag(t[0] + d);
                }else{
                    for (int m = 0; m < M ; m++) {
                        if (r[wordNumber][numberOfSymbolInWord].imag() >= t[m] && r[wordNumber][numberOfSymbolInWord].imag() < t[m+1]) {
                            signalLevelPrediction[1] = m;
                            sPrediction[wordNumber][numberOfSymbolInWord].imag(t[m] + d);
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
                int mSource = 0.0001+ fabs( (s[wordNumber][numberOfSymbolInWord].real() / d + (double)M - 1.0) / 2.0 );
                

                unsigned XOR = binaryToGray(signalLevelPrediction[0]) ^ binaryToGray(mSource);
                if (XOR > 0){
//                    cout << "\ns[wordNumber][numberOfSymbolInWord] / d + (double)M = " << fabs((s[wordNumber][numberOfSymbolInWord] / d + (double)M - 1.0) / 2.0);
//                    cout << "sPrediction = " << sPrediction[wordNumber][numberOfSymbolInWord] << "  s = " << s[wordNumber][numberOfSymbolInWord] << "\n";

//                    cout << "\nsignalLevelPrediction = " << signalLevelPrediction << "  mSource = " <<  mSource;
                    numberOfSymbolsErrorsOverall += NumOf1Bits(XOR);
//                    cout <<  "\nnumOfBits = " << NumOfBits(XOR);
//                    numberOfSymbolsErrorsOverall += 1;
                }
                
                mSource = 0.0001+ fabs( (s[wordNumber][numberOfSymbolInWord].imag() / d + (double)M - 1.0) / 2.0 );
                
                
                XOR = binaryToGray(signalLevelPrediction[1]) ^ binaryToGray(mSource);
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
        return (double)numberOfSymbolsErrorsOverall/ (double)(log2(M)*numberOfWords*wordLenght*2.0);
    }else{
        return (double)numberOfSymbolsErrorsOverall/ (double)(numberOfWords*wordLenght);
    }
}


int main(int argc, const char * argv[])
{
    ofstream outfile2 ("/Users/mike/Documents/Study/8semestr/Telecom/lab3/lab1/example.txt");
    
    sPrediction = (complex<double> **)malloc(numberOfWords * sizeof(complex<double>*));
    s = (complex<double>**)malloc(numberOfWords * sizeof(complex<double>*));
    r = (complex<double>**)malloc(numberOfWords * sizeof(complex<double>*));
    
    for (int wordNumber = 0; wordNumber < numberOfWords ; wordNumber++){
        sPrediction[wordNumber] = (complex<double>*)malloc(wordLenght*sizeof(complex<double>));
        s[wordNumber] = (complex<double>*)malloc(wordLenght*sizeof(complex<double>));
        r[wordNumber] = (complex<double>*)malloc(wordLenght*sizeof(complex<double>));
    }

    for (double SNR = -5; SNR <= 20; SNR += 0.5) {
            outfile2 << SNR;
        for (M=2; M<=8; M*=2) {
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