#include <vector>
#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_datatypes.h>

using namespace std;

int main(int argc, char** argv)
{
  ros::init(argc, argv, "simple_navigation_goals");

  actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> ac("move_base",true);

  geometry_msgs::Quaternion quaternions[4];

  double angle = M_PI/2;
  int angle_count = 0;
  for(angle_count = 0; angle_count < 4;angle_count++ )
  {
      quaternions[angle_count] = tf::createQuaternionMsgFromRollPitchYaw(0, 0, angle);
      angle = angle + M_PI/2;
  }
  const double square_size = 1.0;
  const double offset = 1.0;

  geometry_msgs::Point point;
  geometry_msgs::Pose pose_list[4];
  point.x = square_size + offset;
  point.y = 0.0 + offset;
  point.z = 0.0;
  pose_list[0].position = point;
  pose_list[0].orientation = quaternions[0];

  point.x = square_size + offset;
  point.y = square_size + offset;
  point.z = 0.0;
  pose_list[1].position = point;
  pose_list[1].orientation = quaternions[1];

  point.x = 0.0 + offset;
  point.y = square_size + offset;
  point.z = 0.0;
  pose_list[2].position = point;
  pose_list[2].orientation = quaternions[2];

  point.x = 0.0 + offset;
  point.y = 0.0 + offset;
  point.z = 0.0;
  pose_list[3].position = point;
  pose_list[3].orientation = quaternions[3];


  ROS_INFO("Waiting for the move_base action server to come up");

  if(!ac.waitForServer(ros::Duration(60)))
  {
    ROS_INFO("Can't connected to move base server");
    return 1;
  }

  int idx = 0;
  while(ros::ok())
  {

     move_base_msgs::MoveBaseGoal goal;
     goal.target_pose.header.frame_id = "map";
     goal.target_pose.header.stamp = ros::Time::now();
     goal.target_pose.pose = pose_list[idx];

     ac.sendGoal(goal);

    if(!ac.waitForResult(ros::Duration(180)))
    {
        ac.cancelGoal();
        ROS_INFO("Timed out achieving goal");
    }
    else
    {
        //We made it!
        if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        {
            ROS_INFO("Goal%d succeeded!", idx);
        }
        else
        {
            ROS_INFO("The base failed to move for some reason");
        }
    }

    idx += 1;
    if (idx > 3)
       break;

    ros::spinOnce();
  }

  ROS_INFO("Hooray, All goals succeeded!");

  return 0;
}