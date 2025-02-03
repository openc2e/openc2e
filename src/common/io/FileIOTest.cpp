#include "common/Random.h"
#include "common/io/FileReader.h"
#include "common/io/FileWriter.h"
#include "common/io/IOException.h"

#include <fmt/format.h>
#include <ghc/filesystem.hpp>
#include <gtest/gtest.h>
#include <stdint.h>
#include <string.h>
#include <vector>

namespace fs = ghc::filesystem;

static auto vec(const char* s) {
	// note this removes the trailing NUL, which is usually what we want
	return std::vector<uint8_t>{
		reinterpret_cast<const uint8_t*>(s),
		reinterpret_cast<const uint8_t*>(s) + strlen(s)};
}

static auto fname() {
	return ::testing::UnitTest::GetInstance()->current_test_info()->name();
}

class FileTest : public testing::Test {
  protected:
	FileTest() = default;
	FileTest(const FileTest&) = delete;
	FileTest& operator=(const FileTest&) = delete;

	void SetUp() {
		original_path_ = fs::current_path();
		auto dirname = fs::temp_directory_path() / fmt::format("openc2e-test-{:08x}", rand_uint32());
		create_directory(dirname);
		fs::current_path(dirname);
	}

	void TearDown() {
		if (!original_path_.empty()) {
			fs::current_path(original_path_);
		}
	}

	fs::path original_path_;
};

TEST_F(FileTest, file_write_and_read_to_end) {
	FileWriter(fname()).write("hello world");
	auto contents = FileReader(fname()).read_to_end();
	EXPECT_EQ(contents, vec("hello world"));
}

TEST_F(FileTest, file_write_overwrites) {
	FileWriter(fname()).write("hello world");
	FileWriter(fname()).write("ciao mondo");
	auto contents = FileReader(fname()).read_to_end();
	EXPECT_EQ(contents, vec("ciao mondo"));
}

TEST_F(FileTest, file_write_append) {
	FileWriter(fname()).write("hello");
	FileWriter(fname(), FileWriter::option_append).write(" world");
	auto contents = FileReader(fname()).read_to_end();
	EXPECT_EQ(contents, vec("hello world"));
}

TEST_F(FileTest, file_write_read_flush_read) {
	FileWriter w(fname());
	w.write("hello");

	FileReader r(fname());
	// no data until flushed
	EXPECT_FALSE(r.has_data_left());
	w.flush();
	EXPECT_TRUE(r.has_data_left());
}

TEST_F(FileTest, file_read_nonexistent_errors) {
	EXPECT_THROW(FileReader f(fname()), IOException);
}

TEST_F(FileTest, file_read_parts) {
	FileWriter(fname()).write("hello world");

	FileReader f(fname());
	std::vector<uint8_t> part(5);
	f.read(part);
	EXPECT_EQ(part, vec("hello"));

	f.read(part);
	EXPECT_EQ(part, vec(" worl"));

	part.resize(1);
	f.read(part);
	EXPECT_EQ(part, vec("d"));
}

TEST_F(FileTest, file_write_parts) {
	{
		FileWriter f(fname());
		f.write("hello");
		f.write(" ");
		f.write("world");
	}
	auto contents = FileReader(fname()).read_to_end();
	EXPECT_EQ(contents, vec("hello world"));
}

TEST_F(FileTest, file_seek_absolute) {
	FileWriter(fname()).write("hello world");
	FileReader f(fname());
	f.seek_absolute(6);
	auto contents = f.read_to_end();
	EXPECT_EQ(contents, vec("world"));
}

TEST_F(FileTest, file_seek_relative_backward) {
	FileWriter(fname()).write("hello world");
	FileReader f(fname());
	(void)f.read_to_end();
	f.seek_relative(-5);
	auto contents = f.read_to_end();
	EXPECT_EQ(contents, vec("world"));
}

TEST_F(FileTest, file_seek_relative_forward) {
	FileWriter(fname()).write("hello world");
	FileReader f(fname());
	f.seek_relative(6);
	auto contents = f.read_to_end();
	EXPECT_EQ(contents, vec("world"));
}

TEST_F(FileTest, file_read_tell) {
	FileWriter(fname()).write("hello world");
	FileReader f(fname());
	std::vector<uint8_t> part(5);
	f.read(part);
	EXPECT_EQ(f.tell(), 5);
}

TEST_F(FileTest, file_read_to_end_tell) {
	FileWriter(fname()).write("hello world");
	FileReader f(fname());
	(void)f.read_to_end();
	EXPECT_EQ(f.tell(), 11);
}

TEST_F(FileTest, file_seek_absolute_tell) {
	FileWriter(fname()).write("hello world");
	FileReader f(fname());
	f.seek_absolute(6);
	EXPECT_EQ(f.tell(), 6);
}

TEST_F(FileTest, file_seek_relative_forward_tell) {
	FileWriter(fname()).write("hello world");
	FileReader f(fname());
	f.seek_relative(6);
	EXPECT_EQ(f.tell(), 6);
}

TEST_F(FileTest, file_seek_relative_backward_tell) {
	FileWriter(fname()).write("hello world");
	FileReader f(fname());
	(void)f.read_to_end();
	f.seek_relative(-5);
	EXPECT_EQ(f.tell(), 6);
}

TEST_F(FileTest, file_read_past_end_errors) {
	FileWriter(fname()).write("hello");
	FileReader f(fname());
	std::vector<uint8_t> contents(11);
	EXPECT_THROW(f.read(contents), IOException);
}

TEST_F(FileTest, file_seek_and_read_past_end_errors) {
	FileWriter(fname()).write("hello");
	FileReader f(fname());
	f.seek_absolute(11);
	std::vector<uint8_t> contents(1);
	EXPECT_THROW(f.read(contents), IOException);
}

TEST_F(FileTest, file_read_parts_and_has_data_left_true) {
	FileWriter(fname()).write("hello world");
	FileReader f(fname());
	std::vector<uint8_t> contents(5);
	f.read(contents);
	EXPECT_TRUE(f.has_data_left());
}

TEST_F(FileTest, file_read_parts_and_has_data_left_false) {
	FileWriter(fname()).write("hello");
	FileReader f(fname());
	std::vector<uint8_t> contents(5);
	f.read(contents);
	EXPECT_FALSE(f.has_data_left());
}

TEST_F(FileTest, file_read_to_end_and_has_data_left_false) {
	FileWriter(fname()).write("hello");
	FileReader f(fname());
	(void)f.read_to_end();
	EXPECT_FALSE(f.has_data_left());
}

TEST_F(FileTest, file_seek_and_has_data_left_false) {
	FileWriter(fname()).write("hello");
	FileReader f(fname());
	f.seek_absolute(5);
	EXPECT_FALSE(f.has_data_left());
}

TEST_F(FileTest, file_with_unicode_name) {
	auto fname = fs::u8path("\xf0\x9f\x99\x82");
	FileWriter(fname).write("hello");
	auto contents = FileReader(fname).read_to_end();
	EXPECT_EQ(contents, vec("hello"));
	EXPECT_TRUE(fs::exists("\U0001F642"));
}