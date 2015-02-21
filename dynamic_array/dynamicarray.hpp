#ifndef DYNAMICARRAY
#define DYNAMICARRAY


#include <memory>


namespace angel{

template<typename T, std::size_t MaxByteSize = 256, std::size_t MaxStackElements = 16>
class DynamicArray {
protected:
    class iterator; // forward declaration

public:
    DynamicArray(std::size_t size) : m_Size(size){
        if (size <= DynamicArray::C_ArraySize)
            this->m_Ptr = &this->m_C_Array[0];
        else{
            this->m_PtrArray = std::make_unique<T[]>(size);
            this->m_Ptr = this->m_PtrArray.get();
        }
    }
    DynamicArray(std::size_t size, const T& element) : DynamicArray(size){
        for (std::size_t i = 0; i < this->m_Size; ++i)
            this->m_Ptr[i] = element;
    }
    DynamicArray(std::initializer_list<T> list) : DynamicArray(list.size()){
        std::size_t iter = 0;
        for (auto& e : list){
            this->m_Ptr[iter] = e;
            ++iter;
        }
    }

    T& operator[] (std::size_t pos) noexcept{
        return this->m_Ptr[pos];
    }

    T& at(std::size_t pos) {
        if (pos >= this->m_Size)
            throw std::out_of_range(R"angel(Exception in function "T& DynamicArray::at(std::size_t pos)": out of range)angel");
        else
            return this->m_Ptr[pos];
    }

    DynamicArray<T>& operator=(const DynamicArray<T>& other) {
        if (other.m_Size != this->m_Size)
            throw std::runtime_error(R"angel(Exception in function "DynamicArray<T>& DynamicArray::operator=(const DynamicArray<T>& other)": sizes miss match!")angel");

        for (std::size_t i = 0; i < this->m_Size; ++i)
            this->m_Ptr[i] = other.m_Ptr[i];

        return *this;
    }
    DynamicArray<T>& operator=(DynamicArray<T>&& other) noexcept(false){
        if (other.m_Size < this->m_Size)
            throw std::runtime_error(R"angel(Exception in function "DynamicArray<T>& DynamicArray::operator=(DynamicArray<T>&& other)": sizes miss match!")angel");

        for (std::size_t i = 0; i < this->m_Size; ++i)
            this->m_Ptr[i] = std::move(other.m_Ptr[i]);

        return *this;
    }

    std::size_t size() const {
        return this->m_Size;
    }


    iterator begin(){
        return iterator(this->m_Ptr, 0);
    }

    iterator end(){
        return iterator(this->m_Ptr, this->m_Size);
    }


protected:

    static constexpr std::size_t calculate_CArray_Size(){
        /*
         * std::size_t maxSize = MaxByteSize / sizeof(T);
         *
         * if (maxSize == 0)
         *     maxSize = 1;    // error with 0 (c array of size 0 isn't allowed)
         * else if (maxSize > MaxStackElements)
         *     maxSize = MaxStackElements;
         * else{}
         *
         * return maxSize;
         *
         * // while this code compiles in clang 3.5,
         * // gcc4.9 doesn't support c++14 constexpr functions -> use c++11 style....
         * // gcc5.0 does    support c++14 version -> to change at release
         *
        */
        return MaxByteSize / sizeof(T) > 0 ? (MaxByteSize / sizeof(T) > MaxStackElements ? MaxStackElements : MaxByteSize / sizeof(T)) : 1;
    }
    static constexpr std::size_t C_ArraySize = DynamicArray::calculate_CArray_Size();


    class iterator : public std::iterator<std::random_access_iterator_tag, T>
    {
    public:
        iterator(T* array, std::size_t pos): m_Array(array), m_Pos(pos) {}
        iterator(const iterator& iter) = default;

        virtual ~iterator() = default;


        bool operator== (const iterator& iter){
            if (iter.m_Array == this->m_Array && this->m_Pos == iter.m_Pos)
                return true;
            return false;
        }

        bool operator!= (const iterator& iter){
            return ! (*this == iter);
        }

        T& operator*(){
            return this->m_Array[this->m_Pos];
        }
        T& operator->(){
            return this->m_Array[this->m_Pos];
        }

        iterator& operator++(){  // iter++ operator
            ++this->m_Pos;
            return *this;
        }
        iterator& operator++(int){   // ++iter operator
            ++this->m_Pos;
            return *this;
        }

        iterator& operator-- (){
            --this->m_Pos;
            return *this;
        }
        iterator& operator-- (int){
            --this->m_Pos;
            return *this;
        }

        iterator& operator+= (int toAdd){
            this->m_Pos += toAdd;
            return *this;
        }
        iterator& operator-= (int toSub){
            this->m_Pos -= toSub;
            return *this;
        }

        iterator operator+ (int toAdd){
            return {this->m_Array, this->m_Pos + toAdd};
        }
        iterator operator- (int toSub){
            return {this->m_Array, this->m_Pos - toSub};
        }

        int operator+ (const iterator& iter){
            return this->m_Pos + iter.m_Pos;
        }
        int operator- (const iterator& iter){
            return this->m_Pos - iter.m_Pos;
        }

        bool operator< (const iterator& iter){
            if (*this - iter > 0)
                return true;
            return false;
        }
        bool operator> (const iterator& iter){
            return iter < *this;
        }

        bool operator<= (const iterator& iter){
            return !(*this < iter);
        }
        bool operator>= (const iterator& iter){
            return !(*this > iter);
        }


        // assignment operators
        iterator& operator= (const iterator&  iter) = default;
        iterator& operator= (      iterator&& iter) = delete;

    private:
        T* m_Array;
        std::size_t m_Pos;
    };


    std::size_t m_Size;
    T* m_Ptr = nullptr;

    T m_C_Array[DynamicArray::C_ArraySize];
    std::unique_ptr<T[]> m_PtrArray = nullptr;
};

} // end namespace angel


namespace std{
// overload std::begin & std::end
template<typename T, size_t MaxByteSize = 256, size_t MaxStackElements = 16>
typename angel::DynamicArray<T, MaxByteSize, MaxStackElements>::iterator begin (const angel::DynamicArray<T, MaxByteSize, MaxStackElements>& array) {
    return array.begin();
}


template<typename T, size_t MaxByteSize = 256, size_t MaxStackElements = 16>
typename angel::DynamicArray<T, MaxByteSize, MaxStackElements>::iterator end (const angel::DynamicArray<T, MaxByteSize, MaxStackElements>& array) {
    return array.end();
}

} // end of namespace std


#endif // DYNAMICARRAY

