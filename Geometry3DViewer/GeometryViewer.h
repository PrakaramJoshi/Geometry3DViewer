#pragma once
#include <Datastructures\DataStructure.h>
#include <OpenGL\ShadersUtility.h>
#include <Geometry\DrawingUtils.h>
#include <OpenGL\glfwWindow.h>
#include "application_settings.h"
#include <vector>
#include <thread>
#include <Geometry3D/CGPoint3D.h>
#include <Geometry3D/CGLine3D.h>
#include <Geometry3D\CGCuboid.h>
#include <Geometry3D\CGTriangle3D.h>
#include "CameraView.h"
class CGSurface3D;
namespace AceGUI{
	class MenuBar;
}
class GeometryViewer
{

	//typedef DrawingUtils::camera<DrawingUtils::camera_motions::no_rotation> CurrentCamera;
	typedef CameraView CurrentCamera;

	GLuint m_programID;

	vao_state m_lines_persistent;
	DrawingUtils::DrawnLines *m_drawn_lines_persistent;

	vao_state m_lines_data;
	DrawingUtils::DrawnLines *m_drawn_lines_data;

	vao_state m_guide_lines;
	DrawingUtils::DrawnLines *m_drawn_guide_lines_data;

	vao_state m_triangles_filled;
	DrawingUtils::DrawnTrianglesFilled *m_triangles_filled_data;

	vao_state m_triangles_wire_frame;
	DrawingUtils::DrawnTrianglesWireFrame *m_triangles_wire_frame_data;

	vao_state m_points;
	DrawingUtils::DrawnPoints *m_points_data;

	vao_state m_box_filled;
	DrawingUtils::DrawnBoxFilled *m_box_filled_data;

	vao_state m_box_wire_frame;
	DrawingUtils::DrawnBoxWireFrame *m_box_wire_frame_data;

	AceWindow m_window;

	ViewPort m_viewPort;

	float m_world_size_x;

	float m_world_size_y;

	float m_world_size_z;

	bool m_draw_guides;

	bool m_global_fill;

	app_settings m_app_settings;

	AceGUI::MenuBar *m_menubar;

	CurrentCamera *m_current_camera;

	void load_shaders();

	void render_pixels();

	void render();

	CGPoint3D normalize_point(const CGPoint3D &_pt)const;

	void clear_triangle_data(bool _filled);

	void clear_box_data(bool _filled);

	void clear_lines_data();

	void clear_points_data();

	void clear_guide_lines();

	void generate_colors();

	void generate_menu_bar();

	std::tuple<float, float, float> get_next_color();

	std::vector<std::tuple<float, float, float> >m_sample_colors;
	int m_color_index;

	void generate_guides(const CGPoint3D &_pt1, const CGPoint3D &_p2,
		std::vector<std::pair<CGPoint3D, CGPoint3D> > &_guides);
	

public:
	GeometryViewer(app_settings &_app_seetings);
	~GeometryViewer();

	bool setup();

	void run();

	void add_points(const std::vector<CGPoint3D> &_points);

	void add_cross_hairs(const std::vector<CGPoint3D> &_points);

	void add_lines(const std::vector<const CGLine3D*> &_lines);

	void add_unicolor_lines(const std::vector<const CGLine3D*> &_lines);

	void add_guide_lines(const std::vector<std::pair<CGPoint3D, CGPoint3D> > &_lines);

	void add_triangles(const std::vector<const CGTriangle3D*> &_triangles, bool _filled =false);

	void add_surfaces(const std::vector<const CGSurface3D*> &_surfaces, bool _filled = false);

	void add_triangles_projection(std::vector<CGTriangle3D> &_triangles, bool _filled = false);

	void add_lines_projection(std::vector<CGLine3D> &_lines);

	void add_box_filled_collection(std::vector<CGCuboid> &_cuboids);

	void add_box_wire_frame_collection(std::vector<CGCuboid> &_cuboids);

	void clear_all();

	bool toggle_auto_rotate();

	void change_camera_rotation_speed_by(const float &_delta);

	void reset_camera();

	bool toggle_guides();

	bool toggle_global_fill();

	bool toggle_menu_bar_orientation();

	bool toggle_lights();

	template<typename T>
	void get_const_vector(std::vector<T*> &_v_in,
		std::vector<const T*> &_v_out)const{

		std::copy(_v_in.begin(), _v_in.end(), std::back_inserter(_v_out));
	}
};

