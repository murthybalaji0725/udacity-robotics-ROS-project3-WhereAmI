#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

using namespace std;

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    ROS_INFO_STREAM("Robot is moving");
 
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    if (!client.call(srv))
	{
    	ROS_ERROR("Failed to call service command_robot");
	}
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    int white_pixel = 255;
    int sphere_position_sum = 0;
    int white_pixels = 0;

    int sphere_pixel_pos;
    int sphere_location;
 
    int i;
		//checking if red_pixel=green_pixel=blue_pixel = white or 255 in the if statement
    for (i = 0; i + 2 < img.data.size(); i = i + 3)
    {
        if ((img.data[i] == 255) && (img.data[i+1] == 255) && (img.data[i+2] == 255))
        {
            sphere_pixel_pos = (i % (img.width * 3)) / 3;
            sphere_position_sum += sphere_pixel_pos;
            white_pixels++;
        }
    }
    
    if (white_pixels == 0) //when no white ball is present infront of camera then the bot should stop, if not then check from each section divided equally based on size of width. Checking done form left to right to see in which section does the ball lie and to move it accordingly in that direction. 
    {
        drive_robot(0, 0);
    }
    else
    {
        sphere_location = sphere_position_sum / white_pixels;
        
        if(sphere_location < img.width / 3) //move left
        {
            drive_robot(0.15, 0.6);
	    ROS_INFO_STREAM("left");
        }
        else if(sphere_location > img.width * 2 / 3) //move right
        {
            drive_robot(0.15, -0.6);
	    ROS_INFO_STREAM("right");
        }
        else
        {
            drive_robot(0.2, 0);
	    ROS_INFO_STREAM("centre"); //move forward
        }
    }

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
