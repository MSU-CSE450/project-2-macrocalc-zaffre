#pragma once
#include <fstream>
#include <iostream>
#include <streambuf>

bool shouldLog;

class Logger {
public:
  Logger()
      : log_stream(&std::cout) {}

  // Overload the << operator for the logger
  template <typename T>
  Logger &operator<<(const T &value) {
    if (shouldLog) {
      std::stringstream ss;
      ss << value;
      // Output to the console
      (*log_stream) << ss.str();
      // Optionally, also output to a file
      logfile << ss.str();
    }
    return *this;
  }

  // Flush the log stream (usually after an endl)
  Logger &operator<<(std::ostream &(*manip)(std::ostream &)) {
    if (shouldLog) {
      manip(*log_stream);   // Apply manipulator like std::endl
      logfile << std::endl; // Also apply to the log file
    }
    return *this;
  }

  // Destructor to close the log file
  ~Logger() {
    if (logfile.is_open()) {
      logfile.close();
    }
  }

private:
  std::ostream *log_stream;         // Pointer to the stream (e.g., std::cout)
  std::ofstream logfile{"log.txt"}; // Log file for writing logs
};

// Define the log object
Logger logger;