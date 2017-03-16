#include "../crow/amalgamate/crow_all.h"

// crow requirements
#include <sstream>

// file manipulation
#include <sys/stat.h>
#include <string>
#include <fstream>
#include <streambuf>

// global mime type map
std::map<std::string,std::string> g_mime_types;

/**
 * Check to see if a path exists
 */
inline bool path_exists(const std::string& name)
{
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

/**
 * Get the content type for a specific file
 */
inline std::string pick_content_type(const std::string& file_path)
{
	std::string::size_type idx;
	std::string extension;

	idx = file_path.rfind('.');

	if(idx != std::string::npos)
	{
		extension = file_path.substr(idx+1);

		// if we haven't found this known extension set to application/unknown
		if(g_mime_types.find(extension) == g_mime_types.end())
		{
			return g_mime_types["uknown"];
		}
		else
		{
			return  g_mime_types[extension];
		}
	}
	else
	{
	    return g_mime_types["uknown"];
	}
}

/**
 * Initialize known mime types
 */
void init_mime_types()
{
	g_mime_types[".html"] 	= "text/html";
	g_mime_types[".js"] 	= "text/javascript";
	g_mime_types[".css"] 	= "text/css";
	g_mime_types["unkown"]  = "application/unknown";
}

int main()
{
    crow::SimpleApp app;

    //simple default route that returns crow-storm version
    CROW_ROUTE(app, "/")
        .name("crow")
    ([]{
        return "Crow Storm v0.0.1!";
    });

    // route for loading static files (html/javascript/css)
    CROW_ROUTE(app, "/<string>")
    ([](std::string path){
    	std::string full_path;
    	full_path =  "./public/" + path;

        if(path_exists(full_path)) {
			std::ifstream t(full_path);
			std::stringstream buffer;
			buffer << t.rdbuf();

			crow::response r(buffer.str());
			r.add_header("Content-Type", pick_content_type(full_path));

			return r;
        }
        else {
        	return crow::response(404);
        }
    });

    // ignore all log
    crow::logger::setLogLevel(crow::LogLevel::Debug);
    //crow::logger::setHandler(std::make_shared<ExampleLogHandler>());

    app.port(18080)
        .multithreaded()
        .run();
}
