/*
This program demonstrate how to use hps communicate with FPGA through light AXI Bridge.
uses should program the FPGA by GHRD project before executing the program
refer to user manual chapter 7 for details about the demo
*/

#include "include/main.h"
 

int main()
{
	
	int i=0;
	bool stop_walk = false;
	sensor.fall_Down_Flag_ = false;
	sensor.stop_Walk_Flag_ = false;

	balance.initialize(30);
	usleep(1000 * 1000);
	init.initial_system();
	usleep(1000 * 1000);
	IK.initial_inverse_kinematic();
	walkinggait.initialize();

	gettimeofday(&walkinggait.timer_start_, NULL);

	while(1)
	{
    	// printf(" ");
		// printf("rpy[0]_ = %f, rpy[1]_ = %f, rpy[2]_ = %f\n", sensor.rpy_[0],sensor.rpy_[1],sensor.rpy_[2]);
		datamodule.load_database();
		if(datamodule.motion_execute_flag_)
			datamodule.motion_execute();
		sensor.load_imu();
		// sensor.load_press_left();
		// sensor.load_press_right();

		// HandIK.load_hand_data();
		//printf("hx = %lf hy = %lf hz = %lf\n",HandIK.hand_point_[0],HandIK.hand_point_[1],HandIK.hand_point_[2]);
		// HandIK.Kinetic_Main(HandIK.hand_point_[0],HandIK.hand_point_[1],HandIK.hand_point_[2]);

		// test.Kinetic_Main(inxyz[0], inxyz[1], inxyz[2]);

 		// printf(" ");
		// sleep(1);
		sensor.load_sensor_setting();
		sensor.sensor_package_generate();
		walkinggait.load_parameter();
		walkinggait.load_walkdata();
		walkinggait.calculate_point_trajectory();

 		// printf(" ");
		gettimeofday(&walkinggait.timer_end_, NULL);
		walkinggait.timer_dt_ = (double)(1000000.0 * (walkinggait.timer_end_.tv_sec - walkinggait.timer_start_.tv_sec) + (walkinggait.timer_end_.tv_usec - walkinggait.timer_start_.tv_usec));

		balance.get_sensor_value();

		if (balance.two_feet_grounded_ && sensor.fall_Down_Flag_)
		{
			sensor.stop_Walk_Flag_ = true;
		}
		else
		{
			sensor.stop_Walk_Flag_ = false;
		}
		if((walkinggait.timer_dt_ >= 30000.0))// && !sensor.stop_Walk_Flag_)
		{
			walkinggait.walking_timer();

			gettimeofday(&walkinggait.timer_start_, NULL);
			// balance.balance_control();
		}
		// printf("\nwalking_stop = %d\n", parameterinfo->complan.walking_stop);
		// printf("samplepointtmp_ = %d, sample_point_ = %d\n", walkinggait.samplepointtmp_, walkinggait.sample_point_);
		// printf("     next_step_ = %d,     now_step_ = %d, pre_step_ = %d\n\n", walkinggait.next_step_, walkinggait.now_step_, walkinggait.pre_step_);
		// printf("    footstep_ = %d\n", walkinggait.footstep_);
		// printf("get_walkdata_ = %d\n\n", walkinggait.get_walkdata_);
		// printf("           X = %f,             y = %f,            z = %f\n", parameterinfo->X, parameterinfo->Y, parameterinfo->Z);
		// printf("step_length_ = %f, shift_length_ = %f, step_height_ = %f\n\n", walkinggait.step_length_, walkinggait.shift_length_, walkinggait.step_height_);
		// printf(" last_step_length_ = %f, last_length_ = %f, now_length_ = %f\n", walkinggait.last_step_length_, walkinggait.last_length_, walkinggait.now_length_);
		// printf("last_shift_length_ = %f,  last_shift_ = %f,  now_shift_ = %f\n\n\n", walkinggait.last_shift_length_, walkinggait.last_shift_, walkinggait.now_shift_);
 		// printf(" ");
		// usleep(500 * 1000); 
		if((walkinggait.locus_flag_))
		{
 
			balance.setSupportFoot();
			balance.endPointControl();
			if(walkinggait.LIPM_flag_)
			{
				balance.balance_control();
			}
			locus.get_cpg_with_offset();

			IK.calculate_inverse_kinematic(walkinggait.motion_delay_);
			locus.do_motion();
			walkinggait.LIPM_flag_ = false;
			// printf("walkinggait.footstep_ = %d\n", walkinggait.footstep_);
			walkinggait.send_footstep_to_ipc();
			walkinggait.locus_flag_ = false;
		}
	}

		if(walkinggait.plot_once_ == true)
		{
			balance.saveData();
			IK.saveData();
			balance.resetControlValue();
			walkinggait.plot_once_ = false;
		}
	

	// clean up our memory mapping and exit
	init.Clear_Memory_Mapping();

	return( 0 );
}
