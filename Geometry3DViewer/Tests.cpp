#include "Tests.h"
#include <CGTriangle3D.h>
#include <CGPoint3D.h>
#include <CGCuboid.h>
#include <CGLine3D.h>
#include <CGSurface3D.h>
#include <CGMath.h>
#include <queue>
#include <Logger\Logger.h>
using namespace AceLogger;
Tests::Tests()
{
}

void Tests::generate_test_data_triangles(bool _filled){
	std::vector<CGPoint3D> intersections;
	std::vector<CGTriangle3D> triangles;
	std::vector<CGLine3D> lines;
	//--------------------------------Triangle Intersection tests------------------------------------------
	triangles.push_back(CGTriangle3D(CGPoint3D(450, 450, 0), CGPoint3D(550, 550, 0), CGPoint3D(600, 200, 0)));
	triangles.push_back(CGTriangle3D(CGPoint3D(450, 445, 0), CGPoint3D(550, 540, 0), CGPoint3D(600, 00, 0)));
	//triangles.push_back(CGTriangle3D(CGPoint3D(450, 450, 450), CGPoint3D(550, 550, 550), CGPoint3D(600, 200, 100)));
	//triangles[0].IntersectionPoints(triangles[1], intersections);

	//CGLine3D line3 = triangles[0].GetUnitNormal();
	//line3.extend(50, false);
	//line3.extend(50);

	//triangles[0].IntersectionPoints(line3, intersections);
	//	lines.push_back(line3);
	//
	//std::vector<CGLine3D*>l;
	//triangles[0].GetLinesForBooleanOr(triangles[1],l);
	//	for (auto ll : l){
	//		lines.push_back(*ll);
	//		delete ll;
	//	}

	triangles[0].IntersectionPoints(triangles[1], intersections);
	std::vector<CGTriangle3D*> t;
	triangles[0].Merge(triangles[1], t);
	triangles.clear();
	for (auto triangle : t){
		triangles.push_back(CGTriangle3D(*triangle));
		delete triangle;
	}
	/*add_triangles(triangles, _filled);
	add_lines(lines);
	//add_triangles_projection(triangles, _filled);
	add_cross_hairs(intersections);*/

	//Ace::TextManager::Instance().AddText(AceGL::Text("Hello World", 50, 50)); 
	//Ace::TextManager::Instance().AddText(AceGL::Text("Hello World again", 50, 100));



	/*std::vector<CGPoint3D> pts;
	std::vector<CGLine3D> lines;
	std::vector<CGPoint3D> cross_hair;
	std::vector<CGCuboid> cuboids;
	std::vector<CGTriangle3D> triangles;

	/*pts.push_back(CGPoint3D(0, 0, 0));

	pts.push_back(CGPoint3D(100, 0, 0));
	pts.push_back(CGPoint3D(0, 100, 0));
	pts.push_back(CGPoint3D(0, 0, 100));

	pts.push_back(CGPoint3D(300, 300, 300));


	cross_hair.push_back(CGPoint3D(500, 500,100));
	cross_hair.push_back(CGPoint3D(50,100,200));

	lines.push_back(CGLine3D(CGPoint3D(-50, -50, -50), CGPoint3D(-100, -100, -100)));
	lines.push_back(CGLine3D(CGPoint3D(50, 50, 67), CGPoint3D(100, 100, 100)));

	//----------------------------------cuboid tests---------------------------------------------------
	cuboids.push_back(CGCuboid(CGPoint3D(100, 200, 300), 100, 200, 400));
	cuboids.push_back(CGCuboid(CGPoint3D(10, 20, 300), 100, 200, 400));

	//----------------------------------Line Intersection test----------------------------------------
	//CGLine3D line1(CGPoint3D(0, 0, 0), CGPoint3D(100, 100, 100));
	//	CGLine3D line2(CGPoint3D(100, 0, 0), CGPoint3D(0,100,100));

	std::vector<CGPoint3D> intersections;
	//	line1.IntersectionPoints(line2, intersections);
	//	lines.push_back(line1);
	//	lines.push_back(line2);

	//--------------------------------Triangle Intersection tests------------------------------------------
	triangles.push_back(CGTriangle3D(CGPoint3D(450, 450, 500), CGPoint3D(550, 550, 600), CGPoint3D(600, 200, 90)));
	//triangles.push_back(CGTriangle3D(CGPoint3D(450, 450, 450), CGPoint3D(550, 550, 550), CGPoint3D(600, 200, 100)));
	//triangles[0].IntersectionPoints(triangles[1], intersections);

	CGLine3D line3 =  triangles[0].GetUnitNormal();
	line3.extend(50, false);
	line3.extend(50);


	//CGLine3D normal = triangles[0].GetNormal();
	triangles[0].IntersectionPoints(line3, intersections);
	lines.push_back(line3);
	//lines.push_back(normal);

	render_cross_hair_collection(intersections);
	render_lines_collection(lines);
	//render_points_collection(pts);

	//render_box_collection(cuboids);
	render_triangle_collection(triangles);

	render_triangle_collection_projection(triangles);
	render_lines_collection_projection(lines);


	// render the selected cities */
}

double Tests::generate_random_fraction(){


	double num = static_cast<double>(rand());
	double denom = 0;

	while(denom==0)
		denom=static_cast<double>(rand());
	if (denom < num){
		denom += num;
	}
	double fraction = num / denom;
	return fraction;
}

double Tests::generate_number_between(const double &_min,
	const double &_max){

	int num = rand() % static_cast<int>(std::ceil(_max - _min));

	double random_number = static_cast<double>(num)+_min;
	double random_fraction = generate_random_fraction();
	if ((random_number + random_fraction) <= _max && (random_number + random_fraction) >= _min)
		return random_fraction + random_number;
	return random_number;

}

CGTriangle3D *Tests::generate_triangle(const CGCuboid &_bounds,
	double _max_edge_length){
	if (_max_edge_length <= 0){
		CGPoint3D p1 = generate_point(_bounds);
		CGPoint3D p2 = generate_point(_bounds);
		CGPoint3D p3 = generate_point(_bounds);

		return new CGTriangle3D(p1, p2, p3);
	}
	CGPoint3D p1 = generate_point(_bounds);
	CGPoint3D p2;
	CGPoint3D p3;
	do{
		p2 = generate_point(_bounds);
	} while (p2.Distance(p1) > _max_edge_length);

	do{
		p3 = generate_point(_bounds);
	} while (p3.Distance(p1) > _max_edge_length&&
		p3.Distance(p2) > _max_edge_length);

	return new CGTriangle3D(p1, p2, p3);
}

CGLine3D * Tests::generate_line(const CGCuboid &_bounds){

	CGPoint3D p1 = generate_point(_bounds);
	CGPoint3D p2 = generate_point(_bounds);

	return new CGLine3D(p1, p2);
}

CGSurface3D * Tests::generate_surface(const CGCuboid &_bounds,
	const unsigned int _number_of_triangles){

	double max_dimension = std::max(_bounds.GetLengthAlongZ(), std::max(_bounds.GetLengthAlongX(), _bounds.GetLengthAlongY()));
	double min_triangle_length = max_dimension / _number_of_triangles;
	double max_triangle_edge_length = (rand() % static_cast<int>(max_dimension)) + min_triangle_length;

	CGTriangle3D *triangle = generate_triangle(_bounds, max_triangle_edge_length);

	std::vector<CGTriangle3D*> triangles;
	int total_triangles_created = 1;

	triangles.push_back(triangle); 

	std::queue<const CGLine3D*> q;
	q.push(triangle->GetEdge1());
	q.push(triangle->GetEdge2());
	q.push(triangle->GetEdge3());
	
	int retry_count = 20;
	while (total_triangles_created < _number_of_triangles&& !q.empty()){
		auto pt = generate_point(_bounds);
		
		auto e = q.front();
		max_triangle_edge_length = (rand() % static_cast<int>(max_dimension)) + min_triangle_length;

		if (e->Distance(pt)>max_triangle_edge_length)
			continue;
		retry_count--;
		auto t = new CGTriangle3D(e->GetStart(), e->GetEnd(), pt);
		bool valid = true;
		for (auto current_triangle : triangles){
			if (current_triangle->IsInside(pt)){
				//Log("ignored triangle for surface , cause: inside another triangle");
				valid = false;
			}
			if (!valid)
				break;
			std::vector<CGPoint3D> pts;
			current_triangle->IntersectionPoints(*t, pts); 
			
			bool shares_edge = false;
			for (auto p : pts){
				if (!(p==current_triangle->GetP1()||p==current_triangle->GetP2()||p==current_triangle->GetP3())){
					//Log("ignored triangle for surface , cause: intersects another existing triangle");
					valid = false;
					break;
				}
				else{
					shares_edge = true;
				}

			}
			if (!valid)
				break;
			if (shares_edge){
				CGVector3D v1(pt-e->GetEnd());
				CGVector3D v2(e->GetStart() - e->GetEnd());
				CGPoint3D not_shared_pt;
				if (!e->IsEndPoint(current_triangle->GetP1())){
					not_shared_pt = current_triangle->GetP1();
				}
				else if (!e->IsEndPoint(current_triangle->GetP2())){
					not_shared_pt = current_triangle->GetP2();
				}
				else{
					not_shared_pt = current_triangle->GetP3();
				}
				CGVector3D v3(not_shared_pt - e->GetEnd());

				auto normal1 = v1.cross(v2).UnitVector();
				auto normal2 = v2.cross(v3).UnitVector();
				double angle = CGMath::GeoRTD(normal1.angle(normal2));
				if (angle > 45 ||angle <0){
					valid = false;
					//Log("ignored triangle for surface , cause: angle is not within limits");
				}
			}
			
			if (!valid)
				break;			
		}
		
		if (valid){
			q.pop();
			triangles.push_back(t);
			if (!(t->GetEdge1()->IsEndPoint(e->GetStart()) && t->GetEdge1()->IsEndPoint(e->GetEnd())))
				q.push(t->GetEdge1());
			if (!(t->GetEdge2()->IsEndPoint(e->GetStart()) && t->GetEdge2()->IsEndPoint(e->GetEnd())))
				q.push(t->GetEdge2());
			if (!(t->GetEdge3()->IsEndPoint(e->GetStart()) && t->GetEdge3()->IsEndPoint(e->GetEnd())))
				q.push(t->GetEdge3());
			total_triangles_created++;
			retry_count = 20;
		}
		else{
			delete t;
			if (retry_count == 0){
				q.pop();
				retry_count = 20;
			}
		}
		
	}
	Log("Generated surface of " + StringUtils::get_string(total_triangles_created) + " triangles , input size: " + StringUtils::get_string(_number_of_triangles));
	return new CGSurface3D(triangles);
}

CGPoint3D Tests::generate_point(const CGCuboid &_bounds){

	double x = generate_number_between(_bounds.GetMinX(), _bounds.GetMaxX());
	double y = generate_number_between(_bounds.GetMinY(), _bounds.GetMaxY());
	double z = generate_number_between(_bounds.GetMinZ(), _bounds.GetMaxZ());

	return CGPoint3D(x, y, z);
}

void Tests::get_random_lines(const CGCuboid& _bounds,
	const unsigned int _count,
	std::vector<CGLine3D*> &_lines){

	for (unsigned int i = 0; i < _count; i++)
		_lines.push_back(generate_line(_bounds));
}

void Tests::get_random_points(const CGCuboid& _bounds,
	const unsigned int _count,
	std::vector<CGPoint3D> &_points){

	for (unsigned int i = 0; i < _count; i++)
		_points.push_back(generate_point(_bounds));
}

void Tests::get_random_triangles(const CGCuboid& _bounds,
	const unsigned int _count,
	std::vector<CGTriangle3D*> &_triangles){

	for (unsigned int i = 0; i < _count; i++)
		_triangles.push_back(generate_triangle(_bounds));
}

void Tests::get_random_surfaces(const CGCuboid& _bounds,
	const unsigned int _count,
	const unsigned int _number_of_triangles,
	std::vector<CGSurface3D*>&_surfaces){

	for (unsigned int i = 0; i < _count; i++)
		_surfaces.push_back(generate_surface(_bounds, _number_of_triangles));
}


void Tests::get_line_line_intersections_test(const CGCuboid &_bounds,
	CGLine3D *&_line1,
	CGLine3D *&_line2,
	std::vector<CGPoint3D> &_pts){
	
	_line1 = generate_line(_bounds);
	_line2 = generate_line(_bounds);

	_line1->IntersectionPoints(*_line2, _pts);
}


void Tests::get_line_triangle_intersections_test(const CGCuboid &_bounds,
	CGLine3D *&_line1,
	CGTriangle3D *&_triangle,
	std::vector<CGPoint3D> &_pts){
	
	_line1 = generate_line(_bounds);
	_triangle = generate_triangle(_bounds);

	_triangle->IntersectionPoints(*_line1, _pts);
}


void Tests::get_triangle_triangle_intersections_test(const CGCuboid &_bounds,
	CGTriangle3D *&_triangle1,
	CGTriangle3D *&_triangle2,
	std::vector<CGPoint3D> &_pts){
	_triangle1 = generate_triangle(_bounds);
	_triangle2 = generate_triangle(_bounds);

	_triangle1->IntersectionPoints(*_triangle2, _pts);
}

void Tests::get_surface_line_intersections_test(const CGCuboid &_bounds,
	CGLine3D *&_line1,
	CGSurface3D *&_surface,
	const unsigned int _number_of_triangles,
	std::vector<CGPoint3D> &_pts){
	_line1 = generate_line(_bounds);
	_surface = generate_surface(_bounds, _number_of_triangles);

	_surface->IntersectionPoints(*_line1, _pts);
}

void Tests::get_surface_triangle_intersections_test(const CGCuboid &_bounds,
	CGTriangle3D *&_triangle,
	CGSurface3D *&_surface,
	const unsigned int _number_of_triangles,
	std::vector<CGPoint3D> &_pts){
	_triangle = generate_triangle(_bounds);
	_surface = generate_surface(_bounds, _number_of_triangles);

	_surface->IntersectionPoints(*_triangle, _pts);
}

void Tests::get_surface_surface_intersections_test(const CGCuboid &_bounds,
	CGSurface3D *&_surface1,
	const unsigned int _number_of_triangles1,
	CGSurface3D *&_surface2,
	const unsigned int _number_of_triangles2,
	std::vector<CGPoint3D> &_pts){
	_surface1 = generate_surface(_bounds, _number_of_triangles1);
	_surface2 = generate_surface(_bounds, _number_of_triangles2);

	_surface1->IntersectionPoints(*_surface2, _pts);
}

void Tests::get_surface_surface_merge_test(const CGCuboid &_bounds,
	CGSurface3D *&_surface1,
	const unsigned int _number_of_triangles1,
	CGSurface3D *&_surface2,
	const unsigned int _number_of_triangles2,
	CGSurface3D* &_merged){
	_surface1 = generate_surface(_bounds, _number_of_triangles1);
	_surface2 = generate_surface(_bounds, _number_of_triangles2);

	_merged = _surface1->Merge(*_surface2);

}


Tests::~Tests()
{
}
