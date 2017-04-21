#pragma once

namespace base {

	// Frame_time::instance().current_time();

	class Frame_time {
	public : 
		static Frame_time& instance();
		static const Frame_time& const_instance();

	public :
		void next_frame();

	public :
		unsigned long current_time_ms() const;
		unsigned long delta_time_ms() const; 
		double delta_time_sec() const; 
		double fps() const;

	public :
		static unsigned long time_diff(unsigned long first, unsigned long second);
		
	private :
		Frame_time();

		unsigned long current_time_; 
		unsigned long delta_time_;

		unsigned long frame_count_;
		unsigned long last_fps_sample_;
		double fps_;
	};

}
