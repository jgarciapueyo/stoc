#ifndef STOC_TYPE_H
#define STOC_TYPE_H

#include <memory>
#include <string>
#include <vector>

class Type {
public:
  enum class Kind { BasicType, Signature };

protected:
  Type::Kind typeKind;

  explicit Type(Type::Kind kind);

public:
  Type::Kind getTypeKind();
  virtual std::string getName() = 0;
  virtual bool isInvalid() = 0;

  friend bool typeIsEqual(const std::shared_ptr<Type> lhs, const std::shared_ptr<Type> rhs);
  friend std::ostream &operator<<(std::ostream &os, const std::shared_ptr<Type> &type);
};

class BasicType : public Type {
public:
  enum class Kind {
    BOOL,
    INT,
    FLOAT,
    STRING,
    VOID, // only use internally but not in the language
    INVALID
  };

private:
  BasicType::Kind kind;
  std::string name;

public:
  /* it should not be used, only public to be able to return a std::shared_ptr */
  BasicType(Kind kind, std::string name);

public:
  static std::shared_ptr<BasicType> getBoolType();
  static std::shared_ptr<BasicType> getIntType();
  static std::shared_ptr<BasicType> getFloatType();
  static std::shared_ptr<BasicType> getStringType();
  static std::shared_ptr<BasicType> getVoidType();
  static std::shared_ptr<BasicType> getInvalidType();

  // Getters
  BasicType::Kind getKind();
  std::string getName() override;

  bool isNumeric();
  bool isString();
  bool isBoolean();
  bool isInvalid() override;
  bool isComparable();
  bool isOrdered();

  friend bool typeIsEqual(std::shared_ptr<BasicType> lhs, std::shared_ptr<BasicType> rhs);
};

class FunctionType : public Type {
private:
  // for now, all the parameters have to be of basic type because there are not other types
  std::vector<std::shared_ptr<BasicType>> params;
  std::shared_ptr<BasicType> result;

public:
  FunctionType(std::vector<std::shared_ptr<BasicType>> params, std::shared_ptr<BasicType> result);

  // Getters
  std::vector<std::shared_ptr<BasicType>> getParams();
  std::shared_ptr<BasicType> getResult();
  std::string getName() override;
  bool isInvalid() override;

  friend bool areParametersEqual(std::vector<std::shared_ptr<BasicType>> lhs,
                                 std::vector<std::shared_ptr<BasicType>> rhs);
  friend bool typeIsEqual(std::shared_ptr<FunctionType> lhs, std::shared_ptr<FunctionType> rhs);
};

bool areParametersEqual(std::vector<std::shared_ptr<BasicType>> lhs,
                        std::vector<std::shared_ptr<BasicType>> rhs);

#endif // STOC_TYPE_H
