#include "../crow/amalgamate/crow_all.h"

// crow requirements
#include <sstream>

// file manipulation
#include <sys/stat.h>
#include <string>
#include <fstream>
#include <streambuf>

// used for simple "security" validation
#include <algorithm>

////////////////////// CONTENT TYPES ///////////////////////

// global content type map
std::map<std::string,std::string> g_content_types;

/**
 * Initialize known HTTP content types
 */
void init_content_types()
{
	g_content_types[".html"] 	= "text/html";
	g_content_types[".js"] 		= "text/javascript";
	g_content_types[".css"] 	= "text/css";
	g_content_types["unkown"]  	= "application/unknown";
}

/**
 * Get the content type for a specific file based on extension
 */
inline std::string get_content_type(const std::string& file_path)
{
	std::string::size_type idx;
	std::string extension;

	idx = file_path.rfind('.');

	if(idx != std::string::npos)
	{
		extension = file_path.substr(idx+1);

		// if we haven't found this known extension set to application/unknown
		if(g_content_types.find(extension) == g_content_types.end())
		{
			return g_content_types["uknown"];
		}
		else
		{
			return  g_content_types[extension];
		}
	}
	else
	{
	    return g_content_types["uknown"];
	}
}

/////////////////////////////// SYMBOL LIST ///////////////////////////////////

std::map<std::string,std::string> g_symbol_list;

/** 
 * Initialize the symbol list with data from Nasdaq's site
 */
void init_symbol_list()
{

}

/////////////////////////////// HELPER FUNCTIONS //////////////////////////////

/**
 * Check to see if a path exists
 */
inline bool path_exists(const std::string& name)
{
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

/**
 * Main crow based C++ HTTP server used for delivering the content needed by the CrowStorm platform
 */
int main()
{
    crow::SimpleApp app;

    // *NOTE* ~ performance concern with static file loading
    //          the files are loaded fully into memory before being returned
    //          ideally, the bytes could be streamed from the disk/cache instead
    //          this is especially important in large files

    // Initialize supported content types
    init_content_types();

    //simple default route that returns index.html
    CROW_ROUTE(app, "/")
        .name("crow")
    ([]{
    	std::string full_path = "./public/index.html";
		std::ifstream t(full_path);
		std::stringstream buffer;
		buffer << t.rdbuf();

		crow::response r(buffer.str());
		r.add_header("Content-Type", get_content_type(full_path));

		return r;
    });

    // route for loading static files (html/javascript/css)
    // VERY insecure as it does no URL validation and could be subject to
    // reverse directory traversal or other URL based request
    CROW_ROUTE(app, "/<string>")
    ([](std::string path){

    	// if there is ever '..', in the path, assume they are up to something
    	// will catch simple attempts to use "../" to go backwards in the directory structure
    	// will NOT catch more advanced encoding vulnerabilities (unicode, double unicode etc.)!
    	if (path.find("..") != std::string::npos)
    	{
    		return crow::response(404);
    	}

    	// we are "safe" to continue loading a static file
    	// for this simple example assume all static files live in the "./public/" directory
    	std::string full_path;
    	full_path =  "./public/" + path;

        if(path_exists(full_path)) {
			std::ifstream t(full_path);
			std::stringstream buffer;
			buffer << t.rdbuf();

			crow::response r(buffer.str());
			r.add_header("Content-Type", get_content_type(full_path));

			return r;
        }
        else {
        	return crow::response(404);
        }
    });

    // ignore all logging
    crow::logger::setLogLevel(crow::LogLevel::Debug);

    app.port(18080)
        .multithreaded()
        .run();
}
