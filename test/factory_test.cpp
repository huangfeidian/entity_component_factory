#include "factory.h"
#include <iostream>
using namespace spiritsaway::entity_component_event;
using namespace std;

struct Animal
{
	virtual void makeNoise() = 0;
};
using AnimalFactory = poly_hash_factory<Animal, shr_ptr_t, int>;


class Dog : public AnimalFactory::sub_class<Dog> {
public:
  Dog(int x) : m_x(x) {}
public:
  void makeNoise() { std::cerr << "Dog: " << m_x << "\n"; }

protected:
  int m_x;
};

class Cat : public AnimalFactory::sub_class<Cat> {
public:
  Cat(int x) : m_x(x) {}

  void makeNoise() { std::cerr << "Cat: " << m_x << "\n"; }

protected:
  int m_x;
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
struct Creature : poly_name_factory<Creature, unq_ptr_t, std::unique_ptr<int>> {
  Creature() {}
  virtual void makeNoise() = 0;
};

class Ghost : public Creature::sub_class<Ghost> {
public:
  Ghost(std::unique_ptr<int> &&x) : m_x(*x) {}

  void makeNoise() { std::cerr << "Ghost: " << m_x << "\n"; }

private:
  int m_x;
};



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
	auto z = Creature::make<Ghost>(std::make_unique<int>(4));
	z->makeNoise();
}

int main() {
	test_hash();
	test_name();
	return 0;
}