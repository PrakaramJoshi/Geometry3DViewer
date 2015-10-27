// Geometry3DViewer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <thread>
#include <Logger\Logger.h>
#include <OpenGL\glfwWindow.h>
#include <OpenGL\ShadersUtility.h>
#include "application_settings.h"
#include "GeometryViewer.h"

REGISTER_LOGGER(R"(B:\Logs)", "Geometry3DViewer", "0.0.0.1");

using namespace std;
using namespace AceLogger;
void exitApp(){
	// exit sequence 
	
	//close the logger
	FinishLog();
};

bool read_app_settings(app_settings &_app_ssettings_val){
	std::string app_setting_file = R"(B:\Workspace\Geometry3DViewer\app_settings.txt)";
	Log("Reading app settings file at : " + app_setting_file);
	return _app_ssettings_val.read_settings(app_setting_file);
}

void exe_loop(app_settings *_app_ssettings_val){
	
	GeometryViewer geo_viewer(*_app_ssettings_val);
	if (geo_viewer.setup()){
		Log("Starting the 3D viewer...");
		geo_viewer.run();
	}
	else
		Log("unable to complete pre start sequence!", LOG_ERROR);
	
}

int _tmain(int argc, _TCHAR* argv[])
{
	atexit(exitApp);
	app_settings app_ssettings_val;
	if (!read_app_settings(app_ssettings_val)){
		Log("Cannot continue as unable to read settings file.", LOG_ERROR);
		exit(0);
	}
	std::thread normal_view(exe_loop, &app_ssettings_val);
	normal_view.join();
	return 0;
}

