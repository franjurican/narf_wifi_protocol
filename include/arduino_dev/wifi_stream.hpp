#ifndef LINUX_WIFI_STREAM_H
#define LINUX_WIFI_STREAM_H

#include <linux/wireless.h>
#include <linux/ioctl.h>
#include <string>
#include <vector>

#define IW_DESCR_FLAG_NONE        0x0000
#define IW_DESCR_FLAG_DUMP        0x0001
#define IW_DESCR_FLAG_EVENT       0x0002
#define IW_DESCR_FLAG_RESTRICT    0x0004
#define IW_DESCR_FLAG_NOMAX       0x0008
#define IW_DESCR_FLAG_WAIT        0x0100

struct iw_event_stream {
    char *end;
    char *current;
    char *value;
};

typedef struct wireless_scan_info {
    struct wireless_scan_info *next;	
    float freq;		
    std::string essid;
    std::string accesPointMac;
    int signalStrenght;
    std::vector<int> speed;		
} wireless_scan_info;

typedef struct wireless_scan_struct {
    wireless_scan_info *result;	
    int step;
} wireless_scan_struct;

#define IW_HEADER_TYPE_NULL    0    
#define IW_HEADER_TYPE_CHAR    2    
#define IW_HEADER_TYPE_UINT    4    
#define IW_HEADER_TYPE_FREQ    5    
#define IW_HEADER_TYPE_ADDR    6    
#define IW_HEADER_TYPE_POINT    8   
#define IW_HEADER_TYPE_PARAM    9   
#define IW_HEADER_TYPE_QUAL    10   

struct iw_ioctl_description
{
    __u8    header_type;        
    __u8    token_type;        
    __u16    token_size;       
    __u16    min_tokens;       
    __u16    max_tokens;       
    __u32    flags;            
};

// Fill Missing Defines
#ifndef SIOCSIWMODUL
#define SIOCSIWMODUL    0x8B2E        /* set Modulations settings */
#endif
#ifndef SIOCGIWMODUL
#define SIOCGIWMODUL    0x8B2F        /* get Modulations settings */
#endif

const struct iw_ioctl_description standard_ioctl_descr[] = {
    [SIOCSIWCOMMIT    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_NULL,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCGIWNAME    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_CHAR,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = IW_DESCR_FLAG_DUMP,
    },
    [SIOCSIWNWID    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = IW_DESCR_FLAG_EVENT,
    },
    [SIOCGIWNWID    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = IW_DESCR_FLAG_DUMP,
    },
    [SIOCSIWFREQ    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_FREQ,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = IW_DESCR_FLAG_EVENT,
    },
    [SIOCGIWFREQ    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_FREQ,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = IW_DESCR_FLAG_DUMP,
    },
    [SIOCSIWMODE    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_UINT,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = IW_DESCR_FLAG_EVENT,
    },
    [SIOCGIWMODE    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_UINT,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = IW_DESCR_FLAG_DUMP,
    },
    [SIOCSIWSENS    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCGIWSENS    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCSIWRANGE    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_NULL,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCGIWRANGE    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = sizeof(struct iw_range),
        .flags          = IW_DESCR_FLAG_DUMP,
    },
    [SIOCSIWPRIV    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_NULL,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCGIWPRIV    - SIOCIWFIRST] = { 
        .header_type    = IW_HEADER_TYPE_NULL,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCSIWSTATS    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_NULL,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCGIWSTATS    - SIOCIWFIRST] = { 
        .header_type    = IW_HEADER_TYPE_NULL,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = IW_DESCR_FLAG_DUMP,
    },
    [SIOCSIWSPY    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = sizeof(struct sockaddr),
        .min_tokens     = 0,
        .max_tokens     = IW_MAX_SPY,
        .flags          = 0,
    },
    [SIOCGIWSPY    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = sizeof(struct sockaddr) +
                          sizeof(struct iw_quality),
        .min_tokens     = 0,
        .max_tokens     = IW_MAX_SPY,
        .flags          = 0,
    },
    [SIOCSIWTHRSPY    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = sizeof(struct iw_thrspy),
        .min_tokens     = 1,
        .max_tokens     = 1,
        .flags          = 0,
    },
    [SIOCGIWTHRSPY    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = sizeof(struct iw_thrspy),
        .min_tokens     = 1,
        .max_tokens     = 1,
        .flags          = 0,
    },
    [SIOCSIWAP    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_ADDR,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCGIWAP    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_ADDR,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = IW_DESCR_FLAG_DUMP,
    },
    [SIOCSIWMLME    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = sizeof(struct iw_mlme),
        .max_tokens     = sizeof(struct iw_mlme),
        .flags          = 0,
    },
    [SIOCGIWAPLIST    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = sizeof(struct sockaddr) +
                          sizeof(struct iw_quality),
        .min_tokens     = 0,
        .max_tokens     = IW_MAX_AP,
        .flags          = IW_DESCR_FLAG_NOMAX,
    },
    [SIOCSIWSCAN    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = sizeof(struct iw_scan_req),
        .flags          = 0,
    },
    [SIOCGIWSCAN    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = IW_SCAN_MAX_DATA,
        .flags          = IW_DESCR_FLAG_NOMAX,
    },
    [SIOCSIWESSID    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = IW_ESSID_MAX_SIZE + 1,
        .flags          = IW_DESCR_FLAG_EVENT,
    },
    [SIOCGIWESSID    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = IW_ESSID_MAX_SIZE + 1,
        .flags          = IW_DESCR_FLAG_DUMP,
    },
    [SIOCSIWNICKN    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = IW_ESSID_MAX_SIZE + 1,
        .flags          = 0,
    },
    [SIOCGIWNICKN    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = IW_ESSID_MAX_SIZE + 1,
        .flags          = 0,
    },
    [0x8B1E    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [0x8B1F    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCSIWRATE    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCGIWRATE    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCSIWRTS    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCGIWRTS    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCSIWFRAG    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCGIWFRAG    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCSIWTXPOW    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCGIWTXPOW    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCSIWRETRY    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCGIWRETRY    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCSIWENCODE    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = IW_ENCODING_TOKEN_MAX,
        .flags          = IW_DESCR_FLAG_EVENT | IW_DESCR_FLAG_RESTRICT,
    },
    [SIOCGIWENCODE    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = IW_ENCODING_TOKEN_MAX,
        .flags          = IW_DESCR_FLAG_DUMP | IW_DESCR_FLAG_RESTRICT,
    },
    [SIOCSIWPOWER    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCGIWPOWER    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCSIWMODUL    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCGIWMODUL    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCSIWGENIE    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = IW_GENERIC_IE_MAX,
        .flags          = 0,
    },
    [SIOCGIWGENIE    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = IW_GENERIC_IE_MAX,
        .flags          = 0,
    },
    [SIOCSIWAUTH    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCGIWAUTH    - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_PARAM,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [SIOCSIWENCODEEXT - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = sizeof(struct iw_encode_ext),
        .max_tokens     = sizeof(struct iw_encode_ext) +
                          IW_ENCODING_TOKEN_MAX,
        .flags          = 0,
    },
    [SIOCGIWENCODEEXT - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = sizeof(struct iw_encode_ext),
        .max_tokens     = sizeof(struct iw_encode_ext) +
                          IW_ENCODING_TOKEN_MAX,
        .flags          = 0,
    },
    [SIOCSIWPMKSA - SIOCIWFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = sizeof(struct iw_pmksa),
        .max_tokens     = sizeof(struct iw_pmksa),
        .flags          = 0,
    },
};

const unsigned int standard_ioctl_num = (sizeof(standard_ioctl_descr) / sizeof(struct iw_ioctl_description));

// events
const struct iw_ioctl_description standard_event_descr[] = {
    [IWEVTXDROP    - IWEVFIRST] = {
        .header_type    = IW_HEADER_TYPE_ADDR,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [IWEVQUAL    - IWEVFIRST] = {
        .header_type    = IW_HEADER_TYPE_QUAL,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [IWEVCUSTOM    - IWEVFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = IW_CUSTOM_MAX,
        .flags          = 0,
    },
    [IWEVREGISTERED    - IWEVFIRST] = {
        .header_type    = IW_HEADER_TYPE_ADDR,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [IWEVEXPIRED    - IWEVFIRST] = {
        .header_type    = IW_HEADER_TYPE_ADDR,
        .token_type     = 0,
        .token_size     = 0,
        .min_tokens     = 0,
        .max_tokens     = 0,
        .flags          = 0,
    },
    [IWEVGENIE    - IWEVFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = IW_GENERIC_IE_MAX,
        .flags          = 0,
    },
    [IWEVMICHAELMICFAILURE    - IWEVFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = sizeof(struct iw_michaelmicfailure),
        .flags          = 0,
    },
    [IWEVASSOCREQIE    - IWEVFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = IW_GENERIC_IE_MAX,
        .flags          = 0,
    },
    [IWEVASSOCRESPIE    - IWEVFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = IW_GENERIC_IE_MAX,
        .flags          = 0,
    },
    [IWEVPMKIDCAND    - IWEVFIRST] = {
        .header_type    = IW_HEADER_TYPE_POINT,
        .token_type     = 0,
        .token_size     = 1,
        .min_tokens     = 0,
        .max_tokens     = sizeof(struct iw_pmkid_cand),
        .flags          = 0,
    },
};

const unsigned int standard_event_num = (sizeof(standard_event_descr) / sizeof(struct iw_ioctl_description));

// Size (in bytes) of various events
const int event_type_size[] = {
    IW_EV_LCP_PK_LEN,    // IW_HEADER_TYPE_NULL
    0,
    IW_EV_CHAR_PK_LEN,    // IW_HEADER_TYPE_CHAR
    0,
    IW_EV_UINT_PK_LEN,    // IW_HEADER_TYPE_UINT
    IW_EV_FREQ_PK_LEN,    // IW_HEADER_TYPE_FREQ
    IW_EV_ADDR_PK_LEN,    // IW_HEADER_TYPE_ADDR
    0,
    IW_EV_POINT_PK_LEN,    // Without variable payload
    IW_EV_PARAM_PK_LEN,    // IW_HEADER_TYPE_PARAM
    IW_EV_QUAL_PK_LEN,    // IW_HEADER_TYPE_QUAL *
};

///////////////////////
// event from stream //
///////////////////////
int wirelessExtractEventFromStream(iw_event_stream *stream, struct iw_event *iwe, int we_version)
{
    const struct iw_ioctl_description *descr = NULL;
    int event_type = 0;
    unsigned int event_len = 1;   
    char *pointer;
    unsigned cmd_index;

    // end of stream
    if((stream->current + IW_EV_LCP_PK_LEN) > stream->end)
        return 0;

    memcpy((char *) iwe, stream->current, IW_EV_LCP_PK_LEN);

    // invalid event
    if(iwe->len <= IW_EV_LCP_PK_LEN)
        return -1 ;

    //type and length of event 
    if(iwe->cmd <= SIOCIWLAST)
    {
        cmd_index = iwe->cmd - SIOCIWFIRST;
        if(cmd_index < standard_ioctl_num)
            descr = &(standard_ioctl_descr[cmd_index]);
    }
    else
    {
        cmd_index = iwe->cmd - IWEVFIRST;
        if(cmd_index < standard_event_num)
            descr = &(standard_event_descr[cmd_index]);
    }

    if(descr != NULL)
        event_type = descr->header_type;

    // unknown events -> event_type=0 => IW_EV_LCP_PK_LEN
    event_len = event_type_size[event_type];

    // fixup for earlier version of WE
    if((we_version <= 18) && (event_type == IW_HEADER_TYPE_POINT))
        event_len += IW_EV_POINT_OFF;

    // check if we know about this event
    if(event_len <= IW_EV_LCP_PK_LEN)
    {
        // next event
        stream->current += iwe->len;
        return 2;
    }
    event_len -= IW_EV_LCP_PK_LEN;

    // pointer on data
    if(stream->value != NULL)
        pointer = stream->value;
    else
        pointer = stream->current + IW_EV_LCP_PK_LEN;

    // copy the rest of the event
    if((pointer + event_len) > stream->end)
    {
        // next event
        stream->current += iwe->len;
        return -2;
    }

    // fixup for WE-19 and later: pointer no longer in the stream
    if((we_version > 18) && (event_type == IW_HEADER_TYPE_POINT))
        memcpy((char *)iwe + IW_EV_LCP_LEN + IW_EV_POINT_OFF, pointer, event_len);
    else
        memcpy((char *)iwe + IW_EV_LCP_LEN, pointer, event_len);

    // skip event in the stream
    pointer += event_len;

    // special processing for iw_point events
    if(event_type == IW_HEADER_TYPE_POINT)
    {
        // lenght of event
        unsigned int extra_len = iwe->len - (event_len + IW_EV_LCP_PK_LEN);
        if(extra_len > 0)
        {
            // set pointer on variable part
            iwe->u.data.pointer = pointer;

            // check that we have a descriptor for the command
            if(descr == NULL) 
                iwe->u.data.pointer = NULL;    
            else
            {
                // check other parts of event
                unsigned int token_len = iwe->u.data.length * descr->token_size;

                // ugly fixup for alignement issues (64 bits!!)
                if((token_len != extra_len) && (extra_len >= 4))
                {
                    unsigned short alt_dlen = *((unsigned short *) pointer);
                    unsigned int alt_token_len = alt_dlen * descr->token_size;

                    if((alt_token_len + 8) == extra_len)
                    {
                        // align
                        pointer -= event_len;
                        pointer += 4;

                        //local alignement
                        memcpy((char *) iwe + IW_EV_LCP_LEN + IW_EV_POINT_OFF, pointer, event_len);
                        pointer += event_len + 4;
                        iwe->u.data.pointer = pointer;
                        token_len = alt_token_len;
                    }
                }

                // more tokens than data...
                if(token_len > extra_len)
                    iwe->u.data.pointer = NULL;

                // buffer overflow
                if((iwe->u.data.length > descr->max_tokens) && !(descr->flags & IW_DESCR_FLAG_NOMAX))
                    iwe->u.data.pointer = NULL; 

                // buffer underflow
                if(iwe->u.data.length < descr->min_tokens)
                    iwe->u.data.pointer = NULL;
            }
        }
        else
            // no data
            iwe->u.data.pointer = NULL;

        // next event
        stream->current += iwe->len;
    }
    else
    {
        // ugly fixup for alignement issues (64 bits!!)
        if((stream->value == NULL) && ((((iwe->len - IW_EV_LCP_PK_LEN) % event_len) == 4) || ((iwe->len == 12) && 
                ((event_type == IW_HEADER_TYPE_UINT) || (event_type == IW_HEADER_TYPE_QUAL))) ))
        {
            // align
            pointer -= event_len;
            pointer += 4;
            memcpy((char *) iwe + IW_EV_LCP_LEN, pointer, event_len);
            pointer += event_len;
        }

        // more data in event
        if((pointer + event_len) <= (stream->current + iwe->len))
            stream->value = pointer;
        else
        {
            stream->value = NULL;
            stream->current += iwe->len;
        }
    }
    return 1;
}

//////////////////
// proces token //
//////////////////
struct wireless_scan_info* wirelessEventToList(struct iw_event *event, struct wireless_scan_info *wscan)
{
    struct wireless_scan_info *oldwscan;
    char dummy[IW_ESSID_MAX_SIZE] = {0};
    char *mac, mac_addr[18] = {0};

    // get event type
    switch(event->cmd)
    {
        case SIOCGIWAP:
            oldwscan = wscan;
            wscan = new struct wireless_scan_info;
        
            if(oldwscan != NULL)
	            oldwscan->next = wscan;
            
            // save AP MAC
            bzero(wscan, sizeof(struct wireless_scan_info));
            mac = event->u.ap_addr.sa_data;
            sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0] & 0xFF, mac[1] & 0xFF, mac[2] & 0xFF, mac[3] & 0xFF, mac[4] & 0xFF, mac[5] & 0xFF);
            wscan->accesPointMac = mac_addr;
            break;

        case SIOCGIWFREQ:
            wscan->freq = (float)event->u.freq.m / 1e9 * pow(10, event->u.freq.e);
            break;

        case SIOCGIWESSID:
            if((event->u.essid.pointer) && (event->u.essid.length))
            {
                strncpy(dummy, (char *)event->u.essid.pointer, strlen((char *)event->u.essid.pointer) - 1);
	            wscan->essid = dummy;
            }
            break;

        case IWEVQUAL:
            if(event->u.qual.updated & IW_QUAL_DBM)
            {
                if((char)event->u.qual.level < -110)
                    wscan->signalStrenght = 0;
                else if((char)event->u.qual.level > -40)
                    wscan->signalStrenght = 100;
                else
                    wscan->signalStrenght = round(((char)event->u.qual.level + 110.0) * 100 / 70);
            }
            else
                wscan->signalStrenght = -1;
            break;

        case SIOCGIWRATE:
            wscan->speed.insert(std::lower_bound(wscan->speed.cbegin(), wscan->speed.cend(), event->u.bitrate.value / 1e6), event->u.bitrate.value / 1e6);
            break;
            
        default:
            break;
    }
    return wscan;
}

////////////////////
// start scanning //
////////////////////
int wirelessScanStep(int fd, std::string ifname, int we_version, wireless_scan_struct *info, int time_iter, int initial_wait = 250, int step_wait = 100)
{
    struct iwreq req;
    unsigned char *buffer = NULL;		
    int buflen = IW_SCAN_MAX_DATA; 
    unsigned char *newbuf;

    // request
    memset(&req, 0, sizeof(req));
    strncpy(req.ifr_name, ifname.c_str(), ifname.length() + 1);

    // for time
    info->step++;
    if(info->step > time_iter)
        throw std::string("Scan timeout");

    // start scanning
    if(info->step == 1)
    {
        req.u.data.pointer = NULL;
        req.u.data.flags = 0;
        req.u.data.length = 0;
        
        if((ioctl(fd, SIOCSIWSCAN, &req) != -1) && (errno != EPERM))
	        return initial_wait;
        else
            throw std::string("Scan start error - SIOCSIWSCAN");
    }

realloc:
    buffer = (unsigned char *)realloc(buffer, buflen);

    // read the results
    req.u.data.pointer = buffer;
    req.u.data.flags = 0;
    req.u.data.length = buflen;
    if(ioctl(fd, SIOCGIWSCAN, &req) < 0)
    {
        // check if buffer was too small
        if((errno == E2BIG) && (we_version > 16))
	    {
	        if(req.u.data.length > buflen)
	            buflen = req.u.data.length;
	        else
	            buflen *= 2;

	        goto realloc; // bad
	    }

        // check if results not available yet
        if(errno == EAGAIN)
	    {
	        free(buffer);
	        return step_wait;
	    }
        
        // error
        free(buffer);
        throw std::string("Scan get results error - SIOCGIWSCAN");
    }

    // process results
    if(req.u.data.length)
    {
        struct iw_event iwe;
        struct iw_event_stream stream;
        struct wireless_scan_info *wscan = NULL;
        int	ret;

        // init event stream
        memset(&stream, 0, sizeof(stream));
        stream.current = (char *)buffer;
        stream.end = (char *)buffer + req.u.data.length;
        info->result = NULL;

        // every event token
        do{
	        // get event
	        ret = wirelessExtractEventFromStream(&stream, &iwe, we_version);
	        if(ret > 0)
	        {
	            // get data
	            wscan = wirelessEventToList(&iwe, wscan);
                
                // error
	            if(wscan == NULL)
		        {
		            free(buffer);
		            throw std::string("Unpacking event error - WSCAN");
		        }

	            // save list
	            if(info->result == NULL)
		            info->result = wscan;
	        }
	    } while(ret > 0);
    }

    free(buffer);
    return 0;
}
#endif // LINUX_WIFI_STREAM_H