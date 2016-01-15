#ifndef CORE_FINANCE_H_
#define CORE_FINANCE_H_

#include <string>
#include <memory>

namespace finans {
  class Finans;
}

class Finans {
public:
  /* Construction */
  static std::shared_ptr<Finans> CreateNew();
  static void CreateDefault(const std::string& src);

  ~Finans();

public:
  void Load();
  void Save();

private:
  Finans(const std::string& path);
  std::string path_;
  std::unique_ptr<finans::Finans> finans_;
};

#endif
