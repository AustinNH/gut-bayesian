//math constants
const float pi = 3.14159265359;

//useful constants
const float alphaOneEx = 0.016946;
const float alphaTwoEx = 0.033793;
const float alphaThreeEx = 0.1181;
const float sigmaOne = 3.49e-6;
const float sigmaTwo = 1.94e-5;
const float sigmaThree = 11.0e-4;
const float bOneSm = 41.0/10.0;
const float bTwoSm = -19.0/6.0;
const float bThreeSm = -7.0;
const float bOneNcMssm = 11.0/2.0;//mssm without colored superpartners
const float bTwoNcMssm = -1.0/2.0;
const float bThreeNcMssm = -7.0;
const float bOneMssm = 33.0/5.0;
const float bTwoMssm = 1.0;
const float bThreeMssm = -3.0;
const float bBlLr = 19.0;
const float bRLr = 14.0;
const float bTwoLr = 14.0;
const float bThreeLr = 10.0;
const float bBlPs = 6.0;
const float bRPs = 20.0;
const float bTwoPs = 14.0;
const float bThreePs = 6.0;
const float mZ = 91.1876;

//min and max values for variables of integration
const float alphaMax = 1.0;
const float alphaMin = 1.0/137.0;
const float mMax = 1.22e19;
const float mMin = mZ;
const float deltaMin = 1;
float deltaMax = 1000*zeroToOneScan;
const float hMin = 1;
const float hMax = 10;
const float fMin = 1.0/10;
const float fMax = 10;

float gaussian (float x, float mean, float sigma){
	return 1.0/sqrt (2*pi)/sigma*exp (-0.5*pow (x-mean, 2)/pow (sigma, 2));
}

float doSample(){
	//set variables of integration
	//unification scale and coupling
	vec4 random = rand ();
	float alphaU = alphaMin*exp (random.x*log (alphaMax/alphaMin));
	float mBl = mMin*exp (random.y*log (mMax/mMin));

	//intermediate scale
	//to run MSSM set mR = mBl
	float f = fMin*exp (random.z*log (fMax/fMin));
	float mR = mBl/f;

	//susy scale
	//to ignore susy threshold set mSn = mSc
	//to run SM set mSn = mSc = mR = mBl
	float mSc = mMin*exp (random.w*log (mMax/mMin));
	random = rand();//have used all four components of random, need to refresh
	float h = hMin*exp (random.w*log (hMax/hMin));
	float mSn = mSc/h;

	//threshold corrections
	//to ignore unification threshold corrections parameters set all three of them to zero
	vec4 randomSign = 2*round (rand ()) - vec4 (1,1,1,1);// -1 or 1 with 50% prob
	float deltaOne = 0;//randomSign.x*deltaMin*exp (random.x*log (deltaMax/deltaMin));
	float deltaTwo = 0;//randomSign.y*deltaMin*exp (random.y*log (deltaMax/deltaMin));
	float deltaThree = 0;//randomSign.z*deltaMin*exp (random.z*log (deltaMax/deltaMin));

	float intOne;
	float intTwo;
	float intThree;

	//intermediate shift
	if (mR < mBl){
		intOne = (2.0/5.0*bBlLr + 3.0/5.0*bRLr)/(2*pi)*log(mBl/mR);
		intTwo = bTwoLr/(2*pi)*log(mBl/mR);
		intThree = bThreeLr/(2*pi)*log(mBl/mR);
	}
	else{
		intOne = (2.0/5.0*bBlPs + 3.0/5.0*bRPs)/(2*pi)*log(mR/mBl);
		intTwo = bTwoPs/(2*pi)*log(mR/mBl);
		intThree = bThreePs/(2*pi)*log(mR/mBl);
	}

	//intermediate variables
	float mI = min (mBl, mR);
	float alphaOne = 1.0/(
		1.0/alphaU + deltaOne/4/pi + 
		intOne +
		bOneMssm/(2*pi)*log(mI/mSc) + 
		bOneNcMssm/(2*pi)*log(mSc/mSn) +
		bOneSm/(2*pi)*log(mSn/mZ));
	float alphaTwo = 1.0/(
		1.0/alphaU + deltaTwo/4/pi +  
		intTwo +
		bTwoMssm/(2*pi)*log(mI/mSc) + 
		bTwoNcMssm/(2*pi)*log(mSc/mSn) +
		bTwoSm/(2*pi)*log(mSn/mZ));
	float alphaThree = 1.0/(
		1.0/alphaU + deltaThree/4/pi +  
		intThree +
		bThreeMssm/(2*pi)*log(mI/mSc) + 
		bThreeNcMssm/(2*pi)*log(mSc/mSn) +
		bThreeSm/(2*pi)*log(mSn/mZ));

	//return integrand
	return gaussian (alphaOne, alphaOneEx, sigmaOne)
		* gaussian (alphaTwo, alphaTwoEx, sigmaTwo)
		* gaussian (alphaThree, alphaThreeEx, sigmaThree)
		* step (mZ, mSn) * step (mSn, mSc) * step (mSc, mI);

	//don't need to multiply result by parameter volume because it just cancels the parameter volume already in the integrand
}
