#include "cache.h"
#include "queue.hh"
#include <unordered_map>
#include <cstring> //for "std::memcpy" in set
#include <iostream> //for queue.display

struct Cache::Impl {
private:
	index_type maxmem_;
	hash_func hasher_;
	index_type memused_;
	std::unordered_map<std::string, void*, hash_func> data_;
	Queue evictor_queue;
	
public:
	Impl(index_type maxmem, hash_func hasher)
	 : maxmem_(maxmem), hasher_(hasher), memused_(0), data_(0, hasher_)
	{
		data_.max_load_factor(0.5);
	}

	~Impl()
	{
		for (auto kvpair : data_) //free all ptrs
		{
			free(data_[kvpair.first]);
			evictor_queue.rem(kvpair.first);
		}
	}

	//returns 0: successful set
	//returns 1: item larger than maxmem
	int set(key_type key, val_type val, index_type size)
	{
		if(size > maxmem_) {return 1;}
		std::string* keyp = new std::string;
		*keyp = key;

		if(data_[key] != 0)
		{
			free(data_[key]);	//free existing ptr if editing
			uint32_t sz = evictor_queue.rem(key);
			memused_ -= sz;
		}
		void *val_ptr = malloc(size);			//malloc an empty pointer...
		std::memcpy(val_ptr, val, size);		//and deep-copy to it.
		data_[key] = val_ptr;					//then store it in data_, and...
		memused_ += size; 						//increase memused_ by its size
		evictor_queue.enqueue(keyp, size); 		//enqueue a pointer to key

		while(memused_ > maxmem_)	// if we need to do some eviction...
		{
			key_type header = *(evictor_queue.head->value);
			//std::cout << "evicting key...\t\t" << (header) << std::endl;
			del(header); //delete the first key in data_
		}
		return 0;
	}
	
	//returns ptr: successful get
	//returns NULL: no ptr associated with key
	val_type get(key_type key, index_type& val_size)
	{
		if(data_[key] != 0)
		{
			val_size = evictor_queue.pushback(key);
			return data_[key];
		} //fetch key if exists.
		else {data_.erase(key); return NULL;} 	//if key is nonexistent, make sure we don't keep it!
		//takes key and size of retrieved value
		//return a pointer to key in array
	}

	//returns 0: successful delete
	//returns 1: no pointer associated with key
	int del(key_type key)
	{
		if(data_[key] != 0)
		{
			int sz = evictor_queue.rem(key);
			memused_ -= sz;		//decrement memuse
			free(data_[key]);	//free pointer to data
			data_.erase(key);	//make data_ forget the key.
			return 0;
		}
		else
		{
			data_.erase(key);
			return 1;
		}
	}

	index_type space_used() const
	{
		return memused_;
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
