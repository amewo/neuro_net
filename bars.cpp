#include <fstream>
#include <array>
#include <boost/lexical_cast.hpp>

#include "bars.h"

//-----------------------------------------------------------------------------
bars::bars() noexcept
{
}
//-----------------------------------------------------------------------------
bars::~bars() noexcept
{
}
//-----------------------------------------------------------------------------
size_t bars::get_size() const noexcept
{
    return m_bars.size();
}
//-----------------------------------------------------------------------------
bar& bars::get_bar(size_t indx) throw (std::range_error)
{
    if( indx >= m_bars.size() )
    {
        throw std::range_error("indx >= m_bars.size()");
    }

    return m_bars[indx];
}
//-----------------------------------------------------------------------------
bar& bars::operator[](size_t indx) throw (std::range_error)
{
    if( indx >= m_bars.size() )
    {
        throw std::range_error("indx >= m_bars.size()");
    }

    return m_bars[indx];
}
//-----------------------------------------------------------------------------
void bars::push_back(const bar& b) noexcept
{
    m_bars.push_back(b);
}
//-----------------------------------------------------------------------------
void bars::insert(std::size_t indx, const bar& b) throw (std::range_error)
{
    if( indx > m_bars.size() )
    {
        throw std::range_error("can't insert bar - error indx");
    }

    m_bars.insert(m_bars.begin() + indx, b);
}
//-----------------------------------------------------------------------------
void bars::erase(size_t indx) throw (std::range_error)
{
    if( indx >= m_bars.size() )
    {
        throw std::range_error("can't erase bar - error indx");
    }

    m_bars.erase(m_bars.begin() + indx);
}
//-----------------------------------------------------------------------------
bool bars::load_from_csv(const std::string& file_name) noexcept
{
    std::ifstream scv_file(file_name);

    if( scv_file.is_open() == false )
    {
        return false;
    }

    m_bars.clear();

    std::array<char, 256> csv_char_str;

    while( true )
    {
        scv_file.getline(&csv_char_str[0], csv_char_str.size(), '\n');

        if( scv_file.eof() == true )
        {
            break;
        }

        std::string csv_str(&csv_char_str[0]);

        std::stringstream stream(csv_str);

        std::string date_str;        std::getline(stream, date_str, ',');

        std::string date_year_str(date_str, 0, 4);
        std::string date_month_str(date_str, 5, 2);
        std::string date_day_str(date_str, 8, 2);

        std::string time_str;        std::getline(stream, time_str, ',');

        std::string time_hour_str(time_str, 0, 2);
        std::string time_minute_str(time_str, 3, 2);

        std::string open_price_str;  std::getline(stream, open_price_str, ',');
        std::string high_price_str;  std::getline(stream, high_price_str, ',');
        std::string low_price_str;   std::getline(stream, low_price_str, ',');
        std::string close_price_str; std::getline(stream, close_price_str, ',');

        bar new_bar;

        struct tm bar_time;
        memset(&bar_time, 0, sizeof(bar_time));

        bar_time.tm_year = boost::lexical_cast<int>(date_year_str);
        bar_time.tm_mon = boost::lexical_cast<int>(date_month_str) + 1;
        bar_time.tm_mday = boost::lexical_cast<int>(date_day_str);

        bar_time.tm_hour = boost::lexical_cast<int>(time_hour_str);
        bar_time.tm_min = boost::lexical_cast<int>(time_minute_str);

        new_bar.time = mktime(&bar_time);

        new_bar.open = boost::lexical_cast<float>(open_price_str);
        new_bar.high = boost::lexical_cast<float>(high_price_str);
        new_bar.low = boost::lexical_cast<float>(low_price_str);
        new_bar.close = boost::lexical_cast<float>(close_price_str);

        insert(0, new_bar);
    }

    scv_file.close();

    return true;
}
//-----------------------------------------------------------------------------
