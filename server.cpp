#include "crow_all.h"
#include "cache.h"
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    crow::SimpleApp app;
    if(argc < 2)
    {
    	std::cout << "Need correct arguments" << std::endl;
    	std::cout << "Usage: \"./server.exe maxmem port\"" << std::endl;
        return 0;
    }

    int port = atoi(argv[2]);
    int cache_size = atoi(argv[1]);
    Cache *cache = new Cache(cache_size);

    CROW_ROUTE(app, "/memsize")
    ([&cache](){
        crow::json::wvalue x;
        x["memused"] = cache->space_used();
        return x;
    });
    
    CROW_ROUTE(app, "/key/<string>").methods("GET"_method)
    ([&cache](std::string s){
        crow::json::wvalue x;
        uint32_t sz = 0;
	Cache::val_type ptr = cache->get(s, sz); 
        std::string str(static_cast<const char*>(ptr), sz);
        std::ostringstream os;
        os << str << "\n";
        return crow::response(os.str());
    });

    CROW_ROUTE(app, "/key/<string>/<string>").methods("PUT"_method)
    ([&cache](std::string k, std::string v){
        std::string *vptr = new std::string(v);
        const auto& str = *static_cast<const std::string*>(vptr);
        cache->set(k, str.c_str(), str.size() + 1);
        crow::logger(str.c_str(), crow::LogLevel::Debug);
        return str.c_str();
    });

    CROW_ROUTE(app, "/key/<string>").methods("DELETE"_method)
    ([&cache](Cache::key_type key){
        cache->del(key);
        return "Deleted\n";
    });
    
    CROW_ROUTE(app, "/").methods("HEAD"_method)
    ([](){
        std::string vers = "Server: NoahServ0.1 (Unix)\n";

        time_t rawtime;
        struct tm * timeinfo;
        char buffer[80];
      
        time (&rawtime);
        timeinfo = localtime(&rawtime);
      
        strftime(buffer,sizeof(buffer),"%a, %d %b %Y %X %z",timeinfo);
        std::string str(buffer);        
      
        std::string date = "Date: " + str + "\n";
        std::string accept = "Accept: */*\n";
        std::string type = "Content-Type: application/json\n"; 
        return vers + date + accept + type;
    });

    CROW_ROUTE(app, "/shutdown").methods("POST"_method)
    ([&cache, &app](){
        app.stop();
        return crow::response(200);	
    });

    app.port(port).run();
}
