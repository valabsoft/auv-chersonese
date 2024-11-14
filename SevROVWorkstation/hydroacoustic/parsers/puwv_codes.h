#ifndef PUWV_CODES_H
#define PUWV_CODES_H

enum PUWV_COMM{
    ACK = 0, SET_WRITE,
    RC_REQUEST, RC_RESPONSE, RC_TIMEOUT, RC_ASYNC_IN,
    AMB_DATA_CFG, AMB_DATA,
    SETTING_READ,
    D2H_PT_SETTINGS,
    H2D_PT_SETTINGS_WRITE, H2H_PT_SETTINGS_WRITE,
    PT_SEND, PT_FAILED, PT_DELIVERED, PT_RECIEVED,
    PT_ITG, PT_TMO, PT_ITG_RESP,
    DINFO_GET, DINFO,
};

#define NMEA_SNT_STR              '$'
#define NMEA_SNT_END              '\n'
#define NMEA_SNT_END1             '\r'
#define NMEA_PAR_SEP              ","
#define NMEA_CHK_SEP              '*'

#define IC_D2H_ACK                '0'        // $PUWV0,cmdID,errCode
#define IC_H2D_SETTINGS_WRITE     '1'        // $PUWV1,rxChID,txChID,styPSU,isCmdMode,isACKOnTXFinished,gravityAcc
#define IC_H2D_RC_REQUEST         '2'        // $PUWV2,txChID,rxChID,rcCmdID
#define IC_D2H_RC_RESPONSE        '3'        // $PUWV3,txChID,rcCmdID,propTime_seс,snr,[value],[azimuth]
#define IC_D2H_RC_TIMEOUT         '4'        // $PUWV4,txChID,rcCmdID
#define IC_D2H_RC_ASYNC_IN        '5'        // $PUWV5,rcCmdID,snr,[azimuth]
#define IC_H2D_AMB_DTA_CFG        '6'        // $PUWV6,isSaveInFlash,periodMs,isPrs,isTemp,isDpt,isBatV
#define IC_D2H_AMB_DTA            '7'        // $PUWV7,prs_mBar,temp_C,dpt_m,batVoltage_V

// packet mode
#define IC_H2D_PT_SETTINGS_READ   'D'        // $PUWVD,reserved
#define IC_D2H_PT_SETTINGS        'E'        // $PUWVE,isPTMode,ptAddress
#define IC_H2H_PT_SETTINGS_WRITE  'F'        // $PUWVF,isSaveInFlash,isPTMode,ptAddress

#define IC_H2D_PT_SEND            'G'        // $PUWVG,tareget_ptAddress,[maxTries],dataPacket
#define IC_D2H_PT_FAILED          'H'        // $PUWVH,tareget_ptAddress,triesTaken,dataPacket
#define IC_D2H_PT_DLVRD           'I'        // $PUWVI,tareget_ptAddress,triesTaken,dataPacket
#define IC_D2H_PT_RCVD            'J'        // $PUWVJ,sender_ptAddress,dataPacket

#define IC_H2D_PT_ITG             'K'        // $PUWVK,target_ptAddress,pt_itg_dataID
#define IC_D2H_PT_TMO             'L'        // $PUWVL,target_ptAddress,pt_itg_dataID
#define IC_D2H_PT_ITG_RESP        'M'        // $PUWVM,target_ptAddress,pt_itg_dataID,[dataValue],pTime,[azimuth]

#define IC_H2D_DINFO_GET          '?'        // $PUWV?,reserved
#define IC_D2H_DINFO              '!'        // $PUWV!,serialNumber,sys_moniker,sys_version,core_moniker [release],core_version,acBaudrate,rxChID,txChID,isCmdMode

#define IC_D2H_UNKNOWN            '-'

enum PUWV_ERR{
    LOC_ERR_NO_ERROR,
    LOC_ERR_INVALID_SYNTAX,
    LOC_ERR_UNSUPPORTED,
    LOC_ERR_TRANSMITTER_BUSY,
    LOC_ERR_ARGUMENT_OUT_OF_RANGE,
    LOC_ERR_INVALID_OPERATION,
    LOC_ERR_UNKNOWN_FIELD_ID,
    LOC_ERR_VALUE_UNAVAILIBLE,
    LOC_ERR_RECEIVER_BUSY,
    LOC_ERR_TX_BUFFER_OVERRUN,
    LOC_ERR_CHKSUM_ERROR,
    LOC_ACK_TX_FINISHED,
    LOC_ACK_BEFORE_STANDBY,
    LOC_ACK_AFTER_WAKEUP,
    LOC_ERR_SVOLTAGE_TOO_HIGH
};

enum PUWV_RC{
    RC_PING=0,
    RC_PONG,
    RC_DPT_GET,
    RC_TMP_GET,
    RC_BAT_V_GET,
    RC_ERR_NSUP,
    RC_ACK,
    RC_USR_CMD_000,
    RC_USR_CMD_001,
    RC_USR_CMD_002,
    RC_USR_CMD_003,
    RC_USR_CMD_004,
    RC_USR_CMD_005,
    RC_USR_CMD_006,
    RC_USR_CMD_007,
    RC_USR_CMD_008,
    RC_MSG_ASYNC_IN,
};


#endif // PUWV_CODES_H
