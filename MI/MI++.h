#pragma once

#include <MI.h>
#include <string>
#include <tuple>
#include <map>

namespace MI
{
    class Session;
    class Instance;
    class Operation;
    class Class;

    class Application
    {
    private:
        MI_Application m_app;
        friend class Session;

    public:
        Application(const std::wstring& appId = L"");
        virtual ~Application();
        Instance* NewInstance(const std::wstring& className);
        Session* NewSession(const std::wstring& protocol = L"", const std::wstring& computerName = L".");
    };

    class Session
    {
    private:
        MI_Session m_session;
        Session(MI_Session session) : m_session(session) {}

        friend Application;

    public:
        Operation* ExecQuery(const std::wstring& ns, const std::wstring& query, const std::wstring& dialect = L"WQL");
        Instance* InvokeMethod(Instance& instance, const std::wstring& methodName, const Instance* inboundParams);
        Instance* InvokeMethod(const std::wstring& ns, const std::wstring& className, const std::wstring& methodName, const Instance& inboundParams);
        Class* GetClass(const std::wstring& ns, const std::wstring& className);
        virtual ~Session();
    };

    struct Qualifier
    {
    public:
        std::wstring m_name;
        MI_Type m_type;
        MI_Value m_value;
        MI_Uint32 m_flags;
    };

    struct ParameterInfo
    {
    public:
        std::wstring m_name;
        unsigned m_index;
        MI_Type m_type;
        std::map<std::wstring, Qualifier> m_qualifiers;
    };

    struct MethodInfo
    {
    public:
        std::wstring m_name;
        unsigned m_index;
        std::map<std::wstring, Qualifier> m_qualifiers;
        std::map<std::wstring, ParameterInfo> m_parameters;
    };

    class MElementsEnum
    {
    public:
        virtual unsigned GetElementsCount() const = 0;
        virtual std::tuple<MI_Value, MI_Type, MI_Uint32> operator[] (const wchar_t* name) const = 0;
        virtual std::tuple<const MI_Char*, MI_Value, MI_Type, MI_Uint32> operator[] (unsigned index) const = 0;
    };

    class Class : public MElementsEnum
    {
    private:
        MI_Class* m_class = NULL;
        Class(MI_Class* miClass) : m_class(miClass) {}
        void Delete();

        friend Instance;
        friend Operation;

    public:
        unsigned GetElementsCount() const;
        std::tuple<MI_Value, MI_Type, MI_Uint32> operator[] (const wchar_t* name) const;
        std::tuple<const MI_Char*, MI_Value, MI_Type, MI_Uint32> operator[] (unsigned index) const;
        unsigned GetMethodCount() const;
        MethodInfo GetMethodInfo(const wchar_t* name) const;
        MethodInfo GetMethodInfo(unsigned index) const;
        virtual ~Class();
    };

    class Instance : public MElementsEnum
    {
    private:
        MI_Instance* m_instance = NULL;
        std::wstring m_namespace;
        std::wstring m_className;
        bool m_ownsInstance = false;
        Instance(MI_Instance* instance, bool ownsInstance) : m_instance(instance), m_ownsInstance(ownsInstance) {}
        void Delete();

        friend Application;
        friend Operation;
        friend Session;

    public:
        Instance* Instance::Clone() const;
        Class* GetClass() const;
        std::wstring GetClassName();
        std::wstring GetNamespace();
        unsigned GetElementsCount() const;
        std::tuple<MI_Value, MI_Type, MI_Uint32> operator[] (const wchar_t* name) const;
        std::tuple<const MI_Char*, MI_Value, MI_Type, MI_Uint32> operator[] (unsigned index) const;
        void AddElement(const std::wstring& name, const MI_Value* value, MI_Type valueType);
        void SetElement(const std::wstring& name, const MI_Value* value, MI_Type valueType);
        void SetElement(unsigned index, const MI_Value* value, MI_Type valueType);
        MI_Type GetElementType(const std::wstring& name) const;
        MI_Type GetElementType(unsigned index) const;
        void ClearElement(const std::wstring& name);
        void ClearElement(unsigned index);
        virtual ~Instance();
    };

    class Operation
    {
    private:
        MI_Operation m_operation;
        Operation(MI_Operation& operation) : m_operation(operation) {}
        MI_Boolean m_hasMoreResults = TRUE;

        friend Session;

    public:
        Instance* GetNextInstance();
        Class* GetNextClass();
        operator bool() { return m_hasMoreResults != FALSE; }        
        virtual ~Operation();
    };
};
