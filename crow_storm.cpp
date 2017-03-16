#include "../crow/amalgamate/crow_all.h"

#include <sstream>

class ExampleLogHandler : public crow::ILogHandler {
    public:
        void log(std::string /*message*/, crow::LogLevel /*level*/) override {
	// cerr << "ExampleLogHandler -> " << message;
        }
};

int main()
{
    crow::SimpleApp app;

    //simple default route that returns crow-storm version
    CROW_ROUTE(app, "/")
        .name("crow")
    ([]{
        return "Crow Storm v0.0.1!";
    });

    // ignore all log
    crow::logger::setLogLevel(crow::LogLevel::Debug);
    //crow::logger::setHandler(std::make_shared<ExampleLogHandler>());

    app.port(18080)
        .multithreaded()
        .run();
}
