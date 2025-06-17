#pragma once

#ifdef LANG_CN
const static char *ConfigID[] = {
	"��������",
	"����ʱ��",
	"����ʱ",
	"�ֳ���ʾ",
	"�ֳ�ͨ��",
	"�����",
	"�������",
	"��Ƶ",
	"ͨ��",
	"�ַ�����",
	"�ƻ�����",
	"��������",
	"�������",
	"¼������",
	"¼��ƻ�",
	"�ƶ��ƻ�",
	"�������ƻ�",
	"�������",
	"����߼�",
	"DDNS",
	"����",
	"�û�",
	"����������",
	"��������������",
	"�ƶ���������",
	"�ƶ���������",
	"��Ƶ��ʧ��������",
	"��̨����",
	"������",
	"��Ƶ��ɫ"
};
#else
const static char *ConfigID[] = {
	"Basic Configuration",
	"Date and Time",
	"DST",
	"Current Display",
	"Current Channel",
	"Main Output",
	"Second Output",
	"Audio",
	"Channel",
	"OSD",
	"Planned Encoding",
	"Alarm Encoding",
	"Network Encoding",
	"Record Configuration",
	"Record Schedule",
	"Motion Schedule",
	"Sensor Schedule",
	"Basic Network",
	"Advanced Network",
	"DDNS",
	"E-mail",
	"User",
	"Sensor Configuration",
	"Sensor trigger alarm",
	"Motion Alarm Configuration",
	"Motion trigger alarm",
	"Video loss trigger alarm",
	"PTZ Configuration",
	"Buzzer",
	"Video color"
};
#endif

#ifdef LANG_CN
const static char *ConfigChoice[] = {
	"NTSC|PAL|",
	"|������|������|������",
	"GMT -12|GMT -11|GMT -10|GMT -9|GMT -8|GMT -7|GMT -6|GMT -5|GMT -4:30|GMT -4|GMT -3:30|GMT -3|GMT -2|GMT -1|GMT 0|GMT +1|GMT +2|GMT +3|GMT +3:30|GMT +4|GMT +4:30|GMT +5|GMT +5:30|GMT +5:45|GMT +6|GMT +6:30|GMT +7|GMT +8|GMT +9|GMT +9:30|GMT +10|GMT +11|GMT +12|",
	"��|����",
	"һ��|����|����|����|����|����|����|����|����|ʮ��|ʮһ��|ʮ����",
	"����|��һ|�ܶ�|����|����|����|����",
	"��|����",
	"640x480|720x480|720x576|800x600|1024x768|1280x960|1280x1024|1920x1080",
	"|������|�̶�����|",
	//"QCIF|CIF|HD1|D1|QVGA|VGA|XVGA|QQVGA",
	
	"640x480|720x480|720x576|800x600|1024x768|1280x960|1280x1024|1920x1080|320x240|352x240|480x240|704x480|704x576|960x480|960x576|960x1080|1280x720|1600x1200|1920x1536|2048x1536|2304x1296|2592x1520|2560x1440|2592x1944|3840x2160|352x288",
	"|���|�ϵ�|��|�е�|�ϸ�|���",
	"|Ӣ��|��������|��������|��������|ϣ����|��������|����|Ų����|��������|�������|�ݿ���|����|ϣ������|����|����|������|����������|ӡ����������|����D|̩��|��������|��������",
	"|12Сʱ��|24Сʱ��",
	"1x1|2x2|2x3|3x3|4x4|5x5|6x6",
	"|88IP|DNS2P|MEIBU|DYNDNS|NOIPDNS|DVRDYDNS|MINTDNS|MYQSEE|DVRLIS|EASTERNDNS|NEWDDNS",
	"����|����",
	"NULL|PELCOP|PELCOD|LILIN|MINKING|NEON|STAR|VIDO|DSCP|VISCA|SAMSUNG|RM110|HY",
	"|����Ա|�߼��û�|��ͨ�û�",
	"|h264|h265|mjpeg|h264+|h265+",
};
#else
const static char *ConfigChoice[] = {
	"NTSC|PAL|",
	"|Year-Month-Day|Month-Day-Year|Day-Month-Year",
	"GMT -12|GMT -11|GMT -10|GMT -9|GMT -8|GMT -7|GMT -6|GMT -5|GMT -4:30|GMT -4|GMT -3:30|GMT -3|GMT -2|GMT -1|GMT 0|GMT +1|GMT +2|GMT +3|GMT +3:30|GMT +4|GMT +4:30|GMT +5|GMT +5:30|GMT +5:45|GMT +6|GMT +6:30|GMT +7|GMT +8|GMT +9|GMT +9:30|GMT +10|GMT +11|GMT +12|",
	"Week|Date",
	"January|February|March|April|May|June|July|August|September|October|November|December",
	"Sunday|Monday|Tuesday|Wednesday|Thursday|Friday|Saturday",
	"Week|Date",
	"640x480|720x480|720x576|800x600|1024x768|1280x960|1280x1024|1920x1080",
	"|Variable Bit Rate|Constant Bit Rate|",
	//"QCIF|CIF|HD1|D1|QVGA|VGA|XVGA|QQVGA",

	"640x480|720x480|720x576|800x600|1024x768|1280x960|1280x1024|1920x1080|320x240|352x240|480x240|704x480|704x576|960x480|960x576|960x1080|1280x720|1600x1200|1920x1536|2048x1536|2304x1296|2592x1520|2560x1440|2592x1944|3840x2160|352x288",
	"|lowest|lower|low|medium|higher|highest",
	"|English|Simplified Chinese|Traditional Chinese|Portuguese|Greek|Spanish|Russian|Norwegian|Turkish|Italian|Czech|German|Hebrew|Japanese|French|Polish|Bulgarian|Indonesian|Russian D|Thai|Hun|Lithuanian",
	"|12-hour clock|24-hour clock",
	"1x1|2x2|2x3|3x3|4x4|5x5|6x6",
	"|88IP|DNS2P|MEIBU|DYNDNS|NOIPDNS|DVRDYDNS|MINTDNS|MYQSEE|DVRLIS|EASTERNDNS|NEWDDNS",
	"Always Open|Always Close",
	"NULL|PELCOP|PELCOD|LILIN|MINKING|NEON|STAR|VIDO|DSCP|VISCA|SAMSUNG|RM110|HY",
	"|Administrator|Advanced user|Normal user",
	"|h264|h265|mjpeg|h264+|h265+",
};
#endif

#ifdef LANG_CN
#define SYSBASIC_HEADER _T("�豸����|�豸���|��Ƶ��ʽ|�ֱ���|ˢ����|��Ļ����|ϵͳ����|�������|������")
#define DATETIME_HEADER _T("���ڸ�ʽ|ʱ���ʽ|ʱ��|ntpͬ��|ntp�˿�|ntp��ַ")
#define DST_WEEK_HEADER _T("����|ƫ��|����|��-��|��-��|��-����|��-��|��-��|��-��|��-����|��-��")
#define DST_DATE_HEADER _T("����|ƫ��|����|��-��|��-��|��-��|��-��|��-��|��-��")
#define LIVE_HEADER		_T("��ʾϵͳʱ��|��ʾ����״̬|��ʾӲ����Ϣ|��ʾUSB��Ϣ|��ʾ��������|��ʾ�������")
#define LIVE_CHNN_HEADER _T("��ʾͨ������|��ʾ¼��״̬")
#define SPLIT_HEADER_1x1 _T("����ģʽ|��̨ʱ��|λ��1")
#define SPLIT_HEADER_2x2 _T("����ģʽ|��̨ʱ��|λ��1|λ��2|λ��3|λ��4")
#define SPLIT_HEADER_2x3 _T("����ģʽ|��̨ʱ��|λ��1|λ��2|λ��3|λ��4|λ��5|λ��6")
#define SPLIT_HEADER_3x3 _T("����ģʽ|��̨ʱ��|λ��1|λ��2|λ��3|λ��4|λ��5|λ��6|λ��7|λ��8|λ��9")
#define SPLIT_HEADER_4x4 _T("����ģʽ|��̨ʱ��|λ��1|λ��2|λ��3|λ��4|λ��5|λ��6|λ��7|λ��8|λ��9|λ��10|λ��11|λ��12|λ��13|λ��14|λ��15|λ��16")
#define SPLIT_HEADER_5x5 _T("����ģʽ|��̨ʱ��|λ��1|λ��2|λ��3|λ��4|λ��5|λ��6|λ��7|λ��8|λ��9|λ��10|λ��11|λ��12|λ��13|λ��14|λ��15|λ��16|λ��17|λ��18|λ��19|λ��20|λ��21|λ��22|λ��23|λ��24|λ��25")
#define SPLIT_HEADER_6x6 _T("����ģʽ|��̨ʱ��|λ��1|λ��2|λ��3|λ��4|λ��5|λ��6|λ��7|λ��8|λ��9|λ��10|λ��11|λ��12|λ��13|λ��14|λ��15|λ��16|λ��17|λ��18|λ��19|λ��20|λ��21|λ��22|λ��23|λ��24|λ��25|λ��26|λ��27|λ��28|λ��29|λ��30|λ��31|λ��32|λ��33|λ��34|λ��35|λ��36")
#define LIVE_AUDIO_HEADER _T("ͣ��ʱ��|����|ͨ�����")
#define CHNN_HEADER		 _T("��ʾͨ��|ͨ������")
#define OSD_HEADER		 _T("ͨ������|ʱ���|ʱ���������|�Զ����ı�|ͨ������λ��|ʱ���λ��|�Զ����ı�λ��|�Զ����ı�")
#define ENCODE_HEADER		 _T("�ֱ���|֡��|����|����|�������|��С����|����|�����ʽ|GOP")
#define COLOR_HEADER		 _T("����|�Աȶ�|ɫ��|���Ͷ�")
#define RECORD_HEADER		 _T("¼����Ƶ|¼����Ƶ|��Ӧ����Ƶͨ��|����¼��|��ǰʱ��|����ʱ��|����ʱ��")
#define NET_BASIC_HEADER     _T("��̬�����ַ|�����ַ|��������|����|������������|������������|ʹ��PPPoE|�ʺ�|����")
#define NET_ADVANCE_HEADER     _T("HTTP�˿�|���ݶ˿�|��Ϣ����˿�|�����˿�|�����û���Ŀ|�ಥ��ַ|MTU�ֽ���")
#define DDNS_HEADER     _T("����DDNS|�ϱ�����|����������|�Ƿ���������|DDNS�ʺ�|DDNS����|��������")
#define SMTP_HEADER     _T("�˿�|����SSL|������|���͵�ַ|����|���յ�ַ1|���յ�ַ2|���յ�ַ3")
#define MOTION_SETUP_HEADER     _T("����|�ӳ�ʱ��|������|����������|����������")
#define ALARM_TRIGGER_HEADER     _T("������|����|�ʼ�|�ϴ���������|�������")
#define SENSOR_SETUP_HEADER     _T("����|�豸����|�ӳ�ʱ��|�豸����")
#define PTZ_SETUP_HEADER     _T("����|��ַ|Э��|������|����λ|ֹͣλ|��żУ��λ|����������")
#define BUZZER_HEADER     _T("����|�ӳ�ʱ��")
#define USER_HEADER     _T("����|��mac|����|mac|�û���|����|��־����|ϵͳ����|�ļ�����|���̹���|Զ�̵���|�����Խ�|ϵͳά��|�û�����|�ػ�����|�������|���籨��|���紮��")
#else
#define SYSBASIC_HEADER _T("Device Name|Device ID|Vedio Format|Resolution|Refresh Rate|Screen Protector|System Language|Check Password|Allow Override")
#define DATETIME_HEADER  _T("Date Format|Time Format|Time Zone|NTP Sync|NTP Port|NTP Address")
#define DST_WEEK_HEADER _T("Start|Offset|Type|In-Month|In-Week|In-Day|In-Second|Out-Month|Out-Week|Out-Day|Out-Second")
#define DST_DATE_HEADER _T("Start|Offset|Type|In-Month|In-Day|In-Second|Out-Month|Out-Day|Out-Second")
#define LIVE_HEADER		_T("Display System Time|Display Net Status|Display Disk Info|Display USB Info|Display Alarm Input|Display Alarm Output")
#define LIVE_CHNN_HEADER _T("Display Channel Name|Display Record Status")
#define SPLIT_HEADER_1x1 _T("Split Mode|Platform Time|Position1")
#define SPLIT_HEADER_2x2 _T("Split Mode|Platform Time|Position1|Position2|Position3|Position4")
#define SPLIT_HEADER_2x3 _T("Split Mode|Platform Time|Position1|Position2|Position3|Position4|Position5|Position6")
#define SPLIT_HEADER_3x3 _T("Split Mode|Platform Time|Position1|Position2|Position3|Position4|Position5|Position6|Position7|Position8|Position9")
#define SPLIT_HEADER_4x4 _T("Split Mode|Platform Time|Position1|Position2|Position3|Position4|Position5|Position6|Position7|Position8|Position9|Position10|Position11|Position12|Position13|Position14|Position15|Position16")
#define SPLIT_HEADER_5x5 _T("Split Mode|Platform Time|Position1|Position2|Position3|Position4|Position5|Position6|Position7|Position8|Position9|Position10|Position11|Position12|Position13|Position14|Position15|Position16|Position17|Position18|Position19|Position20|Position21|Position22|Position23|Position24|Position25")
#define SPLIT_HEADER_6x6 _T("Split Mode|Platform Time|Position1|Position2|Position3|Position4|Position5|Position6|Position7|Position8|Position9|Position10|Position11|Position12|Position13|Position14|Position15|Position16|Position17|Position18|Position19|Position20|Position21|Position22|Position23|Position24|Position25|Position26|Position27|Position28|Position29|Position30|Position31|Position32|Position33|Position34|Position35|Position36")
#define LIVE_AUDIO_HEADER _T("Residence Time|Volumn|Channel ID")
#define CHNN_HEADER		 _T("Display Channel|Channel Name")
#define OSD_HEADER		 _T("Channel Name|Time Stamp|Time Stamp and Week|Custom Text|Channel Name Position|Time Stamp Position|Custom Text Position|Custom Text")
#define ENCODE_HEADER		 _T("Resolution|Frame Rate|Coding|Image Quality|Max Rate|Min Rate|Rate|Encode Model|GOP")
#define COLOR_HEADER		 _T("Brightness|Contrast|Chroma|Saturation")
#define RECORD_HEADER		 _T("Record Video|Record Audio|Corresponding Audio Channel|Redundant Record|Time Before Alarm|Time After Alarm|Expiration Time")
#define NET_BASIC_HEADER     _T("Dynamic network Address|Network Address|Subnet Mask|Gateway|Main Domain Server|Main Domain Server|Use PPPoE|Account|Password")
#define NET_ADVANCE_HEADER     _T("HTTP Port|Data Port|Message Command Port|Alarm Port|Online User Number|Multicast Address|MTU Bytes")
#define DDNS_HEADER     _T("Enable DDNS|Report Period|Server Type|Is Host Name|DDNS Account|DDNS Password|Host Name")
#define SMTP_HEADER     _T("Port|Enable SSL|Server|Send Address|Password|Receive Address1|Receive Address2|Receive Address3")
#define MOTION_SETUP_HEADER     _T("Enable|Delay Time|Sensitivity|Transverse Grid Number|Longitudinal Grid Number")
#define ALARM_TRIGGER_HEADER     _T("Buzzer|Big Picture|E-mail|Report Alarm Center|Alarm Output")
#define SENSOR_SETUP_HEADER     _T("Enable|Device Type|Delay Time|Device Name")
#define PTZ_SETUP_HEADER     _T("Enable|Address|Protocol|Baud Rate|Data Bit|Stop Bit|Parity Bit|Data Flow Control")
#define BUZZER_HEADER     _T("Enable|Delay Time")
#define USER_HEADER     _T("Enable|Bind mac|Belong|mac|User|Password|LoG Search|System Config|File Manage|Disk Manage|Remote Login|Voice Com|System Maintenance|User Manage|Reboot|Alarm Output|Network Alarm|Network Serial Port")
#endif


static char *INDEX_CSS = "\
body {\
font: normal 15px auto 'Trebuchet MS', Verdana, Arial, Helvetica, sans-serif;\
color: #4f6b72;\
background: #E6EAE9;\
}\
a {\
color: #c75f3e;\
}\
";

static char *CONFIG_CSS = "\
body {\
font: normal 11px auto 'Trebuchet MS', Verdana, Arial, Helvetica, sans-serif;\
color: #4f6b72;\
background: #E6EAE9;\
}\
a {\
color: #c75f3e;\
}\
table {\
width: 800px;\
padding: 0;\
margin: 0;\
}\
caption {\
padding: 0 0 5px 0;\
width: 700px;\
font: italic 11px 'Trebuchet MS', Verdana, Arial, Helvetica, sans-serif;\
text-align: right;\
}\
th {\
font: bold 11px 'Trebuchet MS', Verdana, Arial, Helvetica, sans-serif;\
color: #4f6b72;\
border-right: 1px solid #C1DAD7;\
border-bottom: 1px solid #C1DAD7;\
border-top: 1px solid #C1DAD7;\
letter-spacing: 2px;\
text-transform: uppercase;\
text-align: left;\
padding: 6px 6px 6px 12px;\
background: #CAE8EA no-repeat;\
}\
th.nobg {\
border-top: 0;\
border-left: 0;\
border-right: 1px solid #C1DAD7;\
background: none;\
}\
td {\
border-right: 1px solid #C1DAD7;\
border-bottom: 1px solid #C1DAD7;\
background: #fff;\
font-size:11px;\
padding: 6px 6px 6px 12px;\
color: #4f6b72;\
}\
td.alt {\
background: #F5FAFA;\
color: #797268;\
}\
th.spec {\
border-left: 1px solid #C1DAD7;\
border-top: 0;\
background: #fff no-repeat;\
font: bold 10px 'Trebuchet MS', Verdana, Arial, Helvetica, sans-serif;\
}\
th.specalt {\
border-left: 1px solid #C1DAD7;\
border-top: 0;\
background: #f5fafa no-repeat;\
font: bold 10px 'Trebuchet MS', Verdana, Arial, Helvetica, sans-serif;\
color: #797268;\
}\
html>body td{ font-size:11px;}\
body,td,th {\
font-family: ����, Arial;\
font-size: 12px;\
}";