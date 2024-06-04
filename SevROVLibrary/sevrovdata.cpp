#include "sevrovdata.h"

SevROVData::SevROVData()
{
}

std::string SevROVData::toFloatString(float value, int precision)
{
    return QString::number(value, 'f', precision).toStdString();
}

std::string SevROVData::toIntString(int8_t value)
{
    return QString::number(value, 'i', 0).toStdString();
}
