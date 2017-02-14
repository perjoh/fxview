#pragma once

namespace graphics
{
	namespace curve
	{
		//
		template <typename curve_type>
		void render_curve(	const curve_type& c, 
							unsigned num_samples, 
							const glm::mat4& m)
		{
			::glLoadMatrixf(&m[0][0]);
			::glColor3f(1.0f, 1.0f, 1.0f);
			::glBegin(GL_POINTS);
			const auto& points = c.get_points();
			for (unsigned i = 0; i < points.size() - 1; ++i)
			{
				const auto& p0 = points[i + 0];
				::glVertex3f(p0.x, p0.y, p0.z);
				const auto& p1 = points[i + 1];
				::glVertex3f(p1.x, p1.y, p1.z);
			}

			::glColor3f(0.0f, 1.0f, 0.0f);
			for (unsigned i = 0; i < num_samples; ++i)
			{ 
				const float t0 = static_cast<float>(i + 0) / static_cast<float>(num_samples);
				auto p0 = c.sample(t0);
				::glVertex3f(p0.x, p0.y, p0.z);
				const float t1 = static_cast<float>(i + 1) / static_cast<float>(num_samples);
				auto p1 = c.sample(t1);
				::glVertex3f(p1.x, p1.y, p1.z); 
			}

			::glEnd();
		}

		//
		template <typename curve_type>
		void render_curves(	const curve_type& curves, 
							unsigned num_samples, 
							const glm::mat4& m)
		{
			for (const auto& curve : curves)
			{
				render_curve(curve, num_samples, m);
			}
		}

	}
}
