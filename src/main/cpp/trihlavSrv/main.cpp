#include <iostream>
#include <stdexcept>
#include <string>
#include <Wt/WApplication>
#include <Wt/WServer>

#include "trihlavApp.hpp"

#if defined(__unix__)
#include <signal.h>
#endif

#if defined(__WIN32) || defined(__WIN64)
#include <process.h>
#endif

#ifdef ANDROID
#include "Android.h"
#endif

using namespace trihlav;
using namespace Wt;

static const std::string K_TRIHLAV_WT_HTTPD_CFG("/etc/trihlav/wt_httpd.ini");

int main(int argc, char **argv) {
	try {
		// use argv[0] as the application name to match a suitable entry
		// in the Wt configuration file, and use the default configuration
		// file (which defaults to /etc/wt/wt_config.xml unless the environment
		// variable WT_CONFIG_XML is set)
		WServer server(argv[0]);
		// WTHTTP_CONFIGURATION is e.g. "/etc/wt/wthttpd"
		server.setServerConfiguration(argc, argv, K_TRIHLAV_WT_HTTPD_CFG);
		// add a single entry point, at the default location (as determined
		// by the server configuration's deploy-path)
		server.addEntryPoint(Wt::Application, App::createApplication);
		if (server.start()) {
			int sig = WServer::waitForShutdown(argv[0]);
			std::cerr << "Shutdown (signal = " << sig << ")" << std::endl;
			server.stop();
			if (sig == SIGHUP)
				WServer::restart(argc, argv, environ);
		}
	} catch (WServer::Exception& e) {
		std::cerr << e.what() << "\n";
		return 1;
	} catch (std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
		return 1;
	}
}