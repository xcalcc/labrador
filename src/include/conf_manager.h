/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// conf_manager.h
// ====================================================================
//
// configure file manager
//

#include "xsca_defs.h"
#include <fstream>
#include <utility>
#include <vector>

#ifdef linux
#include <unistd.h>
#include <limits.h>
#elif __APPLE__
#include <sys/param.h>
#include <mach-o/dyld.h>
#include <sys/syslimits.h>
#endif

namespace xsca {

class ConfigureManager {
private:
  std::string _conf_path;
  std::vector<std::string> _cxx_identifiers;
  std::vector<std::string> _c_cxx_identifiers;
  std::vector<std::string> _jump_functions;
  std::vector<std::string> _danger_functions;


  ConfigureManager(const ConfigureManager &) = delete;

  ConfigureManager &operator=(const ConfigureManager &) = delete;

  enum {
    CXX_IDENTIFIERS = 0,
    C_CXX_IDENTIFIERS = 1,
    JUMP_FUNCTIONS = 2,
    DANGER_FUNCTIONS = 3,
    ALL,
  };

public:
  ConfigureManager(std::string conf_path) : _conf_path(std::move(conf_path)) {
    Initialize();
  }

  ~ConfigureManager() = default;

  void Initialize() {
    InitPath();
    LoadFile("cxx_identifier.conf", _cxx_identifiers);
    LoadFile("c_cxx_identifier.conf", _c_cxx_identifiers);
    LoadFile("jump_function.conf", _jump_functions);
    LoadFile("danger_function.conf", _danger_functions);
  }

#ifdef linux
  std::string get_selfpath() {
    char buff[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
    if (len != -1) {
      buff[len] = '\0';
      return std::string(buff);
    }
    /* handle error condition */
  }
#elif __APPLE__
  std::string get_selfpath() {
    char buff[PATH_MAX];
    char real_path[PATH_MAX];
    const char *res;
    uint32_t size = sizeof(buff);
    ssize_t len = _NSGetExecutablePath(buff, &size);
    res = realpath(buff, real_path);
    if (res) {
      return std::string(res);
    }
  }
#endif

  void InitPath() {
    if (auto path = std::getenv("XSCA_HOME")) {
      _conf_path = path;
      if (_conf_path.back() != '/')
        _conf_path += '/';
      _conf_path += "conf/";
    } else {

#if defined(linux) || __APPLE__
      std::string exe = get_selfpath();
      while(exe.back() != '/') {
        exe.pop_back();
      }
      _conf_path = exe + "../conf/";
#endif
    }
  }

  void LoadFile(const std::string &conf_name, std::vector<std::string> &tokens) {
    std::fstream istream;
    std::string identifier;

    // CXX_Identifers -> conf/cxx_identifier.conf
    istream.open(_conf_path + conf_name, std::ios_base::in);
    DBG_ASSERT(istream.is_open(), "Open conf file %s failed",
               conf_name.c_str());

    while (!istream.eof()) {
      std::getline(istream, identifier);
      tokens.push_back(identifier);
    }
  }

  bool FindCXXKeyword(const std::string &str) const {
    auto res = std::find(_cxx_identifiers.begin(), _cxx_identifiers.end(), str);
    return (res != _cxx_identifiers.end());
  }

  bool FindCAndCXXKeyword(const std::string &str) const {
    auto res =
        std::find(_c_cxx_identifiers.begin(), _c_cxx_identifiers.end(), str);
    return (res != _c_cxx_identifiers.end());
  }

  bool IsDangerFunction(const std::string &str) const {
    auto res = std::find(_danger_functions.begin(), _danger_functions.end(), str);
    return (res != _danger_functions.end());
  }

  bool IsJumpFunction(const std::string &str) const {
    auto res = std::find(_jump_functions.begin(), _jump_functions.end(), str);
    return (res != _jump_functions.end());
  }

  template<unsigned conf>
  bool Match(std::string &str) {
    if (conf == CXX_IDENTIFIERS) {
      return FindCXXKeyword(str);
    } else if (conf == C_CXX_IDENTIFIERS) {
      return FindCAndCXXKeyword(str);
    } else if (conf == JUMP_FUNCTIONS) {
      return IsJumpFunction(str);
    } else if (conf == ALL) {
      return FindCXXKeyword(str) || FindCAndCXXKeyword(str) || IsJumpFunction(str);
    } else {
      TRACE0();
      return false;
    }
  }
};

} // namespace xsca
