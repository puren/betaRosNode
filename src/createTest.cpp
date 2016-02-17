/*#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl_ros/point_cloud.h>
#include "std_msgs/String.h"

#include <iostream>

ros::Publisher pubInit;
ros::Publisher pubFinal;

void createInitPC_cb(const sensor_msgs::PointCloud2ConstPtr& input)
{
	pcl::PointCloud<pcl::PointXYZ> cloud;
	
	int width  = 25;
	int depth  = 25;
	int height = 1;
	
	cloud.width    = 25*25*1;
  	cloud.height   = 1;
  	
  	cloud.is_dense = false;
    cloud.points.resize (cloud.width * cloud.height);
    
    int i=0;
    
    for (int z=0; z<height; z++)
    {
    	for(int y=0; y<depth; y++)
    	{
    		for(int x=0; x<width; x++)
    		{
    			i = (z*depth + y) * width + x;
    			cloud.points[i].x = x;
    			cloud.points[i].y = y;
    			cloud.points[i].z = z;
    		}
    	}
    }
    cloud.points[0].data[3] = 1;
    
    
    sensor_msgs::PointCloud2 output;
    pcl::toROSMsg(cloud, output);
    
    pubInit.publish(output);
    
    
    std::cout<<"initial PC ic created"<<std::endl;
}

void createFinalPC_cb()
{
	pcl::PointCloud<pcl::PointXYZI> cloud;
	
	int width  = 25;
	int depth  = 25;
	int height = 1;
	
	cloud.width    = 25*25*1;
  	cloud.height   = 1;
  	
  	cloud.is_dense = false;
    cloud.points.resize (cloud.width * cloud.height);
    
    int i=0;
    
    for (int z=0; z<height; z++)
    {
    	for(int y=0; y<depth; y++)
    	{
    		for(int x=0; x<width; x++)
    		{
    			i = (z*depth + y) * width + x;
    			cloud.points[i].x = x;
    			cloud.points[i].y = y;
    			cloud.points[i].z = z;
    		}
    	}
    }
    cloud.points[0].data[3] = 1;
    
    //sensor_msgs::std_msgs::String msg;
   
    std::stringstream ss;
    ss << "hello world " << count;
    msg.data = ss.str();PointCloud2 output;
    //pcl_conversions::fromPCL(cloud, output);
    
    std_msgs::String msg;
    std::stringstream ss;
    ss << "hello world " << 0;
    msg.data = ss.str();
    
    pubFinal.publish(msg);
    
    std::cout<<"final PC ic created"<<std::endl;
}

int main (int argc, char** argv)
{
	ros::init (argc, argv, "createTest");
	ros::NodeHandle nh;
	
	ros::Subscriber sub = nh.subscribe ("input", 1, createInitPC_cb);
	
	pubInit = nh.advertise<sensor_msgs::PointCloud2> ("init", 1);
	//pubFinal = nh.advertise<pcl::PointCloud<pcl::PointXYZI> > ("final", 1);
	//pubFinal = nh.advertise<std_msgs::String> ("final", 1);
	
	ros::spin ();
	
}*/

#include <ros/ros.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
#include <math.h>  
#include <iostream>


typedef pcl::PointCloud<pcl::PointXYZI> PointCloud;

int main(int argc, char** argv)
{
  ros::init (argc, argv, "pub_pcl");
  ros::NodeHandle nh;
  ros::Publisher pub1 = nh.advertise<PointCloud> ("points_init", 1);
  
  ros::Publisher pub2 = nh.advertise<PointCloud> ("points_def", 1);

  PointCloud::Ptr msg_init (new PointCloud);
  msg_init->header.frame_id = "/map";
  
  PointCloud::Ptr msg_def (new PointCloud);
  msg_def->header.frame_id = "/map";
  
  //PointCloud::Ptr msg2 (new PointCloud);
  //msg2->header.frame_id = "/map";
  
  int width = 5; 
  int height = 2; 
  int depth = 5; 
  
  msg_init->is_dense = false;
  msg_init->points.resize (width*height*depth);  
  msg_init->width    = msg_init->points.size (); 
  msg_init->height   = 1;
  
  msg_def->is_dense = false;
  msg_def->points.resize (width*height*depth);  
  msg_def->width    = msg_def->points.size (); 
  msg_def->height   = 1;
  
  //msg2->is_dense = false;
  //msg2->points.resize (width*height*depth);  
  //msg2->width    = msg2->points.size (); 
  //msg2->height   = 1;

  /*for (size_t i = 0; i < msg->points.size (); ++i)
  {
    msg->points[i].x = 1024 * rand () / (RAND_MAX + 1.0f);
    msg->points[i].y = 1024 * rand () / (RAND_MAX + 1.0f);
    msg->points[i].z = 1024 * rand () / (RAND_MAX + 1.0f);
  }*/
  
  double step = 1;
  int p = 0; 
  double z_def = 0;
  double z_init = 0;
  double x_init = 0;
  double y_init = 0;
  double a = 5*step;
  double b = 0;
  double c = 2*step;
  double meanx = 2*step;
  double meany = 2*step;
  
  int count = 0;
  
  for (size_t i = 0; i < width; i++) 
    for (size_t j = 0; j < depth; j++) 
      for (size_t k = 0; k < height; k++, p++) 
      { 
      
      	x_init = i*step;
      	y_init = j*step;
      	z_init = 0.001+k*step;
      	z_def  = z_init- a*exp(-(pow((x_init-meanx),2)/(2*pow(c,2))) - (pow((y_init-meany),2)/(2*pow(c,2))) );
      	std::cout<< x_init<<" "<< y_init<<" "<< std::setprecision(10) <<z_def<<" "<<count++<<std::endl;
      	
      	//std::cout<< x_init<<" "<< y_init<<" "<< std::setprecision(10) <<z_init<<" "<<count++<<std::endl;
         
        msg_init->points[p].x = x_init;
        msg_init->points[p].y = y_init;
        msg_init->points[p].z = z_init;
        
        msg_def->points[p].x = x_init;
        msg_def->points[p].y = y_init;
        msg_def->points[p].z = z_def;
        
        //std::cout<<z_def<<std::endl;
        
        
        //msg2->points[p].x=i*0.1;
        //msg2->points[p].y=j*0.1;
      
        
        //msg2->points[p].z=z_def;
         
      } 
  
 // msg->points.push_back (pcl::PointXYZ(1.0, 2.0, 3.0));

  ros::Rate loop_rate(4);
  while (nh.ok())
  {
    msg_init->header.stamp = ros::Time::now().toNSec();
    pub1.publish (msg_init);
    
    msg_def->header.stamp = ros::Time::now().toNSec();
    pub2.publish (msg_def);
    
    //msg2->header.stamp = ros::Time::now().toNSec();
    //pub.publish (msg2);
    
    ros::spinOnce ();
    loop_rate.sleep ();
  }
}
