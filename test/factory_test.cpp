#include "factory.h"
#include <iostream>
using namespace spiritsaway::entity_component_event;
using namespace std;
struct Animal;
using AnimalFactory = basic_poly_factory<shr_ptr_t, Animal, int>;

struct Animal
{
public:
	Animal(const AnimalFactory::construct_key& key, int x)
		: m_x(x)
		, m_type_id(key.m_type_id)
	{

	}
	virtual void makeNoise() = 0;
	virtual std::string_view class_name() const
	{
		return "";
	}
protected:
	int m_x;
public:
	const std::size_t m_type_id;
	template <typename T>
	std::enable_if_t<std::is_base_of_v<Animal, T>, bool> is_a() const
	{
		return inherit_mapper<Animal>::is_sub_class(m_type_id, base_type_hash<Animal>::template hash<T>());
	}
	bool is_a(std::size_t in_type_id) const
	{
		return inherit_mapper<Animal>::is_sub_class(m_type_id, in_type_id);
	}
};



class Dog : public AnimalFactory::sub_class<Dog> {
public:
	Dog(const AnimalFactory::construct_key& key, int x)
		: AnimalFactory::sub_class<Dog>(key, x)
	{

	}
public:
  void makeNoise() { std::cerr << "Dog: " << m_x << "with type id "<<m_type_id<< "\n"; }
  static std::string_view static_class_name()
  {
	  return "Dog";
  }
  std::string_view class_name() const override
  {
	  return "Dog";
  }
};

class Cat : public AnimalFactory::sub_class<Cat> {
public:
  Cat(const AnimalFactory::construct_key& key, int x)
	  : AnimalFactory::sub_class<Cat>(key, x)
  {

  }

  void makeNoise() { std::cerr << "Cat: " << m_x << "with type id " << m_type_id << "\n"; }
  static std::string_view static_class_name()
  {
	  return "Cat";
  }
  std::string_view class_name() const override
  {
	  return "Cat";
  }
};

class Tiger : public AnimalFactory::sub_class<Tiger, Cat>
{
public:
	Tiger(const AnimalFactory::construct_key& key, int x)
		: AnimalFactory::sub_class<Tiger, Cat>(key, x)
	{

	}

	void makeNoise() { std::cerr << "Tiger: " << m_x << "with type id " << m_type_id << "\n";
	}
	static std::string_view static_class_name()
	{
		return "Tiger";
	}
	std::string_view class_name() const override
	{
		return "Tiger";
	}
};

class UglyCat : public Cat
{
public:
	UglyCat(const AnimalFactory::construct_key& key, int x)
		: Cat(key, x)
	{

	}

	void makeNoise() {
		std::cerr << "UglyCat: " << m_x << "with type id " << m_type_id << "\n";
	}
	static std::string_view static_class_name()
	{
		return "UglyCat";
	}
	std::string_view class_name() const override
	{
		return "UglyCat";
	}
};



void test_hash()
{
	auto x = AnimalFactory::make<Dog>(3);
	auto y = AnimalFactory::make<Cat>(2);
	auto z = AnimalFactory::make<Tiger>(4);
	x->makeNoise();
	y->makeNoise();
	z->makeNoise();
	std::vector < std::shared_ptr < Animal> > all_animals = { x, y, z };
	for (std::size_t i = 0; i < all_animals.size(); i++)
	{
		for (std::size_t j = 0; j < all_animals.size(); j++)
		{
			if (all_animals[i]->is_a(all_animals[j]->m_type_id))
			{
				std::cout << all_animals[i]->class_name() << " is a " << all_animals[j]->class_name() << std::endl;
			}
		}
	}
}

void test_name()
{
	auto x = AnimalFactory::make_by_name("Dog", 3);
	auto y = AnimalFactory::make_by_name("Cat", 2);
	auto z = AnimalFactory::make_by_name("Tiger", 4);
	x->makeNoise();
	y->makeNoise();
	z->makeNoise();
	std::vector < std::shared_ptr < Animal> > all_animals = { x, y, z };
	for (std::size_t i = 0; i < all_animals.size(); i++)
	{
		for (std::size_t j = 0; j < all_animals.size(); j++)
		{
			if (all_animals[i]->is_a(all_animals[j]->m_type_id))
			{
				std::cout << all_animals[i]->class_name() << " is a " << all_animals[j]->class_name() << std::endl;
			}
		}
	}
}

void test_invalid()
{
	auto x = AnimalFactory::make_by_name("UglyCat", 3);
	auto y = AnimalFactory::make<UglyCat>(3);
	if (x || y)
	{
		std::cout << "should not create ugly cat" << std::endl;
	}
}
int main() {
	test_hash();
	test_name();
	test_invalid();
	return 0;
}