#include "GeometryViewer.h"
#include <OpenGL\ShadersUtility.h>
#include <algorithm>
#include <random>
#include <TextManager.h>
#include <AceButton.h>
#include <MenuBar.h>
#include <GUIElementManager.h>
#include <CGSurface3D.h>
#include "Tests.h"


int window_width;
int window_height;

void window_resize(GLFWwindow *_window, int _width, int _height){
	if (_window != glfwGetCurrentContext())
		return;
	window_width = _width;
	window_height = _height;
	glViewport(0, 0, _width, _height);
	AceGUI::GUIElementManager::Instance().window_resized(_width, _height);
	Ace::TextManager::Instance().on_window_resize(_width, _height);
}

void mouseclick(GLFWwindow*window, int button, int action, int mod){
	
	if (window != glfwGetCurrentContext())
		return;
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	AceGUI::mouse_click_callback_params mouse_click(action,button,mod);
	AceGUI::GUIElementManager::Instance().on_mouse_click(mouse_click,x,y);	

}

void mousemove(GLFWwindow *_window, double _x, double _y){
	
	AceGUI::GUIElementManager::Instance().on_mouse_move(_x,_y);
}

GeometryViewer::GeometryViewer(app_settings &_app_seetings) 
{
	m_app_settings = _app_seetings;
	m_programID = 0;

	m_world_size_x = m_app_settings.p_world_size_x;
	m_world_size_y = m_app_settings.p_world_size_y;
	m_world_size_z = m_app_settings.p_world_size_z;

	m_drawn_lines_persistent = nullptr;
	m_drawn_lines_data = nullptr;
	m_drawn_guide_lines_data = nullptr;
	m_triangles_wire_frame_data = nullptr;
	m_triangles_filled_data = nullptr;
	m_points_data = nullptr;
	m_box_filled_data = nullptr;
	m_box_wire_frame_data = nullptr;

	m_viewPort.p_start_x = 0;
	m_viewPort.p_start_y = 0;
	m_viewPort.p_width = 0;
	m_viewPort.p_height = 0;

	window_width = m_viewPort.p_width;
	window_height = m_viewPort.p_height;
	m_color_index = 0;

	m_current_camera = nullptr;

	m_draw_guides = true;
	m_global_fill = false;

	
}


GeometryViewer::~GeometryViewer()
{
	
	delete m_drawn_lines_persistent;
	m_drawn_lines_persistent = nullptr;

	delete m_drawn_lines_data;
	m_drawn_lines_data = nullptr;

	delete m_drawn_guide_lines_data; 
	m_drawn_guide_lines_data = nullptr;

	delete m_triangles_wire_frame_data;
	m_triangles_wire_frame_data = nullptr;

	delete m_triangles_filled_data;
	m_triangles_filled_data = nullptr;

	delete m_points_data;
	m_points_data = nullptr;

	delete m_box_filled_data;
	m_box_filled_data = nullptr;

	delete m_box_wire_frame_data;
	m_box_wire_frame_data = nullptr;

	delete m_current_camera; 
	m_current_camera = nullptr;


	delete_vao_state(m_lines_persistent);
	delete_vao_state(m_lines_data);
	delete_vao_state(m_triangles_filled);
	delete_vao_state(m_triangles_wire_frame);
	delete_vao_state(m_points);
	delete_vao_state(m_box_filled);
	delete_vao_state(m_box_wire_frame);
	delete_vao_state(m_guide_lines);

	glDeleteProgram(m_programID);

	AceGUI::GUIElementManager::Instance().clean_all();
}

CGPoint3D GeometryViewer::normalize_point(const CGPoint3D &_pt)const{
	CGPoint3D pt = _pt;
	pt.SetX(_pt.GetX()*2.0 / m_world_size_x);
	pt.SetY(_pt.GetY()*2.0 / m_world_size_y);
	pt.SetZ(_pt.GetZ()*2.0 / m_world_size_z);
	return pt;
}

void GeometryViewer::generate_colors(){
	int total = 500;

	std::set<std::tuple<float, float, float>> colors;
	while (colors.size() != total){
		float r = static_cast<float>(rand() % 256)/255.0;
		float g = static_cast<float>(rand() % 256) / 255.0;
		float b = static_cast<float>(rand() % 256) / 255.0;
		colors.insert(std::make_tuple(r, g, b));
	}

	for (auto c : colors)
		m_sample_colors.push_back(c);
	auto engine = std::default_random_engine{ };
	std::shuffle(std::begin(m_sample_colors), std::end(m_sample_colors), engine);
}

std::tuple<float, float, float> GeometryViewer::get_next_color(){
	
	auto c = m_sample_colors[m_color_index];
	m_color_index++;
	if (m_color_index == 500)
		m_color_index = 0;
	return c;
}

void GeometryViewer::add_points(const std::vector<CGPoint3D> &_points){
	std::vector<float> points_data;
	points_data.reserve(_points.size() * 3);
	for (auto p : _points){
		CGPoint3D pt = normalize_point(p);
		points_data.push_back(pt.GetX());
		points_data.push_back(pt.GetY());
		points_data.push_back(pt.GetZ());
	}
	m_points_data->add_vertices(points_data);
	
}

void GeometryViewer::add_lines(const std::vector<const CGLine3D*> &_lines){
	std::vector<float> points_data;
	std::vector<float> colors_data;
	points_data.reserve(_lines.size() * 3*2);
	for (auto p : _lines){
		CGPoint3D start = normalize_point(p->GetStart());
		CGPoint3D end = normalize_point(p->GetEnd());
		points_data.push_back(start.GetX());
		points_data.push_back(start.GetY());
		points_data.push_back(start.GetZ());

		points_data.push_back(end.GetX());
		points_data.push_back(end.GetY());
		points_data.push_back(end.GetZ());

		auto c = get_next_color();
		colors_data.push_back(std::get<0>(c));
		colors_data.push_back(std::get<1>(c));
		colors_data.push_back(std::get<2>(c));
		colors_data.push_back(std::get<0>(c));
		colors_data.push_back(std::get<1>(c));
		colors_data.push_back(std::get<2>(c));
	}
	DrawingUtils::LinesCollection lines_collection(points_data, colors_data);
	lines_collection.get_vertices(m_drawn_lines_data->get_vertices_ref());
	lines_collection.get_colors(m_drawn_lines_data->get_colors_ref());
	
}

void GeometryViewer::add_unicolor_lines(const std::vector<const CGLine3D*> &_lines){
	std::vector<float> points_data;
	std::vector<float> colors_data;
	points_data.reserve(_lines.size() * 3 * 2);
	auto c = get_next_color();
	for (auto p : _lines){
		CGPoint3D start = normalize_point(p->GetStart());
		CGPoint3D end = normalize_point(p->GetEnd());
		points_data.push_back(start.GetX());
		points_data.push_back(start.GetY());
		points_data.push_back(start.GetZ());

		points_data.push_back(end.GetX());
		points_data.push_back(end.GetY());
		points_data.push_back(end.GetZ());

		
		colors_data.push_back(std::get<0>(c));
		colors_data.push_back(std::get<1>(c));
		colors_data.push_back(std::get<2>(c));
		colors_data.push_back(std::get<0>(c));
		colors_data.push_back(std::get<1>(c));
		colors_data.push_back(std::get<2>(c));
	}
	DrawingUtils::LinesCollection lines_collection(points_data, colors_data);
	lines_collection.get_vertices(m_drawn_lines_data->get_vertices_ref());
	lines_collection.get_colors(m_drawn_lines_data->get_colors_ref());

}

void GeometryViewer::generate_guides(const CGPoint3D &_pt1, const CGPoint3D &_p2,
	std::vector<std::pair<CGPoint3D, CGPoint3D> > &_guides){
	double length_of_guide = 0.01;
	int  total_guides = static_cast<int>(_pt1.Distance(_p2) / length_of_guide);
	bool p1 = true;
	bool p2 = false;
	CGPoint3D pt1 = _pt1;
	CGPoint3D pt2;

	double m = static_cast<double>(total_guides);
	for (auto i = 1; i < total_guides; i++){
		double n = static_cast<double>(i);
		if (p1){
			pt2 = ((_p2*n) + (_pt1*(m - n))) / m;
			p2 = true;
		}
		else{
			pt1 = ((_p2*n) + (_pt1*(m - n))) / m;
			p1 = true;
		}

		if (p1&&p2){
			_guides.push_back(std::make_pair(pt1, pt2));
			p1 = false;
			p2 = false;
		}
	}
}

void GeometryViewer::add_guide_lines(const std::vector<std::pair<CGPoint3D,CGPoint3D> > &_lines){
	std::vector<float> points_data;
	std::vector<float> colors_data;
	points_data.reserve(_lines.size() * 3 * 2);
	for (auto p : _lines){
		CGPoint3D start = normalize_point(p.first);
		CGPoint3D end = normalize_point(p.second);
		std::vector<std::pair<CGPoint3D, CGPoint3D> >guides;
		generate_guides(start, end, guides);
		auto c = get_next_color();
		for (auto g : guides){
			points_data.push_back(g.first.GetX());
			points_data.push_back(g.first.GetY());
			points_data.push_back(g.first.GetZ());

			points_data.push_back(g.second.GetX());
			points_data.push_back(g.second.GetY());
			points_data.push_back(g.second.GetZ());

			
			colors_data.push_back(std::get<0>(c));
			colors_data.push_back(std::get<1>(c));
			colors_data.push_back(std::get<2>(c));
			colors_data.push_back(std::get<0>(c));
			colors_data.push_back(std::get<1>(c));
			colors_data.push_back(std::get<2>(c));
		}
	}
	DrawingUtils::LinesCollection lines_collection(points_data, colors_data);
	lines_collection.get_vertices(m_drawn_guide_lines_data->get_vertices_ref());
	lines_collection.get_colors(m_drawn_guide_lines_data->get_colors_ref());
	
}

void GeometryViewer::add_lines_projection(std::vector<CGLine3D> &_lines){
	std::vector<float> points_data_xy,points_data_yz,points_data_zx;
	for (auto p : _lines){
		CGPoint3D start = normalize_point(p.GetStart());
		CGPoint3D end = normalize_point(p.GetEnd());

		//--------------xy-projection
		points_data_xy.push_back(start.GetX());
		points_data_xy.push_back(start.GetY());
		points_data_xy.push_back(0);

		points_data_xy.push_back(end.GetX());
		points_data_xy.push_back(end.GetY());
		points_data_xy.push_back(0);

		//--------------yz-projection
		points_data_yz.push_back(0);
		points_data_yz.push_back(start.GetY());
		points_data_yz.push_back(start.GetZ());

		points_data_yz.push_back(0);
		points_data_yz.push_back(end.GetY());
		points_data_yz.push_back(end.GetZ());

		//--------------zx-projection
		points_data_zx.push_back(start.GetX());
		points_data_zx.push_back(0);
		points_data_zx.push_back(start.GetZ());

		points_data_zx.push_back(end.GetX());
		points_data_zx.push_back(0);
		points_data_zx.push_back(end.GetZ());
	}
	DrawingUtils::LinesCollection lines_collection_xy(points_data_xy,1.0,1.0,0.0);
	lines_collection_xy.get_vertices(m_drawn_lines_data->get_vertices_ref());
	lines_collection_xy.get_colors(m_drawn_lines_data->get_colors_ref());
	

	DrawingUtils::LinesCollection lines_collection_yz(points_data_yz,0.0,1.0,1.0);
	lines_collection_yz.get_vertices(m_drawn_lines_data->get_vertices_ref());
	lines_collection_yz.get_colors(m_drawn_lines_data->get_colors_ref());


	DrawingUtils::LinesCollection lines_collection_zx(points_data_zx,1.0,0.0,1.0);
	lines_collection_zx.get_vertices(m_drawn_lines_data->get_vertices_ref());
	lines_collection_zx.get_colors(m_drawn_lines_data->get_colors_ref());
	
}

void GeometryViewer::add_cross_hairs(const std::vector<CGPoint3D> &_points){
	std::vector<float> points_data;
	points_data.reserve(_points.size() * 3);
	for (auto p : _points){
		CGPoint3D pt = normalize_point(p);
		points_data.push_back(pt.GetX());
		points_data.push_back(pt.GetY());
		points_data.push_back(pt.GetZ());
	}
	DrawingUtils::CrossHairCollection cross_hair_collection(points_data,0.01);
	cross_hair_collection.get_vertices(m_drawn_lines_data->get_vertices_ref());
	cross_hair_collection.get_colors(m_drawn_lines_data->get_colors_ref());
	
}

void GeometryViewer::add_box_wire_frame_collection(std::vector<CGCuboid> &_cuboids){

	for (auto p : _cuboids){
		CGPoint3D pt = p.GetCenter();
		pt =normalize_point(pt);
		CGPoint3D dimensions(p.GetLengthAlongX(), p.GetLengthAlongY(), p.GetLengthAlongZ());
		dimensions = normalize_point(dimensions);
		DrawingUtils::Box box(pt.GetX(), pt.GetY(), pt.GetZ(), dimensions.GetX(), dimensions.GetY(), dimensions.GetZ(), false);
		box.get_colors(m_box_wire_frame_data->get_colors_ref());
		box.get_vertices(m_box_wire_frame_data->get_vertices_ref());
		
	}
}

void GeometryViewer::add_box_filled_collection(std::vector<CGCuboid> &_cuboids){

	for (auto p : _cuboids){
		CGPoint3D pt = p.GetCenter();
		pt = normalize_point(pt);
		CGPoint3D dimensions(p.GetLengthAlongX(), p.GetLengthAlongY(), p.GetLengthAlongZ());
		dimensions = normalize_point(dimensions);
		DrawingUtils::Box box(pt.GetX(), pt.GetY(), pt.GetZ(), dimensions.GetX(), dimensions.GetY(), dimensions.GetZ(), true);
		box.get_colors(m_box_filled_data->get_colors_ref());
		box.get_vertices(m_box_filled_data->get_vertices_ref());
	
	}
}


void GeometryViewer::add_triangles(const std::vector<const CGTriangle3D*> &_triangles,bool _filled){
	
	std::vector<float> points;
	_filled = _filled || m_global_fill;
	points.reserve(_triangles.size() * 9);
	std::vector<float> colors_data;
	std::vector<std::pair<CGPoint3D, CGPoint3D>> guides;
	for (const auto &t : _triangles){
		
		auto p = normalize_point(t->GetP1());
		points.push_back(p.GetX());
		points.push_back(p.GetY());
		points.push_back(p.GetZ());

		p = normalize_point(t->GetP2());
		points.push_back(p.GetX());
		points.push_back(p.GetY());
		points.push_back(p.GetZ());

		p = normalize_point(t->GetP3());
		points.push_back(p.GetX());
		points.push_back(p.GetY());
		points.push_back(p.GetZ());

		guides.push_back(std::make_pair(t->GetP1(), t->GetCenteroid()));
		guides.push_back(std::make_pair(t->GetP2(), t->GetCenteroid()));
		guides.push_back(std::make_pair(t->GetP3(), t->GetCenteroid()));

		auto c = get_next_color();
		colors_data.push_back(std::get<0>(c));
		colors_data.push_back(std::get<1>(c));
		colors_data.push_back(std::get<2>(c));

		colors_data.push_back(std::get<0>(c));
		colors_data.push_back(std::get<1>(c));
		colors_data.push_back(std::get<2>(c));

		colors_data.push_back(std::get<0>(c));
		colors_data.push_back(std::get<1>(c));
		colors_data.push_back(std::get<2>(c));
	}

	DrawingUtils::TriangleCollection triangle_collection(points,colors_data);
	if (_filled){
		triangle_collection.get_vertices(m_triangles_filled_data->get_vertices_ref());
		triangle_collection.get_colors(m_triangles_filled_data->get_colors_ref());
		
	}
	else{
		triangle_collection.get_vertices(m_triangles_wire_frame_data->get_vertices_ref());
		triangle_collection.get_colors(m_triangles_wire_frame_data->get_colors_ref());
		
	}
	add_guide_lines(guides);
}

void GeometryViewer::add_surfaces(const std::vector<const CGSurface3D*> &_surfaces, bool _filled){

	std::vector<std::pair<CGPoint3D, CGPoint3D> > guides;
	_filled = _filled || m_global_fill;
	std::vector<const CGLine3D*> edges;
	for (auto s : _surfaces){
		std::vector<const CGTriangle3D*> triangles;
		s->GetTriangles(triangles);
		std::vector<float> points;

		points.reserve(triangles.size() * 9);
		std::vector<float> colors_data;
		auto c = get_next_color();
		for (const auto &t : triangles){

			auto p1 = normalize_point(t->GetP1());
			points.push_back(p1.GetX());
			points.push_back(p1.GetY());
			points.push_back(p1.GetZ());

			auto p2 = normalize_point(t->GetP2());
			points.push_back(p2.GetX());
			points.push_back(p2.GetY());
			points.push_back(p2.GetZ());

			auto p3 = normalize_point(t->GetP3());
			points.push_back(p3.GetX());
			points.push_back(p3.GetY());
			points.push_back(p3.GetZ());

			guides.push_back(std::make_pair(t->GetCenteroid(), t->GetP1()));
			guides.push_back(std::make_pair(t->GetCenteroid(), t->GetP2()));
			guides.push_back(std::make_pair(t->GetCenteroid(), t->GetP3()));

			
			edges.push_back(t->GetEdge1());
			edges.push_back(t->GetEdge2());
			edges.push_back(t->GetEdge3());
			
			colors_data.push_back(std::get<0>(c));
			colors_data.push_back(std::get<1>(c));
			colors_data.push_back(std::get<2>(c));

			colors_data.push_back(std::get<0>(c));
			colors_data.push_back(std::get<1>(c));
			colors_data.push_back(std::get<2>(c));

			colors_data.push_back(std::get<0>(c));
			colors_data.push_back(std::get<1>(c));
			colors_data.push_back(std::get<2>(c));
		}
		DrawingUtils::TriangleCollection triangle_collection(points, colors_data);
		if (_filled){
			triangle_collection.get_vertices(m_triangles_filled_data->get_vertices_ref());
			triangle_collection.get_colors(m_triangles_filled_data->get_colors_ref());
			add_unicolor_lines(edges);
			
		}
		else{
			triangle_collection.get_vertices(m_triangles_wire_frame_data->get_vertices_ref());
			triangle_collection.get_colors(m_triangles_wire_frame_data->get_colors_ref());
			
		}
	}
	add_guide_lines(guides);

}

void GeometryViewer::add_triangles_projection(std::vector<CGTriangle3D> &_triangles, bool _filled){

	std::vector<float> points_xy,point_yz,points_zx;
	_filled = _filled || m_global_fill;
	for (const auto &t : _triangles){

		//----------------xy-projection
		auto p = normalize_point(t.GetP1());
		points_xy.push_back(p.GetX());
		points_xy.push_back(p.GetY());
		points_xy.push_back(0);

		p = normalize_point(t.GetP2());
		points_xy.push_back(p.GetX());
		points_xy.push_back(p.GetY());
		points_xy.push_back(0);

		p = normalize_point(t.GetP3());
		points_xy.push_back(p.GetX());
		points_xy.push_back(p.GetY());
		points_xy.push_back(0);

		//----------------yz-projection
		p = normalize_point(t.GetP1());
		point_yz.push_back(0);
		point_yz.push_back(p.GetY());
		point_yz.push_back(p.GetZ());

		p = normalize_point(t.GetP2());
		point_yz.push_back(0);
		point_yz.push_back(p.GetY());
		point_yz.push_back(p.GetZ());

		p = normalize_point(t.GetP3());
		point_yz.push_back(0);
		point_yz.push_back(p.GetY());
		point_yz.push_back(p.GetZ());

		//----------------zx-projection
		p = normalize_point(t.GetP1());
		points_zx.push_back(p.GetX());
		points_zx.push_back(0);
		points_zx.push_back(p.GetZ());

		p = normalize_point(t.GetP2());
		points_zx.push_back(p.GetX());
		points_zx.push_back(0);
		points_zx.push_back(p.GetZ());

		p = normalize_point(t.GetP3());
		points_zx.push_back(p.GetX());
		points_zx.push_back(0);
		points_zx.push_back(p.GetZ());
	}

	DrawingUtils::TriangleCollection triangle_collection_xy(points_xy,1.0,1.0,0.0);

	DrawingUtils::TriangleCollection triangle_collection_yz(point_yz,0.0,1.0,1.0);
	
	DrawingUtils::TriangleCollection triangle_collection_zx(points_zx,1.0,0.0,1.0);
		
	if (_filled){
		triangle_collection_xy.get_vertices(m_triangles_filled_data->get_vertices_ref());
		triangle_collection_xy.get_colors(m_triangles_filled_data->get_colors_ref());
		

		triangle_collection_yz.get_vertices(m_triangles_filled_data->get_vertices_ref());
		triangle_collection_yz.get_colors(m_triangles_filled_data->get_colors_ref());
		

		triangle_collection_zx.get_vertices(m_triangles_filled_data->get_vertices_ref());
		triangle_collection_zx.get_colors(m_triangles_filled_data->get_colors_ref());
	
	}
	else{
		triangle_collection_xy.get_vertices(m_triangles_wire_frame_data->get_vertices_ref());
		triangle_collection_xy.get_colors(m_triangles_wire_frame_data->get_colors_ref());
	

		triangle_collection_yz.get_vertices(m_triangles_wire_frame_data->get_vertices_ref());
		triangle_collection_yz.get_colors(m_triangles_wire_frame_data->get_colors_ref());
	

		triangle_collection_zx.get_vertices(m_triangles_wire_frame_data->get_vertices_ref());
		triangle_collection_zx.get_colors(m_triangles_wire_frame_data->get_colors_ref());
	
	};
}



void GeometryViewer::render(){

	glUseProgram(m_programID);

	m_viewPort.p_width = window_width;
	m_viewPort.p_height = window_height;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera for the view
	glEnable(GL_DEPTH_TEST);

	m_current_camera->move(m_viewPort.p_width, m_viewPort.p_height);

	m_drawn_lines_persistent->draw(true);
	m_triangles_filled_data->draw(false);
	m_triangles_wire_frame_data->draw(false);
	m_drawn_lines_data->draw(false);
	m_points_data->draw(false);
	m_box_filled_data->draw(false);
	m_box_wire_frame_data->draw(false);
	if (m_draw_guides)
		m_drawn_guide_lines_data->draw(false);
	
	Ace::TextManager::Instance().render();
	AceGUI::GUIElementManager::Instance().render();
	

}

void GeometryViewer::render_pixels(){
	float fov = m_app_settings.p_fov;
	float zNear = m_app_settings.p_zNear;
	float zFar = m_app_settings.p_zFar;
	float zoom_factor = m_app_settings.p_zoom_factor; 
	auto window = m_window.get_window();

	m_current_camera =new CurrentCamera(window, fov, zNear, zFar, zoom_factor, m_app_settings.p_auto_rotate, m_app_settings.p_rotate_x, m_app_settings.p_rotate_y, m_app_settings.p_rotate_z,
		m_app_settings.p_rotate_about_x, m_app_settings.p_rotate_about_y, m_app_settings.p_rotate_about_z,
		m_app_settings.p_rotate_speed,true);

	m_current_camera->setup_phong_lighting(m_programID);

	while (!glfwWindowShouldClose(window)) {
		//render the scene
		render();
		//other stuff : FPS, check for user input
		m_window.FPSStatus(window_width, window_height);
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

}

void GeometryViewer::load_shaders(){

	if (m_programID)
		glDeleteProgram(m_programID);
	std::string shader_vs = R"(B:\Workspace\Geometry3dViewer\Geometry3dViewer\Shaders\vertexcolor_vs.glsl)";
	std::string shader_fs = R"(B:\Workspace\Geometry3dViewer\Geometry3dViewer\Shaders\vertexcolor_fs.glsl)";
	std::vector<std::pair<std::string, GLenum>> shaders;

	shaders.push_back(std::make_pair(shader_vs, GL_VERTEX_SHADER));
	shaders.push_back(std::make_pair(shader_fs, GL_FRAGMENT_SHADER));
	m_programID = GetProgramID_FileShaders(shaders);

	CHECK_GL_ERROR
}

bool GeometryViewer::setup(){

	m_window.set_title("Geometry3DViewer");
	int screen_width = 640;
	int screen_height = 480;
	if (!m_window.init(screen_width, screen_height, m_app_settings.p_full_screen)){
		AceLogger::Log("unable to initialize the display!", AceLogger::LOG_ERROR);
		return false;
	}

	glfwGetFramebufferSize(m_window.get_window(), &m_viewPort.p_width, &m_viewPort.p_height);
	window_height = m_viewPort.p_height;
	window_width = m_viewPort.p_width;

	AceGUI::GUIElementManager::Instance().set_window_size(window_width, window_height);
	Ace::TextManager::Instance().on_window_resize(window_width, window_height);
	// unknown reason for the first text to be offset in y
	// creating empty text as workaround.
	Ace::TextManager::InstanceGUI().AddText("");
	Ace::TextManager::Instance().AddText("");


	glfwSetFramebufferSizeCallback(m_window.get_window(), window_resize);
	glfwSetMouseButtonCallback(m_window.get_window(), mouseclick);
	glfwSetCursorPosCallback(m_window.get_window(), mousemove);
	CHECK_GL_ERROR
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	CHECK_GL_ERROR

	load_shaders();
	
	generate_gl_buffers(m_lines_persistent);
	generate_gl_buffers(m_lines_data);
	generate_gl_buffers(m_triangles_filled);
	generate_gl_buffers(m_triangles_wire_frame);
	generate_gl_buffers(m_points);
	generate_gl_buffers(m_box_filled);
	generate_gl_buffers(m_box_wire_frame);
	generate_gl_buffers(m_guide_lines);

	CHECK_GL_ERROR
	glUseProgram(m_programID);
	CHECK_GL_ERROR

		m_drawn_lines_persistent = new DrawingUtils::DrawnLines(m_lines_persistent.p_vertexbuffer, m_lines_persistent.p_colours_vbo,  m_lines_persistent.p_vao);
	m_drawn_lines_data = new DrawingUtils::DrawnLines(m_lines_data.p_vertexbuffer, m_lines_data.p_colours_vbo, m_lines_data.p_vao);
	m_drawn_guide_lines_data = new DrawingUtils::DrawnLines(m_guide_lines.p_vertexbuffer, m_guide_lines.p_colours_vbo, m_guide_lines.p_vao);
	m_triangles_filled_data = new DrawingUtils::DrawnTrianglesFilled(m_triangles_filled.p_vertexbuffer, m_triangles_filled.p_colours_vbo, m_triangles_filled.p_vao);
	m_triangles_wire_frame_data = new DrawingUtils::DrawnTrianglesWireFrame(m_triangles_wire_frame.p_vertexbuffer, m_triangles_wire_frame.p_colours_vbo,m_triangles_wire_frame.p_vao);
	m_points_data = new DrawingUtils::DrawnPoints(m_points.p_vertexbuffer, m_points.p_colours_vbo, m_points.p_vao);
	m_box_filled_data = new DrawingUtils::DrawnBoxFilled(m_box_filled.p_vertexbuffer, m_box_filled.p_colours_vbo, m_box_filled.p_vao);
	m_box_wire_frame_data = new DrawingUtils::DrawnBoxWireFrame(m_box_wire_frame.p_vertexbuffer, m_box_wire_frame.p_colours_vbo, m_box_wire_frame.p_vao);


	DrawingUtils::CoordinateAxisXYZ axis(0.5);
	axis.get_vertices(m_drawn_lines_persistent->get_vertices_ref());
	axis.get_colors(m_drawn_lines_persistent->get_colors_ref());
	

	DrawingUtils::CoordinatePlanes coordinate_planes(m_app_settings.p_coordinate_plane_size, m_app_settings.p_octant);
	coordinate_planes.get_vertices(m_drawn_lines_persistent->get_vertices_ref());
	coordinate_planes.get_colors(m_drawn_lines_persistent->get_colors_ref());
	


	if (m_app_settings.p_draw_lattice){
		float normalized_size_x = m_app_settings.p_lattice_size_x / m_world_size_x;
		float normalized_size_y = m_app_settings.p_lattice_size_y / m_world_size_y;
		float normalized_size_z = m_app_settings.p_lattice_size_z / m_world_size_z;

		float normalized_row_size_x = m_app_settings.p_lattice_row_size_x / m_world_size_x;
		float normalized_row_size_y = m_app_settings.p_lattice_row_size_y / m_world_size_y;
		float normalized_row_size_z = m_app_settings.p_lattice_row_size_z / m_world_size_z;
		DrawingUtils::Lattice lattice(0, 0, 0,
			normalized_size_x, normalized_size_y, normalized_size_z,
			normalized_row_size_x, normalized_row_size_y, normalized_row_size_z);
		lattice.get_vertices(m_drawn_lines_persistent->get_vertices_ref());
		lattice.get_colors(m_drawn_lines_persistent->get_colors_ref());
		
	}
	
	generate_colors();
	generate_menu_bar();
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	return true;
}

void GeometryViewer::generate_menu_bar(){

	auto this_ptr = this;
	m_menubar = new AceGUI::MenuBar(true);
	auto menu_bar_ptr = m_menubar;
	//-------------------generate random point
	auto generate_point = [this_ptr](const AceGUI::call_back_params*){
		CGPoint3D min_bound(0, 0, 0);
		CGPoint3D max_bound(750, 750, 750);
		CGCuboid bounds(min_bound, max_bound);

		Tests t;
		std::vector<CGPoint3D> pt;
		t.get_random_points(bounds,1,pt);
		this_ptr->add_cross_hairs(pt);
	};

	//-------------------generate random line
	auto generate_line = [this_ptr](const AceGUI::call_back_params*){
		CGPoint3D min_bound(0, 0, 0);
		CGPoint3D max_bound(750, 750, 750);
		CGCuboid bounds(min_bound, max_bound);

		Tests t;
		std::vector<CGLine3D*> o;
		t.get_random_lines(bounds, 1, o);

		std::vector<const CGLine3D*> const_container;
		this_ptr->get_const_vector(o, const_container);

		this_ptr->add_lines(const_container);
		for (auto l : o)
			delete l;
	};
	
	//-------------------generate random triangle
	auto generate_triangle = [this_ptr](const AceGUI::call_back_params*){
		CGPoint3D min_bound(0, 0, 0);
		CGPoint3D max_bound(750, 750, 750);
		CGCuboid bounds(min_bound, max_bound);

		Tests t;
		std::vector<CGTriangle3D*> o;
		t.get_random_triangles(bounds, 1, o);

		std::vector<const CGTriangle3D*> const_container;
		this_ptr->get_const_vector(o, const_container);

		this_ptr->add_triangles(const_container);
		for (auto l : o)
			delete l;
	};

	//-------------------generate random surface
	auto generate_surface = [this_ptr](const AceGUI::call_back_params*){
		CGPoint3D min_bound(0, 0, 0);
		CGPoint3D max_bound(750, 750, 750);
		CGCuboid bounds(min_bound, max_bound);

		Tests t;
		std::vector<CGSurface3D*> o;
		int number_of_triangles =  rand() % 100 + 1;
		t.get_random_surfaces(bounds, 1, number_of_triangles,o);

		std::vector<const CGSurface3D*> const_container;
		this_ptr->get_const_vector(o, const_container);

		this_ptr->add_surfaces(const_container);
		for (auto l : o)
			delete l;
	};

	//-------------------generate test for line line intersection
	auto generatge_l_l = [this_ptr](const AceGUI::call_back_params*){
		CGPoint3D min_bound(0, 0, 0);
		CGPoint3D max_bound(500, 500, 500);
		CGCuboid bounds(min_bound, max_bound);

		Tests t;
		CGLine3D *l1, *l2;
		std::vector<CGPoint3D> pts;
		std::vector<const CGLine3D*> const_container;
		
		t.get_line_line_intersections_test(bounds, l1, l2, pts);

		const_container.push_back(l1);
		const_container.push_back(l2);

		this_ptr->add_lines(const_container);
		this_ptr->add_cross_hairs(pts);
		delete l1;
		delete l2;
	};


	//-------------------generate test for line Triangle intersection
	auto generatge_l_t = [this_ptr](const AceGUI::call_back_params*){
		CGPoint3D min_bound(0, 0, 0);
		CGPoint3D max_bound(750, 750, 750);
		CGCuboid bounds(min_bound, max_bound);

		Tests t;
		CGLine3D *l;
		CGTriangle3D *triangle;
		std::vector<CGPoint3D> pts;
		std::vector<const CGLine3D*> const_container;
		std::vector<const CGTriangle3D*> const_container2;

		t.get_line_triangle_intersections_test(bounds, l, triangle, pts);

		const_container.push_back(l);
		const_container2.push_back(triangle);

		this_ptr->add_lines(const_container);
		this_ptr->add_triangles(const_container2);
		this_ptr->add_cross_hairs(pts);

		std::vector<std::pair<CGPoint3D, CGPoint3D> > guides;
		for (auto p : pts){
			guides.push_back(std::make_pair(p, triangle->GetP1()));
			guides.push_back(std::make_pair(p, triangle->GetP2()));
			guides.push_back(std::make_pair(p, triangle->GetP3()));
		}
		this_ptr->add_guide_lines(guides);

		delete l;
		delete triangle;
	};

	//-------------------generate test for Line Surface intersection
	auto generatge_l_s = [this_ptr](const AceGUI::call_back_params*){
		CGPoint3D min_bound(0, 0, 0);
		CGPoint3D max_bound(750, 750, 750);
		CGCuboid bounds(min_bound, max_bound);

		Tests t;
		CGLine3D *l1;
		CGSurface3D *s1;
		std::vector<CGPoint3D> pts;
		std::vector<const CGLine3D*> const_container1;
		std::vector<const CGSurface3D*> const_container2;

		int number_of_triangles1 = rand() % 100 + 1;
		t.get_surface_line_intersections_test(bounds, l1, s1, number_of_triangles1, pts);

		const_container1.push_back(l1);
		const_container2.push_back(s1);

		this_ptr->add_lines(const_container1);
		this_ptr->add_surfaces(const_container2);
		this_ptr->add_cross_hairs(pts);

		delete l1;
		delete s1;
	};

	//-------------------generate test for Triangle Triangle intersection
	auto generatge_t_t = [this_ptr](const AceGUI::call_back_params*){
		CGPoint3D min_bound(0, 0, 0);
		CGPoint3D max_bound(750, 750, 750);
		CGCuboid bounds(min_bound, max_bound);

		Tests t;
		CGTriangle3D *triangle1;
		CGTriangle3D *triangle2;
		std::vector<CGPoint3D> pts;
		std::vector<const CGTriangle3D*> const_container;

		t.get_triangle_triangle_intersections_test(bounds, triangle1, triangle2, pts);

		const_container.push_back(triangle1);
		const_container.push_back(triangle2);
		
		this_ptr->add_triangles(const_container);
		this_ptr->add_cross_hairs(pts);

		std::vector<std::pair<CGPoint3D, CGPoint3D> > guides;
		for (auto p : pts){
			guides.push_back(std::make_pair(p, triangle1->GetP1()));
			guides.push_back(std::make_pair(p, triangle1->GetP2()));
			guides.push_back(std::make_pair(p, triangle1->GetP3()));

			guides.push_back(std::make_pair(p, triangle2->GetP1()));
			guides.push_back(std::make_pair(p, triangle2->GetP2()));
			guides.push_back(std::make_pair(p, triangle2->GetP3()));
		}
		this_ptr->add_guide_lines(guides);


		delete triangle1;
		delete triangle2;
	};

	//-------------------generate test for Triangle Surface intersection
	auto generatge_t_s = [this_ptr](const AceGUI::call_back_params*){
		CGPoint3D min_bound(0, 0, 0);
		CGPoint3D max_bound(750, 750, 750);
		CGCuboid bounds(min_bound, max_bound);

		Tests t;
		CGTriangle3D *t1;
		CGSurface3D *s1;
		std::vector<CGPoint3D> pts;
		std::vector<const CGTriangle3D*> const_container1;
		std::vector<const CGSurface3D*> const_container2;

		int number_of_triangles1 = rand() % 100 + 1;
		t.get_surface_triangle_intersections_test(bounds, t1, s1, number_of_triangles1, pts);

		const_container1.push_back(t1);
		const_container2.push_back(s1);

		this_ptr->add_triangles(const_container1);
		this_ptr->add_surfaces(const_container2);
		this_ptr->add_cross_hairs(pts);

		std::vector<std::pair<CGPoint3D, CGPoint3D> > guides;
		for (auto p : pts){

			guides.push_back(std::make_pair(p, t1->GetP1()));
			guides.push_back(std::make_pair(p, t1->GetP2()));
			guides.push_back(std::make_pair(p, t1->GetP3()));

		}
		this_ptr->add_guide_lines(guides);

		delete t1;
		delete s1;
	};

	//-------------------generate test for Surface Surface intersection
	auto generatge_s_s = [this_ptr](const AceGUI::call_back_params*){
		CGPoint3D min_bound(0, 0, 0);
		CGPoint3D max_bound(750, 750, 750);
		CGCuboid bounds(min_bound, max_bound);

		Tests t;
		CGSurface3D *s1;
		CGSurface3D *s2;
		std::vector<CGPoint3D> pts;
		std::vector<const CGSurface3D*> const_container;

		int number_of_triangles1 =rand() % 100 + 1;
		int number_of_triangles2 =rand() % 100 + 1;

		t.get_surface_surface_intersections_test(bounds, s1, number_of_triangles1, s2, number_of_triangles2, pts);

		const_container.push_back(s1);
		const_container.push_back(s2);

		this_ptr->add_surfaces(const_container);
		this_ptr->add_cross_hairs(pts);

		delete s1;
		delete s2;
	};

	//-------------------generate test for Surface Surface Merge
	auto generatge_s_s_m = [this_ptr](const AceGUI::call_back_params*){
		CGPoint3D min_bound(0, 0, 0);
		CGPoint3D max_bound(750, 750, 750);
		CGCuboid bounds(min_bound, max_bound);

		Tests t;
		CGSurface3D *s1;
		CGSurface3D *s2;
		CGSurface3D *merged = nullptr;;
		std::vector<const CGSurface3D*> const_container;

		int number_of_triangles1 = rand() % 100 + 1;
		int number_of_triangles2 = rand() % 100 + 1;

		t.get_surface_surface_merge_test(bounds, s1, number_of_triangles1, s2, number_of_triangles2, merged);

		//const_container.push_back(s1);
		//const_container.push_back(s2);
		if (merged)
			const_container.push_back(merged);

		this_ptr->add_surfaces(const_container);
		
		delete s1;
		delete s2;
		delete merged;
	};

	//------------------clear all
	auto clear_scene = [this_ptr](const AceGUI::call_back_params*){
		this_ptr->clear_all();
	};

	//-------------------toggle auto rotate
	auto auto_rotate = [this_ptr, menu_bar_ptr](const AceGUI::call_back_params*_params){
		std::string rotate_str = "Off";
		if (this_ptr->toggle_auto_rotate()){
			rotate_str = "On";
		}
		const AceGUI::button_click_params* button_click = static_cast<const AceGUI::button_click_params*>(_params);
		auto id = button_click->id();
		menu_bar_ptr->set_text(id, "rot(" + rotate_str + ")");
	};

	//-------------------toggle guides
	auto toggle_draw_guides = [this_ptr, menu_bar_ptr](const AceGUI::call_back_params*_params){
		std::string guides_str = "Off";
		if (this_ptr->toggle_guides()){
			guides_str = "On";
		}
		const AceGUI::button_click_params* button_click = static_cast<const AceGUI::button_click_params*>(_params);
		auto id = button_click->id();
		menu_bar_ptr->set_text(id, "guides(" + guides_str + ")");
	};

	//-------------------toggle global fill
	auto toggle_global_fill = [this_ptr, menu_bar_ptr](const AceGUI::call_back_params*_params){
		std::string fill_str = "Off";
		if (this_ptr->toggle_global_fill()){
			fill_str = "On";
		}
		const AceGUI::button_click_params* button_click = static_cast<const AceGUI::button_click_params*>(_params);
		auto id = button_click->id();
		menu_bar_ptr->set_text(id, "fill(" + fill_str + ")");
	};

	//-------------------toggle menu bar orientation
	auto on_toggle_menu_bar_orientation = [this_ptr, menu_bar_ptr](const AceGUI::call_back_params*_params){
		std::string orientation = "H";
		if (menu_bar_ptr->toggleOrientation()){
			orientation = "V";
		}
		const AceGUI::button_click_params* button_click = static_cast<const AceGUI::button_click_params*>(_params);
		auto id = button_click->id();
		menu_bar_ptr->set_text(id, "Menu(" + orientation + ")");
	};

	//-------------------increase camera rotation speed
	auto camera_rotations_speed_up = [this_ptr](const AceGUI::call_back_params*){
		this_ptr->change_camera_rotation_speed_by(1);
	};

	//-------------------dencrease camera rotation speed
	auto camera_rotations_speed_down = [this_ptr](const AceGUI::call_back_params*){
		this_ptr->change_camera_rotation_speed_by(-1);
	};

	//-------------------reset camera view
	auto reset_camera_view = [this_ptr](const AceGUI::call_back_params*){
		this_ptr->reset_camera();
	};

	//-------------------reset camera lights
	auto toggle_camera_lights = [this_ptr, menu_bar_ptr](const AceGUI::call_back_params*_params){
		std::string lights = "off";
		if (this_ptr->toggle_lights()){
			lights = "on";
		}
		const AceGUI::button_click_params* button_click = static_cast<const AceGUI::button_click_params*>(_params);
		auto id = button_click->id();
		menu_bar_ptr->set_text(id, "Lights(" + lights + ")");
		;
	};

	//-------------------reset camera view
	auto input_window = [this_ptr](const AceGUI::call_back_params*){
		AceDialog window("Dialog window");
		window.show(100, 100);

		
		while (!glfwWindowShouldClose(window.get_window())){
			glfwPollEvents();
			glfwSwapBuffers(window.get_window());
		}
		
	};

	//-------------------create menu bar

	std::string rotate_str = m_app_settings.p_auto_rotate ? "On" : "Off";
	m_menubar->add_button("Menu(V)", CREATE_CALLBACK(on_toggle_menu_bar_orientation));
	m_menubar->add_button("Point", CREATE_CALLBACK(generate_point));
	m_menubar->add_button("Line", CREATE_CALLBACK(generate_line));
	m_menubar->add_button("Triangle", CREATE_CALLBACK(generate_triangle));
	m_menubar->add_button("Surface", CREATE_CALLBACK(generate_surface));
	m_menubar->add_button("L-L", CREATE_CALLBACK(generatge_l_l));
	m_menubar->add_button("L-T", CREATE_CALLBACK(generatge_l_t));
	m_menubar->add_button("L-S", CREATE_CALLBACK(generatge_l_s));
	m_menubar->add_button("T-T", CREATE_CALLBACK(generatge_t_t));
	m_menubar->add_button("T-S", CREATE_CALLBACK(generatge_t_s));
	m_menubar->add_button("S-S", CREATE_CALLBACK(generatge_s_s));
	m_menubar->add_button("S-S-M", CREATE_CALLBACK(generatge_s_s_m));
	m_menubar->add_button("guides(on)", CREATE_CALLBACK(toggle_draw_guides));
	m_menubar->add_button("fill(off)", CREATE_CALLBACK(toggle_global_fill));
	m_menubar->add_button("clear", CREATE_CALLBACK(clear_scene));
	m_menubar->add_button("rot(" + rotate_str+")", CREATE_CALLBACK(auto_rotate));
	m_menubar->add_button("rot ++", CREATE_CALLBACK(camera_rotations_speed_up));
	m_menubar->add_button("rot --", CREATE_CALLBACK(camera_rotations_speed_down));
	m_menubar->add_button("reset cam", CREATE_CALLBACK(reset_camera_view));
	m_menubar->add_button("Lights(on)", CREATE_CALLBACK(toggle_camera_lights));
	m_menubar->add_button("input", CREATE_CALLBACK(input_window));
}

void GeometryViewer::clear_triangle_data(bool _filled){
	if (!_filled){
		m_triangles_wire_frame_data->clear();
		m_triangles_wire_frame_data->send_update_all_to_gpu();
	}
	else{
		m_triangles_filled_data->clear();
		m_triangles_filled_data->send_update_all_to_gpu();
	}
}

void GeometryViewer::clear_box_data(bool _filled){
	if (!_filled){
		m_box_wire_frame_data->clear();
		m_box_wire_frame_data->send_update_all_to_gpu();
	}
	else{
		m_box_filled_data->clear();
		m_box_filled_data->send_update_all_to_gpu();
	}
}

void GeometryViewer::clear_points_data(){
	m_points_data->clear();
	m_points_data->send_update_all_to_gpu();
}

void GeometryViewer::clear_guide_lines(){
	m_drawn_guide_lines_data->clear();
	m_drawn_guide_lines_data->send_update_all_to_gpu();
}

void GeometryViewer::clear_lines_data(){
	m_drawn_lines_data->clear();
	m_drawn_lines_data->send_update_all_to_gpu();
}

void GeometryViewer::clear_all(){
	clear_lines_data();
	clear_triangle_data(false);
	clear_triangle_data(true);
	clear_box_data(false);
	clear_box_data(true);
	clear_points_data();
	clear_guide_lines();
}

bool GeometryViewer::toggle_guides(){
	m_draw_guides = !m_draw_guides;
	return m_draw_guides;
}

bool GeometryViewer::toggle_global_fill(){
	m_global_fill = !m_global_fill;
	return m_global_fill;
}

bool GeometryViewer::toggle_auto_rotate(){

	if (m_current_camera){
		return m_current_camera->toggle_auto_rotate();
	}
	return false;

}

void GeometryViewer::change_camera_rotation_speed_by(const float &_delta){
	if (m_current_camera){
		m_current_camera->change_camera_rotation_speed_by(_delta);
	}
}

void GeometryViewer::reset_camera(){
	if (m_current_camera){
		m_current_camera->Reset();
	}
}

bool GeometryViewer::toggle_menu_bar_orientation(){
	if (m_menubar){
		return m_menubar->toggleOrientation();
	}
	return false;
}

bool GeometryViewer::toggle_lights(){
	if (m_current_camera){
		return m_current_camera->toggle_lighting();
	}
	return false;
}

void GeometryViewer::run(){
	//generate_test_data_triangles(false);
	render_pixels();
}
