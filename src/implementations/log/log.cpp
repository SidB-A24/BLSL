//
// Created by Sid B on 06/01/26.
//

#include "../../headers/log/log.h"
std::string BLSL::Log::box(std::string flag, std::string timeColor, std::string boxColor, std::string flagColor)
{
    std::stringstream ss;
    ss << boxColor << "[" << Mod::CLEAR;
    ss <<  flagColor << std::left << std::setw(BOX_MIN_SIZE) << flag << std::setw(0) << Mod::CLEAR;
    ss << timeColor << TIME << Mod::CLEAR;
    ss << boxColor << "]" << Mod::CLEAR;

    return ss.str();
}
