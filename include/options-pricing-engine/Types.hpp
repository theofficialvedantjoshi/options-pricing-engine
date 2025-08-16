#pragma once

using Rate = double;
using Price = double;
using Greek = double;
namespace options {
enum class OptionType { Call, Put };
enum class ExerciseStyle { European, American };
} // namespace options
