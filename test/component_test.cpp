#include <entity_manager.h>
#include <iostream>
using namespace spiritsaway::entity_component_event;
class AvatarEntity;
using AvatarComponent = base_component<AvatarEntity, const std::string&>;
using AvatarComponentFactory = AvatarComponent::component_factory;

class AvatarEntity final: public component_entity<AvatarComponent, AvatarEntity>, public entity_manager::sub_class<AvatarEntity>
{
public:
	AvatarEntity(entity_construct_key access_key, std::string entity_id)
		: entity_manager::sub_class<AvatarEntity>(access_key,  entity_id)
		, component_entity<AvatarComponent, AvatarEntity>(this)
	{
		std::cout<<"AvatarEntity created with entity_id "<< entity_id <<std::endl;
	}
	static std::string_view  class_name()
	{
		return "AvatarEntity";
	}
protected:
	virtual void destroy() override
	{
		std::cout << "destroy avatar entity" << std::endl;
		component_entity<AvatarComponent, AvatarEntity>::Destroy();
		entity_manager::sub_class<AvatarEntity>::destroy();
	}
public:
	friend class entity_manager;
};

class move_component final: public AvatarComponentFactory::sub_class<move_component>
{
private:
	void OnEnterSpace(int a)
	{
		std::cout<<"move_component OnEnterSpace "<<a<<std::endl;
	}
public:
	move_component(const AvatarComponentFactory::construct_key& in_key, const std::string& a)
		:AvatarComponentFactory::sub_class<move_component>(in_key, a)
	{
		std::cout<<"move_component init with "<<a<<std::endl;
	}
	static std::string_view  static_class_name()
	{
		return "move_component";
	}
	std::string_view name() const override
	{
		return "move_component";
	}
	void OnSetOwner() override
	{
		std::function<void(const entity_events& cur_event, const int& detail)> cur_callback = [this](const entity_events& cur_event, const int& detail)
		{
			this->OnEnterSpace(detail);
		};
		AddEventListener(entity_events::OnEnterSpace, cur_callback);
		std::cout << "OnSetOwner for move_comp" << std::endl;
	}
	void OnRemoveOwner() override
	{
		std::cout << "OnRemoveOwner for move_component" << std::endl;
	}
	~move_component()
	{
		std::cout << "move_component destroy" << std::endl;
	}
};
int main()
{
	auto cur_avatar = entity_manager::make<AvatarEntity>(std::string("lalla"));
	// auto temp_avatar = AvatarEntity(entity_construct_key(), 0, "lal");
	auto cur_comp = AvatarComponentFactory::make<move_component>(std::string("heheh"));
	cur_avatar->AddComponent(cur_comp);
	auto cur_component = cur_avatar->GetComponent<move_component>();
	cur_avatar->DispatchEvent(entity_events::OnEnterSpace, 1);
	cur_avatar->RemoveComponent<move_component>();
	cur_avatar->DispatchEvent(entity_events::OnEnterSpace, 1);
	entity_manager::instance().destroy_entity(cur_avatar->entity_id());
	cur_avatar.reset();
}