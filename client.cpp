#include "cache.h"
#include <unordered_map>
#include <cstring> //for "std::memcpy" in set
#include <string>
#include <iostream> //for queue.display
#include "cli_ex.h"

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>
char *host = "127.0.0.1";
char *port = "10000";

struct Cache::Impl {
private:
	index_type maxmem_;
	hash_func hasher_;
public:
	Net *net;
	Impl(index_type maxmem, hash_func hasher) 
	{
		try { 
			net = new Net(host, port);
			std::string data = "/memsize";
			net->get_http(data.c_str());
		} catch(...) {
			std::cout << "Connection failure";
		}
	}

	~Impl()
	{
		net->post_http("/shutdown", "");
	}

	//returns 0: successful set
	//returns 1: item larger than maxmem
	int set(key_type key, val_type val, index_type size)
	{
		try{
			std::cout << key;
			std::string skey = key;
			const std::string *sval = static_cast<const std::string*>(val);
			std::string data = "/key/" + skey + "/" + *sval;
			std::cout << data << "\n";
			net->put_http(data.c_str());
			return 0;
		} catch(...) {
			return 1;
		}
	}
	
	//returns ptr: successful get
	//returns NULL: no ptr associated with key
	val_type get(key_type key, index_type& val_size)
	{
		//takes key and size of retrieved value
		std::string skey = key;
		std::string data = "/key/" + skey;
		auto http_out = net->get_http(data.c_str());
		std::ostringstream os;
		//std::string *vptr = new std::string((os << http_out).str()); 
		//const auto& void_c = *static_cast<const std::string*>(vptr);
		os << http_out;
		return os.pword(0);
	}

	//returns 0: successful delete
	//returns 1: no pointer associated with key
	int del(key_type key)
	{
		try {
			std::string skey = key;
			std::string data = "/key/" + skey;
			net->delete_http(data.c_str());
			return 0;
		}
		catch(...){
			return 1;
		}
	}

	index_type space_used() const
	{
		std::string st = "/memsize";
		auto ct = net->get_http(st.c_str());
		std::ostringstream os;
		os << ct;
		return os.c_str();
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
