/* AUTO-GENERATED FILE.  DO NOT MODIFY.
 *
 * This class was automatically generated by the
 * aapt tool from the resource data it found.  It
 * should not be modified by hand.
 */

package sk.boinc.androboinc;

public final class R {
    public static final class array {
        /**  States of task (active tasks only) 
 9 PROCESS_SUSPENDED (we've sent it a "suspend" message)        
         */
        public static final int activeTaskStates=0x7f060008;
        /**  Network modes for client (display) 
         */
        public static final int clientModeNetwork=0x7f060005;
        /**  Run modes for client (display) 
         */
        public static final int clientModeRun=0x7f060004;
        /**  Modes for client (values) - both run and network 
         */
        public static final int clientModeValues=0x7f060006;
        /**  Automatic updates of display 
         */
        public static final int prefAutoUpdatesDisplay=0x7f060002;
        public static final int prefAutoUpdatesValues=0x7f060003;
        /**  Screen rotation mode 
         */
        public static final int prefScreenOrientationDisplay=0x7f060000;
        /**  SCREEN_ORIENTATION_SENSOR      
         */
        public static final int prefScreenOrientationValues=0x7f060001;
        /**  States of task (all tasks) 
 6 RESULT_ABORTED           
         */
        public static final int resultStates=0x7f060007;
    }
    public static final class attr {
    }
    public static final class color {
        public static final int active=0x7f070000;
        public static final int nnw=0x7f070002;
        public static final int orange=0x7f070003;
        public static final int suspended=0x7f070001;
    }
    public static final class drawable {
        public static final int boinc=0x7f020000;
        public static final int ic_menu_goto=0x7f020001;
        public static final int ic_menu_refresh=0x7f020002;
        public static final int ic_tab_messages=0x7f020003;
        public static final int ic_tab_messages_dark=0x7f020004;
        public static final int ic_tab_messages_light=0x7f020005;
        public static final int ic_tab_projects=0x7f020006;
        public static final int ic_tab_projects_dark=0x7f020007;
        public static final int ic_tab_projects_light=0x7f020008;
        public static final int ic_tab_tasks=0x7f020009;
        public static final int ic_tab_tasks_dark=0x7f02000a;
        public static final int ic_tab_tasks_light=0x7f02000b;
        public static final int ic_tab_transfers=0x7f02000c;
        public static final int ic_tab_transfers_dark=0x7f02000d;
        public static final int ic_tab_transfers_light=0x7f02000e;
        public static final int progress_blue=0x7f02000f;
        public static final int progress_green=0x7f020010;
        public static final int progress_red=0x7f020011;
        public static final int progress_yellow=0x7f020012;
    }
    public static final class id {
        public static final int dialogText=0x7f0a0000;
        public static final int editHostAddress=0x7f0a0002;
        public static final int editHostCancel=0x7f0a0005;
        public static final int editHostNickname=0x7f0a0001;
        public static final int editHostOk=0x7f0a0006;
        public static final int editHostPassword=0x7f0a0004;
        public static final int editHostPort=0x7f0a0003;
        public static final int hostNoneText1=0x7f0a0007;
        public static final int hostNoneText2=0x7f0a0008;
        public static final int menuAbout=0x7f0a002a;
        public static final int menuAddHost=0x7f0a0029;
        public static final int menuConnect=0x7f0a002d;
        public static final int menuDisconnect=0x7f0a002e;
        public static final int menuHostInfo=0x7f0a002f;
        public static final int menuManage=0x7f0a002b;
        public static final int menuPreferences=0x7f0a002c;
        public static final int menuRefresh=0x7f0a0030;
        public static final int messageBody=0x7f0a000b;
        public static final int messageProject=0x7f0a000a;
        public static final int messageTimestamp=0x7f0a0009;
        public static final int optOutSetting=0x7f0a000c;
        public static final int projectDetails=0x7f0a0013;
        public static final int projectName=0x7f0a000d;
        public static final int projectShare=0x7f0a000e;
        public static final int projectShareActive=0x7f0a000f;
        public static final int projectShareNNW=0x7f0a0011;
        public static final int projectShareSuspended=0x7f0a0010;
        public static final int projectShareText=0x7f0a0012;
        public static final int taskAppName=0x7f0a0014;
        public static final int taskDeadline=0x7f0a0015;
        public static final int taskElapsed=0x7f0a001d;
        public static final int taskProgress=0x7f0a0017;
        public static final int taskProgressFinished=0x7f0a001b;
        public static final int taskProgressRunning=0x7f0a0018;
        public static final int taskProgressSuspended=0x7f0a0019;
        public static final int taskProgressText=0x7f0a001c;
        public static final int taskProgressWaiting=0x7f0a001a;
        public static final int taskProjectName=0x7f0a0016;
        public static final int taskRemaining=0x7f0a001e;
        public static final int transferElapsed=0x7f0a0027;
        public static final int transferFileName=0x7f0a001f;
        public static final int transferProgress=0x7f0a0022;
        public static final int transferProgressRunning=0x7f0a0023;
        public static final int transferProgressSuspended=0x7f0a0024;
        public static final int transferProgressText=0x7f0a0026;
        public static final int transferProgressWaiting=0x7f0a0025;
        public static final int transferProjectName=0x7f0a0021;
        public static final int transferSize=0x7f0a0020;
        public static final int transferSpeed=0x7f0a0028;
    }
    public static final class layout {
        public static final int dialog=0x7f030000;
        public static final int edit_host=0x7f030001;
        public static final int host_list=0x7f030002;
        public static final int main_view=0x7f030003;
        public static final int messages_list_item=0x7f030004;
        public static final int opt_out_dialog=0x7f030005;
        public static final int projects_list_item=0x7f030006;
        public static final int tasks_list_item=0x7f030007;
        public static final int transfers_list_item=0x7f030008;
    }
    public static final class menu {
        public static final int host_list_menu=0x7f090000;
        public static final int main_menu=0x7f090001;
        public static final int manage_client_menu=0x7f090002;
        public static final int refresh_menu=0x7f090003;
    }
    public static final class raw {
        public static final int changelog=0x7f050000;
        public static final int license=0x7f050001;
    }
    public static final class string {
        public static final int aboutHomepageUrl=0x7f080036;
        public static final int aboutText=0x7f080037;
        /**  About 
         */
        public static final int aboutTitle=0x7f080035;
        public static final int app_desc=0x7f080001;
        /**  Basic application stuff 
         */
        public static final int app_name=0x7f080000;
        /**  Some common, generic strings 
         */
        public static final int authorization=0x7f080003;
        public static final int cancel=0x7f080004;
        public static final int changelog=0x7f080005;
        public static final int clientActivityNetworkDialogTitle=0x7f08005e;
        public static final int clientActivityNetworkTitle=0x7f08005d;
        public static final int clientActivityRunDialogTitle=0x7f08005c;
        public static final int clientActivityRunTitle=0x7f08005b;
        public static final int clientConnecting=0x7f080054;
        public static final int clientDoNetCommNotify=0x7f080065;
        public static final int clientDoNetCommSummary=0x7f080064;
        public static final int clientDoNetCommTitle=0x7f080063;
        public static final int clientIdNone=0x7f080059;
        public static final int clientNameNone=0x7f080058;
        public static final int clientRunBenchNotify=0x7f080062;
        public static final int clientRunBenchSummary=0x7f080061;
        public static final int clientRunBenchTitle=0x7f080060;
        public static final int clientShutdownNotify=0x7f080068;
        public static final int clientShutdownSummary=0x7f080067;
        public static final int clientShutdownTitle=0x7f080066;
        public static final int close=0x7f080006;
        public static final int closeWarning=0x7f08002a;
        public static final int connecting=0x7f080007;
        /**  ManageClient Activity 
         */
        public static final int currentHostTitle=0x7f080052;
        public static final int disabled=0x7f080008;
        public static final int dismiss=0x7f080009;
        /**  TransfersActivity 
         */
        public static final int downloadFailed=0x7f08008d;
        public static final int downloadPending=0x7f08008f;
        public static final int downloading=0x7f08008e;
        public static final int editHostAddress=0x7f080074;
        /**  Host edit dialog 
         */
        public static final int editHostNickname=0x7f080073;
        public static final int editHostPassword=0x7f080076;
        public static final int editHostPort=0x7f080075;
        public static final int error=0x7f08000a;
        public static final int homepage=0x7f08000b;
        public static final int hostAddNew=0x7f08006b;
        public static final int hostDelete=0x7f08006d;
        public static final int hostEdit=0x7f08006c;
        /**  HostInfo display 
         */
        public static final int hostInfoPart1=0x7f08009a;
        public static final int hostInfoPart2=0x7f08009b;
        public static final int hostNone=0x7f080070;
        public static final int hostNoneSummary=0x7f080071;
        public static final int hostOperation=0x7f080072;
        public static final int hostSetDefault=0x7f08006e;
        public static final int license=0x7f08000c;
        public static final int manageCategoryActivity=0x7f08005a;
        public static final int manageCategoryAdvanced=0x7f08005f;
        /**  Menu items 
         */
        public static final int menuAbout=0x7f08002b;
        public static final int menuAddNewHost=0x7f08002c;
        public static final int menuConnect=0x7f08002d;
        public static final int menuDisconnect=0x7f08002e;
        public static final int menuHostInfo=0x7f080030;
        public static final int menuHosts=0x7f08002f;
        public static final int menuManage=0x7f080031;
        public static final int menuPreferences=0x7f080032;
        public static final int menuReconnect=0x7f080033;
        public static final int menuRefresh=0x7f080034;
        public static final int messages=0x7f080029;
        /**  Network status 
         */
        public static final int networkUnavailable=0x7f080051;
        public static final int noHostConnected=0x7f080053;
        public static final int noHostDefined=0x7f08006f;
        public static final int notConnected=0x7f08000d;
        public static final int notice=0x7f08000e;
        public static final int ok=0x7f08000f;
        public static final int prefAboutTitle=0x7f08004e;
        public static final int prefAutoConnectDisabled=0x7f08003e;
        public static final int prefAutoConnectEnabled=0x7f08003f;
        public static final int prefAutoConnectTitle=0x7f08003d;
        public static final int prefAutoUpdateIntervalLocalhost=0x7f080043;
        public static final int prefAutoUpdateIntervalMobile=0x7f080042;
        public static final int prefAutoUpdateIntervalSummary=0x7f080044;
        public static final int prefAutoUpdateIntervalWiFi=0x7f080041;
        public static final int prefCategoryAbout=0x7f08004d;
        public static final int prefCategoryNetworkBW=0x7f080040;
        /**  Preferences 
         */
        public static final int prefCategoryUI=0x7f080038;
        public static final int prefChangelogSummary=0x7f08004f;
        public static final int prefKeepScreenOn=0x7f08003a;
        public static final int prefKeepScreenOnDisabled=0x7f08003c;
        public static final int prefKeepScreenOnEnabled=0x7f08003b;
        public static final int prefLicenseSummary=0x7f080050;
        public static final int prefLimitMessagesDisabled=0x7f080046;
        public static final int prefLimitMessagesEnabled=0x7f080047;
        public static final int prefLimitMessagesTitle=0x7f080045;
        public static final int prefNetworkUsageDisabled=0x7f080049;
        public static final int prefNetworkUsageDisclaimer=0x7f08004b;
        public static final int prefNetworkUsageNull=0x7f08004a;
        public static final int prefNetworkUsageStats=0x7f080048;
        public static final int prefOptOutText=0x7f08004c;
        public static final int prefScreenOrientation=0x7f080039;
        public static final int projectANW=0x7f08007c;
        public static final int projectBackoff=0x7f080090;
        public static final int projectCredits=0x7f080081;
        /**  ProjectsActivity 
         */
        public static final int projectCtxMenuTitle=0x7f080077;
        public static final int projectDetailedInfo=0x7f080082;
        public static final int projectNNW=0x7f08007b;
        public static final int projectProperties=0x7f08007d;
        public static final int projectResume=0x7f08007a;
        public static final int projectStatusActive=0x7f08007e;
        public static final int projectStatusNNW=0x7f080080;
        public static final int projectStatusSuspended=0x7f08007f;
        public static final int projectSuspend=0x7f080079;
        public static final int projectSuspendedByUser=0x7f080010;
        public static final int projectUpdate=0x7f080078;
        public static final int projects=0x7f080026;
        public static final int received=0x7f080011;
        public static final int retrievingData=0x7f080012;
        public static final int retrievingInitialData=0x7f080013;
        public static final int retryIn=0x7f080091;
        /**  HostList Activity 
         */
        public static final int selectHost=0x7f08006a;
        public static final int selectHostDialogTitle=0x7f080057;
        public static final int selectHostNoneAvailable=0x7f080056;
        public static final int selectHostTitle=0x7f080055;
        public static final int sent=0x7f080014;
        public static final int service_desc=0x7f080002;
        public static final int shutdown=0x7f080015;
        public static final int sources=0x7f080016;
        public static final int taskAbort=0x7f080086;
        /**  TasksActivity 
         */
        public static final int taskCtxMenuTitle=0x7f080083;
        public static final int taskDetailedInfoCommon=0x7f080089;
        public static final int taskDetailedInfoEnd=0x7f08008c;
        public static final int taskDetailedInfoRes=0x7f08008b;
        public static final int taskDetailedInfoRun=0x7f08008a;
        public static final int taskProperties=0x7f080087;
        public static final int taskResume=0x7f080085;
        public static final int taskSuspend=0x7f080084;
        public static final int taskSuspendedByUser=0x7f080017;
        public static final int tasks=0x7f080027;
        /**  Main Activity 
         */
        public static final int temp_info=0x7f080025;
        public static final int transferAbort=0x7f080092;
        public static final int transferDetailedInfo=0x7f080099;
        public static final int transferProperties=0x7f080093;
        public static final int transferRetry=0x7f080094;
        public static final int transfers=0x7f080028;
        public static final int unitB=0x7f08001e;
        public static final int unitBps=0x7f080022;
        public static final int unitGB=0x7f080021;
        public static final int unitGiB=0x7f08001d;
        public static final int unitKB=0x7f08001f;
        public static final int unitKBps=0x7f080023;
        /**  Units 
         */
        public static final int unitKiB=0x7f08001b;
        public static final int unitMB=0x7f080020;
        public static final int unitMBps=0x7f080024;
        public static final int unitMiB=0x7f08001c;
        public static final int unknown=0x7f080018;
        public static final int upgradedTo=0x7f080019;
        public static final int uploadFailed=0x7f080095;
        public static final int uploadPending=0x7f080097;
        public static final int uploading=0x7f080096;
        public static final int warnAbortTask=0x7f080088;
        public static final int warnAbortTransfer=0x7f080098;
        public static final int warnShutdownText=0x7f080069;
        public static final int warning=0x7f08001a;
    }
    public static final class xml {
        public static final int manage_client=0x7f040000;
        public static final int preferences=0x7f040001;
    }
}
