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

namespace xsca {

class ConfigureManager {
private:
  std::string _conf_path;
  std::vector<std::string> _cxx_identifiers;
  std::vector<std::string> _c_cxx_identifiers;
  std::vector<std::string> _jump_functions;

  // TODO: move this to file
  std::vector<std::string> _danger_functions = {"exit", "abort"};


  ConfigureManager(const ConfigureManager &) = delete;

  ConfigureManager &operator=(const ConfigureManager &) = delete;

  enum {
    CXX_IDENTIFIERS = 0,
    C_CXX_IDENTIFIERS = 1,
    JUMP_FUNCTIONS = 2,
    ALL,
  };

public:
  ConfigureManager(std::string conf_path) : _conf_path(std::move(conf_path)) {
    Initialize();
  }

  ~ConfigureManager() = default;

  void Initialize() {
    LoadFile<CXX_IDENTIFIERS>("cxx_identifier.conf");
    LoadFile<C_CXX_IDENTIFIERS>("c_cxx_identifier.conf");
    LoadFile<JUMP_FUNCTIONS>("jump_function.conf");
  }

  template<unsigned conf>
  void LoadFile(const std::string &conf_name) {
    std::fstream istream;
    std::string identifier;

    // CXX_Identifers -> conf/cxx_identifier.conf
    istream.open(_conf_path + conf_name, std::ios_base::in);
    DBG_ASSERT(istream.is_open(), "Open conf file %s failed",
               conf_name.c_str());

    while (!istream.eof()) {
      std::getline(istream, identifier);
      if (conf == CXX_IDENTIFIERS) {
        _cxx_identifiers.push_back(identifier);
      } else if (conf == C_CXX_IDENTIFIERS) {
        _c_cxx_identifiers.push_back(identifier);
      } else if (conf == JUMP_FUNCTIONS) {
        _jump_functions.push_back(identifier);
      } else {
        printf("Conf file type not set.\n");
      }
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
    } else if (conf == ALL) {
      return FindCXXKeyword(str) || FindCAndCXXKeyword(str);
    } else {
      TRACE0();
      return false;
    }
  }
};

} // namespace xsca
