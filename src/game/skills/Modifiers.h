//
//  Modifiers.h
//  OpenMoM
//
//  Created by Jack on 3/02/19.
//  Copyright (c) 2010 Jack. All rights reserved.
//

#pragma once

#include "common/Common.h"
#include "Effects.h"

#include <vector>
#include <stack>

constexpr float operator"" _x(unsigned long long int v) { return v / 100.0f; }

template<typename T>
class ScalableValue
{
private:
  T _base;
  float _multiplier;

public:
  ScalableValue(T base, float multiplier) : _base(base), _multiplier(multiplier) { assert(_multiplier >= 0.0f); }
  ScalableValue(T base) : ScalableValue(base, 1.0f) { }
  ScalableValue() : ScalableValue(0) { }

  inline ScalableValue operator+(T value) const { return ScalableValue(_base + value, _multiplier); }
  inline ScalableValue operator-(T value) const { return ScalableValue(_base - value, _multiplier); }
  inline ScalableValue operator*(float multiplier) const { return ScalableValue(_base, _multiplier + multiplier); }
  inline ScalableValue operator/(float multiplier) const { return ScalableValue(_base, _multiplier - multiplier); }

  ScalableValue& operator+=(T value) { this->_base += value; return *this; }
  ScalableValue& operator*=(float value) { this->_multiplier += value; return *this; }
  ScalableValue& operator+=(ScalableValue<T> value) { this->_base += value._base; this->_multiplier += value._multiplier; return *this; }

  inline operator T() const { return _base * _multiplier; }
};

using FloatValue = ScalableValue<float>;
using Value = ScalableValue<value_t>;

template<typename T>
struct ModifierDummyGetter { value_t operator()(const T*) const { return 0; } };

template<typename ReturnType, typename T, typename F>
struct Modifier
{
  using value_type = ReturnType;
  using owner_type = T;
  using priority_t = value_t;
  
  enum class Priority { BASE = 0, FIRST = 1, ANY = 512, LAST = 1024, HIGHEST = 2, LOWEST = 511  } priority;
  enum class Type { INTEGER, FLOATING } type;
  enum class Mode { ADDITIVE, ADDITIVE_PARAMETRIC, MULTIPLICATIVE, FIXED  } mode;
  union {
    float multiplier;
    value_t value;
  };

  Modifier() : Modifier(Mode::ADDITIVE, 0, Priority::ANY) { }
  Modifier(Mode mode, value_t value, priority_t priority) : Modifier(mode, value, static_cast<Priority>(priority)) { }

  Modifier(Mode mode, float value, Priority priority = Priority::ANY) : priority(priority), type(Type::FLOATING), mode(mode), multiplier(value)
  { 
    assert(isFloating());
  }

  Modifier(Mode mode, value_t value, Priority priority = Priority::ANY) : priority(priority), type(Type::INTEGER), mode(mode), value(value)
  { 
    assert(!isFloating());
  }

  Modifier(value_t value) : priority(Priority::ANY), mode(Mode::ADDITIVE), type(Type::INTEGER), value(value) { }
  Modifier(float multiplier) : priority(Priority::ANY), mode(Mode::ADDITIVE_PARAMETRIC), type(Type::FLOATING), multiplier(multiplier) { }

  ReturnType transformValue(ReturnType previous, const T* owner) const
  {
    switch (mode)
    {
      case Mode::ADDITIVE:
        return previous + (type == Type::FLOATING ? static_cast<ReturnType>(multiplier) : static_cast<ReturnType>(value));
      case Mode::ADDITIVE_PARAMETRIC:
        return previous + static_cast<ReturnType>(F()(owner)*multiplier);
      case Mode::MULTIPLICATIVE:
        return static_cast<ReturnType>(previous * multiplier);
      case Mode::FIXED:
        return value;
      default:
        assert(false);
        return 0;
    }
  }
  
  bool isFloating() const { return type == Type::FLOATING; }
  value_t truncatedValue() const { return isFloating() ? multiplier : value; }
 
  Order compareMagnitude(const T* owner, const Modifier<ReturnType, T, F>& other) const
  {
    ReturnType v1 = transformValue(0, owner), v2 = transformValue(0, owner);
    
    assert(mode == Mode::ADDITIVE || mode == Mode::ADDITIVE_PARAMETRIC);
    assert(other.mode == Mode::ADDITIVE || other.mode == Mode::ADDITIVE_PARAMETRIC);

    if (v1 > v2) return Order::GREATER;
    else if (v1 < v2) return Order::LESSER;
    else return Order::EQUAL;
  }

  Order compareForSorting(const Modifier<ReturnType, T, F>& other) const
  {
    if (priority < other.priority) return Order::LESSER;
    else if (priority > other.priority) return Order::GREATER;
    else
    {
      if (type < other.type) return Order::LESSER;
      else if (type > other.type) return Order::GREATER;
      else return Order::EQUAL;
    }

  }

  bool operator<(const Modifier<ReturnType, T, F>& other) { return compareForSorting(other) == Order::LESSER; }

  static inline Priority priorityFor(priority_t priority)
  { 
    assert(priority >= (priority_t)Priority::HIGHEST && priority <= (priority_t)Priority::LOWEST);
    return static_cast<Priority>(priority);
  }
};

template<typename EffectBase, typename ModifierBase, typename PropertyType>
class ModifierEffect : public EffectBase
{
public:
  using modifier_type = ModifierBase;
  using property_type = PropertyType;
  using owner_type = typename EffectBase::owner_type;
  using base_type = typename EffectBase::base_type;
  using predicate_t = predicate<const owner_type*>;

protected:
  PropertyType _property;
  ModifierBase _value;
  mutable predicate_t _predicate; //TODO: mutable because they're set 

public:
  ModifierEffect(base_type type, PropertyType property, ModifierBase value) : EffectBase(type), _property(property), _value(value), _predicate([](auto owner) { return true; }) { }
  ModifierEffect(base_type type, PropertyType property, ModifierBase value, predicate_t predicate) : EffectBase(type), _property(property), _value(value), _predicate(predicate) { }

  void setCondition(predicate_t condition) const { _predicate = condition; }

  const ModifierBase& modifier() const { return _value; }
  bool isModifier() const override { return true; }

  bool isAffecting(PropertyType property) const { return _property == property; }

  Order compare(const owner_type* owner, const EffectBase* other) const override
  {
    //TODO: this doesn't check if kind of modifier is the same so it should be used only when this is sure (eg in an yaml defined SkillGroup) 
    if (other->isModifier())
    {
      return modifier().compareMagnitude(owner, other->template as<ModifierEffect<EffectBase, ModifierBase, PropertyType>>()->modifier());
    }
    else
      return Order::UNCOMPARABLE;
  }

  typename ModifierBase::value_type transformValue(PropertyType property, typename ModifierBase::value_type value, const owner_type* owner) const
  { 
    return (property == _property && _predicate(owner)) ? _value.transformValue(value, owner) : value;
  }
};

template<typename EffectBase, typename EffectBase::base_type EffectType, typename ModifierBase, typename PropertyType>
class SpecificModifierEffect : public ModifierEffect<EffectBase, ModifierBase, PropertyType>
{
public:
  using modifier_type = ModifierBase;
  using property_type = PropertyType;
  using owner_type = typename EffectBase::owner_type;
  using base_type = typename EffectBase::base_type;
  static constexpr base_type effect_type = EffectType;

public:
  SpecificModifierEffect(PropertyType property, ModifierBase value) : ModifierEffect<EffectBase, ModifierBase, PropertyType>(EffectType, property, value) { }
  SpecificModifierEffect(PropertyType property, ModifierBase value, predicate<const owner_type*> predicate) : ModifierEffect<EffectBase, ModifierBase, PropertyType>(EffectType, property, value, predicate) { }

};


template<typename M, typename R, typename T>
class modifier_list
{
private:
  std::vector<M> modifiers;

public:
  modifier_list() { }
  modifier_list(const std::initializer_list<M>& modifiers) : modifiers(modifiers) { }
  modifier_list& operator+=(M modifier) { modifiers.push_back(modifier); return *this; }

  R reduce(const T* o = nullptr) const
  {
    return std::accumulate(modifiers.begin(), modifiers.end(), R(),
                          [o](R p, const M& mod) { return mod.transformValue(p, o);
    });
  }
};


class Player;

using CityModifierValue = Modifier<value_t, City, ModifierDummyGetter<City>>;
using WizardModifierValue = Modifier<Value, Player, ModifierDummyGetter<Player>>;
