#include <iostream>
#include <string_view>

class A
{
public:
	virtual std::string_view getName1(int x) { return "A"; }
	virtual std::string_view getName2(int x) { return "A"; }
};

class B : public A
{
public:
	virtual std::string_view getName1(int x) override { return "B"; } // note: parameter is a short
	virtual std::string_view getName2(int x) const { return "B"; } // note: function is const
};

int main()
{
	B b{};
	A& rBase{ b };
	std::cout << rBase.getName1(1) << '\n';
	// std::cout << rBase.getName2(2) << '\n';
    std::cout << sizeof(A) << "\n";
    std::cout << sizeof(B) << "\n";
	return 0;
}