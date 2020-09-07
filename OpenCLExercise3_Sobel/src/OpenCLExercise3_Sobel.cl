#ifndef __OPENCL_VERSION__
#include <OpenCL/OpenCLKernel.hpp> // Hack to make syntax highlighting in Eclipse work
#endif

#define TWOPI 6.28318530718

//////////////////////////////////////////////////////////////////////////////
// Projection of Pixels onto Edge
//////////////////////////////////////////////////////////////////////////////
__kernel void sobelKernel(global const float* d_input, global const float* d_inputEdge, global float* d_distance, global float* d_intensity,
		float A, float B, float C) {

	size_t i = get_global_id(0);
	size_t j = get_global_id(1);
	size_t countX = get_global_size(0);
	size_t countY = get_global_size(1);

	float temp;

	if(d_inputEdge[i + countX * j] == 255){
		temp = 0;
	}
	else{
		temp = ((A*i)+(B*j)+C);
	}
	d_distance[i + countX * j] = (temp/sqrt((A*A) + (B*B)));
	d_intensity[i + countX * j] = d_input[i + countX * j];
}

//////////////////////////////////////////////////////////////////////////////
// FFT Implementation
//////////////////////////////////////////////////////////////////////////////
__kernel void naivefft2( __global const float* srcx, __global const float* srcy, __global float* dstx, __global float* dsty,
	                    const unsigned int n) {
    const float ph = -TWOPI / n;
    const int gid = get_global_id(0);

    float resx = 0.0f;
    float resy = 0.0f;

    for (int k = 0; k < n; k++) {
        const float tx = srcx[k];
        const float ty = srcy[k];

        const float val = ph * k * gid;
        float cs;
	    float sn = sincos(val, &cs);
        resx += tx * cs - ty * sn;
        resy += ty * cs + tx * sn;
    }

    dstx[gid] = resx;
    dsty[gid] = resy;
}

//////////////////////////////////////////////////////////////////////////////
// Differentiation
//////////////////////////////////////////////////////////////////////////////
__kernel void differ(global const float* d_inputDiffInten, global const float* d_inputDiffDist, global float* d_outputDiffInten,
		global float* d_outputDiffDist) {

	size_t i = get_global_id(0);
	size_t countX = get_global_size(0);

	float prev_intensity = 0;
	float next_intensity = 0;

	if(i==0 || i==(countX - 1)){
		d_outputDiffInten[i] = 0;
		d_outputDiffDist[i] = d_inputDiffDist[i];
	}else{
		prev_intensity = d_inputDiffInten[i-1];
		next_intensity = d_inputDiffInten[i+1];
		d_outputDiffInten[i] = fabs((next_intensity - prev_intensity)/2);
		d_outputDiffDist[i] = d_inputDiffDist[i];
	}
}
