#include <boost/beast.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream> 
#include <string>
        
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class Net {
	private:
		boost::asio::io_context ioc;
		tcp::socket socket;
		tcp::resolver resolver;
		char* host;
	public:
		Net(char* ip, char* port);
		~Net();
		http::response<http::dynamic_body> get_http(const char *target);
		http::response<http::dynamic_body> post_http(const char *target, char *post_data);
		http::response<http::dynamic_body> put_http(const char *target);
		http::response<http::dynamic_body> delete_http(const char *target);
		http::response<http::dynamic_body> head_http(const char *target);
};
