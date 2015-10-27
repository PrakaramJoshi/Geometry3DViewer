#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H
#include <string>
#include <vector>
#include <boost\program_options.hpp>
#include <Logger\Logger.h>
#include <boost\filesystem.hpp>
#include <boost\algorithm\string.hpp>
#include <CGMath.h>
#include <CG3DConstants.h>
struct app_settings{

	float p_fov;
	float p_zNear;
	float p_zFar;
	float p_zoom_factor;

	float p_lattice_size_x;
	float p_lattice_size_y;
	float p_lattice_size_z;

	float p_lattice_row_size_x;
	float p_lattice_row_size_y;
	float p_lattice_row_size_z;

	bool p_draw_lattice;

	float p_world_size_x;
	float p_world_size_y;
	float p_world_size_z;

	float p_coordinate_plane_size;

	bool p_auto_rotate;
	float p_rotate_x;
	float p_rotate_y;
	float p_rotate_z;
	float p_rotate_about_x;
	float p_rotate_about_y;
	float p_rotate_about_z;
	float p_rotate_speed;

	bool p_full_screen;

	CG3DContants::OCTANTS p_octant;

	app_settings(){

		p_fov = 50.0;
		p_zNear = 0.01;
		p_zFar = 1000.0;
		p_zoom_factor = 5.0;

		p_lattice_size_x = 0;
		p_lattice_size_y = 0;
		p_lattice_size_z = 0;

		p_lattice_row_size_x = 0;
		p_lattice_row_size_y = 0;
		p_lattice_row_size_z = 0;

		p_draw_lattice = false;

		p_world_size_x = 2000;
		p_world_size_y = 2000;
		p_world_size_z = 2000;

		p_coordinate_plane_size = 0.1;
		p_auto_rotate = false;

		p_rotate_x = 0;
		p_rotate_y = 1;
		p_rotate_z = 0;

		p_rotate_about_x = 0;
		p_rotate_about_y = 1;
		p_rotate_about_z = 0;

		p_rotate_speed = 0.5;

		p_octant = CG3DContants::OCTANTS::PLUS_PLUS_PLUS;

		p_full_screen = false;

	};

	bool read_settings(const std::string &_settings_file){
		try
		{
			std::vector<std::string> options;
			if (!read_file(_settings_file, options)){
				AceLogger::Log("Cannot read settings as the settings file not found.", AceLogger::LOG_ERROR);
				return false;
			}

			namespace po = boost::program_options;
			po::options_description desc("Options");

			std::string draw_lattice;
			std::string auto_rotate;
			std::string octant;
			std::string full_screen;
			desc.add_options()
				("help", "Print help messages")
				("zoom_factor", po::value<float>(&p_zoom_factor)->default_value(5), "camera zoom factor")
				("fov", po::value<float>(&p_fov)->default_value(0), "camera fov")
				("znear", po::value<float>(&p_zNear)->default_value(0), "camera closest z rendered")
				("zfar", po::value<float>(&p_zFar)->default_value(0), "camera furthest z rendered")
				("lattice_size_x", po::value<float>(&p_lattice_size_x)->default_value(0), "lattice size x")
				("lattice_size_y", po::value<float>(&p_lattice_size_y)->default_value(0), "lattice size y")
				("lattice_size_z", po::value<float>(&p_lattice_size_z)->default_value(0), "lattice size z")
				("lattice_row_size_x", po::value<float>(&p_lattice_row_size_x)->default_value(0), "lattice row size x")
				("lattice_row_size_y", po::value<float>(&p_lattice_row_size_y)->default_value(0), "lattice row size y")
				("lattice_row_size_z", po::value<float>(&p_lattice_row_size_z)->default_value(0), "lattice row size z")
				("draw_lattice", po::value<std::string>(&draw_lattice)->default_value("No"), "draw lattice(yes/no)")
				("world_size_x", po::value<float>(&p_world_size_x)->default_value(0), "world size x")
				("world_size_y", po::value<float>(&p_world_size_y)->default_value(0), "world size y")
				("world_size_z", po::value<float>(&p_world_size_z)->default_value(0), "world size z")
				("auto_rotate", po::value<std::string>(&auto_rotate)->default_value("No"), "auto rotate the view")
				("rotate_x", po::value<float>(&p_rotate_x)->default_value(0), "rotate about point x")
				("rotate_y", po::value<float>(&p_rotate_y)->default_value(0), "rotate about point y")
				("rotate_z", po::value<float>(&p_rotate_z)->default_value(0), "rotate about point z")
				("rotate_about_x", po::value<float>(&p_rotate_about_x)->default_value(0), "rotate about x")
				("rotate_about_y", po::value<float>(&p_rotate_about_y)->default_value(0), "rotate about y")
				("rotate_about_z", po::value<float>(&p_rotate_about_z)->default_value(0), "rotate about z")
				("rotate_speed", po::value<float>(&p_rotate_speed)->default_value(0), "rotate speed degrees per second")
				("coordinate_plane_grid_size", po::value<float>(&p_coordinate_plane_size)->default_value(0.1), "grid size for coordinate planes")
				("coordinate_plane_grid_octant", po::value<std::string>(&octant)->default_value("PLUS_PLUS_PLUS"), "coordinate plane grid octant")
				("fullscreen", po::value<std::string>(&full_screen)->default_value("No"), "full screen (yes/no)");
			


			po::variables_map vm;
			try
			{
				po::store(po::command_line_parser(options).options(desc).run(),
					vm);
				if (vm.count("help"))
				{
					AceLogger::Log("Help not available", AceLogger::LOG_WARNING);
					return false;
				}

				po::notify(vm);
				
				if (!get_bool(draw_lattice, p_draw_lattice)){
					AceLogger::Log("invalid option for draw_lattice( choose yes or no", AceLogger::LOG_ERROR);
					return false;
				}

				if (!get_bool(auto_rotate, p_auto_rotate)){
					AceLogger::Log("invalid option for auto_rotate( choose yes or no", AceLogger::LOG_ERROR);
					return false;
				}

				if (!get_bool(full_screen, p_full_screen)){
					AceLogger::Log("invalid option for fullscreen( choose yes or no", AceLogger::LOG_ERROR);
					return false;
				}

				p_octant = CG3DContants::GetOctant(octant);
				if (p_octant == CG3DContants::OCTANTS::UNKNOWN_OCTANT){
					AceLogger::Log("invalid option for octant:" + octant, AceLogger::LOG_ERROR);
					return false;
				}

				return validate_settings();
			}
			catch (po::error& e)
			{
				AceLogger::Log(e.what(), AceLogger::LOG_ERROR);
				return false;
			}


		}
		catch (std::exception& e)
		{
			AceLogger::Log("Unhandled Exception", AceLogger::LOG_ERROR);
			AceLogger::Log(e.what(), AceLogger::LOG_ERROR);
			return false;

		}
	}
private:
	bool read_file(const std::string &_file_path,
		std::vector<std::string> &_data)const{
		if (!boost::filesystem::exists(boost::filesystem::path(_file_path))){
			AceLogger::Log("file doesn't exist: " + _file_path, AceLogger::LOG_ERROR);
			return false;
		}
		std::ifstream ifs(_file_path);
		std::string temp;
		while (std::getline(ifs, temp)){
			_data.push_back(temp);
		}
		ifs.close();
		return true;
	};

	bool get_bool(std::string _str, bool &_val){
		boost::trim(_str);
		if (boost::iequals(_str, "Yes")){
			_val = true;
			return true;
		}
		else if (boost::iequals(_str, "No")){
			_val = false;
			return true;
		}
		return false;

	}

	bool fill_gradient(const std::string &_val,
		std::vector<float>&_c){
		std::vector<std::string>tokens;
		StringUtils::split(_val, tokens, ',');
		if (tokens.size() != 3){
			AceLogger::Log("invalid info for color gradient", AceLogger::LOG_ERROR);
			return false;
		}
		if (!StringUtils::is_type<float>(tokens[0], _c[0])){
			AceLogger::Log("invalid data for r in color gradient", AceLogger::LOG_ERROR);
			return false;
		}
		if (!StringUtils::is_type<float>(tokens[1], _c[1])){
			AceLogger::Log("invalid data for g in color gradient", AceLogger::LOG_ERROR);
			return false;
		}
		if (!StringUtils::is_type<float>(tokens[2], _c[2])){
			AceLogger::Log("invalid data for b in color gradient", AceLogger::LOG_ERROR);
			return false;
		}
		return true;
	}

	bool validate_settings()const {
		bool returnVal = true;
		if (p_world_size_x <= 0){
			AceLogger::Log("world size x cannot be less than equal to 0", AceLogger::LOG_ERROR);
			returnVal = false;
		}

		if (p_world_size_y <= 0){
			AceLogger::Log("world size y cannot be less than equal to 0", AceLogger::LOG_ERROR);
			returnVal = false;
		}

		if (p_world_size_z <= 0){
			AceLogger::Log("world size z cannot be less than equal to 0", AceLogger::LOG_ERROR);
			returnVal = false;
		}
		if (returnVal)
			AceLogger::Log("input data validated");

		return returnVal;
	}
};
#endif