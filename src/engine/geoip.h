/*
    unsigned char*
*/
#ifndef ENGINE_GEOIP_H
#define ENGINE_GEOIP_H

#include "kernel.h"
#include <string>
#include <csignal>

class IGeoIP : public IInterface
{
	MACRO_INTERFACE("geoip", 0)
public:
    struct GeoInfo
    {
        GeoInfo()
        {
        	str_copy(m_aCountryCode, "NULL", sizeof(m_aCountryCode));
        	str_copy(m_aCountryName, "NULL", sizeof(m_aCountryName));
        }

        char m_aCountryCode[8];
        char m_aCountryName[32];
    };

    struct InfoGeoIPThread
    {
        IGeoIP *m_pGeoIP;
        IGeoIP::GeoInfo *m_pGeoInfo;
        char ip[64];
    };

    virtual void Search(InfoGeoIPThread *pGeoInfo) = 0;
    virtual bool IsActive() const = 0;
    virtual void Init() = 0;
};
#endif
