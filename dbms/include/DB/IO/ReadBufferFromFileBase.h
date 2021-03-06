#pragma once

#include <string>
#include <ctime>
#include <fcntl.h>

#include <DB/IO/ReadBuffer.h>
#include <DB/IO/BufferWithOwnMemory.h>

#ifdef __APPLE__
#include <common/apple_rt.h>
#endif

namespace DB
{

class ReadBufferFromFileBase : public BufferWithOwnMemory<ReadBuffer>
{
public:
	ReadBufferFromFileBase(size_t buf_size, char * existing_memory, size_t alignment);
	virtual ~ReadBufferFromFileBase();
	off_t seek(off_t off, int whence = SEEK_SET);
	virtual off_t getPositionInFile() = 0;
	virtual std::string getFileName() const = 0;
	virtual int getFD() const = 0;

	/// Есть возможность получать информацию о времени каждого чтения.
	struct ProfileInfo
	{
		size_t bytes_requested;
		size_t bytes_read;
		size_t nanoseconds;
	};

	using ProfileCallback = std::function<void(ProfileInfo)>;

	/// CLOCK_MONOTONIC_COARSE более чем достаточно для отслеживания долгих чтений - например, залипаний на секунды.
	void setProfileCallback(const ProfileCallback & profile_callback_, clockid_t clock_type_ = CLOCK_MONOTONIC_COARSE)
	{
		profile_callback = profile_callback_;
		clock_type = clock_type_;
	}

protected:
	ProfileCallback profile_callback;
	clockid_t clock_type;

	virtual off_t doSeek(off_t off, int whence) = 0;
};

}
