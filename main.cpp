#include <unordered_map>
#include <string>
#include <memory>
#include <unordered_map>
#include <typeinfo>
#include <iostream>

#pragma mark - Base
class Base
{
protected:
    typedef void (*FunctionPtr)();
    typedef std::unordered_map<std::string, FunctionPtr>        VirtualFunctions;
    typedef std::unordered_map<std::size_t, VirtualFunctions>   VirtualTable;
    
    const std::unique_ptr<std::size_t>            pClassId_;
    const static std::shared_ptr<VirtualTable>    s_pVirtualTable_;
    
public:
    Base() : pClassId_(std::make_unique<std::size_t>(typeid(*this).hash_code()))
    {}
    
    double virtualFunctionDouble(double x, double y)
    {
        try
        {
            typedef double (*ThisFunctionPtr)(Base*, double, double);
            return reinterpret_cast<ThisFunctionPtr>(this->virtualCall("virtualFunctionDouble"))(this, x, y);
        }
        catch (const std::exception& e)
        {
            std::cout << "Base.virtualFunctionDouble = x / y = " << x / y << std::endl;
            return x / y;
        }
    }
    
    int virtualFunctionInt(int x)
    {
        try
        {
            typedef int (*ThisFunctionPtr)(Base*, int);
            return reinterpret_cast<ThisFunctionPtr>(this->virtualCall("virtualFunctionInt"))(this, x);
        }
        catch (const std::exception& e)
        {
            std::cout << "Base.virtualFunctionInt = x = " << x << std::endl;
            return x;
        }
    }
         
private:
    
    FunctionPtr virtualCall(const std::string& functionName)
    {
        return Base::s_pVirtualTable_->at(*this->pClassId_).at(functionName);
    }
};

#pragma mark - Derived
class Derived : public Base
{
public:
    Derived()
    {
        *this->pClassId_ = typeid(*this).hash_code();
        VirtualFunctions virtualFunctions =
        {
            std::make_pair("virtualFunctionDouble", reinterpret_cast<FunctionPtr>(call_virtualFunctionDouble)),
            std::make_pair("virtualFunctionInt", reinterpret_cast<FunctionPtr>(call_virtualFunctionInt))
        };
        Base::s_pVirtualTable_->emplace(*this->pClassId_, virtualFunctions);
    }
    
    double virtualFunctionDouble(double x, double y)
    {
        std::cout << "Derived.virtualFunctionDouble = (x / y) * 2.0 = " << (x / y) * 2.0 << std::endl;
        return (x / y) * 2.0;
    }
    
    int virtualFunctionInt(int x)
    {
        std::cout << "Derived.virtualFunctionInt = x + 1 = " << x + 1 << std::endl;
        return x + 1;
    }
    
private:
    
    static double call_virtualFunctionDouble(Derived* pSelf, double x, double y)
    {
        return pSelf->virtualFunctionDouble(x, y);
    }
    
    static int call_virtualFunctionInt(Derived* pSelf, int x)
    {
        return pSelf->virtualFunctionInt(x);
    }
};

const std::shared_ptr<Base::VirtualTable> Base::s_pVirtualTable_ = std::make_shared<Base::VirtualTable>();

#pragma mark - main()

int main()
{
    std::cout << "Non polymorph base class:" << std::endl;
    Base* nonPolyBase = new Base;
    nonPolyBase->virtualFunctionDouble(3.0, 4.0);
    nonPolyBase->virtualFunctionInt(5);
    delete nonPolyBase;
    
    std::cout  << std::endl;

    std::cout << "Non polymorph derived class:" << std::endl;
    Derived* nonPolyDerived = new Derived;
    nonPolyDerived->virtualFunctionDouble(3.0, 4.0);
    nonPolyDerived->virtualFunctionInt(5);
    delete nonPolyDerived;
    
    std::cout  << std::endl;
    
    std::cout << "polymorph class:" << std::endl;
    Base* poly = new Derived;
    poly->virtualFunctionDouble(3.0, 4.0);
    poly->virtualFunctionInt(5);
    delete poly;
    
    return 0;
}
