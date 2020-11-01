#include <entity_manager.h>
#include <iostream>
using namespace spiritsaway::entity_component_event;
class AvatarEntity;
using AvatarComponent = base_component<AvatarEntity, const std::string&>;
using AvatarComponentFactory = poly_hash_factory<AvatarComponent, raw_ptr_t, const std::string&>;

class AvatarEntity: public component_entity<AvatarComponent, AvatarEntity>, public entity_factory::sub_class<AvatarEntity>
{
public:
	AvatarEntity(std::size_t entity_type_id, std::string entity_id)
		: entity_factory::sub_class<AvatarEntity>(entity_type_id, entity_id)
		, component_entity<AvatarComponent, AvatarEntity>(this)
	{
		std::cout<<"AvatarEntity created with entity_id "<< entity_id <<std::endl;
	}
};
class move_component: public AvatarComponentFactory::sub_class<move_component>
{
private:
	void OnEnterSpace(int a)
	{
		std::cout<<"move_component OnEnterSpace "<<a<<std::endl;
	}
public:
	move_component(const std::string& a)
	{
		std::cout<<"move_component init with "<<a<<std::endl;
	}
	std::string_view name() const override
	{
		return type_name<move_component>();
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
	auto cur_avatar = entity_factory::make<AvatarEntity>(std::string("lalla"));
	auto cur_comp = AvatarComponentFactory::make<move_component>(std::string("heheh"));
	cur_avatar->AddComponent(cur_comp);
	auto cur_component = cur_avatar->GetComponent<move_component>();
	cur_avatar->DispatchEvent(entity_events::OnEnterSpace, 1);
	cur_avatar->RemoveComponent<move_component>();
	cur_avatar->DispatchEvent(entity_events::OnEnterSpace, 1);
	cur_avatar.reset();
}