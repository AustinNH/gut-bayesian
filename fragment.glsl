#version 400 core

uniform int iteration;
uniform int numberOfSamples;
uniform float zeroToOneScan;

uniform sampler2D texture;

out vec4 color;

//constants for philox CBPRNG generation
const uint multiplierOne = uint (0xcd9e8d57);
const uint multiplierTwo = uint (0xd2511f53);
const uint keyOne = uint (0x00000000);
const uint keyTwo = uint (0x00000001);
const uint keyOffset = uint (0x7f4a7c15);//last 32 bits of one of the constants given in Salmon
const uint max = 0xFFFFFFFF;

const int windowWidth = 1000;
const int windowHeight = 500;

uvec4 counter;

uvec4 philox (){
	uvec4 mixed = counter;
       	counter.x = counter.x + uint (1);//increments the first 32-bit word in the counter. No problem as long as numberOfSamples/windowHeight < 2^32
	uvec4 permuted;
	uint roundKeyOne = keyOne;
	uint roundKeyTwo = keyTwo;
	for (int i = 0; i < 7; i++){
		//permute is this the fastest way?
		permuted = ivec4 (mixed.x, mixed.w, mixed.z, mixed.y);

		//mix
		umulExtended (permuted.y, multiplierOne, mixed.y, mixed.x);
		umulExtended (permuted.w, multiplierTwo, mixed.w, mixed.z);
		mixed.y = mixed.y ^ roundKeyOne ^ permuted.x;
		mixed.w = mixed.w ^ roundKeyTwo ^ permuted.z;
	}
	return mixed;
}

void initializeCounter (){
	ivec2 coord = ivec2 (gl_FragCoord);
	int pixel = (coord.x+coord.y*windowWidth);//second word will be the pixel number so that two pixels don't do the same sample
	counter = uvec4 (0, pixel, iteration, 0);//third word is the iteration number so we don't do the same sample twice
}

vec4 rand (){//between 0 and 1, uniformly distributed
	return vec4 (philox ())/max;
}

