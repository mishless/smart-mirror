# Smart Mirror

Smart Mirror project combines image processing techniques and machine learning approaches in onrder to provide functionality for face detection and tracking, face recogntion and authentication as well as non-invasive extraction of heartbeat, inter-bit-interval, respiration rate, blood pressure and drawsiness. More details on the implementation are coming soon. 

##Project Setup

Follow the steps below in order to setup the project.

1. Install Visual Studio 2015 IDE.
2. Clone the Smart Mirror project and open it in Visual Studio.
3. Download and install [OpenCV](https://sourceforge.net/projects/opencvlibrary/?source=top3_dlp_t5) simply by unzipping it somewhere in the filesystem. Remember where!
4. Download and install [Boost](https://sourceforge.net/projects/boost/files/boost/1.60.0/) libraries simply by unzipping it somewhere in the filesystem. Remember where!
5. Download and install [pthreads](https://www.sourceware.org/pthreads-win32/) library for Windows simply by unzipping it somewhere in the filesystem. Remember where!
6. Create an environment variable OPENCV_DIR that point to PathToOpenCV\Build\x64\vc14, where PathToOpenCV is the path to where you unzipped OpenCV.
6. Create an environment variable BOOST_DIR that points to where you unzipped it e.g. C:\Boost.
7. Create an environment variable PTHREAD_DIR that points to PathToPthread\Pre-built.2, where PathToPthread is the path to where you unzipped pthreads files.
8. In the project propterties set in "Configuration Properties" --> "Debugging" --> "Enviroment" --> PATH=$(PTHREAD_DIR)\dll\x64;%PATH%
9. In the general Boost directory search for a bootstrap.bat script and execute it.
10. After it finished it will generate two files: b2.exe and bjam.exe.
11. Run b2.exe.
12. Run bjam.exe.
13. Pray and try to build. May the force be with you.
>>>>>>> a4244f2b0bb090ffb1805c50d99b5573f677c1ac
