#pragma once



// A way to wrap a group of implementations into one object and resolve them by their interface.
// This is NOT static/singletonized.
// use like: 
	// -- main --
	// DIContainer<IPointerManager, ICheatManager> appdi{ std::make_shared<PointerManagerImpl>, std::make_shared<CheatManagerImpl> };
	// SomeObjectNeedingDependencies(appdi);
	// SomeObjectNeedingFewerDependencies(appdi);
	// 
	// 
	//  -- header for SomeObjectNeedingDependencies --
	// #include "IPointerManager.h"
	// #include "ICheatManager.h"
	// SomeObjectNeedingDependencies(IDIContainer& di)
	// {
	// mPointerManager = di.resolve<IPointerManager>();
	// mCheatManager = di.resolve<IPointerManager>();
	// }
	// 
	// 	//  -- header for SomeObjectNeedingFewerDependencies --
	// #include "IPointerManager.h"
	// SomeObjectNeedingFewerDependencies(IDIContainer& di)
	// {
	// mPointerManager = di.resolve<IPointerManager>();
	// }
	// 
	// --
// problems with this class: depenency registration and resolution are done at runtime and throw runtime errors if something goes wrong.
// it'd be nice if this could throw a compile time error instead


//class IDIContainer {
//
//protected:
//
//	IDIContainer() = default;
//
//	std::map<std::string, std::shared_ptr<void>> mMapping;
//
//	template<class Impl, class Concrete>
//	void registerDependency(std::shared_ptr<Concrete> input)
//	{
//		std::cout << "registering dependency\n";
//		const std::type_info* typeId = &typeid(Impl);
//		std::string typeString = typeId->raw_name();
//
//		// no duplicates
//		std::map<std::string, std::shared_ptr<void>>::iterator iter = mMapping.find(typeString);
//		if (iter == mMapping.end())
//		{
//			mMapping[typeString] = input;
//		}
//		else
//		{
//			throw HCMInitException(std::format("Type already exists in this DIContainer: {}", typeString));
//		}
//	}
//
//
//
//public:
//	template<typename Get>
//	std::shared_ptr<Get> Resolve()
//	{
//		const std::type_info* typeId = &typeid(Get);
//		std::string typeString = typeId->raw_name();
//
//		PLOG_DEBUG << "Resolving type: " << typeString;
//
//		if (!mMapping.contains(typeString)) throw HCMInitException(std::format("Could not locate type in DIContainer: {}", typeString));
//
//		// I should change the design of DI containers.. probably use std::any or something
//		// because converting from shared_ptr<void> to shared_ptr<T> is uh a bit sketchy.
//		return	std::reinterpret_pointer_cast<Get>(mMapping.at(typeString));
//		//return (std::shared_ptr<Get>)mMapping.at(typeString);
//		//return	std::static_pointer_cast<Get>(mMapping.at(typeString));
//
//
//	}
//
//
//};
//
//template <class... T>
//class DIContainer :public IDIContainer
//{
//public:
//	DIContainer(std::shared_ptr<T> ... inputs)
//	{
//		// https://stackoverflow.com/questions/7230621/how-can-i-iterate-over-a-packed-variadic-template-argument-list
//		([&]
//			{
//				registerDependency<T>(inputs);
//			} (), ...
//				);
//	}
//};
//




class IDIContainer {

protected:

	IDIContainer() = default;

	std::map<std::string, std::any> mMapping;

	template<class Impl, class Concrete>
	void registerDependency(std::shared_ptr<Concrete> input)
	{
		std::cout << "registering dependency\n";
		const std::type_info* typeId = &typeid(Impl);
		std::string typeString = typeId->raw_name();

		// no duplicates
		std::map<std::string, std::any>::iterator iter = mMapping.find(typeString);
		if (iter == mMapping.end())
		{
			mMapping[typeString] = std::any(input);
		}
		else
		{
			throw HCMInitException(std::format("Type already exists in this DIContainer: {}", typeString));
		}
	}




public:
	template<typename Get>
	std::weak_ptr<Get> Resolve()
	{
		const std::type_info* typeId = &typeid(Get);
		std::string typeString = typeId->raw_name();

		//PLOG_DEBUG << "Resolving type: " << typeString;

		if (!mMapping.contains(typeString)) throw HCMInitException(std::format("Could not locate type in DIContainer: {}", typeString));


		return	std::weak_ptr<Get>(std::any_cast<std::shared_ptr<Get>>(mMapping.at(typeString)));


	}



};

template <class... T>
class DIContainer :public IDIContainer
{
public:
	DIContainer(std::shared_ptr<T> ... inputs)
	{
		// https://stackoverflow.com/questions/7230621/how-can-i-iterate-over-a-packed-variadic-template-argument-list
		([&]
			{
				registerDependency<T>(inputs);
			} (), ...
				);
	}
};
