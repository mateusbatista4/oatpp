/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#ifndef oatpp_data_type_Object_hpp
#define oatpp_data_type_Object_hpp

#include "./Type.hpp"

#include "./Any.hpp"
#include "./Primitive.hpp"
#include "./Enum.hpp"
#include "./UnorderedMap.hpp"
#include "./PairList.hpp"
#include "./List.hpp"
#include "./Vector.hpp"
#include "./UnorderedSet.hpp"

#include "oatpp/core/base/memory/ObjectPool.hpp"
#include "oatpp/core/base/Countable.hpp"

#include <type_traits>

namespace oatpp { namespace data { namespace mapping { namespace type {

/**
 * Base class of all object-like mapping-enabled structures ex.: oatpp::DTO.
 */
class BaseObject : public oatpp::base::Countable {
public:

  /**
   * Class to map object properties.
   */
  class Property {
  public:

    /**
     * Editional Info about Property.
     */
    struct Info {
      /**
       * Description.
       */
      std::string description = "";
      std::string pattern = "";
    };

  private:
    const v_int64 offset;
  public:

    /**
     * Constructor.
     * @param pOffset - memory offset of object field from object start address.
     * @param pName - name of the property.
     * @param pType - &l:Type; of the property.
     */
    Property(v_int64 pOffset, const char* pName, const Type* pType);

    /**
     * Property name.
     */
    const char* const name;

    /**
     * Property type.
     */
    const Type* const type;

    /**
     * Property additional info.
     */
    Info info;

    /**
     * Set value of object field mapped by this property.
     * @param object - object address.
     * @param value - value to set.
     */
    void set(BaseObject* object, const Void& value);

    /**
     * Get value of object field mapped by this property.
     * @param object - object address.
     * @return - value of the field.
     */
    Void get(BaseObject* object);

    /**
     * Get reference to ObjectWrapper of the object field.
     * @param object - object address.
     * @return - reference to ObjectWrapper of the object field.
     */
    Void& getAsRef(BaseObject* object);

  };

  /**
   * Object type properties table.
   */
  class Properties {
  private:
    std::unordered_map<std::string, Property*> m_map;
    std::list<Property*> m_list;
  public:

    /**
     * Add property to the end of the list.
     * @param property
     */
    Property* pushBack(Property* property);

    /**
     * Add all properties to the beginning of the list.
     * @param properties
     */
    void pushFrontAll(Properties* properties);

    /**
     * Get properties as unordered map for random access.
     * @return reference to std::unordered_map of std::string to &id:oatpp::data::mapping::type::BaseObject::Property;*.
     */
    const std::unordered_map<std::string, Property*>& getMap() const {
      return m_map;
    }

    /**
     * Get properties in ordered way.
     * @return std::list of &id:oatpp::data::mapping::type::BaseObject::Property;*.
     */
    const std::list<Property*>& getList() const {
      return m_list;
    }

  };

  class PropertyTraverser {
  private:
    bool isKnownType(const ClassId& classId) const;

    const Type* findPropertyType(const Type* baseType,
                                 const std::vector<std::string>& path,
                                 v_uint32 pathPosition,
                                 const std::vector<std::string>& allowedInterpretations) const;

    Void findPropertyValue(const Void& baseObject,
                           const std::vector<std::string>& path,
                           v_uint32 pathPosition,
                           const std::vector<std::string>& allowedInterpretations) const;

  private:
    std::vector<bool> m_knownTypes;
  public:

    PropertyTraverser();

    const Type* findPropertyType(const Type* baseType,
                                 const std::vector<std::string>& path,
                                 const std::vector<std::string>& allowedInterpretations) const;

    Void findPropertyValue(const Void& baseObject,
                           const std::vector<std::string>& path,
                           const std::vector<std::string>& allowedInterpretations) const;

    void setKnownType(const ClassId& classId, bool isKnownType);
    void addKnownTypes(const std::vector<ClassId>& knownTypes);

  };

private:
  void* m_basePointer = this;
private:
  void set(v_int64 offset, const Void& value);
  Void get(v_int64 offset) const;
  Void& getAsRef(v_int64 offset) const;
protected:
  void setBasePointer(void* basePointer);
  void* getBasePointer() const;
};

namespace __class {

  /**
   * AbstractObject class.
   */
  class AbstractObject {
  public:

    class PolymorphicDispatcher {
    public:

      virtual type::Void createObject() const = 0;

      virtual const type::BaseObject::Properties* getProperties() const = 0;

    };

  public:

    /**
     * Class id.
     */
    static const ClassId CLASS_ID;

  };

  /**
   * Template for Object class of type T.
   * @tparam T - object type.
   */
  template<class T>
  class Object : public AbstractObject {
  public:

    class PolymorphicDispatcher : public AbstractObject::PolymorphicDispatcher {
    public:

      type::Void createObject() const override {
        return type::Void(std::make_shared<T>(), getType());
      }

      const type::BaseObject::Properties* getProperties() const override {
        return propertiesGetter();
      }

    };

  private:

    static type::BaseObject::Properties* initProperties() {
      T obj; // initializer;
      T::Z__CLASS_EXTEND(T::Z__CLASS::Z__CLASS_GET_FIELDS_MAP(), T::Z__CLASS_EXTENDED::Z__CLASS_GET_FIELDS_MAP());
      return T::Z__CLASS::Z__CLASS_GET_FIELDS_MAP();
    }

    static const BaseObject::Properties* propertiesGetter() {
      static type::BaseObject::Properties* properties = initProperties();
      return properties;
    }

  public:

    /**
     * Get type describing this class.
     * @return - &id:oatpp::data::mapping::type::Type;
     */
    static Type* getType() {
      static Type* type = new Type(CLASS_ID, T::Z__CLASS_TYPE_NAME(), new PolymorphicDispatcher());
      return type;
    }
    
  };
  
}

/**
 * ObjectWrapper for &l:DTO;. AKA `oatpp::Object<T>`.
 * @tparam ObjT - class extended from &l:DTO;.
 */
template<class ObjT>
class DTOWrapper : public ObjectWrapper<ObjT, __class::Object<ObjT>> {
public:
  typedef ObjT TemplateObjectType;
  typedef __class::Object<ObjT> TemplateObjectClass;
public:

  OATPP_DEFINE_OBJECT_WRAPPER_DEFAULTS(DTOWrapper, TemplateObjectType, TemplateObjectClass)

  static DTOWrapper createShared() {
    return std::make_shared<TemplateObjectType>();
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::nullptr_t>::value, void>::type
  >
  inline bool operator == (T){
    return this->m_ptr.get() == nullptr;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, std::nullptr_t>::value, void>::type
  >
  inline bool operator != (T){
    return this->m_ptr.get() != nullptr;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, DTOWrapper>::value, void>::type
  >
  inline bool operator == (const T &other) const {
    if(this->m_ptr.get() == other.m_ptr.get()) return true;
    if(!this->m_ptr || !other.m_ptr) return false;
    return *this->m_ptr == *other.m_ptr;
  }

  template<typename T,
    typename enabled = typename std::enable_if<std::is_same<T, DTOWrapper>::value, void>::type
  >
  inline bool operator != (const T &other) const {
    return !operator == (other);
  }

};

/**
 * Base class for all DTO objects.
 * For more info about Data Transfer Object (DTO) see [Data Transfer Object (DTO)](https://oatpp.io/docs/components/dto/).
 */
class DTO : public BaseObject {
  template<class T>
  friend class __class::Object;
public:
  typedef oatpp::data::mapping::type::Void Void;
  typedef oatpp::data::mapping::type::Any Any;
  typedef oatpp::data::mapping::type::String String;
  typedef oatpp::data::mapping::type::Int8 Int8;
  typedef oatpp::data::mapping::type::UInt8 UInt8;
  typedef oatpp::data::mapping::type::Int16 Int16;
  typedef oatpp::data::mapping::type::UInt16 UInt16;
  typedef oatpp::data::mapping::type::Int32 Int32;
  typedef oatpp::data::mapping::type::UInt32 UInt32;
  typedef oatpp::data::mapping::type::Int64 Int64;
  typedef oatpp::data::mapping::type::UInt64 UInt64;
  typedef oatpp::data::mapping::type::Float32 Float32;
  typedef oatpp::data::mapping::type::Float64 Float64;
  typedef oatpp::data::mapping::type::Boolean Boolean;

  template <class T>
  using Object = DTOWrapper<T>;

  template <class T>
  using Enum = oatpp::data::mapping::type::Enum<T>;

  template <class T>
  using Vector = oatpp::data::mapping::type::Vector<T>;

  template <class T>
  using UnorderedSet = oatpp::data::mapping::type::UnorderedSet<T>;

  template <class T>
  using List = oatpp::data::mapping::type::List<T>;

  template <class Value>
  using Fields = oatpp::data::mapping::type::PairList<String, Value>;

  template <class Value>
  using UnorderedFields = oatpp::data::mapping::type::UnorderedMap<String, Value>;

private:
  
  static BaseObject::Properties* Z__CLASS_EXTEND(BaseObject::Properties* properties, BaseObject::Properties* extensionProperties);
  static data::mapping::type::BaseObject::Properties* Z__CLASS_GET_FIELDS_MAP();
  
public:

  virtual v_uint64 defaultHashCode() const {
    return (v_uint64) reinterpret_cast<v_buff_usize>(this);
  }

  virtual bool defaultEquals(const DTO& other) const {
    return this == &other;
  }

  v_uint64 hashCode() const {
    return defaultHashCode();
  }

  bool operator==(const DTO& other) const {
    return defaultEquals(other);
  }

};
  
}}}}

namespace std {

  template<class T>
  struct hash<oatpp::data::mapping::type::DTOWrapper<T>> {

    typedef oatpp::data::mapping::type::DTOWrapper<T> argument_type;
    typedef v_uint64 result_type;

    result_type operator()(argument_type const &ow) const noexcept {
      if(ow) {
        return ow->hashCode();
      }
      return 0;
    }

  };

}

#endif /* oatpp_data_type_Object_hpp */
