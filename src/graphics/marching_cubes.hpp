#pragma once

namespace graphics {

    const size_t invalid = ~size_t(0);

    struct Edge 
    {
        Edge() = default;

        Edge(size_t a, size_t b)
            : vertex_a(a)
            , vertex_b(b)
            { }

        size_t vertex_a{invalid};
        size_t vertex_b{invalid};
    };

    struct Vertex 
    {
        glm::vec3 position;
        std::vector<Edge> edges;
    };

    const float left = -1.0f;
    const float right = -left;

    const float top = 1.0f;
    const float bottom = -top;

    const float far = -1.0f;
    const float near = -far;

    static const std::array<glm::vec3, 8> points{   {left, top, far},
                                                    {right, top, far},
                                                    {right, top, near},
                                                    {left, top, near},
                                                    {left, bottom, far},
                                                    {right, bottom, far},
                                                    {right, bottom, near},
                                                    {left, bottom, near}};

    static const std::array<Edge, 12> edges{    {0, 1}, // top, far
                                                {1, 2}, // top, right
                                                {2, 3}, // top, near
                                                {3, 0}, // top, left
                                                {4, 5}, // bottom, far
                                                {5, 6}, // bottom, right
                                                {6, 7}, // bottom, near
                                                {7, 4}, // bottom, left
                                                {0, 4}, // far, left
                                                {1, 5}, // far, right
                                                {2, 6}, // near, right
                                                {3, 7}}; // near, left

    typedef std::vector<glm::vec3> Triangles;

    // Geometry is clockwise front facing.
    static const Triangles geometry_table[256] = {
    };

    inline glm::vec3 calc_mid_point(const Edge& edge, const glm::vec3& offset = glm::vec3(0.0f))
    {
        return (points[edge.vertex_a] + points[edge.vertex_b])*0.5f + offset;
    }

    // 
    void merge_edge_index(std::set<size_t>& indices, size_t vertex_index)
    {
        for (size_t edge_index = 0; edge_index < edges.size(); ++edge_index)
        {
            if (edges[edge_index].vertex_a == vertex_index || edges[edge_index].vertex_b == vertex_index)
                indices.insert(vertex_index);
        }
    }

    static const std::array<glm::vec3, 12> mid_points{  calc_mid_point(edges[0]),
                                                        calc_mid_point(edges[1]),
                                                        calc_mid_point(edges[2]),
                                                        calc_mid_point(edges[3]),
                                                        calc_mid_point(edges[4]),
                                                        calc_mid_point(edges[5]),
                                                        calc_mid_point(edges[6]),
                                                        calc_mid_point(edges[7]),
                                                        calc_mid_point(edges[8]),
                                                        calc_mid_point(edges[9]),
                                                        calc_mid_point(edges[10]),
                                                        calc_mid_point(edges[11]) };

    //
    void remove_overlap_vertex(std::vector<size_t>& triangles, size_t overlap_vertex)
    {
        std::vector<size_t> output;

        for (size_t i = 0; i < triangles.size(); ++i)
        { 
            if (triangles[i] != overlap_vertex)
            {
                output.push_back(triangles[i]);
            }
            else
            {
                for (size_t j = i + 1; j < i + 4; ++j)
                {
                    const size_t k = j%triangles.size();
                    if (triangles[k] != overlap_vertex)
                    {
                        output.push_back(triangles[k]);
                        i = k;
                        break;
                    }
                }
            }
        }
    }

    // 
    void remove_overlapping_vertices(std::vector<size_t>& triangles)
    {
        assert(triangles.size()%3 == 0);

        size_t overlaps[12] = {0};

        for (auto& i : triangles)
        {
            ++overlaps[i];
        }

        std::vector<size_t> output;

        for (size_t i = 0; i < 12; ++i)
        {
            assert(overlaps[i] < 3);
            if (overlaps[i] == 2)
            { 
                remove_overlap_vertex(triangles, i);
            }
        }
    }

    //
    void generate_geometry(size_t mask)
    {
    }

    // In look up table, store indexes to edges that are intersected by the 
    // "blobs", in correct order to ease geometry construction/adjustment in 
    // runtime.

    inline bool is_inside(const glm::vec3& point)
    {
        static const glm::vec3 sphere_a;
        static const glm::vec3 sphere_b;

        return 1.0f < (1.0f / (glm::dot(point, sphere_a) + glm::dot(point, sphere_b)));
    }

    // 
    inline bool is_set(size_t mask, size_t bit)
    {
        assert(bit < 8 && 0 <= bit); 
        return ((mask >> bit)&1) != 0;
    }

    // For a given mask, return the edges/mid points in order that can be used to 
    // construct triangle geometry.
    std::set<size_t> pick_edges(size_t mask)
    {
        const size_t edges[12] = {0};
        for (size_t bit = 0; bit < 8; ++bit)
        {
            if (is_set(mask, bit))
            {
                merge_edge_index(result, bit);
            }
        }

        return result;
    }

    // 
    void order_points_convex(const std::vector<glm::vec3>& points)
    {
        std::vector<glm::vec3> result;

        size_t a = 0;
        size_t b = a + 1;

        while (result.size() != points.size())
        { 
            for (size_t c = 0; c < edge_indexes.size(); ++c)
            {
                if (c != a && a != b)
                {
                    if (glm::dot(calc_normal(points[a], point[b], points[c])) < 0.0f)
                    {
                        b = c;
                        break;
                    }
                }
            }

            if (a == 0)
            {
                result.push_back(points[a]);
            }

            result.push_back(points[b]);
        }

        a = b;
        result.push_back(edge_indexes[a]);
        
    }

	// 1. Start with arbitrary point, A. 
	// 2. Remove A from set.
	// 3. Find point closest to A, B. 
	// 4. Remove B from set.
	// 5. Find point closest to B, C.
	// 6. Remove C from set.
	// 7. A, B, C forms a triangle. Add it to the result.
	// 8. If there is more points in the set, find point closest to C.
	// 9. 

	using Point = ;
	using Points = std::vector<Point>;

	std::pair<size_t, size_t> furthest_points(const Points& points)
	{
		size_t a = 0;
		size_t b = 0;
		float max_dist = -1.0f;
		float tmp_dist = 0.0f;
		std::pair<size_t, size_t> result;

		while (max_dist < tmp_dist)
		{ 
			max_dist = tmp_dist;
			tmp_dist = 0.0f;

			result.first = a;
			result.second = b;

			a = b;

			for (size_t i = 0; i < points.size(); ++i)
			{
				if (i != a)
				{
					const float dist = distance(points[a], points[i]);
					if (tmp_dist < dist)
					{
						tmp_dist = dist;
						b = i;
					}
				}
			} 

		}

		return result;
	}

}
