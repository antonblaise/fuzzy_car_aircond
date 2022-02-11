#include "fis_header.h"
#include <Servo.h>

Servo temp;
Servo fan;

int val;
int tempPin = 1;

// Number of inputs to the fuzzy inference system
const int fis_gcI = 1;
// Number of outputs to the fuzzy inference system
const int fis_gcO = 2;
// Number of rules to the fuzzy inference system
const int fis_gcR = 5;

FIS_TYPE g_fisInput[fis_gcI];
FIS_TYPE g_fisOutput[fis_gcO];

// Setup routine runs once when you press reset:
void setup()
{
    // initialize the Analog pins for input.
    // Pin mode for Input: Cabin_temperature
//    pinMode(0 , INPUT);
    pinMode(tempPin , INPUT);
    pinMode(7, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(4, OUTPUT);

//    // initialize the Analog pins for output.
//    // Pin mode for Output: Temperature
//    pinMode(1 , OUTPUT);
//    // Pin mode for Output: Fan_speed
//    pinMode(2 , OUTPUT);
    temp.attach(11);
    fan.attach(10);
    Serial.begin(9600);

}

// Loop routine runs over and over again forever:
void loop()
{
    float sum = 0.0, mv = 0.0, cel = 0.0, len = 500.0;
    
    Serial.print("\n#################################################\n");  
//    Denoise/Smoothing with moving average
    for (int i = 0; i < len; i++)
    {
      val = analogRead(tempPin);
      mv = ( val / 1024.0) * 5000;
      cel = mv / 10;
      sum += cel;
//      Serial.print(i+1);
//      Serial.print(". val = ");
//      Serial.println(val);
    }
    
//    Serial.print("sum = ");
//    Serial.println(sum);
    cel = (sum/len) - 5;
    Serial.print("Avg cel = ");
    Serial.println(cel);
    

////  Normal
//    val = analogRead(tempPin);
//    mv = ( val / 1024.0) * 5000;
//    cel = (mv / 10) - 5;


    // Read Input: Cabin_temperature
    int low = 15, high = 45;
    if (cel < low)
    {
      cel = low;
    } else if (cel > high)
    {
      cel = high;
    } else 
    {}
    g_fisInput[0] = cel;

    g_fisOutput[0] = 0;
    g_fisOutput[1] = 0;

    fis_evaluate();

    // Set output vlaue: Temperature
//    analogWrite(1 , g_fisOutput[0]);
    temp.write(180 - (180 * g_fisOutput[0] / 100));
    // Set output vlaue: Fan_speed
//    analogWrite(2 , g_fisOutput[1]);
    fan.write(90 - (90 * g_fisOutput[1] / 100));

    Serial.print("\nCabin temperature = ");
    Serial.print(cel);
    Serial.println("*C");
    Serial.print("Temperature knob = ");
    Serial.print(g_fisOutput[0]);
    Serial.println("%");
    Serial.print("Fan speed = ");
    Serial.print(g_fisOutput[1]);
    Serial.println("%");
    Serial.print("\n#################################################\n");

    if (g_fisOutput[1] > 75 && g_fisOutput[1] <= 100)
    {
      digitalWrite(7, HIGH);
      digitalWrite(6, LOW);
      digitalWrite(5, LOW);
      digitalWrite(4, LOW);
    } else if (g_fisOutput[1] > 50 && g_fisOutput[1] <= 75)
    {
      digitalWrite(7, LOW);
      digitalWrite(6, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(4, LOW);
    } else if (g_fisOutput[1] > 25 && g_fisOutput[1] <= 50)
    {
      digitalWrite(7, LOW);
      digitalWrite(6, LOW);
      digitalWrite(5, HIGH);
      digitalWrite(4, LOW);
    } else if (g_fisOutput[1] >= 0 && g_fisOutput[1] <= 25)
    {
      digitalWrite(7, LOW);
      digitalWrite(6, LOW);
      digitalWrite(5, LOW);
      digitalWrite(4, HIGH);
    } else
    {
      digitalWrite(7, LOW);
      digitalWrite(6, LOW);
      digitalWrite(5, LOW);
      digitalWrite(4, LOW);
    }
    
}

//***********************************************************************
// Support functions for Fuzzy Inference System                          
//***********************************************************************
// Gaussian Member Function
FIS_TYPE fis_gaussmf(FIS_TYPE x, FIS_TYPE* p)
{
    FIS_TYPE s = p[0], c = p[1];
    FIS_TYPE t = (x - c) / s;
    return exp(-(t * t) / 2);
}

FIS_TYPE fis_min(FIS_TYPE a, FIS_TYPE b)
{
    return min(a, b);
}

FIS_TYPE fis_max(FIS_TYPE a, FIS_TYPE b)
{
    return max(a, b);
}

FIS_TYPE fis_array_operation(FIS_TYPE *array, int size, _FIS_ARR_OP pfnOp)
{
    int i;
    FIS_TYPE ret = 0;

    if (size == 0) return ret;
    if (size == 1) return array[0];

    ret = array[0];
    for (i = 1; i < size; i++)
    {
        ret = (*pfnOp)(ret, array[i]);
    }

    return ret;
}


//***********************************************************************
// Data for Fuzzy Inference System                                       
//***********************************************************************
// Pointers to the implementations of member functions
_FIS_MF fis_gMF[] =
{
    fis_gaussmf
};

// Count of member function for each Input
int fis_gIMFCount[] = { 5 };

// Count of member function for each Output 
int fis_gOMFCount[] = { 5, 5 };

// Coefficients for the Input Member Functions
FIS_TYPE fis_gMFI0Coeff1[] = { 3.185, 15 };
FIS_TYPE fis_gMFI0Coeff2[] = { 3.185, 22.5 };
FIS_TYPE fis_gMFI0Coeff3[] = { 3.185, 30 };
FIS_TYPE fis_gMFI0Coeff4[] = { 3.185, 37.5 };
FIS_TYPE fis_gMFI0Coeff5[] = { 3.185, 45 };
FIS_TYPE* fis_gMFI0Coeff[] = { fis_gMFI0Coeff1, fis_gMFI0Coeff2, fis_gMFI0Coeff3, fis_gMFI0Coeff4, fis_gMFI0Coeff5 };
FIS_TYPE** fis_gMFICoeff[] = { fis_gMFI0Coeff };

// Coefficients for the Output Member Functions
FIS_TYPE fis_gMFO0Coeff1[] = { 10.62, -2.22e-16 };
FIS_TYPE fis_gMFO0Coeff2[] = { 10.62, 25 };
FIS_TYPE fis_gMFO0Coeff3[] = { 10.62, 50 };
FIS_TYPE fis_gMFO0Coeff4[] = { 10.62, 75 };
FIS_TYPE fis_gMFO0Coeff5[] = { 10.62, 100 };
FIS_TYPE* fis_gMFO0Coeff[] = { fis_gMFO0Coeff1, fis_gMFO0Coeff2, fis_gMFO0Coeff3, fis_gMFO0Coeff4, fis_gMFO0Coeff5 };
FIS_TYPE fis_gMFO1Coeff1[] = { 10.62, 0 };
FIS_TYPE fis_gMFO1Coeff2[] = { 10.62, 25 };
FIS_TYPE fis_gMFO1Coeff3[] = { 10.62, 50 };
FIS_TYPE fis_gMFO1Coeff4[] = { 10.62, 75 };
FIS_TYPE fis_gMFO1Coeff5[] = { 10.62, 100 };
FIS_TYPE* fis_gMFO1Coeff[] = { fis_gMFO1Coeff1, fis_gMFO1Coeff2, fis_gMFO1Coeff3, fis_gMFO1Coeff4, fis_gMFO1Coeff5 };
FIS_TYPE** fis_gMFOCoeff[] = { fis_gMFO0Coeff, fis_gMFO1Coeff };

// Input membership function set
int fis_gMFI0[] = { 0, 0, 0, 0, 0 };
int* fis_gMFI[] = { fis_gMFI0};

// Output membership function set
int fis_gMFO0[] = { 0, 0, 0, 0, 0 };
int fis_gMFO1[] = { 0, 0, 0, 0, 0 };
int* fis_gMFO[] = { fis_gMFO0, fis_gMFO1};

// Rule Weights
FIS_TYPE fis_gRWeight[] = { 1, 1, 1, 1, 1 };

// Rule Type
int fis_gRType[] = { 1, 1, 1, 1, 1 };

// Rule Inputs
int fis_gRI0[] = { 1 };
int fis_gRI1[] = { 2 };
int fis_gRI2[] = { 3 };
int fis_gRI3[] = { 4 };
int fis_gRI4[] = { 5 };
int* fis_gRI[] = { fis_gRI0, fis_gRI1, fis_gRI2, fis_gRI3, fis_gRI4 };

// Rule Outputs
int fis_gRO0[] = { 5, 1 };
int fis_gRO1[] = { 4, 2 };
int fis_gRO2[] = { 3, 3 };
int fis_gRO3[] = { 2, 4 };
int fis_gRO4[] = { 1, 5 };
int* fis_gRO[] = { fis_gRO0, fis_gRO1, fis_gRO2, fis_gRO3, fis_gRO4 };

// Input range Min
FIS_TYPE fis_gIMin[] = { 15 };

// Input range Max
FIS_TYPE fis_gIMax[] = { 45 };

// Output range Min
FIS_TYPE fis_gOMin[] = { 0, 0 };

// Output range Max
FIS_TYPE fis_gOMax[] = { 100, 100 };

//***********************************************************************
// Data dependent support functions for Fuzzy Inference System           
//***********************************************************************
FIS_TYPE fis_MF_out(FIS_TYPE** fuzzyRuleSet, FIS_TYPE x, int o)
{
    FIS_TYPE mfOut;
    int r;

    for (r = 0; r < fis_gcR; ++r)
    {
        int index = fis_gRO[r][o];
        if (index > 0)
        {
            index = index - 1;
            mfOut = (fis_gMF[fis_gMFO[o][index]])(x, fis_gMFOCoeff[o][index]);
        }
        else if (index < 0)
        {
            index = -index - 1;
            mfOut = 1 - (fis_gMF[fis_gMFO[o][index]])(x, fis_gMFOCoeff[o][index]);
        }
        else
        {
            mfOut = 0;
        }

        fuzzyRuleSet[0][r] = fis_min(mfOut, fuzzyRuleSet[1][r]);
    }
    return fis_array_operation(fuzzyRuleSet[0], fis_gcR, fis_max);
}

FIS_TYPE fis_defuzz_centroid(FIS_TYPE** fuzzyRuleSet, int o)
{
    FIS_TYPE step = (fis_gOMax[o] - fis_gOMin[o]) / (FIS_RESOLUSION - 1);
    FIS_TYPE area = 0;
    FIS_TYPE momentum = 0;
    FIS_TYPE dist, slice;
    int i;

    // calculate the area under the curve formed by the MF outputs
    for (i = 0; i < FIS_RESOLUSION; ++i){
        dist = fis_gOMin[o] + (step * i);
        slice = step * fis_MF_out(fuzzyRuleSet, dist, o);
        area += slice;
        momentum += slice*dist;
    }

    return ((area == 0) ? ((fis_gOMax[o] + fis_gOMin[o]) / 2) : (momentum / area));
}

//***********************************************************************
// Fuzzy Inference System                                                
//***********************************************************************
void fis_evaluate()
{
    FIS_TYPE fuzzyInput0[] = { 0, 0, 0, 0, 0 };
    FIS_TYPE* fuzzyInput[fis_gcI] = { fuzzyInput0, };
    FIS_TYPE fuzzyOutput0[] = { 0, 0, 0, 0, 0 };
    FIS_TYPE fuzzyOutput1[] = { 0, 0, 0, 0, 0 };
    FIS_TYPE* fuzzyOutput[fis_gcO] = { fuzzyOutput0, fuzzyOutput1, };
    FIS_TYPE fuzzyRules[fis_gcR] = { 0 };
    FIS_TYPE fuzzyFires[fis_gcR] = { 0 };
    FIS_TYPE* fuzzyRuleSet[] = { fuzzyRules, fuzzyFires };
    FIS_TYPE sW = 0;

    // Transforming input to fuzzy Input
    int i, j, r, o;
    for (i = 0; i < fis_gcI; ++i)
    {
        for (j = 0; j < fis_gIMFCount[i]; ++j)
        {
            fuzzyInput[i][j] =
                (fis_gMF[fis_gMFI[i][j]])(g_fisInput[i], fis_gMFICoeff[i][j]);
        }
    }

    int index = 0;
    for (r = 0; r < fis_gcR; ++r)
    {
        if (fis_gRType[r] == 1)
        {
            fuzzyFires[r] = FIS_MAX;
            for (i = 0; i < fis_gcI; ++i)
            {
                index = fis_gRI[r][i];
                if (index > 0)
                    fuzzyFires[r] = fis_min(fuzzyFires[r], fuzzyInput[i][index - 1]);
                else if (index < 0)
                    fuzzyFires[r] = fis_min(fuzzyFires[r], 1 - fuzzyInput[i][-index - 1]);
                else
                    fuzzyFires[r] = fis_min(fuzzyFires[r], 1);
            }
        }
        else
        {
            fuzzyFires[r] = FIS_MIN;
            for (i = 0; i < fis_gcI; ++i)
            {
                index = fis_gRI[r][i];
                if (index > 0)
                    fuzzyFires[r] = fis_max(fuzzyFires[r], fuzzyInput[i][index - 1]);
                else if (index < 0)
                    fuzzyFires[r] = fis_max(fuzzyFires[r], 1 - fuzzyInput[i][-index - 1]);
                else
                    fuzzyFires[r] = fis_max(fuzzyFires[r], 0);
            }
        }

        fuzzyFires[r] = fis_gRWeight[r] * fuzzyFires[r];
        sW += fuzzyFires[r];
    }

    if (sW == 0)
    {
        for (o = 0; o < fis_gcO; ++o)
        {
            g_fisOutput[o] = ((fis_gOMax[o] + fis_gOMin[o]) / 2);
        }
    }
    else
    {
        for (o = 0; o < fis_gcO; ++o)
        {
            g_fisOutput[o] = fis_defuzz_centroid(fuzzyRuleSet, o);
        }
    }
}
