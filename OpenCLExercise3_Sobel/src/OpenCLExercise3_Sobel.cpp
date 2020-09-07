//============================================================================
// Name        : MTF.cpp
// Author      : Gunjan Gupta (3375253), Ayazahmed Kazi (3374898)
//============================================================================

//////////////////////////////////////////////////////////////////////////////
// OpenCL Project 02 : Implementation of Modulation Transfer Function (MTF)
//				       measurement on Slanted Edge: ISO 12233:2017
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//Includes
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <valarray>
#include <complex>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include </usr/include/opencv2/opencv.hpp>

#include <Core/Assert.hpp>
#include <Core/Time.hpp>
#include <Core/Image.hpp>
#include <OpenCL/cl-patched.hpp>
#include <OpenCL/Program.hpp>
#include <OpenCL/Event.hpp>
#include <OpenCL/Device.hpp>

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace cv;

const double PI = 3.141592653589793238460;

typedef complex<double> Comp;
typedef valarray<Comp> CArray;

//////////////////////////////////////////////////////////////////////////////
//Global variables
//////////////////////////////////////////////////////////////////////////////

float A, B, C;
float slope = 0;
float intercept = 0;
Mat imagegr, roi, edges;
bool ldown = false;
bool lup = false;
Point corner1, corner2;
Rect box;

//////////////////////////////////////////////////////////////////////////////
// CPU implementation
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Selection of rectangular ROI
//////////////////////////////////////////////////////////////////////////////
static void mouse_callback(int event, int x, int y,int,void *){

	if(event == EVENT_LBUTTONDOWN)
	{
		ldown = true;
		corner1.x = x;
		corner1.y = y;
		cout<<"Corner1 recorded at"<< corner1<<endl;
	}

	if(event == EVENT_LBUTTONUP)
	{
		if(abs(x-corner1.x)>20 && abs(y-corner1.y)>20)
		{
			lup = true;
			corner2.x = x;
			corner2.y = y;
			cout<<"Corner2 recorded at" << corner2<<endl<<endl;
		}
		else{
			cout<<"Please select a bigger region"<<endl;
			ldown = false;
		}
	}
	if(ldown == true && lup == false)
	{
		Point pt;
		pt.x = x;
		pt.y = y;

		Mat locale_img = imagegr.clone();

		rectangle(locale_img, corner1, pt, Scalar(0,0,255));
		imshow("Cropping App", locale_img);
	}

	if(ldown == true && lup== true)
	{
		box.width = abs(corner1.x-corner2.x);
		box.height  = abs(corner1.y-corner2.y);

		box.x = min(corner1.x, corner2.x);
		box.y = min(corner1.y, corner2.y);

		Mat crop(imagegr,box);
		namedWindow("ROI");
		imshow("ROI",crop);
		roi = crop.clone();

		ldown = false;
		lup = false;

	}
}

//////////////////////////////////////////////////////////////////////////////
// Generation of Hamming window Vector of length m, where m = no of pixels in
// each line of the 2D image
//////////////////////////////////////////////////////////////////////////////
void hamming_window1(unsigned int length, float mid, vector<float> &hammingVector){
	float width1, width2, width;
	float temp2;
	width1 = mid - 1;
	width2 = length - mid;
	width = max(width1, width2);
	for(unsigned int i = 0; i<length; i++){
		temp2 = 0.54 + (0.46 * cos(PI * ((i - mid) / width)));
		hammingVector.push_back(temp2);
	}
}

//////////////////////////////////////////////////////////////////////////////
// Convolution of 2 Vectors
//////////////////////////////////////////////////////////////////////////////
void convol(float *x1, float *x2, float *temp, unsigned int pixels){

	for(unsigned int i = 0; i < pixels; i++){
		if(i != 0){
			temp[i] = (x2[i] * x1[0]) + (x2[i-1] * x1[1]);
			if(i==1){
				temp[0] = temp[i];
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// Element-wise Multiplication of Hamming Vector and Convolution result
//////////////////////////////////////////////////////////////////////////////
void convolMul(float *x1, vector<float> &hammingVector, float *x2, unsigned int pixels){
	for(unsigned int i = 0; i < pixels; i++){
		x2[i] = x1[i] * hammingVector[i];
	}
}

//////////////////////////////////////////////////////////////////////////////
// Calculation of Centroid of each line of an image
//////////////////////////////////////////////////////////////////////////////
int calcCentroid(float *x1, unsigned int pixels){
	float temp1 = 0;
	float temp2 = 0;
	unsigned int k = 1;
	for(unsigned int i = 0; i < pixels; i++){
		temp1 += x1[i];
		temp2 += x1[i] * k;
		k++;
	}
	return floor(temp2/temp1);
}

//////////////////////////////////////////////////////////////////////////////
// Linear Fitting function
// Input -
// Output - Slope and Offset
//////////////////////////////////////////////////////////////////////////////
void linearFit(vector<float> &linesVector, vector<float> &centroidVector, unsigned int n){

	unsigned int N = centroidVector.size();

	double temp[2*n+1];                        			//Array that will store the values of sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n)
	for (unsigned int i = 0; i < 2*n+1; i++)
	{
		temp[i]=0;
		for (unsigned int j = 0; j < N; j++)
			temp[i]=temp[i]+pow(linesVector[j],i);      //consecutive positions of the array will store N,sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n)
	}

	double temp3[n+1][n+2],a[n+1];            			//B is the Normal matrix(augmented) that will store the equations, 'a' is for value of the final coefficients
	for (unsigned int i = 0; i <= n; i++)
		for (unsigned int j = 0; j <= n; j++)
			temp3[i][j]=temp[i+j];           		    //Build the Normal matrix by storing the corresponding coefficients at the right positions except the last column of the matrix

	double temp2[n+1];                                  //Array to store the values of sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
	for (unsigned int i = 0; i < n+1; i++)
	{
		temp2[i]=0;
		for (unsigned int j = 0; j < N; j++)
		temp2[i]=temp2[i]+pow(linesVector[j],i)*centroidVector[j];  //consecutive positions will store sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
	}

	for (unsigned int i = 0; i <= n; i++)
		temp3[i][n+1]=temp2[i];                            //load the values of Y as the last column of B(Normal Matrix but augmented)

	n=n+1;                                                 //n is made n+1 because the Gaussian Elimination part below was for n equations, but here n is the degree of polynomial and for n degree we get n+1 equations

	for (unsigned int i = 0; i < n; i++)                   //From now Gaussian Elimination starts(can be ignored) to solve the set of linear equations (Pivotisation)
		for (unsigned int k = i+1; k < n; k++)
			if (temp3[i][i] < temp3[k][i])
				for (unsigned int j = 0; j <= n; j++)
				{
					double temper = temp3[i][j];
					temp3[i][j] = temp3[k][j];
					temp3[k][j] = temper;
				}

	for (unsigned int i = 0; i < n-1; i++)            		//loop to perform the guass elimination
		for (unsigned int k = i+1; k < n; k++)
		{
			double t = temp3[k][i]/temp3[i][i];
			for (unsigned int j = 0; j <= n; j++)
				temp3[k][j] = temp3[k][j]-t*temp3[i][j];    //make the elements below the pivot elements equal to zero or eliminate the variables
		}

	for (unsigned int i = n-1; i >= 0; i--)                //back-substitution
	{                        							   //x is an array whose values correspond to the values of x,y,z..
		a[i] = temp3[i][n];                				   //make the variable to be calculated equal to the rhs of the last equation
		for (unsigned int j = 0; j < n; j++)
			if (j!=i)            						   //then subtract all the lhs values except the coefficient of the variable whose value is being calculated
				a[i]=a[i]-temp3[i][j]*a[j];
		a[i]=a[i]/temp3[i][i];            				   //now finally divide the rhs by the coefficient of the variable to be calculate
		slope = a[1];
		intercept = a[0];
		if(i == 0) break;

	}
	return;
}

//////////////////////////////////////////////////////////////////////////////
// Read and Pre-process the Input Image : Hamming window Vector, Convolution,
// Centroid, Linear Fit
//////////////////////////////////////////////////////////////////////////////
void MTFReadImage1(){

	Mat cedges;
	bool sel;

	unsigned int lines, pixels;
	float hamming1[2] = {-0.5, 0.5};

	vector<float> hammingVector;
	vector<float> centroidVector;
	vector<float> centroidVector2;
	vector<float> linesVector;

	cout << "Select Horizontal(0) or Vertical Image(1) : " << endl;
	cin >> sel;

	if(sel == 0){
		imagegr = imread( "h_edge_mono.tif", 0);
		transpose(imagegr, imagegr);
		flip(imagegr, imagegr, 1);
	}
	else{
		imagegr = imread( "v_edge_mono.tif", 0);
	}

	cout << "Select Region of Interest : (Press q once the selection is done) \n" << endl;
	namedWindow("Cropping App");
	imshow("Cropping App",imagegr);
	setMouseCallback("Cropping App",mouse_callback);
	while(char(waitKey(1))!='q'){};

	Canny(roi,edges,50,50,3,false);

	lines = roi.rows;
	pixels = roi.cols;

	cout << "Rows : " << lines << " " << "Cols : " << pixels << "\n" << endl;

	hamming_window1(pixels, (pixels+1)/2, hammingVector);

	int tempCent;

	for(unsigned int i = 0; i < lines; i++){

		float roiRow[pixels] = {0};
		float temp[pixels] = {0};
		float samp[pixels] = {0};

		linesVector.push_back(i);

		for(unsigned int j = 0; j < pixels; j++){
			roiRow[j] = roi.at<uchar>(i,j);
		}

		convol(hamming1, roiRow, temp, pixels);
		convolMul(temp, hammingVector, samp, pixels);
		tempCent = calcCentroid(samp, pixels);
		centroidVector.push_back(tempCent);
	}

	linearFit(linesVector, centroidVector, 1);
	cout << "Initial Slope : " << slope << " " << "Initial Intercept : " << intercept << "\n" << endl;

	unsigned int k = 1;
	float place[lines] = {0};
	vector<float> hammingVector2;

	for(unsigned int i = 0; i < lines ; i++){

		float roiRow[pixels] = {0};
		float temp[pixels] = {0};
		float samp[pixels] = {0};

		place[i] = intercept + (slope * k);
		hamming_window1(pixels, place[i], hammingVector2);

		for(unsigned int j = 0; j < pixels; j++){
			roiRow[j] = roi.at<uchar>(i,j);
		}

		convol(hamming1, roiRow, temp, pixels);
		convolMul(temp, hammingVector2, samp, pixels);
		tempCent = calcCentroid(samp, pixels);
		centroidVector2.push_back(tempCent);
		k++;
	}

	linearFit(linesVector, centroidVector2, 1);
	cout << "Final Slope : " << slope << " " << "Final Intercept : " << intercept << "\n" << endl;

	A = (1/slope);
	B = -1;
	C = -(intercept/slope);
}

//////////////////////////////////////////////////////////////////////////////
// Sort the pixels using Bubble Sort based on their distance from the Edge
//////////////////////////////////////////////////////////////////////////////
void sort_pixel_by_ascending_distance(vector<float> &dist, vector<float> &intensity){
	float temp_intensity;
	float temp_distance;
	cout << "Number of pixels in ROI : " << dist.size() << "\n" << endl;
	if(dist.size() == intensity.size()){
		for (unsigned int i=0; i < dist.size(); i++){
			for (unsigned int j=0; j < dist.size()-1; j++){
				if(dist[j] > dist[j+1]){
 					temp_distance = dist[j];
					dist[j] =  dist[j+1];
					dist[j+1] =  temp_distance;

					temp_intensity = intensity[j];
					intensity[j] =  intensity[j+1];
					intensity[j+1] =  temp_intensity;
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// Data Binning
//////////////////////////////////////////////////////////////////////////////
void image_binning_averaging(vector<float> &dist, vector<float> &intensity, unsigned int bin_size,
		vector<float> &averaged_distance, vector<float> &averaged_intensity){
	unsigned int i,j;
	float temp_distance;
	float temp_intensity;
	unsigned int count;
	if(dist.size() == intensity.size()){
		for(i = 0; i <  dist.size() ; i=i+bin_size){
			temp_distance = 0;
			temp_intensity = 0;
			count = 0;
			for(j = 0; j < bin_size; j++){
				if((i + j) <= dist.size()){
					temp_distance += dist[i+j];
					temp_intensity += intensity[i+j];
					count++;

				}else{
					break;
				}
			}
			if(count == bin_size){
				temp_distance = temp_distance/count;
				temp_intensity = temp_intensity/count;

				averaged_distance.push_back(temp_distance);
				averaged_intensity.push_back(temp_intensity);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// Normalize Point Spread Function (PSF)
//////////////////////////////////////////////////////////////////////////////
void normalise_psf(vector<float> &psf_distance, vector<float> &psf_intensity){
	float max_intensity = FLT_MIN;
	unsigned int i = 0;
	for(i = 0; i < psf_intensity.size(); i++){
		if(psf_intensity[i] > max_intensity){
			max_intensity = psf_intensity[i];
		}
	}

	for(i = 0; i < psf_intensity.size(); i++){
			psf_intensity[i] = psf_intensity[i]/max_intensity;
		}
}

//////////////////////////////////////////////////////////////////////////////
// Magnitude of Fast Fourier Transform (FFT)
//////////////////////////////////////////////////////////////////////////////
void obtain_magnitude_of_fft_values(vector<float> &h_fftRealOutput, vector<float> &h_fftImgOutput, vector<float> &fft_magnitude){
	for(unsigned int i = 0; i < h_fftRealOutput.size(); i++){
		float temp = (h_fftRealOutput[i] * h_fftRealOutput[i]) + (h_fftImgOutput[i] * h_fftImgOutput[i]);
		fft_magnitude[i] = sqrtf((float)temp);
	}
}

//////////////////////////////////////////////////////////////////////////////
// Normalize Fast Fourier Transform (FFT)
//////////////////////////////////////////////////////////////////////////////
void normalize_fft(vector<float> &fft_magnitude){
	float max_intensity = FLT_MIN;
	unsigned int i = 0;
	for(i = 0; i < fft_magnitude.size(); i++){
		if(fft_magnitude[i] > max_intensity){
			max_intensity = fft_magnitude[i];
		}
	}

	for(i = 0; i < fft_magnitude.size(); i++){
		fft_magnitude[i] = fft_magnitude[i]/max_intensity;
	}
}

//////////////////////////////////////////////////////////////////////////////
// Sort Magnitude values of Fast Fourier Transform (FFT)
//////////////////////////////////////////////////////////////////////////////
void sort_fft_value(vector<float> &fft_magnitude){
	float temp_fft_value = 0;
	for(unsigned int i = 0; i < fft_magnitude.size() ; i++){
		for(unsigned int j = 0; j < fft_magnitude.size()-1 ; j++){
			if(fft_magnitude[j] < fft_magnitude[j+1]){
				temp_fft_value = fft_magnitude[j+1];
				fft_magnitude[j+1] = fft_magnitude[j];
				fft_magnitude[j] = temp_fft_value;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// Write Edge Spread Function / Edge Response Function data into Excel
//////////////////////////////////////////////////////////////////////////////
void write_ERF_to_excel(vector<float> &dist, vector<float> &intensity){
	ofstream ex_file;
    ex_file.open("ValuesESF.csv");
    if(dist.size() == intensity.size()){
		for (unsigned int i=0; i < dist.size(); i++){
			ex_file << dist[i] << "," << intensity[i] << endl;
		}
	}
    ex_file.close();
}

//////////////////////////////////////////////////////////////////////////////
// Write Point Spread Function data into Excel
//////////////////////////////////////////////////////////////////////////////
void write_PSF_to_excel(vector<float> &dist, vector<float> &intensity){
	ofstream ex_file;
    ex_file.open("ValuesPSF.csv");
    if(dist.size() == intensity.size()){
    		for (unsigned int i=0; i < dist.size(); i++){
    			ex_file << dist[i] << "," << intensity[i] << endl;
    		}
	}
    ex_file.close();
}

//////////////////////////////////////////////////////////////////////////////
// Write Spatial Frequency Response / Modulation Transfer Function data into
// Excel
//////////////////////////////////////////////////////////////////////////////
void write_MTF_to_excel(vector<float> &fft_magnitude, float frequency){
	ofstream ex_file;
	ex_file.open("ValuesMTF.csv");
	float interval = 0;
	for (unsigned int i=0; i < fft_magnitude.size(); i++){
		ex_file << interval << "," << fft_magnitude[i] << endl;
		interval += frequency;
	}
	ex_file.close();
}

//////////////////////////////////////////////////////////////////////////////
// Main Function
//////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {

	// Declarations

	vector<float> dist;
	vector<float> intensity;

	vector<float> averaged_distance;
	vector<float> averaged_intensity;

	vector<float> hammingVector;

	vector<float> erf_cubic_fit_distance;
	vector<float> erf_cubic_fit_intensity;

	vector<float> psf_distance;
	vector<float> psf_intensity;

	vector<double> fft_magnitude;

	unsigned int bin_size = 25;
	float frequency = 0.1;

	// Create a context
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	if (platforms.size() == 0) {
		std::cerr << "No platforms found" << std::endl;
		return 1;
	}
	int platformId = 0;
	for (size_t i = 0; i < platforms.size(); i++) {
		if (platforms[i].getInfo<CL_PLATFORM_NAME>() == "AMD Accelerated Parallel Processing") {
			platformId = i;
			break;
		}
	}

	cl_context_properties prop[4] = { CL_CONTEXT_PLATFORM, (cl_context_properties) platforms[platformId] (), 0, 0 };
	std::cout << "Using platform '" << platforms[platformId].getInfo<CL_PLATFORM_NAME>() << "' from '" << platforms[platformId].getInfo<CL_PLATFORM_VENDOR>() << "'" << std::endl;
	cl::Context context(CL_DEVICE_TYPE_GPU, prop);

	// Get the first device of the context
	std::cout << "Context has " << context.getInfo<CL_CONTEXT_DEVICES>().size() << " devices" << std::endl;
	cl::Device device = context.getInfo<CL_CONTEXT_DEVICES>()[0];
	std::vector<cl::Device> devices;
	devices.push_back(device);
	OpenCL::printDeviceInfo(std::cout, device);

	// Create a command queue
	cl::CommandQueue queue(context, device, CL_QUEUE_PROFILING_ENABLE);

	// Load the source code
	cl::Program program = OpenCL::loadProgramSource(context, "src/OpenCLExercise3_Sobel.cl");

	// Compile the source code. This is similar to program.build(devices) but will print more detailed error messages
	// This will pass the value of wgSize as a preprocessor constant "WG_SIZE" to the openCL C compiler
	OpenCL::buildProgram(program, devices);

	//Read Image
	MTFReadImage1();

	// ESF Calculation
	// Create a kernel object
	cl::Kernel sobelKernel(program, "sobelKernel");

	// Declare some values
	std::size_t wgSizeX = 16; 						// Number of work items per work group in X direction
	std::size_t wgSizeY = 16;
	std::size_t countX = roi.cols;  	 			// Overall number of work items in X direction = Number of elements in X direction
	std::size_t countY = roi.rows;
	std::size_t count = countX * countY; 			// Overall number of elements
	std::size_t size = count * sizeof (cl_uint); 	// Size of data in bytes

	// Allocate space for output data from CPU and GPU on the host
	std::vector<float> h_input (count);
	std::vector<float> h_inputEdge (count);
	std::vector<float> h_outputInten (count);
	std::vector<float> h_outputDist (count);

	// Allocate space for output data on the device
	cl::Buffer d_input(context, CL_MEM_READ_WRITE, size);
	cl::Buffer d_inputEdge(context, CL_MEM_READ_WRITE, size);
	cl::Buffer d_distance(context, CL_MEM_READ_WRITE, size);
	cl::Buffer d_intensity(context, CL_MEM_READ_WRITE, size);

	// Initialize memory to 0xff (useful for debugging because otherwise GPU memory will contain information from last execution)
	memset(h_outputInten.data(), 255, size);
	memset(h_outputDist.data(), 255, size);

	//////// Load input data ////////////////////////////////
	{
		for (size_t j = 0; j < countY; j++) {
			for (size_t i = 0; i < countX; i++) {
				h_input[i + countX * j] = roi.at<uchar>(j,i);
				h_inputEdge[i + countX * j] = edges.at<uchar>(j,i);
			}
		}
	}

	// Copy input data to device
	queue.enqueueWriteBuffer(d_input, true, 0, size, h_input.data(), NULL);
	queue.enqueueWriteBuffer(d_inputEdge, true, 0, size, h_inputEdge.data(), NULL);

	// Launch kernel on the device
	sobelKernel.setArg<cl::Buffer>(0, d_input);
	sobelKernel.setArg<cl::Buffer>(1, d_inputEdge);
	sobelKernel.setArg<cl::Buffer>(2, d_distance);
	sobelKernel.setArg<cl::Buffer>(3, d_intensity);
	sobelKernel.setArg<float>(4, A);
	sobelKernel.setArg<float>(5, B);
	sobelKernel.setArg<float>(6, C);

	queue.enqueueNDRangeKernel(sobelKernel,0,cl::NDRange(countX, countY),cl::NDRange(wgSizeX,wgSizeY),NULL);

	// Copy output data back to host
	queue.enqueueReadBuffer(d_distance, true, 0, size, h_outputDist.data(), NULL);
	queue.enqueueReadBuffer(d_intensity, true, 0, size, h_outputInten.data(), NULL);

	sort_pixel_by_ascending_distance(h_outputDist, h_outputInten);            									 // To remove unwanted lines
	image_binning_averaging(h_outputDist, h_outputInten, bin_size, averaged_distance, averaged_intensity);    // To smooth the curve
	write_ERF_to_excel(averaged_distance, averaged_intensity);
	h_outputDist.clear();
	h_outputInten.clear();
	cout << "ERF obtained" << endl;

	// PSF Calculation
	// Declare some values
	std::size_t wgSize = 256; 						// Number of work items per work group in X direction
	std::size_t countt = averaged_intensity.size(); // Overall number of elements
	std::size_t sizee = countt * sizeof (cl_uint);  // Size of data in bytes

	// Create a kernel object
	cl::Kernel differ(program, "differ");

	// Allocate space for output data from CPU and GPU on the host
	std::vector<float> h_outputDiffInten (countt);
	std::vector<float> h_outputDiffDist (countt);

	// Allocate space for output data on the device
	cl::Buffer d_inputDiffInten(context, CL_MEM_READ_WRITE, sizee);
	cl::Buffer d_inputDiffDist(context, CL_MEM_READ_WRITE, sizee);
	cl::Buffer d_outputDiffInten(context, CL_MEM_READ_WRITE, sizee);
	cl::Buffer d_outputDiffDist(context, CL_MEM_READ_WRITE, sizee);

	// Initialize memory to 0xff (useful for debugging because otherwise GPU memory will contain information from last execution)
	memset(h_outputDiffInten.data(), 255, sizee);
	memset(h_outputDiffDist.data(), 255, sizee);

	// Copy input data to device
	queue.enqueueWriteBuffer(d_inputDiffInten, true, 0, sizee, averaged_intensity.data(), NULL);
	queue.enqueueWriteBuffer(d_inputDiffDist, true, 0, sizee, averaged_distance.data(), NULL);

	// Launch kernel on the device
	differ.setArg<cl::Buffer>(0, d_inputDiffInten);
	differ.setArg<cl::Buffer>(1, d_inputDiffDist);
	differ.setArg<cl::Buffer>(2, d_outputDiffInten);
	differ.setArg<cl::Buffer>(3, d_outputDiffDist);

	queue.enqueueNDRangeKernel(differ, 0, countt, wgSize, NULL);

	// Copy output data back to host
	queue.enqueueReadBuffer(d_outputDiffInten, true, 0, sizee, h_outputDiffInten.data(), NULL);
	queue.enqueueReadBuffer(d_outputDiffDist, true, 0, sizee, h_outputDiffDist.data(), NULL);

	averaged_distance.clear();
	averaged_intensity.clear();
	normalise_psf(h_outputDiffDist, h_outputDiffInten);
	write_PSF_to_excel(h_outputDiffDist, h_outputDiffInten);
	cout << "PSF obtained" << endl;

	// MTF Calculation
	// Allocate space for output data from CPU and GPU on the host
	std::vector<float> h_fftImgInput(h_outputDiffInten.size(), 0.0f);
	std::vector<float> h_fftRealOutput(h_outputDiffInten.size());
	std::vector<float> h_fftImgOutput(h_outputDiffInten.size());
	std::vector<float> h_fftOutput(h_outputDiffInten.size());

	// Initialize memory to 0xff (useful for debugging because otherwise GPU memory will contain information from last execution)
	memset(h_fftRealOutput.data(), 255, h_outputDiffInten.size() * sizeof (float));
	memset(h_fftImgOutput.data(), 255, h_outputDiffInten.size() * sizeof (float));

	// Create a kernel object
	cl::Kernel naivefft2(program, "naivefft2");

	// Allocate space for output data on the device
	cl::Buffer d_fftRealInput(context, CL_MEM_READ_WRITE, h_outputDiffInten.size() * sizeof (float));
	cl::Buffer d_fftImgInput(context, CL_MEM_READ_WRITE, h_outputDiffInten.size() * sizeof (float));
	cl::Buffer d_fftRealOutput(context, CL_MEM_READ_WRITE, h_outputDiffInten.size() * sizeof (float));
	cl::Buffer d_fftImgOutput(context, CL_MEM_READ_WRITE, h_outputDiffInten.size() * sizeof (float));

	// Copy input data to device
	queue.enqueueWriteBuffer(d_fftRealInput, true, 0, h_outputDiffInten.size()* sizeof (float), h_outputDiffInten.data() , NULL);
	queue.enqueueWriteBuffer(d_fftImgInput, true, 0, h_outputDiffInten.size()* sizeof (float), h_fftImgInput.data() , NULL);

	// Launch kernel on the device
	naivefft2.setArg<cl::Buffer>(0, d_fftRealInput);
	naivefft2.setArg<cl::Buffer>(1, d_fftImgInput);
	naivefft2.setArg<cl::Buffer>(2, d_fftRealOutput);
	naivefft2.setArg<cl::Buffer>(3, d_fftImgOutput);
	naivefft2.setArg<unsigned int>(4, h_outputDiffInten.size());

	queue.enqueueNDRangeKernel(naivefft2, 0, h_outputDiffInten.size(), cl::NDRange(wgSizeX,wgSizeY), NULL);

	// Copy output data back to host
	queue.enqueueReadBuffer(d_fftRealOutput, true, 0, h_outputDiffInten.size() * sizeof (float), h_fftRealOutput.data(), NULL);
	queue.enqueueReadBuffer(d_fftImgOutput, true, 0, h_outputDiffInten.size() * sizeof (float), h_fftImgOutput.data(), NULL);

	obtain_magnitude_of_fft_values(h_fftRealOutput, h_fftImgOutput, h_fftOutput);
	psf_distance.clear();
	psf_intensity.clear();
	h_fftRealOutput.clear();
	h_fftImgOutput.clear();
	normalize_fft(h_fftOutput);
	sort_fft_value(h_fftOutput);
	write_MTF_to_excel(h_fftOutput, frequency);
	cout << "MTF obtained" << "\n" << endl;
	cout << "Please refer to the Python script for the ESF, PSF, MTF plot" << endl;

	return 0;
}
