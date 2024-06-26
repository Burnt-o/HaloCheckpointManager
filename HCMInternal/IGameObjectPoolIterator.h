#pragma once
#include "IGameObject.h"
#include <iterator>
#include <ranges>




/*
thinking;
combine uintptr_t to gameobject into gameobjectmeta ? GameObjectMeta isn't virtual anyway
that GameObjectConverter takes gameObjectMeta and converts it into type you want (checking the meta to make sure correct)


actually make abstract gameObjectMeta
with getAsObject/getAsUnit etc etc as member funcs
all member funcs need to be passed IGameObjectMetaReader - which ObjectPool implements
but WHAT is the value_type of the iterator? can we just out the index lmao? no.. maybe.. 
how is abstract gameObjectMeta created? Can we pass args to iterator dereference?

DUH just store a reference to a GameObjectMetaFactory which objectpool implements too
this is basically what we already have except GameObjectMeta is abstract

GameObjectMeta impls will prolly wanna store reference to the metaReader instead of passing each call- easier that way
this can skip a virtual call as a bonus
GameObjectMeta can also expose GetAsUnit etc instead of having a seperate converter
iterator value type can be std::expected<etc> ! can the begin/end be this too tho? check the static_assert
*/


//https://gist.github.com/TheMatt2/1de1afdace157e45826d8b9ad35b80cf


//class IGameObjectPoolIterator {
//public:
//
//
//    virtual std::unique_ptr<IGameObjectMeta> operator*() = 0;
//    virtual IGameObjectPoolIterator& operator++() { objectIndex++; }
//    virtual IGameObjectPoolIterator& operator++() { objectIndex++; }
//    virtual bool operator==(IGameObjectPoolIterator const& other) { return objectIndex == other.objectIndex; }
//    virtual bool operator!=(IGameObjectPoolIterator const& other) { return objectIndex != other.objectIndex; }
//    IGameObjectPoolIterator(uint16_t objectIndex) : objectIndex(objectIndex) {}
//protected:
//    uint16_t objectIndex;
//};

//
//class IGameObjectPoolIterator
//{
//public:
//	using iterator_category = std::random_access_iterator_tag;
//	using difference_type = std::ptrdiff_t;
//	using value_type = IGameObjectMeta;
//	using pointer = value_type*;
//	using reference = value_type&;
//
//	virtual reference operator*() const = 0;
//	virtual pointer operator->() = 0;
//
//
//	// Prefix increment
//	virtual IGameObjectPoolIterator& operator++() = 0;
//
//	// Postfix increment
//	virtual IGameObjectPoolIterator operator++(int) = 0;
//
//	// Prefix decrement
//	virtual IGameObjectPoolIterator& operator--() = 0;
//
//	// Postfix decrement
//	virtual IGameObjectPoolIterator operator--(int) = 0;
//
//	virtual friend bool operator== (const IGameObjectPoolIterator& a, const IGameObjectPoolIterator& b) = 0;
//	virtual friend bool operator!= (const IGameObjectPoolIterator& a, const IGameObjectPoolIterator& b) = 0;
//	virtual friend size_t operator- (const IGameObjectPoolIterator& a, const IGameObjectPoolIterator& b) = 0;
//	virtual friend bool operator< (const IGameObjectPoolIterator& a, const IGameObjectPoolIterator& b) = 0;
//
//	virtual ~IGameObjectPoolIterator() = default;
//};



