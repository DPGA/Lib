#ifndef __SHMRINGBUFFER_HH__
#define __SHMRINGBUFFER_HH__

#include <string>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>      /* For O_CREAT, O_RDWR */
#include <sys/mman.h>   /* shared memory and mmap() */
#include <sys/stat.h>   /* S_IRWXU */


#define VERSION_SHM  "1.0.0 " __DATE__ " " __TIME__

using std::string;

//
// Shared-memory based Ring buffer.
//
// T must be POD type
#define EVENT_BUFFER_SHM "/shm_ring_buffer"

typedef struct _ShmHeader {
		size_t _capacity; 	// max number of logs
		int _begin;    		// start index of the circular buffer
		int _end;      		// end index of the circular buffer
} ShmHeader;

class Mutex {
public:
	pthread_mutex_t _mutex;
	pthread_mutexattr_t _attr;
	void init(bool pshared = false);
	int lock();
	int trylock();
	int unlock();
};

class Condition {
public:
	pthread_cond_t _cond;
	pthread_condattr_t _attr;

	void init(bool pshared = false);
	int wait(Mutex &m);
	int timedwait(const struct timespec &ts, Mutex &m); 
	int signal();
	int broadcast();
};

class ReadWriteLock {
	public:
		void init(bool pshared = false);
		void read_lock();
		void read_unlock();
		void write_lock();
		void write_unlock();
	private:
		Mutex _mtx;
		Condition _rcond;
		Condition _wcond;
		uint32_t _nread, _nread_waiters;
		uint32_t _nwrite, _nwrite_waiters;
};

string getVersionShm();

template <typename T>
class ShmRingBuffer {
public:
	ShmRingBuffer(size_t cap = 100, bool master = false, const char * path = EVENT_BUFFER_SHM);
//                    _hdr(NULL),_lock(NULL),_v(NULL),_shm_path(path),_shm_size(0),_master(master);

	~ShmRingBuffer();
	
	string getVersion() {return VERSION_SHM;}

	size_t capacity() const;
	size_t begin() const;
	size_t end() const;

	void clear();   // clear buffer
	void push_back(const T&); // insert new event
	T dump_front();
	string unparse() const; // dump contents in the buffer to a string

private:
	ShmHeader * _hdr;
	ReadWriteLock * _lock;
	T * _v;  // pointer to the head of event buffer
	string _shm_path;
	size_t _shm_size; // size(bytes) of shared memory
	bool _master;

    // template <class In> ...
	ShmRingBuffer(const ShmRingBuffer<T> &);
	ShmRingBuffer<T>& operator=(const ShmRingBuffer<T>&);

	bool init(size_t cap, bool master, const char * path);
};


#endif
