#include <math.h>

const int pointsToScan = 1;//set to 1 to just compute a single Bayesian evidence

char* modelFileName = "model.glsl";

const int windowWidth = 1000;//if you change these, change datasize down below too
const int windowHeight = 500;
const int samplesOne = 100000000;//should be divisible by windowWidth
const int samplesTwo = 10000;//should be divisible by windowHeight
//samplesOne*samplesTwo is total number of samples

#include "SDLAbstraction.c"
#include "OpenGLAbstraction.c"

const int dataSize = 1000*500*4;
const int columnsForProgressBar = 38;

void dumpPixels(GLfloat* data){
	int count = 0;
	int i = 0;
	while (count < 36 && i < windowWidth*windowHeight){//just dump a few of the non-zero pixels
		if (data[4*i] != 0.0){
			printf ("pixel %d,\t%.10f,\t%.10f,\t%.10f,\t%.10f\n", i, data[4*i], data[4*i+1], data[4*i+2], data[4*i+3]);
			count++;
		}
		i++;
	}
}

void printProgressBar (int i){
	if (i%((samplesOne/windowWidth)/columnsForProgressBar) == 0){
		printf("\r");
		printf("|");
		int j = 0;
		while (j < i*columnsForProgressBar/(samplesOne/windowWidth)){
			printf("X");
			j++;
		}
		while (j < columnsForProgressBar){
			printf(" ");
			j++;
		}
		printf("|");
		fflush(stdout);
	}
}

void doPoint(float zeroToOne){
	GLfloat* data = malloc (dataSize*sizeof (GLfloat));

	for (int i = 0; i < windowWidth*windowHeight; i++){
		data[4*i] = 0.0;
		data[4*i+1] = 1.0;
		data[4*i+2] = 0.0;
		data[4*i+3] = 1.0;
	}
	initializeCompute(windowWidth, windowHeight, data, modelFileName);

	display();
	for (int i = 0; i < samplesOne/windowWidth; i++){//numberOfSamples divided by windowHeight in the fragment shader
		printProgressBar (i);
		compute (i, samplesTwo, zeroToOne);
		display();
		wait();
	}
	printf("\n");

	readData (data);

	float f = 0;
	float fSquared = 0;
	for (int i = 0; i < windowWidth*windowHeight; i++){
		f += data[4*i];
		fSquared += data[4*i+1];
	}
	f = f/samplesOne;
	fSquared = fSquared/samplesOne;
	float totalSamples = 1.0*samplesOne*samplesTwo;

	printf ("%.2f,\t%.10f +- %.10f\n", zeroToOne, f, sqrt((fSquared-f*f)/totalSamples));
	//dumpPixels(data);
	free (data);
	return;
}

void main(){
	initializeWindow(windowWidth, windowHeight);

	if (pointsToScan == 1){
		doPoint(0);
	}
	else for (int i = 0; i < pointsToScan; i++){
		doPoint(1.0*i/(pointsToScan-1));
	}

	closeWindow();
}