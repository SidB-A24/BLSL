//
// Created by Sid B on 06/01/26.
//

#ifndef BLSLANG_LOG_H
#define BLSLANG_LOG_H
#include <iomanip>
#include <sstream>
#include <string>

#include "../core/blsl.h"

namespace BLSL
{
    namespace Log
    {

        namespace Mod
        {
            inline const std::string CLEAR        = "\x1b[0m";

            inline const std::string BLACK        = "\x1b[30m";
            inline const std::string RED          = "\x1b[31m";
            inline const std::string GREEN        = "\x1b[32m";
            inline const std::string YELLOW       = "\x1b[33m";
            inline const std::string BLUE         = "\x1b[34m";
            inline const std::string MAGENTA      = "\x1b[35m";
            inline const std::string CYAN         = "\x1b[36m";
            inline const std::string WHITE        = "\x1b[37m";


            inline const std::string BRIGHT_BLACK   = "\x1b[90m";  // gray
            inline const std::string BRIGHT_RED     = "\x1b[91m";
            inline const std::string BRIGHT_GREEN   = "\x1b[92m";
            inline const std::string BRIGHT_YELLOW  = "\x1b[93m";
            inline const std::string BRIGHT_BLUE    = "\x1b[94m";
            inline const std::string BRIGHT_MAGENTA = "\x1b[95m";
            inline const std::string BRIGHT_CYAN    = "\x1b[96m";
            inline const std::string BRIGHT_WHITE   = "\x1b[97m";


            inline const std::string BOLD          = "\x1b[1m";
            inline const std::string DIM           = "\x1b[2m";
            inline const std::string UNDERLINE     = "\x1b[4m";

            template <typename... Mods>
            requires (std::convertible_to<Mods, std::string> && ...)
            std::string multiple(Mods&&... mods)
            {
                std::string result;
                result.reserve(sizeof...(mods));

                result.append(std::forward<Mods>(mods)...);

                return result;
            }

        }

        namespace Flag
        {
            inline const std::string LEXER = "LEXER";
            inline const std::string PARSER = "PARSER";
            inline const std::string SANITIZER = "SANITIZER";
            inline const std::string COMPILER = "COMPILER";
            inline const std::string RUNTIME = "RUNTIME";
        }

        struct _TimeStreamDispatchType {}; // Will use this for a chorno trick.

        inline constexpr _TimeStreamDispatchType TIME;

        inline const unsigned BOX_MIN_SIZE = 10;
        std::string box(std::string flag = "", std::string timeColor = Mod::DIM, std::string boxColor = Mod::WHITE, std::string flagColor = Mod::BOLD);


    }
}

namespace std
{
    inline ostream& operator<<(ostream& os, const BLSL::Log::_TimeStreamDispatchType time)
    {
        using clock = std::chrono::system_clock;
        time_t now = clock::to_time_t(clock::now());

        std::tm tm{};
#ifndef WIN32
        localtime_r(&now, &tm);
#else
        localtime_s(&now, %tm);
#endif
        stringstream oss;
        oss << std::put_time(&tm, "%H:%M:%S");
        os << oss.str();
        return os;
    }

}

#endif //BLSLANG_LOG_H