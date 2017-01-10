#pragma once

#include "Common.h"

#include <unordered_map>

class UnitSpec;

class Data
{
public:
  using key_type = std::string;
  template<typename T> using map_t = std::unordered_map<key_type, T>;
  
private:
  template<typename T> static std::unordered_map<key_type, T>& containerFor();
  
public:
  static const Trait& trait(const TraitID trait);
  static const Wizard& wizard(const WizardID wizard);
  
  template<typename T> static bool registerData(const key_type& ident, const T data)
  {
    auto& container = containerFor<T>();
    if (container.find(ident) != container.end())
      return false;
    
    container.insert(std::make_pair(ident, data));
    return true;
  }
  
  template<typename T> static const T get(const key_type& ident)
  {
    return containerFor<T>()[ident];
  }
  
  static const UnitSpec* unit(const key_type& ident) { return get<const UnitSpec*>(ident); }
  
#if defined(DEBUG)
  template<typename T> static void getInfo()
  {
    auto& container = containerFor<T>();
    LOGD("[data] size: %zu", container.size())
    for (const auto& entry : container)
      LOGD("  %s -> %p", entry.first.c_str(), entry.second);
  }
#endif
};
