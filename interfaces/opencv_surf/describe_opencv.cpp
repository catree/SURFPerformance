#include "cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <highgui.h>
#include <ctime>
#include <iostream>
#include <stdio.h>

#include <vector>

using namespace std;
using namespace cv;

std::vector<string> imageNames;

void process( Mat image , FILE *fid , FILE *output)
{
    // location of detected points
    std::vector<KeyPoint> ipts;

    // adjust threshold so that it detects about the same number of features
    // Target is 5220 in graf img1   actual 5251
    //           3429 in bark img1   actual 3867
    //           5676 in leuven img1 actual 5685
    //           6595 in boat img1   actual 7564
    //SurfFeatureDetector detector(110,4,4,false);

    // read in location of points
    while( true ) {
        float x,y;
        float scale,yaw;
        int ret = fscanf(fid,"%f %f %f %f\n",&x,&y,&scale,&yaw);
        if( ret != 4 )
            break;

        KeyPoint p(x,y,scale*20,0);
        ipts.push_back(p);
    }
    // Use OpenCV to detect features because it can't detect orientation when the descriptor is computed
    //    detector.detect(image,ipts);

//    for( size_t i = 0; i < ipts.size(); i++ ) {
//        KeyPoint &p = ipts.at(i);
//        printf("x = %f , y = %f , size = %f , yaw = %f\n",p.pt.x,p.pt.y,p.size,p.angle);
//    }

    printf("Read in a total of %d points.\n",(int)ipts.size());


    // Create Surf Descriptor Object
    SurfDescriptorExtractor extractor;

    printf("points before: %d\n",(int)ipts.size());
    // Extract the descriptors for the ipts
    Mat descriptors;
    extractor.compute( image, ipts, descriptors );

    // output the description
    fprintf(output,"64\n");
    for( size_t i = 0; i < ipts.size(); i++ ) {
        KeyPoint &p = ipts.at(i);
        fprintf(output,"%7.3f %7.3f %7.5f",p.pt.x,p.pt.y,p.angle);
        for( int j = 0; j < 64; j++ ) {
           float val = descriptors.at<float>(i,j);
           fprintf(output," %0.10f",(float)val);
        }
        fprintf(output,"\n");
    }

    printf("Done\n");
}

int main( int argc , char **argv )
{
    if( argc < 3 )
        throw std::runtime_error("[directory] [detected suffix]");

    char *nameDirectory = argv[1];
    char *nameDetected = argv[2];

    char filename[1024];

    printf("directory name: %s\n",nameDirectory);
    printf(" detected name: %s\n",nameDetected);

    for( int i = 1; i <= 6; i++ ) {
        sprintf(filename,"%s/DETECTED_img%d_%s.txt",nameDirectory,i,nameDetected);
        FILE *fid = fopen(filename,"r");
        if( fid == NULL ) {
            fprintf(stderr,"Couldn't open file: %s\n",filename);
            throw std::runtime_error("Failed to open");
        }
        sprintf(filename,"%s/img%d.png",nameDirectory,i);
        Mat img = imread( filename, CV_LOAD_IMAGE_GRAYSCALE );
//        if( img == NULL ) {
//            fprintf(stderr,"Couldn't open image file: %s\n",filename);
//            throw std::runtime_error("Failed to open");
//        }

        sprintf(filename,"%s/DESCRIBE_img%d_%s.txt",nameDirectory,i,"OpenCV_SURF");
        FILE *output = fopen(filename,"w");
        if( fid == NULL ) {
            fprintf(stderr,"Couldn't open file: %s\n",filename);
            throw std::runtime_error("Failed to open");
        }

        printf("Processing %s\n",filename);
        process(img,fid,output);

        fclose(fid);
    }

}
