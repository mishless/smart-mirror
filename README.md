# Smart Mirror

Smart Mirror project combines image processing techniques and machine learning approaches in onrder to provide functionality for face detection and tracking, face recogntion and authentication as well as non-invasive extraction of heartbeat, inter-bit-interval, respiration rate, blood pressure and drawsiness. More details on the implementation are coming soon. 

In order to setup the project the following steps need to be followed:

1. Visual Studio 2015 IDE should be installed prior to every other step.
2. Clone the Smart Mirror project and open it in Visual Studio
3. Download and install [OpenCV](https://sourceforge.net/projects/opencvlibrary/?source=top3_dlp_t5) simply by unzipping it somewhere in the filesystem. Remember where!
4. Download and install [Boost](https://sourceforge.net/projects/boost/files/boost/1.60.0/) libraries simply by unzipping it somewhere in the filesystem. Remember where!
5. Download and install [pthreads](ftp://sourceware.org/pub/pthreads-win32/pthreads-w32-2-9-1-release.zip) library for Windows simply by unzipping it somewhere in the filesystem. Remember where!
6. Create an environment variable OPENCV_DIR that point to PathToOpenCV\Build\x64\vc14, where PathToOpenCV is the path to where you unzipped OpenCV.
6. Create an environment variable BOOST_DIR that points to where you unzipped it e.g. C:\Boost.
7. Create an environment variable PTHREAD_DIR that points to PathToPthread\Pre-built.2, where PathToPthread is the path to where you unzipped pthreads files.
8. In the general Boost directory search for a bootstrap.bat script and execute it.
9. After it finished it will generate two files: b2.exe and bjam.exe.
10. Run b2.exe.
11. Run bjam.exe.
12. Prey and try to build. May the force be with you.
