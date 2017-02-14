#pragma once
#include <initializer_list>
#include <array>
#include <vector>

namespace graphics {
namespace curve {

	template <typename point_type>
	inline point_type lerp(	const point_type& a, 
							const point_type& b, 
							float t)
	{
		return a*(1.0f - t) + b*t;
	}

	template <typename point_type>
	inline point_type lerp(	const point_type& a, 
							const point_type& b, 
							float t0, 
							float t1)
	{
		// assert: t0 + t1 == 1
		return a*t0 + b*t1;
	}


	template <typename point_type>
	class linear_curve
	{
	public :
		linear_curve(	const point_type& p0,
						const point_type& p1)
			: p0_(p0)
			, p1_(p1)
		{}
		
	public :
		point_type sample(float t) const
		{
			return lerp(p0_, p1_, t); 
		}

	private :
		point_type p0_;
		point_type p1_;

	};

	namespace details {

		template <typename point_type, unsigned count>
		struct inner_loop
		{
			static inline void exec(const point_type* source, 
									point_type* dest, 
									float t0, 
									float t1)
			{
				dest[count] = lerp(	source[counter + 0], 
									source[counter + 1], 
									t0, 
									t1);

				inner_loop<point_type, count - 1>::exec(source, 
														dest, 
														t0, 
														t1);
			}
		};

		template <typename point_type>
		struct inner_loop<point_type, 1>
		{
			static void exec(	const point_type*, 
								point_type*, 
								float, 
								float) { }
		};

		template <typename point_type, unsigned count>
		struct outer_loop
		{
			static inline void exec(const point_type* source, 
									point_type* dest, 
									float t0, 
									float t1)
			{
				inner_loop<point_type, count>::exec(source, 
													dest, 
													t0, 
													t1);

				outer_loop<point_type, count - 1>::exec(source, 
														dest, 
														t0, 
														t1);
			}
		};

		template <typename point_type>
		struct outer_loop<point_type, 1>
		{
			static void exec(	const point_type*, 
								point_type*, 
								float, 
								float) { }
		};
	}


	// bezier curve with fixed number of points given at compile time
	template <typename point_type, unsigned num_points>
	class bezier
	{
	public :
		bezier(std::initializer_list<point_type> points)
			: points_(points)
		{ }

	public : 
		inline point_type sample(float t) const
		{
			std::array<point_type, num_points> work_buf(points_);

			const float t0 = 1.0f - t;
			const float t1 = t;
			details::outer_loop<point_type, num_points - 1>::exec(	&work_buf[0], 
																	&work_buf[0], 
																	t0, 
																	t1);
		}

	private :
		static_assert(!(num_points < 3), "num_points cannot be less than 3");
		std::array<point_type, num_points> points_;
	};

	
	// bezier curve with a dynamic number of points that can be set at runtime
	template <typename point_type>
	class bezier_n
	{
	public :
		bezier_n(std::initializer_list<point_type> points)
			: points_(points)
			, work_buf_(points)
		{ }

		bezier_n(bezier_n&& other)
			: points_(std::move(other.points_))
			, work_buf_(std::move(other.work_buf_))
		{ }

	public :
		point_type sample(float t) const
		{
			work_buf_ = points_;

			const float t0 = 1.0f - t;
			const float t1 = t;

			for (unsigned count = work_buf_.size(); count != 1; --count)
			{
				for (unsigned i = 0; i < count - 1; ++i)
				{
					work_buf_[i] = lerp(work_buf_[i + 0], 
										work_buf_[i + 1], 
										t0, 
										t1);
				}
			}

			return work_buf_[0];
		}

	public :
		const std::vector<point_type>& get_points() const { return points_; }

	private :
		std::vector<point_type> points_;
		mutable std::vector<point_type> work_buf_;
	}; 

	template <typename point_type>
	class bezier_weighted
	{
	public :
		bezier_weighted(std::initializer_list<point_type> points, 
						std::initializer_list<float> weights)
			: curve_(points)
			, weight_(weights)
		{ }

	public :
		point_type sample(float t) const
		{
			return curve_.sample(t) / weight_.sample(t);
		}

	private : 
		bezier_n<point_type> curve_;
		bezier_n<float> weight_;
	};

	template <typename point_type>
	class bezier_quadratic
	{
	public :
		bezier_quadratic(	const point_type& p0, 
							const point_type& p1, 
							const point_type& p2, 
							const float w0 = 1.0f, // weights
							const float w1 = 1.0f, 
							const float w2 = 1.0f)
			: points_{p0, p1, p2}
			, weights_{w0, w1, w2}
		{ }

	public :
		point_type sample(float t) const
		{
			const auto point = sample(	points_[0]*weights_[0], 
										points_[1]*weights_[1], 
										points_[2]*weights_[2], 
										t);

			const auto weight = sample(	weights_[0], 
										weights_[1], 
										weights_[2], 
										t);

			return point/weight;
		}

	private :
		template <typename value_type>
		inline static value_type sample(const value_type& a, 
										const value_type& b, 
										const value_type& c, 
										float t)
		{
			const float t2 = t*t; 
			return a*(1.0f - t)*(1.0f - t) + b*(t - t2)*2.0f + c*t2; 
		}

	public :
		const std::array<point_type, 3>& get_points() const { return points_; }
		const std::array<float, 3>& get_weights() const { return weights_; }

	private :
		std::array<point_type, 3> points_;
		std::array<float, 3> weights_;
	};

	namespace details {

		template <typename point_type>
		bezier_quadratic<point_type> emit_curve(const point_type& first, 
												const point_type& mid, 
												const point_type& last,
												const float corner_weight = 1.0f)
		{
			return bezier_quadratic<point_type>((mid - first)*0.5f + first, 
												mid, 
												(last - mid)*0.5f + mid, 
												1.0f, 
												corner_weight, 
												1.0f);
		}
	}

	template <typename point_type>
	std::vector<bezier_quadratic<point_type>> generate_circular_sequence(	std::initializer_list<point_type> points, 
																			float corner_weight = 1.0f)
	{
		assert(points.size() >= 3);

		auto first = points.begin();
		auto mid = first + 1;
		auto last = mid + 1;

		std::vector<bezier_quadratic<point_type>> curves;

		for (; last != points.end(); ++first, ++mid, ++last)
		{
			curves.push_back(details::emit_curve(*first, *mid, *last, corner_weight));
		}

		//++first;
		//++mid;
		last = points.begin();
		curves.push_back(details::emit_curve(*first, *mid, *last, corner_weight));

		++first;
		mid = points.begin();
		++last;
		curves.push_back(details::emit_curve(*first, *mid, *last, corner_weight)); 

		return curves;
	}


	// Sequence of quadratic bezier curves, sampled as one curve.
	// Buggy! Discard
	template <typename point_type>
	class quadratic_sequence
	{
	public :
		quadratic_sequence(	std::initializer_list<bezier_quadratic<point_type>> curves)
			: curves_{curves}
		{ }

		quadratic_sequence( const std::vector<bezier_quadratic<point_type>>& curves)
			: curves_(curves)
		{ }

		quadratic_sequence(std::vector<bezier_quadratic<point_type>>&& curves)
			: curves_(std::move(curves))
		{ }

	public :
		point_type sample(float t) const
		{ 
			const float num_curves = static_cast<float>(curves_.size());
			const float step = 1.0f / num_curves;
			float t0 = 0.0f;
			for (const auto& curve : curves_)
			{ 
				if (t < (t0 + step))
				{ 
					return curve.sample((t - t0)*num_curves);
				}

				t0 += step;
			}

			return curves_.back().sample(t); 
		}

	private :
		std::vector<bezier_quadratic<point_type> > curves_;
	};

}}
