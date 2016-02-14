#ifndef __CLI_COLOR_HPP__
#define __CLI_COLOR_HPP__

#include <iostream>

// usage example
//   std::cout << Cli::RED << "my Text in red" << Cli::DEFAULT << std::endl;

namespace Cli{
#if defined(__linux)
enum class Color {
    DEFAULT = 39, BLACK   = 30, WHITE  = 37,
    RED     = 31, GREEN   = 32, BLUE   = 34,
    CYAN    = 36, MAGENTA = 35, YELLOW = 33
};
#elif defined(_WIN32)
#include <windows.h>

#error "NOT ALL COLORS HAVE BEEN SET FOR WINDOWS!"
enum class Color {
    DEFAULT =  , BLACK   = 0, WHITE  = 15,
    RED     = 3, GREEN   = 2, BLUE   = 1 ,
    CYAN    =  , MAGENTA =  , YELLOW = 12
};
#else
enum class Color { DEFAULT, BLACK, WHITE, RED, GREEN , BLUE, CYAN, MAGENTA, YELLOW }; // functionallity not available
#endif

template<Color color>
struct CliColor{
    constexpr CliColor() = default;
    friend std::ostream& operator<<(std::ostream& os, const CliColor<color>& rhs){
#if defined(__linux)
        return os << "\033[" << static_cast<int>(color) << ";1m";
#elif defined(_WIN32)
        // link: http://stackoverflow.com/questions/9262270/color-console-output-with-c-in-windows
        HANDLE hConsole;
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, static_cast<int>(color));
        return os;
#else
        #warning "Cli Color is not implemented"
        return os;
#endif
    }
};
constexpr CliColor<Color::DEFAULT> DEFAULT = {};
constexpr CliColor<Color::BLACK  > BLACK   = {};
constexpr CliColor<Color::WHITE  > WHITE   = {};

constexpr CliColor<Color::RED    > RED     = {};
constexpr CliColor<Color::GREEN  > GREEN   = {};
constexpr CliColor<Color::BLUE   > BLUE    = {};

constexpr CliColor<Color::CYAN   > CYAN    = {};
constexpr CliColor<Color::MAGENTA> MAGENTA = {};
constexpr CliColor<Color::YELLOW > YELLOW  = {};
}

#endif