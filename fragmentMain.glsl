void main(){
	initializeCounter();

	float result = 0;
	float resultSquared = 0;

	for (int j = 0; j < numberOfSamples/windowHeight; j++){//number of samples is also divided by windowWidth in main.c
		float integrand = doSample();
		result += integrand;
		resultSquared += pow (integrand, 2);
	}

	color = texelFetch (texture, ivec2 (gl_FragCoord), 0);
	color = vec4 (color.r + result/numberOfSamples, color.g + resultSquared/numberOfSamples, 0.0, 0.0);
}
