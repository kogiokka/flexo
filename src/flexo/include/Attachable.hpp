#ifndef ATTACHABLE_H
#define ATTACHABLE_H

#include <functional>
#include <memory>
#include <vector>

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T>
using BarePtr = T*;

class AttachableBase
{
public:
    virtual ~AttachableBase() {};
};

template <typename Host, typename Attachable, template <typename> class Pointer>
class HostBase
{
public:
    using AttachableFactory = std::function<Pointer<Attachable>(Host&)>;

    class RegisteredFactory
    {
    public:
        RegisteredFactory(AttachableFactory factory)
            : m_owner(true)
        {
            auto& factories = GetFactories();
            m_index = factories.size();
            factories.emplace_back(std::move(factory));
        }
        RegisteredFactory(RegisteredFactory&& other)
        {
            m_index = other.m_index;
            m_owner = other.m_owner;
            other.m_owner = false;
        }
        ~RegisteredFactory()
        {
            if (m_owner) {
                auto factories = GetFactories();
                factories[m_index] = nullptr;
            }
        }

    private:
        friend HostBase;
        bool m_owner;
        size_t m_index;
    };

    template <typename Derived = Attachable>
    Derived& Get(RegisteredFactory const& key)
    {
        auto index = key.m_index;

        if (m_container.size() <= index) {
            m_container.resize(index + 1);
        }

        auto factories = GetFactories();
        if (m_container[index] == nullptr) {
            m_container[index] = factories[index](static_cast<Host&>(*this));
        }

        return static_cast<Derived&>(*(m_container[index]));
    }

private:
    using AttachableFactories = std::vector<AttachableFactory>;
    using AttachableContainer = std::vector<Pointer<Attachable>>;

    static AttachableFactories& GetFactories()
    {
        static AttachableFactories factories;
        return factories;
    }

    AttachableContainer m_container;
};

#endif
