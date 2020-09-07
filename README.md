# GPU Programming
Implementation of MTF measurement on Slanted Edge: ISO 12233:2017

# Project Description
Modulation transfer function (MTF) is a commonly used measure to compare the performance of optical systems. While multiple techniques can be employed to estimate MTF, the preferred routine for computing the spatial resolution performance of photographic equipment and X-ray CT systems is the slanted edge method. As specified in ISO standard 12233, this method calculates MTF as a function of the vertical spatial frequencies by analyzing a user-defined rectangular region of interest (ROI) in an image of an extracted slanted-edge target. Next, pixelbinning technique is applied to plot the Edge Spread Function (ESF). Lastly, this ESF is differentiated and Fourier-transformed to estimate MTF. To achieve such high data parallelism in CPU, the speedup factor gets limited to the number of cores. With a motive to accelerate execution time, this project outlines the performance of slanted-edge method to estimate MTF between GPU and CPU.

# Instructions for Implementation
- The MTF Implementation code is present in "OpenCLExercise3_Sobel/src". 
- Run "OpenCLExercise3_Sobel.cpp". Additional libraries required would be opencv_core, opencv_imgproc, opencv_highgui, opencv_ml availabe at "/usr/lib/x86_64-linux-gnu".
- Excel files would haven been generated in the project folder after successful execution.
- use "plot.py" python script to plot the ESF, PSF, MTF curve respectively.
- Documentation of the project including results and runtimes of GPU, CPU can be found in MTF_lab_Report_with_RMSE_results.pdf

NOTE - Since the creation of new project for openCL implementation is creating many errors, an existing project template "OpenCLExercise3_Sobel" is used for implementation of MTF.

