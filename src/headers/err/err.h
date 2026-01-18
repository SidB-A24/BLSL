//
// Created by Sid B on 18/01/26.
//

#ifndef BLSLANG_ERR_H
#define BLSLANG_ERR_H
namespace BLSL
{
    namespace Error
    {
        enum class Static
        {
            UNIDENTIFIED_SEQUENCE,

            UNEXPECTED_TOKEN,
            MISSING_EXPRESSION,
        };

        enum class Runtime
        {

        };
    }
}
#endif //BLSLANG_ERR_H