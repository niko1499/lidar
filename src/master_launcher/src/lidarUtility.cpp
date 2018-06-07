#include <ros/ros.h>
#include <ros/master.h>
// PCL specific includes
#include <iostream>
#include <sensor_msgs/PointCloud2.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/io/pcd_io.h>
#include <pcl/filters/statistical_outlier_removal.h>

ros::Publisher pub;

	void 
cloud_cb (const sensor_msgs::PointCloud2ConstPtr& input)
{
	//Callback to echo any reciveed point cloud topics
	//Comment out as needed. Useful for debuging
	ROS_INFO("Lidar Utility: In Callback");

	// Create a container for the data.
	sensor_msgs::PointCloud2 output;

	// Do data processing here...
	output = *input;

	// Publish the data.
	pub.publish (output);
}

	int
main (int argc, char** argv)
{
	//initialize default topics for subscribing and publishing
	const std::string defaultSubscriber("cloud_pcd");
	const std::string defaultPublisher("lidar_utility_points");

	std::string nodeName("Lidar_Utility");//temp name to initialize with

	// Initialize ROS
	ros::init (argc, argv, nodeName);
	ros::NodeHandle nh;

	nodeName = ros::this_node::getName();//update node name

	//Create Parameters on the new name
	const std::string subscriberParamName(nodeName + "/subscriber");
	const std::string publisherParamName(nodeName + "/publisher");

	//Create constants for expected topics
	const std::string potentialSubscription1("/cloud_pcd");
	const std::string potentialSubscription2("/rslidar_points");
	const std::string potentialSubscription3("/pandar_points");


	ROS_INFO("LIDAR UTILITY MASTER LAUNCHER RUNNING");
	ROS_INFO("Node Name: %s",nodeName.c_str());

	//Create variables that control the topic names
	std::string sTopic;
	std::string pTopic;


	//Check if the user specified a subscription topic
	if(nh.hasParam(subscriberParamName)){
		nh.getParam(subscriberParamName,sTopic);
		ROS_INFO("A param has been set **%s** \n         Setting subsceiber to: %s",subscriberParamName.c_str(), sTopic.c_str());
	}else{
		ROS_INFO("No param set. Searching advertised topics for appropriate subscribers...");
		ros::master::V_TopicInfo master_topics;
		ros::master::getTopics(master_topics);


		std::string currentTopic;
		for(ros::master::V_TopicInfo::iterator it = master_topics.begin() ; it != master_topics.end(); it++){
			const ros::master::TopicInfo& info= *it;
			std::cout <<"topic_"<<it - master_topics.begin()<<": " <<info.name<<std::endl;
			currentTopic=info.name;
			if(currentTopic==potentialSubscription1)
			{
				ROS_INFO("Topic found! Subscribing to: %s",currentTopic.c_str());
				sTopic=info.name;
				break;

			}else if(info.name==potentialSubscription2){
				ROS_INFO("Topic found! Subscribing to: %s",currentTopic.c_str());
				sTopic=info.name;
				break;

			}else if(info.name==potentialSubscription3){
				ROS_INFO("Topic found! Subscribing to: %s",currentTopic.c_str());
				sTopic=info.name;
				break;

			}else{
				ROS_INFO("Searching... If no topic if found default is: %s", defaultSubscriber.c_str());
				sTopic=defaultSubscriber;
			}

		}//end of for loop
	}//end of subscription check

	//Check if the user specified a publishing topic
	if(nh.hasParam(publisherParamName)){
		nh.getParam(publisherParamName,pTopic);
		ROS_INFO("A param has been set **%s** \n          Setting publisher to: %s",publisherParamName.c_str(), pTopic.c_str());
	}else{
		pTopic=defaultPublisher;//set to default if not specified
		ROS_INFO("No param set **%s** \n          Setting publisher to: %s",publisherParamName.c_str(), pTopic.c_str());
	}

	//Clears the assigned parameters Without this default will never be used but instead the last spefified topic
	nh.deleteParam(subscriberParamName);
	nh.deleteParam(publisherParamName);

	// Create a ROS subscriber for the input point cloud
	ros::Subscriber sub = nh.subscribe (sTopic.c_str(), 1, cloud_cb);
	ROS_INFO("Subscribing to %s",sTopic.c_str());
	// Create a ROS publisher for the output point cloud
	pub = nh.advertise<sensor_msgs::PointCloud2> (pTopic, 1);
	ROS_INFO("Publishing to %s",pTopic.c_str());

	// Spin
	ros::spin ();
}