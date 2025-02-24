#include <Application/ApplicationSelection.h>
#include <Application/SIAA_TP4_MotionPlanning.h>
#include <Application/SIAA_TP5_behavior.h>

#include <Application/GL3_TP1.h>

int main(int argc, char ** argv)
{
  ::std::cout<<"Path of the executable: "<<System::Path::executable()<<::std::endl ;
	// Registers the application 
	Application::ApplicationSelection::registerFactory<Application::GL3_TP1>("OpenGL 3 - TP 1");
	Application::ApplicationSelection::registerFactory<Application::SIAA_TP4_MotionPlanning>("ANA - TP 1");
	Application::ApplicationSelection::registerFactory<Application::SIAA_TP5_behavior>("ANA - TP 2");

	// Initializes GLUT and GLEW
	Application::Base::initializeGLUT(argc, argv) ;
	// Selection of the application and run
	Application::ApplicationSelection::selectAndRun() ;
}
