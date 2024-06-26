#pragma once
#include "pch.h"
#include "Datum.h"
#include <cstddef>
#include "IGameObject.h"
#include <ranges>
#include <algorithm>
#include <iterator>

class ObjectPoolContainer
{
public:


class iterator {
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::shared_ptr< IGameObjectMeta>;
    using difference_type = std::ptrdiff_t;
    using reference = value_type; 
    using const_reference = reference;    

    iterator() noexcept = default;
    iterator(const iterator& other) noexcept = default;
    iterator& operator=(const iterator& other) noexcept = default;


    iterator(uint16_t index, std::shared_ptr< IGameObjectMetaFactory> objFactory) : objectIndex(index), objectMetaFactory(objFactory) {}

    reference operator*() noexcept {
        return objectMetaFactory->makeObjectMeta(objectIndex);

    }

    const_reference operator*() const noexcept {
        return objectMetaFactory->makeObjectMeta(objectIndex);
    }

    reference operator[](difference_type index) noexcept {
        return *(*this + index);
    }

    const_reference operator[](difference_type index) const noexcept {
        return *(*this + index);
    }

    iterator& operator++() noexcept {
        ++objectIndex;
        return *this;
    }

    iterator operator++(int) noexcept {
        iterator temp = *this;
        ++(*this);
        return temp;
    }

    iterator& operator--() noexcept {
        --objectIndex;
        return *this;
    }

    iterator operator--(int) noexcept {
        iterator temp = *this;
        --(*this);
        return temp;
    }

    iterator& operator+=(difference_type n) noexcept {
        objectIndex += n;
        return *this;
    }

    iterator operator+(difference_type n) const noexcept {
        iterator result(*this);
        return result += n;
    }

    friend iterator operator+(difference_type    n,
        const iterator& it) noexcept {
        return it + n;
    }

    iterator& operator-=(difference_type n) noexcept {
        return *this += (-n);
    }

    iterator operator-(difference_type n) const noexcept {
        return *this + (-n);
    }

    friend iterator operator-(difference_type    n,
        const iterator& it) noexcept {
        return it - n;
    }

    difference_type operator-(const iterator& other) const noexcept {
        return std::distance(other.objectIndex, objectIndex);
    }

    bool operator==(const iterator& other) const noexcept {
        return (other - *this) == 0;
    }

    bool operator!=(const iterator& other) const noexcept {
        return !(*this == other);
    }

    bool operator<(const iterator& other) const noexcept {
        return 0 < (*this - other);
    }

    bool operator<=(const iterator& other) const noexcept {
        return 0 <= (*this - other);
    }

    bool operator>(const iterator& other) const noexcept {
        return 0 > (*this - other);
    }

    bool operator>=(const iterator& other) const noexcept {
        return 0 >= (*this - other);
    }

private:
    		uint16_t objectIndex;
    		std::shared_ptr< IGameObjectMetaFactory> objectMetaFactory;
            std::shared_ptr<IGameObjectMeta> referenceTmp;
};



	iterator begin() const { return iterator(0, objectMetaFactory); }
	iterator end() const { return iterator(mSize, objectMetaFactory); }


	bool empty() const
	{
		return mSize == 0;
	}

	uint16_t size() const
	{
		return mSize;
	}

    ObjectPoolContainer(std::shared_ptr< IGameObjectMetaFactory> objectMetaFactory, uint16_t size)
        : objectMetaFactory(objectMetaFactory), mSize(size) {}

	 private:
		 uint16_t mSize;
		 std::shared_ptr< IGameObjectMetaFactory> objectMetaFactory;

};

class IProvideObjectPoolContainer
{
public:
    virtual ObjectPoolContainer::iterator begin() = 0;
    virtual ObjectPoolContainer::iterator end() = 0;


    virtual bool empty() = 0;

    virtual uint16_t size() = 0;

    virtual ~IProvideObjectPoolContainer() = default;
};

static_assert(std::ranges::random_access_range<ObjectPoolContainer>);


