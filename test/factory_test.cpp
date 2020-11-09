#include "factory.h"
#include <iostream>
using namespace spiritsaway::entity_component_event;
using namespace std;

struct Animal
{
public:
	Animal(int x)
		: m_x(x)
	{

	}
	virtual void makeNoise() = 0;
protected:
	int m_x;

};
using AnimalFactory = basic_poly_factory<shr_ptr_t, Animal, int>;


class Dog final: public AnimalFactory::sub_class<Dog> {
public:
	Dog(int x)
		: AnimalFactory::sub_class<Dog>(x)
	{

	}
public:
  void makeNoise() { std::cerr << "Dog: " << m_x << "\n"; }
  static std::string_view class_name()
  {
	  return "Dog";
  }
};

class Cat final: public AnimalFactory::sub_class<Cat> {
public:
  Cat(int x)
	  : AnimalFactory::sub_class<Cat>(x)
  {

  }

  void makeNoise() { std::cerr << "Cat: " << m_x << "\n"; }
  static std::string_view class_name()
  {
	  return "Cat";
  }
};

//class Husky : public Dog
//{
//public:
//    Husky(int x)
//        : Dog(x)
//    {
//
//    }
//    void makeNoise()
//    {
//        std::cerr << "Husky: " << m_x << "\n";
//    }
//};




void test_hash()
{
	auto x = AnimalFactory::make<Dog>(3);
	auto y = AnimalFactory::make<Cat>(2);
	x->makeNoise();
	y->makeNoise();

	//auto xx = Dog(1);
	//xx.makeNoise();
	//auto z = new Husky(4);
	//if (!z)
	//{
	//	std::cerr << "cant create husky by AnimalFactory" << std::endl;
	//}
	//else
	//{
	//	z->makeNoise();
	//}
	
}

void test_name()
{
	auto x = AnimalFactory::make_by_name("Dog", 3);
	auto y = AnimalFactory::make_by_name("Cat", 2);
	x->makeNoise();
	y->makeNoise();
}

int main() {
	test_hash();
	test_name();
	return 0;
}