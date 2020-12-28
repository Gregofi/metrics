

#ifndef __PROGTEST__
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
using namespace std;


class CTimeStamp
{
public:
    CTimeStamp                              ( int               year,
                                              int               month,
                                              int               day,
                                              int               hour,
                                              int               minute,
                                              double            sec ):
            m_year(year),
            m_month(month),
            m_day(day),
            m_hour(hour),
            m_minute(minute),
            m_sec(sec){}
    int            Compare                                 ( const CTimeStamp & x ) const;
    friend ostream & operator <<                           ( ostream          & os,
                                                             const CTimeStamp & x );

private:
    int               m_year;
    int               m_month;
    int               m_day;
    int               m_hour;
    int               m_minute;
    double            m_sec;


};
int                CTimeStamp::Compare                     ( const CTimeStamp & x ) const
{
    if(m_year != x.m_year)
        return m_year - x.m_year;
    if(m_month != x.m_month)
        return m_month - x.m_month;
    if(m_day != x.m_day)
        return m_day - x.m_day;
    if(m_hour != x.m_hour)
        return m_hour - x.m_hour;
    if(m_minute != x.m_minute)
        return m_minute - x.m_minute;
    if(m_sec > x.m_sec)
        return 1;
    else if (m_sec < x.m_sec)
        return -1;
    return 0;

}
ostream &          operator<<                              ( ostream          & os,
                                                             const CTimeStamp & x)
{
    os << setfill('0') << setw(4) << x.m_year   << "-"
       << setw(2) << x.m_month  << "-"
       << setw(2) << x.m_day    << " "
       << setw(2) << x.m_hour   << ":"
       << setw(2) << x.m_minute << ":"
       << fixed << setw(5) << setprecision(3) << x.m_sec;
    return os;
}

class CMail
{
public:
    CMail                                   ( const CTimeStamp & timeStamp,
                                              const string     & from,
                                              const string     & to,
                                              const string     & subject ) :
            m_timeStamp(timeStamp),
            m_from(from),
            m_to(to),
            m_subject(subject){}
    int            CompareByTime                           ( const CTimeStamp & x ) const
    {
        return m_timeStamp.Compare(x);
    }
    int            CompareByTime                           ( const CMail      & x ) const
    {
        return m_timeStamp.Compare(x.m_timeStamp);
    }
    const string & From                                    ( void ) const
    {
        return m_from;
    }
    const string & To                                      ( void ) const
    {
        return m_to;
    }
    const string & Subject                                 ( void ) const
    {
        return m_subject;
    }
    const CTimeStamp & TimeStamp                           ( void ) const
    {
        return m_timeStamp;
    }
    friend ostream & operator <<                           ( ostream          & os,
                                                             const CMail      & x );
private:
    CTimeStamp      m_timeStamp;
    string          m_from;
    string          m_to;
    string          m_subject;
};

ostream &          operator <<                             ( ostream          & os,
                                                             const CMail      & x)
{
    os << x.m_timeStamp << " " << x.m_from << " -> " << x.m_to << ", subject: " << x.m_subject;
    return os;
}

namespace MysteriousNamespace {
#endif /* __PROGTEST__ */
//----------------------------------------------------------------------------------------
    class InvalidFormatException{};
    class CMailLog
    {
    public:
        struct SEmail{
            string from;
            string subject = "";

        };

        int            ParseLog                                ( istream           & in );

        list<CMail>    ListMail                                ( const CTimeStamp  & from,
                                                                 const CTimeStamp  & to ) const;

        set<string>    ActiveUsers                             ( const CTimeStamp  & from,
                                                                 const CTimeStamp  & to ) const;
        // private:

        CTimeStamp     ParseTime                               ( const string      & time,
                                                                 const string      & month,
                                                                 int                 day,
                                                                 int                 year );

        static int     ParseMonth                              ( const string      & month );

        vector<CMail>  logged_emails;
        static bool    cmp_emails                              ( const CMail       & x1,
                                                                 const CMail       & x2 );

        static bool    cmp_emails_equal                        ( const CMail       & x1,
                                                                 const CMail       & x2 );


    };

    bool               CMailLog::cmp_emails                    ( const CMail       & x1,
                                                                 const CMail       & x2 )
    {
        return (x1.TimeStamp().Compare(x2.TimeStamp()) < 0);
    }
    bool               CMailLog::cmp_emails_equal              ( const CMail       & x1,
                                                                 const CMail       & x2 )
    {
        return (x1.TimeStamp().Compare(x2.TimeStamp()) <= 0);
    }

    bool               lower_cmp_date                          ( const CMail       & x1,
                                                                 const CTimeStamp  & x2 )
    {
        return (x1.TimeStamp().Compare(x2) < 0);
    }
    bool               upper_cmp_date                          ( const CTimeStamp  & x1,
                                                                 const CMail       & x2 )
    {
        return (x1.Compare(x2.TimeStamp()) < 0);
    }

    int                CMailLog::ParseMonth                    ( const string      & month )
    {
        if(month == "Jan") return 1;
        if(month == "Feb") return 2;
        if(month == "Mar") return 3;
        if(month == "Apr") return 4;
        if(month == "May") return 5;
        if(month == "Jun") return 6;
        if(month == "Jul") return 7;
        if(month == "Aug") return 8;
        if(month == "Sep") return 9;
        if(month == "Oct") return 10;
        if(month == "Nov") return 11;
        if(month == "Dec") return 12;
        return 0;
    }

    CTimeStamp         CMailLog::ParseTime                     ( const string      & time,
                                                                 const string      & month,
                                                                 int                 day,
                                                                 int                 year )
    {
        istringstream iss;
        iss.str(time);
        int hour = 0, min = 0;
        double sec = 0;
        std::string delim1;
        iss >> hour;
        iss.get();
        iss >> min;
        iss.get();
        iss >> sec;
        int dec_month = ParseMonth(month);
        if(dec_month == 0)
            throw InvalidFormatException();
        CTimeStamp a(year, dec_month, day, hour, min, sec );
        return a;
    }


    int                CMailLog::ParseLog                      ( istream           & in )
    {

        int saved_emails_cnt = 0;
        istringstream iss_line;
        string line;

        int    day,
                year;
        string month,
                relay,
                id,
                time_unparsed,
                message;
        /* Map which holds from and subject parts of emails, key is email ID */
        map<string, SEmail> unparsed_emails;
        while( getline(in,line) )
        {
            /* First read message */
            /* Convert line to istringstream object and extract data from it */
            iss_line.clear();
            iss_line.str(line);
            iss_line >> month >> day >> year >> time_unparsed >> relay >> id;

            /*Remove first white space */
            while(iss_line.peek() == ' ')
            {
                if(!iss_line.good())
                    continue;
                iss_line.ignore(1);
            }
            getline(iss_line, message);

            /* In case something went wrong then skip this line */
            if(! (iss_line.eof() ))
            {
                continue;
            }
            /* If message starts with from then create new email in map */
            if(message.find("from=") == 0)
            {
                /* Erase the beginning of message, aka the from= part */
                message.erase(0,5);
                /* Create new temporary email and save it to map */
                SEmail em {message, ""};
                unparsed_emails.insert( make_pair(id, em) );
            }
            else if(message.find("subject=") == 0 )
            {
                message.erase(0,8);
                /* Save subject to existing temporary email */
                unparsed_emails[id].subject = message;

            }
            else if(message.find("to=") == 0)
            {
                message.erase(0,3);
                /* Create and insert email into vector */
                try
                {
                    /* Parse email */
                    CTimeStamp time = ParseTime(time_unparsed, month, day, year);
                    CMail a (time, unparsed_emails[id].from , message ,unparsed_emails[id].subject);
                    auto iter = lower_bound(logged_emails.begin(),logged_emails.end(), a, cmp_emails_equal );
                    if(iter != logged_emails.end())
                        logged_emails.insert(iter, a);
                    else
                        logged_emails.emplace_back(a);
                    saved_emails_cnt++;
                }
                catch(InvalidFormatException & e)
                {
                    continue;
                }
            }
            else
                continue;
        }
        return saved_emails_cnt;
    }
    list<CMail>    CMailLog::ListMail                          ( const CTimeStamp  & from,
                                                                 const CTimeStamp  & to ) const
    {
        /* Find first */
        // CMail from_mail (from,"","","");
        // CMail to_mail   (to  ,"","",""  );
        auto iter_begin  =  lower_bound(logged_emails.begin(), logged_emails.end(), from, lower_cmp_date );
        auto iter_end    =  upper_bound(logged_emails.begin(), logged_emails.end(), to, upper_cmp_date );
        return list<CMail>(iter_begin, iter_end);
    }

    set<string>    CMailLog::ActiveUsers                       ( const CTimeStamp  & from,
                                                                 const CTimeStamp  & to ) const
    {
        // CMail from_mail (from,"","","");
        // CMail to_mail (to,"","","");
        auto iter_begin  =  lower_bound(logged_emails.begin(), logged_emails.end(), from, lower_cmp_date );
        auto iter_end    =  upper_bound(logged_emails.begin(), logged_emails.end(), to, upper_cmp_date);
        set<string> users;
        for(auto iter = iter_begin; iter < iter_end; iter ++)
        {
            users.insert( iter->To() );
            users.insert( iter->From() );
        }
        return users;

    }


//----------------------------------------------------------------------------------------
#ifndef __PROGTEST__
} // namespace


string             printMail                               ( const list<CMail> & all )
{
    ostringstream oss;
    for ( const auto & mail : all )
        oss << mail << endl;
    return oss . str ();
}
string             printUsers                              ( const set<string> & all )
{
    ostringstream oss;
    bool first = true;
    for ( const auto & name : all )
    {
        if ( ! first )
            oss << ", ";
        else
            first = false;
        oss << name;
    }
    return oss . str ();
}
#endif