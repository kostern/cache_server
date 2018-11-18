#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "cache.h"
#include <unordered_map>
#include <cstring> //for "std::memcpy" in set
#include <iostream> //for queue.display
#define PORT 10000
#define HOST "127.0.0.1"

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

struct Cache::Impl {
private:
	index_type maxmem_;
	hash_func hasher_;
	tcp::resolver resv;
	tcp::socket socket;	
public:
	Impl(index_type maxmem, hash_func hasher)
	 : maxmem_(maxmem), hasher_(hasher), memused_(0), data_(0, hasher_)
	{
		try {
			boost::asio::io_context ioc;
			resv = resolver{ioc};
			socket = socket{ioc};
			auto const results = resv.resolve(HOST, PORT);
			boost::asio::connect(socket, results.begin(), results.end());
		
			 // Set up an HTTP GET request message
			http::request<http::string_body> req{http::verb::get, "/memsize", 10};
			req.set(http::field::host, HOST);
			req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

			http::write(socket, req);
			boost::beast::flat_buffer buffer;
			http::response<http::dynamic_body> res;
			http::read(socket, buffer, res);
			std::cout << res << std::endl;

		} catch(std::exception const& e) {
		        std::cerr << "Error: " << e.what() << std::endl;
		}	
	}

	~Impl()
	{
		try {
			boost::system::error_code ec;
        		socket.shutdown(tcp::socket::shutdown_both, ec);

			if(ec && ec != boost::system::errc::not_connected)
				throw boost::system::system_error{ec};
		} catch(std::exception const& e) {
			std::cerr << "Error: " << e.what() << std::endl;
                }

	}

	//returns 0: successful set
	//returns 1: item larger than maxmem
	int set(key_type key, val_type val, index_type size)
	{
		return 0;
	}
	
	//returns ptr: successful get
	//returns NULL: no ptr associated with key
	val_type get(key_type key, index_type& val_size)
	{
		//takes key and size of retrieved value
		//return a pointer to key in array
	}

	//returns 0: successful delete
	//returns 1: no pointer associated with key
	int del(key_type key)
	{
	}

	index_type space_used() const
	{
	}
};

// Create a new cache object with a given maximum memory capacity.
Cache::Cache(index_type maxmem, hash_func hasher)
: pImpl_(new Impl(maxmem, hasher))
{}
Cache::~Cache() = default;

// Add a <key, value> pair to the cache.
// If key already exists, it will overwrite the old value.
// Both the key and the value are to be deep-copied (not just pointer copied).
// If maxmem capacity is exceeded, sufficient values will be removed
// from the cache to accomodate the new value.
int Cache::set(key_type key, val_type val, index_type size)
{
	return pImpl_ ->set(key,val,size);
}

// Retrieve a pointer to the value associated with key in the cache,
// or NULL if not found.
// Sets the actual size of the returned value (in bytes) in val_size.
Cache::val_type Cache::get(key_type key, index_type& val_size) const
{
	return pImpl_ ->get(key, val_size);
}

// Delete an object from the cache, if it's still there
int Cache::del(key_type key)
{
	return pImpl_ ->del(key);
}

// Compute the total amount of memory used up by all cache values (not keys)
Cache::index_type Cache::space_used() const
{
	return pImpl_ ->space_used();
}
