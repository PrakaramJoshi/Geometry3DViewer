#pragma once
#include <vector>
#include <CGPoint3D.h>
class CGTriangle3D;
class CGLine3D;
class CGSurface3D;
class CGCuboid;
class Tests
{
	void generate_test_data_triangles(bool _filled);

	CGTriangle3D *generate_triangle(const CGCuboid &_bounds, 
								double _max_edge_length = -1);

	CGLine3D * generate_line(const CGCuboid &_bounds);

	CGSurface3D * generate_surface(const CGCuboid &_bounds,
		const unsigned int _number_of_triangles);

	CGPoint3D generate_point(const CGCuboid &_bounds);

	double generate_random_fraction();

	double generate_number_between(const double &_min,
		const double &_max);

public:
	Tests();


	void get_random_lines(const CGCuboid& _bounds,
		const unsigned int _count,
		std::vector<CGLine3D*> &_lines);

	void get_random_points(const CGCuboid& _bounds,
		const unsigned int _count,
		std::vector<CGPoint3D> &_points);

	void get_random_triangles(const CGCuboid& _bounds,
		const unsigned int _count,
		std::vector<CGTriangle3D*> &_triangles);

	void get_random_surfaces(const CGCuboid& _bounds,
		const unsigned int _count,
		const unsigned int _number_of_triangles,
		std::vector<CGSurface3D*>&_surfaces);


	void get_line_line_intersections_test(const CGCuboid &_bounds,
		CGLine3D *&_line1,
		CGLine3D *&_line2,
		std::vector<CGPoint3D> &_pts);


	void get_line_triangle_intersections_test(const CGCuboid &_bounds,
		CGLine3D *&_line1,
		CGTriangle3D *&_triangle,
		std::vector<CGPoint3D> &_pts);


	void get_triangle_triangle_intersections_test(const CGCuboid &_bounds,
		CGTriangle3D *&_triangle1,
		CGTriangle3D *&_triangle2,
		std::vector<CGPoint3D> &_pts);

	void get_surface_line_intersections_test(const CGCuboid &_bounds,
		CGLine3D *&_line1,
		CGSurface3D *&_surface,
		const unsigned int _number_of_triangles,
		std::vector<CGPoint3D> &_pts);

	void get_surface_triangle_intersections_test(const CGCuboid &_bounds,
		CGTriangle3D *&_triangle,
		CGSurface3D *&_surface,
		const unsigned int _number_of_triangles,
		std::vector<CGPoint3D> &_pts);

	void get_surface_surface_intersections_test(const CGCuboid &_bounds,
		CGSurface3D *&_surface1,
		const unsigned int _number_of_triangles1,
		CGSurface3D *&_surface2,
		const unsigned int _number_of_triangles2,
		std::vector<CGPoint3D> &_pts);

	void get_surface_surface_merge_test(const CGCuboid &_bounds,
		CGSurface3D *&_surface1,
		const unsigned int _number_of_triangles1,
		CGSurface3D *&_surface2,
		const unsigned int _number_of_triangles2,
		CGSurface3D* &_merged);

	~Tests();
};

