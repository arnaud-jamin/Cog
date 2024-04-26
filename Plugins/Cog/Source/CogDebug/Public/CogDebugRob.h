#pragma once

//--------------------------------------------------------------------------------------------------------------------------
// Hack to access to private members
// https://github.com/YunHsiao/Crysknife/blob/main/SourcePatch/Runtime/Core/Public/Misc/PrivateAccessor.h
// https://bloglitb.blogspot.com/2010/07/access-to-private-members-thats-easy.html
//--------------------------------------------------------------------------------------------------------------------------

template<typename Type, Type& Output, Type Input>
struct TRob
{
	TRob() { Output = Input; }
	static TRob Obj;
};

template<typename Type, Type& Output, Type Input>
TRob<Type, Output, Input> TRob<Type, Output, Input>::Obj;

template<typename OwnerType, typename VariableType>
using TMemberVariableType = VariableType(OwnerType::*);

template<typename OwnerType, typename ReturnType, typename... Args>
using TMemberFunctionType = ReturnType(OwnerType::*)(Args...);

template<typename OwnerType, typename ReturnType, typename... Args>
using TConstMemberFunctionType = ReturnType(OwnerType::*)(Args...) const;

template<typename VariableType>
using TStaticVariableType = VariableType*;

template<typename ReturnType, typename... Args>
using TStaticFunctionType = ReturnType(*)(Args...);

#define INIT_PRIVATE_ACCESSOR(Name, Value) template struct TRob<decltype(Name), Name, &Value>

#define DEFINE_PRIVATE_ACCESSOR(Name, Value, Type, ...) \
	static Type<__VA_ARGS__> Name; \
	INIT_PRIVATE_ACCESSOR(Name, Value)

/****************************** Syntactic Sugars ******************************/

#define DEFINE_PRIVATE_ACCESSOR_VARIABLE(Name, Class, VariableType, VariableName) DEFINE_PRIVATE_ACCESSOR(Name, Class::VariableName, TMemberVariableType, Class, VariableType)
#define DEFINE_PRIVATE_ACCESSOR_STATIC_VARIABLE(Name, Class, VariableType, VariableName) DEFINE_PRIVATE_ACCESSOR(Name, Class::VariableName, TStaticVariableType, VariableType)
#define DEFINE_PRIVATE_ACCESSOR_FUNCTION(Name, Class, ReturnType, FunctionName, ...) DEFINE_PRIVATE_ACCESSOR(Name, Class::FunctionName, TMemberFunctionType, Class, ReturnType, __VA_ARGS__)
#define DEFINE_PRIVATE_ACCESSOR_CONST_FUNCTION(Name, Class, ReturnType, FunctionName, ...) DEFINE_PRIVATE_ACCESSOR(Name, Class::FunctionName, TConstMemberFunctionType, Class, ReturnType, __VA_ARGS__)
#define DEFINE_PRIVATE_ACCESSOR_STATIC_FUNCTION(Name, Class, ReturnType, FunctionName, ...) DEFINE_PRIVATE_ACCESSOR(Name, Class::FunctionName, TStaticFunctionType, ReturnType, __VA_ARGS__)

#define PRIVATE_ACCESS_OBJ(Obj, Name) (Obj.*Name)
#define PRIVATE_ACCESS_PTR(Ptr, Name) (Ptr->*Name)
#define PRIVATE_ACCESS_STATIC(Name) (*Name)
