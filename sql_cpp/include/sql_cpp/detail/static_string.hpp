#pragma once

#include <algorithm>
#include <assert.h>

namespace sql_cpp::detail {

template <typename CharType = char, unsigned BufferSize = 64> struct StaticString {
    using size_type = std::string_view::size_type;
    /// TODO: missing typedef's

    constexpr StaticString() {
        // std::fill is not yet constexpr
        for (auto& e : m_Buffer)
            e = '\0';
    }
    constexpr StaticString(std::string_view sv) : StaticString() {
        std::copy(sv.cbegin(), sv.cend(), begin());
        m_SizeBufferUsed = sv.size();
        _appendBinaryZeroIfRequired();
    }
    constexpr StaticString(const StaticString& rhs) : StaticString() {
        std::copy(rhs.cbegin(), rhs.cend(), std::begin(m_Buffer));
        m_SizeBufferUsed = rhs.m_SizeBufferUsed;
        _appendBinaryZeroIfRequired();
    }

    constexpr StaticString& operator=(const StaticString& rhs) {
        std::copy(std::cbegin(rhs.m_Buffer), std::cend(rhs.m_Buffer), std::begin(m_Buffer));
        m_SizeBufferUsed = rhs.m_SizeBufferUsed;
        _appendBinaryZeroIfRequired();
    }

    constexpr StaticString& operator+=(std::string_view rh) {
        if (m_SizeBufferUsed + rh.size() >= BufferSize)
            throw std::runtime_error("StaticString BufferSize to small");

        std::copy(rh.cbegin(), rh.cend(), end());
        m_SizeBufferUsed += rh.size();
        _appendBinaryZeroIfRequired();
        return *this;
    }
    constexpr StaticString operator+(std::string_view rh) const {
        auto cpy = *this;
        cpy += rh;
        return cpy;
    }

    constexpr auto& operator[](unsigned index) {
        if (index >= m_SizeBufferUsed)
            throw std::out_of_range("index out of range");
        return m_Buffer[index];
    }
    constexpr const auto& operator[](unsigned index) const {
        if (index >= m_SizeBufferUsed)
            throw std::out_of_range("index out of range");
        return m_Buffer[index];
    }

    constexpr auto begin() { return std::begin(m_Buffer); }
    constexpr auto begin() const { return std::cbegin(m_Buffer); }
    constexpr auto cbegin() const { return std::cbegin(m_Buffer); }

    constexpr auto end() { return begin() + m_SizeBufferUsed; }
    constexpr auto end() const { return cend(); }
    constexpr auto cend() const { return cbegin() + m_SizeBufferUsed; }

    constexpr auto& front() { return *begin(); }
    constexpr auto& front() const { return *cbegin(); }

    constexpr auto& back() { return *(end() - 1); }
    constexpr auto& back() const { return *(cend() - 1); }

    constexpr bool empy() const noexcept { return m_SizeBufferUsed == 0; }
    constexpr size_type size() const noexcept { return m_SizeBufferUsed; }
    static consteval size_type max_size() noexcept { return BufferSize; }

    constexpr auto c_str() { return begin(); }
    constexpr auto c_str() const { return cbegin(); }

    constexpr auto data() { return begin(); }
    constexpr auto data() const { return cbegin(); }

    std::string to_string() const { return {cbegin(), cend()}; }
    constexpr std::string_view to_string_view() const { return {cbegin(), cend()}; }

  private:
    constexpr void _appendBinaryZeroIfRequired() {
        if (m_SizeBufferUsed > BufferSize)
            throw std::runtime_error("StaticString BufferSize to small");
        else if (m_SizeBufferUsed == BufferSize && back() != '\0')
            throw std::runtime_error("StaticString BufferSize to small");
        else if (auto& endChar = *end(); endChar != '\0') {
            endChar = '\0';
            ++m_SizeBufferUsed;
        }
    }
    size_type m_SizeBufferUsed = 0;
    CharType m_Buffer[BufferSize];
};

} // namespace sql_cpp::detail
