#include "../crow/amalgamate/crow_all.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <streambuf>

#include <curl/curl.h>
#include <sys/stat.h>

#include <boost/filesystem.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

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
		extension = file_path.substr(idx);

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

/////////////////////////////// SYMBOLS ///////////////////////////////////

std::map<std::string,std::string> g_symbol_locations;

// originally used a simple company name -> symbol, but it's one to many so lets use a set for symbol
std::map<std::string,std::set<std::string>> g_company_list;

// helper functions for writing curl data to an output stream
// http://www.cplusplus.com/forum/unices/45878/

// callback function writes data to a std::ostream
static size_t data_write(void* buf, size_t size, size_t nmemb, void* userp)
{
	if(userp)
	{
		std::ostream& os = *static_cast<std::ostream*>(userp);
		std::streamsize len = size * nmemb;
		if(os.write(static_cast<char*>(buf), len))
			return len;
	}

	return 0;
}

/**
 * timeout is in seconds
 **/
CURLcode curl_read(const std::string& url, std::ostream& os, long timeout = 30)
{
	CURLcode code(CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init();

	if(curl)
	{
		if(CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &os))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str())))
		{
			code = curl_easy_perform(curl);
		}
		curl_easy_cleanup(curl);
	}
	return code;
}


/** 
 * Initialize the symbol list with data from Nasdaq's site
 */
void init_company_list()
{
	// create "data" directory if it doesn't exist
	std::string data_dir = "./data/";
	boost::filesystem::path dir(data_dir);
	if(boost::filesystem::create_directory(data_dir))
	{
	    //std::cerr<< "Directory Created: "<<data_dir<<std::endl;
	}

	// clear out map, useful in case we want to re-load data by calling this function
	g_company_list.clear();

	// config file with list of symbol exchanges we want to grab
	std::ifstream infile("symbol_locations.txt");

	//read in the potential sources for symbols
	std::string line;
	std::string out_file_name;
	std::string input_url;
	size_t equal_pos;

	// 1. Read the possible exchanges we can get data from and load the URL and output file name into map
	while (std::getline(infile, line))
	{
		input_url = "http://www.nasdaq.com/screening/companies-by-name.aspx?letter=0&exchange=" + line + "&render=download";

		// strip out comments and newlines
		if(line.at(0) != '#' &&
		   line.at(0) != '\n')
		{
			out_file_name = data_dir + line + ".csv";
			g_symbol_locations[input_url] = out_file_name;
		}
	}

	// 2. Iterate through map, download the exchange's symbol list
	for (auto& symbol_location : g_symbol_locations) {
		std::ofstream ofs(symbol_location.second);
		if(CURLE_OK == curl_read(symbol_location.first, ofs))
		{
			// Web page successfully written to file
		}
	}


	// 3. Iterate map again, this time pulling out the name and symbol data and popping into symbol_list
	for (auto& symbol_location : g_symbol_locations) {
		std::ifstream infile(symbol_location.second);

		while (std::getline(infile, line))
		{
			std::string column;
			std::string symbol;
			std::string name;
			uint8_t col_num = 0;
			size_t pos = 0;

			// have to split on ", and not just , since names have ',' in them
			std::string delimiter = "\",";

			while ((pos = line.find(delimiter)) != std::string::npos) {
			    column = line.substr(0, pos);

			    // get the first two columns (Symbol and Name), break out after that
				if(col_num > 1)
				{
					break;
				}

				// strip double quotes
				column.erase(std::remove(column.begin(), column.end(), '"'), column.end());

				// skip first line
				if(col_num == 0 &&
				   column.compare("Symbol") == 0)
				{
					break;
				}
				// column 0 is the symbol
				else if(col_num == 0)
				{
					symbol = column;
				}
				// column 1 is the company name
				else if(col_num == 1)
				{
					name = column;
				}

				col_num++;

			    line.erase(0, pos + delimiter.length());
			}

		    // use company name for key so we can sort for use-case
		    g_company_list[name].insert(symbol);
		}
	}
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
 * Build Yahoo URL given the symbol name and the number of days in the past for which we need data.
 */

// boost date formatting used for day/year, month is special in that it's zero indexed
const std::locale fmt_yahoo_start_day_year(std::locale::classic(), new boost::gregorian::date_facet("&b=%d&c=%Y"));
const std::locale fmt_yahoo_end_day_year(std::locale::classic(), new boost::gregorian::date_facet("&e=%d&f=%Y"));

std::string build_yahoo_url(std::string symbol, uint32_t days_prior)
{
	std::string yahoo_url;
	std::string start_month;
	std::string end_month;
	boost::gregorian::date today_local, days_prior_local;
	boost::gregorian::days days_object(days_prior);

	today_local = boost::gregorian::day_clock::local_day();
	days_prior_local = today_local - days_object;

	yahoo_url = "http://ichart.finance.yahoo.com/table.csv?s=" + symbol;

	// build date string, this is a bit annoying because Yahoo does the following
	// 1. month is SINGLE digit, 0 based offset (Jan=0, Feb=1 etc.)
	// 2. day is TWO digits (zero padded)
	// 3. year is FOUR digits
	// sample showing dates between 2017-02-16 and 2017-03-18:
	// "&a=1&b=16&c=2017&d=2&e=18&f=2017"

	// get the start MONTH first, have to subtract by one for zero index
	yahoo_url += "&a=" + std::to_string(days_prior_local.month()-1);

	// now get the start DAY and year using boost's date formatting
	std::ostringstream os_yahoo_start_day_year;
	os_yahoo_start_day_year.imbue(fmt_yahoo_start_day_year);
	os_yahoo_start_day_year << days_prior_local;

	yahoo_url += os_yahoo_start_day_year.str();

	// get the end MONTH first, have to subtract by one for zero index
	yahoo_url += "&d=" + std::to_string(today_local.month()-1);

	// now get the end DAY and year using boost's date formatting
	std::ostringstream os_yahoo_end_day_year;
	os_yahoo_end_day_year.imbue(fmt_yahoo_end_day_year);
	os_yahoo_end_day_year << today_local;

	yahoo_url += os_yahoo_end_day_year.str();

	// add final params to URL
	yahoo_url += "&g=d&ignore=.csv";

	return yahoo_url;
}

/**
 * Main crow based C++ HTTP server used for delivering the content needed by the CrowStorm platform
 */
int main()
{
	// initialize libcurl globally here, do only ONCE
	curl_global_init(CURL_GLOBAL_ALL);

    crow::SimpleApp app;

    // *NOTE* ~ performance concern with static file loading
    //          the files are loaded fully into memory before being returned.
    //          Ideally, the bytes could be streamed from the disk/cache instead
    //          this is especially important in large files

    // Initialize supported content types
    init_content_types();

    // Initialize the company->symbol list on server start
    init_company_list();

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


    // route for returning a list of companies and their symbols based on "prefix"
    // added a return limit for quick ajax return
    CROW_ROUTE(app, "/company/<string>/<int>")
    ([](std::string company_query, uint32_t return_limit){
        crow::json::wvalue companies;
        uint32_t num_companies_found;

		// convert company_query to upper.. ONCE
		std::string prefix = company_query;
		std::transform(prefix.begin(), prefix.end(),prefix.begin(), ::toupper);

		num_companies_found = 0;

        // very simple way to do this, iterate through company list and pull out anything that starts with submission
    	for (auto& company_it : g_company_list)
    	{
    		// if a return_limit was passed in, return after that many records
    		if(return_limit &&
    		   num_companies_found >= return_limit) {
    			break;
    		}

    		// convert company name to upper (should probably cache this for speed)
    		std::string company_name = company_it.first;
    		std::transform(company_name.begin(), company_name.end(),company_name.begin(), ::toupper);
    		std::string company_prefix = company_name.substr(0, prefix.size());

    		// for now, store a list of symbols as a string until we know the crow::json structure better
    		std::string symbol_list;

    		// if the company name starts with the prefix, we have a match
    		if(company_prefix == prefix) {
        		// now dump the symbols for the company
        		for (auto& symbol : company_it.second)
        		{
        			 symbol_list += symbol + "|";
        		}

        		// remove the end "|"
        		symbol_list.pop_back();

        		// add symbol to symbol list for this company
        		companies[company_it.first] = symbol_list;

        		num_companies_found++;
    		}
    	}

    	// have to handle no results found
    	if(!num_companies_found)
    	{
    		companies["NO_COMPANIES_FOUND"] = "";
    	}

    	return companies;
    });

    // route for returning OHLC data for the past 30 days from yahoo's data api using JSON + | delimited rows
    CROW_ROUTE(app, "/symbol/json/<string>")
    ([](std::string symbol_query){
        crow::json::wvalue symbol_prices;
        uint32_t num_prices_found;
        std::string yahoo_url;

        num_prices_found = 0;

        // let's build the URL based on the symbol and 30 for now
        // ENHANCEMENT ~ allow user to choose number of days (add max limit server side)
        yahoo_url = build_yahoo_url(symbol_query, 30);

    	// pull in data
    	std::ostringstream os_yahoo_data_stream;
    	if(CURLE_OK == curl_read(yahoo_url, os_yahoo_data_stream))
    	{
    		std::istringstream in_string(os_yahoo_data_stream.str());
    		std::string line;

    		while (std::getline(in_string, line))
    		{
    			std::string column, date, ohlcv_json;
    			uint8_t col_num = 0;
    			size_t pos = 0;
    			bool break_early = false;

    			// data is fairly clean, can split on just ","
    			std::string delimiter = ",";

    			while ((pos = line.find(delimiter)) != std::string::npos) {
    			    column = line.substr(0, pos);

    				// populate data depending on which column we are on
    				switch(col_num){
    				// date
    				case 0:
    					// skip first line
    					if(column.compare("Date") == 0)
    					{
    						break_early = true;
    					}
    					// otherwise assign date
    					else
    					{
    						date = column;
    					}
    					break;
    				// open
    				case 1:
    					ohlcv_json +=  column + "|";
    					break;
					// high
    				case 2:
    					ohlcv_json += column + "|";
    					break;
					// low
    				case 3:
    					ohlcv_json += column + "|";
						break;
					// close
    				case 4:
    					ohlcv_json += column + "|";
    					break;
    				// volume
    				case 5:
    					ohlcv_json += column;
    					break;
    				default:
    					// all other columns do nothing
    					break;
    				}

    				col_num++;

    				// skip the first line
    				if(break_early)
    				{
    					break;
    				}

    			    line.erase(0, pos + delimiter.length());
    			}

    			num_prices_found++;

    			// only do this if we have a non-empty date
    			if(date != "")
    			{
					// date for key and OHLCV as data
					symbol_prices[date] = ohlcv_json;
    			}
    		}
    	}

    	// have to handle no results found
    	if(!num_prices_found)
    	{
    		symbol_prices["NO_DATA_FOUND"] = "";
    	}

    	return symbol_prices;
    });

    // route for returning OHLC data for the past 30 days from yahoo's data api returning .csv
    CROW_ROUTE(app, "/symbol/csv/<string>")
       ([](std::string symbol_query){
		std::string yahoo_url;

		// let's build the URL based on the symbol and 30 for now
		// ENHANCEMENT ~ allow user to choose number of days (add max limit server side)
		yahoo_url = build_yahoo_url(symbol_query, 30);
			
		// pull in data
		std::ostringstream os_yahoo_data_stream;
		if(CURLE_OK == curl_read(yahoo_url, os_yahoo_data_stream))
		{
			return os_yahoo_data_stream.str();
		}
		else
		{
			return std::string("NO_DATA_FOUND");
		}
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

    // cleanup libcurl globally here, do only ONCE
    curl_global_cleanup();
}
