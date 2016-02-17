#include <iostream>
#include <fstream>
#include <string.h>

#include <ros/ros.h>

// PCL specific includes
#include <sensor_msgs/PointCloud2.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Float32.h>
#include <pcl_ros/point_cloud.h>

#include "opencv2/opencv.hpp"
#include <cv.h>
#include "opencv2/highgui/highgui.hpp"

//SM simulation
#include "deformable.h"

using namespace sensor_msgs;
using namespace std_msgs;

pcl::PointCloud<pcl::PointXYZI> cloudInitial;
pcl::PointCloud<pcl::PointXYZI> cloudFinal;

bool isInitPC = false; //true if initial point cloud arrives
bool isFinalPC = false; //true if final point cloud arrives
bool isInSim = false; //true if both  point clouds are obtained and simulation starts
bool isFirst = true; //true if new initial point cloud arrives
int count = 0;

int initID = 0;
int finalID = 0;

int numPoints=0;

Deformable *mDeformable;

typedef pcl::PointCloud<pcl::PointXYZI> PointCloud;

int width = 600;
int height = 600;
int depth = 600;
int owidth = 600;
int oheight = 300;
int odepth = 300;
double worldToScreen = owidth; //1.0;
double screenToWorld = 1.0/worldToScreen;

ros::Publisher pubSim;
ros::Publisher pubBeta;

/********set manipulated particle********/
Eigen::Vector3d posFixed(0.5, 0.5, 0.9);
int idFixed = 609;
/********set fixed boundary**************/
double bt_x=0, bt_y=0, bt_z=0.0, bb_x=1, bb_y=1, bb_z=10;

int idPC=-1;

void initSim()
{
 
    /***************************************
	 * run simulation
	 * estimate a beta
	 ***************************************/
	
	
	mDeformable->initState();
	
	for (int i=0; i<numPoints; i++) {
        Eigen::Vector3d pos;
       	pos = mDeformable->getOriginalVertexPos(i);
        
        
		if( (pos(0)==bt_x || pos(0)==bb_x) || (pos(1)==bt_y || pos(1)==bb_y) )
		{
    		//std::cout<<idx<<" "<<pos(0)<<" "<<pos(1)<<" "<<pos(2)<<std::endl;
    		mDeformable->fixVertex(i, pos);
		}
		//mDeformable->setVertexPos(idx, pos);
    }         
}
 
 
void simulateEstimate()
{
	
	/*isInSim = true;
    
    std::vector<double> dist;
    std::vector<double> beta;
    double range = 0.1;
    double x, y, z, px, py, pz, min_err = 0.0, min_beta = 0.0, resToStop = 0.01, max_beta = 0.99, beta_est;
	int  min_ind= 0, finish=0;
	
	mDeformable->params.beta=0.0;

	Eigen::Vector3d pos;
	
	
	for (int i = 0; i < numPoints; i++) {
        pos = mDeformable->getVertexPos(i);
        //std::cout<<pos(0)<<","<<pos(1)<<","<<pos(2)<<" ";
    }

	

	cv::namedWindow( "View1", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );
	cv::namedWindow( "View2", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );
    cv::Mat image(cv::Size(width,height),CV_8UC3);
    cv::Mat image2(cv::Size(width,height),CV_8UC3);
    
	while (std::abs(range)>0.001) {
        while(mDeformable->params.beta<=max_beta && mDeformable->params.beta>=min_beta){
            
            int k=0;
            
            mDeformable->params.bounds.max(0) = 1;//width * screenToWorld;
			mDeformable->params.bounds.max(1) = height * screenToWorld;
			mDeformable->params.bounds.max(2) = depth * screenToWorld;
			
            initSim();
            
            
            double mMassToRadius = 0.01;
            
            finish=0;
            
            k=0;
            while(1)
            {
            
            
            	image = cv::Mat::zeros(height+20, width,CV_8UC3);
            	image2 = cv::Mat::zeros(height+20, width,CV_8UC3);
                cv::line( image,
                         cv::Point(0,height),cv::Point(width-1,height),
                         cv::Scalar( 255,255 , 255 ),1);
                double res=0.0;
                
                if(k>2)
				{
					//mDeformable->fixVertex(idFixed, posFixed);
				}
                
                
                
                
                for (int i = 0; i < numPoints; i++) {
                    pos = mDeformable->getVel(i);
                    res += sqrt(pow(pos(0),2) + pow(pos(1),2) + + pow(pos(2),2));
                }
                res=res/numPoints;
                
                if(res<resToStop)
                {
                    finish++;
                }
                else
                {
                    finish=0;
                }
                
                for( int i = 0; i<numPoints; i++)
                {
                    pos = mDeformable->getVertexPos(i);
                    
                    double r = 5;//sqrtf(mDeformable->getMass(i) * mMassToRadius) * worldToScreen;
                    x = pos(0)*worldToScreen;
                    y = pos(1)*worldToScreen;
                    z = pos(2)*worldToScreen;
                    
                    y = height - y;
                    z = depth - z;
                    
                    circle(image, cv::Point(x-r, y-r), r, cv::Scalar(255, 255, 255), -1);
                    circle(image2, cv::Point(x-r, z-r), r, cv::Scalar(255, 255, 255), -1);
                    if(mDeformable->isFixed(i))
                    {
                    	 circle(image, cv::Point(x-r, y-r), r, cv::Scalar(0, 0, 255), -1);
                    	 circle(image2, cv::Point(x-r, z-r), r, cv::Scalar(0, 0, 255), -1);
                    }
                    //std::cout<<i<<" "<<pos(0) <<","<< pos(1)<<" "<<pos(2)<<std::endl;
                }
                
                imshow( "View1", image);
                imshow( "View2", image2);
                
                
                int key=cvWaitKey(1);
                
                switch(key){
                	case 'q':
                		exit(1);
                }
                
                if(finish>10 && k>3)
                {
                    break;
                }
                
                mDeformable->timeStep();
                
                k++;
            }
            
           
            double sum = 0.0;
            for (int i = 0; i < numPoints; i++) {
                
                x = cloudFinal.points[i].x;
                y = cloudFinal.points[i].y;
                z = cloudFinal.points[i].z;
                
                pos = mDeformable->getVertexPos(i);
                px = pos(0); py = pos(1); pz = pos(2);
                
                sum += sqrt(pow((x-px), 2) + pow((y-py), 2) + pow((z-pz), 2));
            }
            sum = sum / numPoints;
            
            dist.push_back(sum);
            beta.push_back(mDeformable->params.beta);      
             
            std::cout<<"beta:"<<mDeformable->params.beta<<" difference:"<<sum<<std::endl;
            
            mDeformable->params.beta += range;
            
            std::cout<<"beta:"<<mDeformable->params.beta<<" difference:"<<sum<<std::endl;
            
        }
   		
   		
        std::vector<double>::iterator result = std::min_element(dist.begin(), dist.end());
        min_ind =  std::distance(dist.begin(), result);
        beta_est = beta.at(min_ind);
        min_err = dist.at(min_ind);
         
        int N = dist.size();
        if (min_ind<N-1 && min_ind>0) {
        
            if (std::abs(dist.at(min_ind)-dist.at(min_ind-1)) < std::abs(dist.at(min_ind+1)-dist.at(min_ind))) {
                range = -range/10;
                min_beta = beta.at(min_ind-1);
                max_beta = beta.at(min_ind+1);
                //std::cout<<"aaaa"<<std::endl;
            }
            else if(std::abs(dist.at(min_ind)-dist.at(min_ind-1)) > std::abs(dist.at(min_ind+1)-dist.at(min_ind)))
            {
                range = range/10;
                min_beta = beta.at(min_ind-1);
                max_beta = beta.at(min_ind+1);
               // std::cout<<"aaaa"<<std::endl;
            }
            //std::cout<<std::abs(dist.at(min_ind)-dist.at(min_ind-1))<<" "<<std::abs(dist.at(min_ind)-dist.at(min_ind+1))<<std::endl;
        }
        else if (min_ind == N-1)
        {
            range = -range/10;
            min_beta = beta.at(N-2);
            max_beta = beta.at(N-1);
            //std::cout<<"aaaa"<<std::endl;
        }
        else if (min_ind == 0)
        {
            range = range/10;
            min_beta = beta.at(0);
            max_beta = beta.at(1);
           // std::cout<<"aaaa"<<std::endl;
        }
        dist.clear();
        beta.clear();
        mDeformable->params.beta = beta_est;
        
        std::cout<<"the estimated beta is "<<beta_est<<" range  "<<range<<std::endl;
    }
    
    std::cout<<"\nEstimatin is finished\n";
       
    PointCloud::Ptr msg (new PointCloud);
  	msg->header.frame_id = "/map";
	
	msg->is_dense = false;
  	msg->points.resize (numPoints);  
  	msg->width    = msg->points.size (); 
  	msg->height   = 1;
 
 	for (int i = 0; i < numPoints; i++) {
        pos = mDeformable->getVertexPos(i);
        std::cout<<pos(0)<<","<<pos(1)<<","<<pos(2)<<"\n";
        msg->points[i].x = pos(0);
        msg->points[i].y = pos(1);
        msg->points[i].z = pos(2);
    }
    
    pubSim.publish (msg);
    
    std::cout<<"\nSimulated point cloud is published\n";
	
	Float32 betaPub;
	betaPub.data =beta_est;
	
	pubBeta.publish(betaPub);
	
	std::cout<<"\nBeta is published "<<beta_est<<"\n";*/
	
	float beta_est = ((double) std::rand() / (RAND_MAX));	
		
	Float32MultiArray betaPub;
	betaPub.data.push_back(beta_est);
  	betaPub.data.push_back(idPC);
  	pubBeta.publish(betaPub);
  	
}



//get the initial point cloud. Initialise the simulation
void cloud_cb_initial (const PointCloud2ConstPtr& input)
{
  	//if(!isInSim)
  	{
		//point cloud from ROS is obtained, turned to pcl point cloud
		pcl::PointCloud<pcl::PointXYZI> cloud;
		pcl::fromROSMsg (*input, cloud);
		cloudInitial = cloud;
	
		//id of the point cloud is obtained
		idPC = cloudInitial.points[0].data[3];
	
		//set the initial point cloud ID
		initID = idPC;
	
		numPoints = cloudInitial.size();
	
		isInitPC = true;
	
		std::string f_cur="";
		std::stringstream fPath;
		fPath.str("./data/");
		fPath << "/media/Raid/home/puren/catkin_ws/src/deformablePCL/data/initConfig.txt";
		f_cur = fPath.str();
		std::ofstream f;
		f.open(f_cur.c_str(), std::ofstream::out );
	
		f<<0<<" "<<0<<" "<<0<<"\n";
		f<<25<<" "<<25<<" "<<25<<"\n";
		f<<numPoints<<"\n";
	
		//create the initial configuration file	
		for(int i=0; i<numPoints; i++)
		{
			f <<cloudInitial.points[i].x<<" "<<cloudInitial.points[i].y<<" "<<cloudInitial.points[i].z<<" 0.100000\n";
		}
		f<<"0.01000"<<"\n"; //time step
		f<<"0.000000 0.000000 -9.870000\n";
		f<<"1.000000\n";
		f<<"0.000000\n";
		f<<"0\n";
		f<<"1\n";
		f<<"0\n";
		f<<"1\n";
		f<<"0.010000";
		
		f.close();

		mDeformable = new Deformable();
		mDeformable->loadFromFile("/media/Raid/home/puren/catkin_ws/src/deformablePCL/data/initConfig.txt");
	
	}
}

void cloud_cb_final ( const PointCloud2ConstPtr& input)
{	
	//if(!isInSim)
  	{
		//point cloud from ROS is obtained, turned to pcl point cloud
		pcl::PointCloud<pcl::PointXYZI> cloud;
		pcl::fromROSMsg (*input, cloud);
		cloudFinal = cloud;
	
		//id of the point cloud is obtained
		int idPC = cloud.points[0].data[3];
	
		//set the final point cloud ID
		finalID = idPC;
	
		int final_numPoints = cloudFinal.size();
	
		isFinalPC = true;
	
		std::cout<<isInitPC<<" "<<isFinalPC<<" "<<numPoints<<" "<<final_numPoints<<std::endl;
		if(isInitPC && isFinalPC && (numPoints==final_numPoints)) 
		{
			std::cout<< "Final PC is obtained. NumPoints " << final_numPoints<<std::endl;
			simulateEstimate();
		}
	}
}


int main (int argc, char** argv)
{
	ros::init (argc, argv, "deformablePCL");
	ros::NodeHandle nh;
	ROS_INFO("Starting ");
                
	ros::Subscriber sub1 = nh.subscribe ("points_init", 1, cloud_cb_initial);	
	ros::Subscriber sub2 = nh.subscribe ("points_def", 1, cloud_cb_final);

	
	//pubSim = nh.advertise<PointCloud> ("sim", 1);  
	pubBeta = nh.advertise<	Float32MultiArray> ("beta", 1);  
	
    // Spin
	ros::spin();
	
    return 0;
}