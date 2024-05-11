#pragma once

#include "Shader.h"
#include "Utilities.h"

#include "glad/glad.h"

#include <unordered_map>
#include <memory>
#include <mutex>

template <class Key, class Value, class... Args>
class Bank
{
public:
    static Bank& instance()
    {
        static Bank* inst = nullptr;

        if (!inst)
        {
            inst = new Bank();
        }

        return *inst;
    }

    void addValue(const Key& key, Value&& val)
    {
        container[key] = std::move(val);
    }

    void addValue(const Key& key, const Value& val)
    {
        container[key] = val;
    }

    Value& getValue(const Key& key, Args... args)
    {
        throw("Please specialize this instance of the Bank template.");
    }

    bool contains(const Key& key) const
    {
        return container.contains(key);
    }

    void remove(const Key& key)
    {
        if constexpr (std::is_pointer_v<Value>)
        {
            auto& val = getValue(key);
            container.erase(key);
            delete val;
        }
        else
        {
            container.erase(key);
        }
    }

    void clear()
    {
        container.clear();
    }

private:
    Bank(){};

    std::unordered_map<Key, Value> container;
    std::mutex mutex;
};

template <>
inline std::unique_ptr<Shader>& Bank<ShaderType, std::unique_ptr<Shader>>::getValue(const ShaderType& key)
{
    if (!container.contains(key))
    {
        std::lock_guard<std::mutex> lock(mutex);
        switch (key)
        {
            case ShaderType::Basic:
                addValue(key, std::make_unique<BasicShader>());
                break;
            case ShaderType::CtViewport:
                addValue(key, std::make_unique<CtViewportShader>());
                break;
            case ShaderType::MainViewport:
                addValue(key, std::make_unique<MainViewportShader>());
                break;
            default:
                throw std::exception();
        }
    }

    return container.at(key);
}

using ShaderBank = Bank<ShaderType, std::unique_ptr<Shader>>;
