#include <ros/ros.h>
#include <tf2_ros/transform_listener.h>
#include <geometry_msgs/TransformStamped.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/JointState.h>
#include <std_msgs/Float32.h>
#include <tf/transform_datatypes.h>
#include <angles/angles.h>

int main(int argc, char** argv){
  ros::init(argc, argv, "my_tf2_listener");

  ros::NodeHandle node;
  //ros::NodeHandle n;

  ros::Publisher joint_pub = node.advertise<sensor_msgs::JointState>("joint_states", 1);
  ros::Publisher degree_pub = node.advertise<std_msgs::Float32>("degree", 1);

  std_msgs::Float32 degree;

  sensor_msgs::JointState joint_state;
  joint_state.name.resize(1);
  joint_state.position.resize(1);

  joint_state.name[0] ="base_to_metacarpal_joint";


  tf2_ros::Buffer tfBuffer;
  tf2_ros::TransformListener tfListener(tfBuffer);

  ros::Rate rate(10.0);
  while (node.ok()){

	joint_state.header.stamp = ros::Time::now();
	//joint_state.position[0] = 1.0;

	//joint_pub.publish(joint_state);

    geometry_msgs::TransformStamped transformStamped;
    try{
      transformStamped = tfBuffer.lookupTransform("intermediate_phalanx", "distal_phalanx",
                               ros::Time(0));
    }
    catch (tf2::TransformException &ex) {
      ROS_WARN("%s",ex.what());
      ros::Duration(1.0).sleep();
      continue;
    }
	//rot-y 
     //ROS_INFO("trans: x >%f<, y >%f<, z >%f< ", transformStamped.transform.translation.x, transformStamped.transform.translation.y, transformStamped.transform.translation.z);

     //ROS_INFO("rot: x >%f<, y >%f<, z >%f<, w >%f< ", transformStamped.transform.rotation.x, transformStamped.transform.rotation.y, transformStamped.transform.rotation.z, transformStamped.transform.rotation.w );
     
     double r,y,p;
     double d1,d2,d3;
     geometry_msgs::Quaternion q = transformStamped.transform.rotation;
     tf::Quaternion tfq;
     tf::quaternionMsgToTF(q, tfq);
     tf::Matrix3x3(tfq).getEulerYPR(y,p,r);
     d1 = angles::to_degrees(y);
     d2 = angles::to_degrees(p);
     d3 = angles::to_degrees(r);
     //ROS_INFO("euler-angles::to_deg y >%f<, p >%f<, r >%f<", d1, d2, d3);

     double r1;
     r1 = angles::from_degrees(d2);
     //ROS_INFO("radians >%f<", (float)r1);

	joint_state.position[0] = (float)r1; // transformStamped.transform.rotation.y;
	joint_pub.publish(joint_state);
	degree.data = (float)d2; //transformStamped.transform.rotation.y;
	degree_pub.publish(degree);

    rate.sleep();
  }
  return 0;
};
