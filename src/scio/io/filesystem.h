#ifndef scio_A40C38A6_30F0_4124_BF1A_9C80B5BDA01E
#define scio_A40C38A6_30F0_4124_BF1A_9C80B5BDA01E

#include <filesystem>
#include <fstream>
#include <scio/core/asio.h>
#include <sstream>

namespace scio {

namespace filesystem {
/**
 * @brief read string from file
 * @param path
 * @return string
 */
constexpr auto read =
    [](const std::string &path) -> net::awaitable<std::string> {
  std::ifstream file(path);
  std::stringstream buffer;
  buffer << file.rdbuf();
  co_return buffer.str();
};

/**
 * @brief write string to file
 * @param path file path
 * @param content string to write
 * @return true if success
 */
constexpr auto write = [](const std::string &path,
                          const std::string &content) -> net::awaitable<bool> {
  std::ofstream file(path);
  if (file.is_open()) {
    file << content;
    file.close();
    co_return true;
  }
  co_return false;
};

/**
 * @brief create directory
 * @param path
 * @return true if success
 */
constexpr auto create_directory =
    [](const std::string &path) -> net::awaitable<bool> {
  co_return std::filesystem::create_directory(path);
};

/**
 * @brief remove directory
 * @param path
 * @return remove counts
 */
constexpr auto remove_directory =
    [](const std::string &path) -> net::awaitable<bool> {
  co_return std::filesystem::remove_all(path);
};

/**
 * @brief remove file
 * @param path
 * @return true if success
 */
constexpr auto remove_file =
    [](const std::string &path) -> net::awaitable<bool> {
  co_return std::filesystem::remove(path);
};

/**
 * @brief file exists
 * @param path
 * @return true if exists
 */
constexpr auto exists = [](const std::string &path) -> net::awaitable<bool> {
  co_return std::filesystem::exists(path);
};

} // namespace filesystem

} // namespace scio

#endif /* scio_A40C38A6_30F0_4124_BF1A_9C80B5BDA01E */
