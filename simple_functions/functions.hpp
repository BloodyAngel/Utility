

#ifndef __FUNCTIONS_HPP__
#define __FUNCTIONS_HPP__

template<typename T, int v>
struct _int_max_helper_function{
    static const T value = (_int_max_helper_function<T, v - 1>::value << 1) | 1;
};
template<typename T>
struct _int_max_helper_function<T, 1>{
    static const T value = 1;
};

template<typename T>
struct int_max{
    static const T value = static_cast<T>(static_cast<T>(1) << static_cast<T>(sizeof(T) * 8 - 1)) < static_cast<T>(0)
              ? _int_max_helper_function<T, sizeof(T) * 8 - 1>::value //   signed value
              : _int_max_helper_function<T, sizeof(T) * 8 - 0>::value;// unsigned value
};


#endif

