#include "cli_ex.h"

Net::Net(char* ip, char* port) 
: socket(ioc)
, resolver(ioc) {
        auto const results = resolver.resolve(ip, port);
        boost::asio::connect(socket, results.begin(), results.end());
	host = ip;
}

Net::~Net() {
        boost::system::error_code ec;
        socket.shutdown(tcp::socket::shutdown_both, ec);

        if(ec && ec != boost::system::errc::not_connected)
            throw boost::system::system_error{ec};
}

http::response<http::dynamic_body> Net::get_http(const char *target)
{
        http::request<http::string_body> req{http::verb::get, target, 10};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        http::write(socket, req);
        boost::beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(socket, buffer, res);
        std::cout << res << std::endl;
	return res;
}

http::response<http::dynamic_body> Net::post_http(const char* target, char* post_data)
{
        http::request<http::string_body> req{http::verb::get, target, 10};
        req.set(http::field::host, host);
	req.method(boost::beast::http::verb::post);
	req.set(boost::beast::http::field::content_type, "application/json");
	req.body() = post_data;
	req.prepare_payload();
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        http::write(socket, req);
        boost::beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(socket, buffer, res);
        std::cout << res << std::endl;
	return res;
}

http::response<http::dynamic_body> Net::put_http(const char *target)
{
        http::request<http::string_body> req{http::verb::put, target, 10};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        http::write(socket, req);
        boost::beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(socket, buffer, res);
        std::cout << res << std::endl;
	return res;
}

http::response<http::dynamic_body> Net::delete_http(const char *target)
{
        http::request<http::string_body> req{http::verb::get, target, 10};
        req.set(http::field::host, host);
	req.method(boost::beast::http::verb::delete_);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        http::write(socket, req);
        boost::beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(socket, buffer, res);
        std::cout << res << std::endl;
	return res;
}

http::response<http::dynamic_body> Net::head_http(const char *target)
{
        http::request<http::string_body> req{http::verb::get, target, 10};
        req.set(http::field::host, host);
	req.method(boost::beast::http::verb::head);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        http::write(socket, req);
        boost::beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(socket, buffer, res);
        std::cout << res << std::endl;
	return res;
}
