#ifndef KDL_TRAINING_KDL_TRANSFORM_IK_HPP
#define KDL_TRAINING_KDL_TRANSFORM_IK_HPP

#include <ros/ros.h>
#include <kdl_parser/kdl_parser.hpp>
#include <kdl/chainiksolverpos_lma.hpp>
#include <kdl_training/kdl_functions.hpp>
#include <kdl/frames_io.hpp>

namespace kdl_training
{
	class Transform_ik
	{
	    public:
		Transform_ik(const ros::NodeHandle& nh): nh_(nh), base_frame_("base_link"), target_frame_("r_gripper_tool_frame"), once_(true)//, translation_(0,0,0), x_rot_(0,0,0), y_rot_(0,0,0), z_rot_(0,0,0), rotation_(x_rot_,y_rot_,z_rot_), goal_frame_(rotation_, translation_)
		{}

		~Transform_ik() {}
		
		void chatterCallback(const sensor_msgs::JointState::ConstPtr& msg)
		{

			if(once_)
			{
				joint_indeces_ = createJointIndices(createIndexMap(msg, joint_names_), joint_names_);
				once_ = false;
			}
			q_out_ = calculateIK(ik_solver_, goal_frame_, toKDL(*msg, joint_indeces_));
			using KDL::operator<<;
		//	ROS_DEBUG_STREAM("IK: " << std::endl << q_out_ << std::endl);
		//	for (size_t i = 0; i < q_out_.rows(); ++i)		
		//		ROS_INFO("%f",q_out_(i)); 
		}

		void createChain(const std::string& robot_desc)
		{
			KDL::Tree my_tree;
		
			if (!kdl_parser::treeFromString(robot_desc, my_tree))
		        	throw std::runtime_error( "Could not construct the tree" );


			if (!my_tree.getChain(base_frame_, target_frame_, chain_))
		                throw std::runtime_error( "Could not construct the chian" );
		}

		void start()
		{			
		        std::string robot_desc;
			nh_.param("robot_description", robot_desc, std::string());
			createChain(robot_desc);
			ik_solver_ = std::make_shared<KDL::ChainIkSolverPos_LMA>(KDL::ChainIkSolverPos_LMA(chain_));	
			joint_names_ = getJointNames(chain_);	
		 	//sub_ = nh_.subscribe("joint_states", 1, &Transform::chatterCallback, this);
	    	}
	
	    private:
		ros::NodeHandle nh_;
		std::shared_ptr<KDL::ChainIkSolverPos_LMA> ik_solver_;
		std::string base_frame_;
		std::string target_frame_;
		KDL::Chain chain_; 
		std::vector<size_t> joint_indeces_;
		std::vector<std::string> joint_names_;		
		bool once_;

		KDL::JntArray q_out_;
		KDL::Frame goal_frame_;
		KDL::Vector translation_;
		KDL::Vector x_rot_;
		KDL::Vector y_rot_;
		KDL::Vector z_rot_;
		KDL::Rotation rotation_;
	};
}

#endif // KDL_TRAINING_KDL_TRANSFORM_HPP
