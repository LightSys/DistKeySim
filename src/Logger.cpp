#include <numeric>
#include <regex>
#include <libunwind.h>

#include <google/protobuf/text_format.h>

#include "Logger.h"

static ofstream logOutputStream;
static ofstream logStatsStream;

int Logger::timeslot = 0;
int Logger::shared = 0;
int Logger::rate = 0;

void Logger::deleteOldLog() {
    remove(filename);
    remove(statslog);
    remove(infoMsg);
    remove(keyMsg);
    logOutputStream.open(filename, ofstream::app);
    logStatsStream.open(statslog, ofstream::app);
}

void Logger::log(string message) {
    time_t date = chrono::system_clock::to_time_t(chrono::system_clock::now());
    logOutputStream << message << " -- " << ctime(&date);  // log message and timestamp
    logOutputStream.flush();
}

void Logger::logStats(vector<string> stats) {
    if (stats.size() == csvHeaders.size()) {
        logStatsStream << join(stats, ",");
        logStatsStream << "\n";
    } else {  // don't log if array isn't accurate size
        log("Stats array was incorrect length");
    }
}

void Logger::logMsg (const string procName, const Message &message) {
    std::string prototextOutput;
    google::protobuf::TextFormat::PrintToString(message, &prototextOutput);
    // Logger::log(Formatter() << procName << ": " << prototextOutput);

    // Log the message as JSON on one line (this is not real JSON, but close enough)
    auto removeNewLine = std::regex_replace(prototextOutput, std::regex("([^{])\\n([^}])"), "$1, $2");
    auto squeezeWhitespace = std::regex_replace(removeNewLine, std::regex("\\s+"), " ");
    Logger::log(Formatter() << procName << ": {" << squeezeWhitespace << "}");
}

void Logger::setCSVHeaders() {
    logStats(csvHeaders);  // first line should be column titles
}

int Logger::getTimeslot(bool increment) {
    if (increment) {  // only increment if it is from doAllHearbeat()
        timeslot++;
    }
    return timeslot;
}

int Logger::getShared(bool clear, int more) {
    shared += more;
    if (clear) {  // reset after timeslot changes
        shared = 0;
    }
    return shared;
}

int Logger::getConsumption(bool clear, int more) {
    rate += more;
    if (clear) {  // reset after timeslot changes
        rate = 0;
    }
    return rate;
}

std::string Logger::copyFile(string path) {
    logOutputStream.close();
    logStatsStream.close();
    // create directory if last char of path is /
    if (path.size() > 0) {
        if (path.back() == '/') {
            filesystem::create_directory(path);
        }
    }

    int num = 1;
    // look for the num.txt at the path
    ifstream numFile(path + "num.txt");
    if (numFile.is_open()) {
        numFile >> num;
        numFile.close();
    }

    // increment num.txt file
    ofstream numFileII(path + "num.txt", std::ofstream::out | std::ofstream::trunc);
    numFileII << to_string(num + 1);  // replace and increment
    numFileII.close();

    // copy the statslog
    ifstream src("./statslog.csv", std::ios::binary);
    ofstream dest(path + "statslog" + to_string(num) + ".csv", std::ios::binary);
    // move over the information
    dest << src.rdbuf();
    // close files
    src.close();
    dest.close();

    // copy the logoutput.txt
    src.open("./logOutput.txt", std::ios::binary);
    dest.open(path + "logOutput" + to_string(num) + ".txt", std::ios::binary);
    // move over the information
    dest << src.rdbuf();
    // close files, and the open next
    src.close();
    dest.close();

    // copy the config files
    src.open("config.json", std::ios::binary);
    dest.open(path + "copy_of_config" + to_string(num) + ".json", std::ios::binary);
    // move over the information
    dest << src.rdbuf();
    // close files
    src.close();
    dest.close();

    return path + "full_config" + to_string(num) + ".json";
}

std::string Logger::join(vector<int> ints) {
    // Without the test for empty, we crash on Linux (but not MacOS)
    if (ints.empty()) {
        return std::string();
    }
    return std::accumulate(ints.begin()+1, ints.end(), std::to_string(ints[0]),
        [](const std::string& a, int b){
            return a + ',' + std::to_string(b);
        });
}

std::string Logger::join(vector<string> strings, string separator) {
    if (strings.empty()) {
        return std::string();
    }
    return std::accumulate(strings.begin()+1, strings.end(), strings[0],
        [separator](const std::string& a, string b){
            return a + separator + b;
        });
}

void Logger::logBackTrace() {
  unw_cursor_t cursor;
  unw_context_t context;

  // Initialize cursor to current frame for local unwinding.
  unw_getcontext(&context);
  unw_init_local(&cursor, &context);

  // Unwind frames one by one, going up the frame stack.
  while (unw_step(&cursor) > 0) {
    unw_word_t offset, pc;
    unw_get_reg(&cursor, UNW_REG_IP, &pc);
    if (pc == 0) {
      break;
    }
    // printf("0x%lx:", pc);
    char str[1000];
    // sprintf(str, "0x%lx:", pc);
    // Logger::log(std::string(str));

    char sym[256];
    if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
    //   printf(" (%s+0x%lx)\n", sym, offset);
      sprintf(str, " (%s+0x%lx)", sym, offset);
      Logger::log(std::string(str));
    } else {
      Logger::log("ERROR: unable to obtain symbol name for this frame");
    }
  }
}
