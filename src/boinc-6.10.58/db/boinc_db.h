// This file is part of BOINC.
// http://boinc.berkeley.edu
// Copyright (C) 2008 University of California
//
// BOINC is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// BOINC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with BOINC.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _BOINC_DB_
#define _BOINC_DB_

// Structures corresponding to database records.
// Some of these types have counterparts in client/types.h,
// but don't be deceived - client and server have different variants.

// The parse and write functions are for use in scheduler RPC.
// They don't necessarily serialize the entire records.

#include <cstdio>
#include <vector>

#include "db_base.h"

extern DB_CONN boinc_db;

// Sizes of text buffers in memory, corresponding to database BLOBs.
// The following is for regular blobs, 64KB

#define BLOB_SIZE   65536

// The following are for "medium blobs",
// which are 16MB in the DB
//
#define APP_VERSION_XML_BLOB_SIZE   262144
#define MSG_FROM_HOST_BLOB_SIZE     262144
#define MSG_TO_HOST_BLOB_SIZE       262144

// Dummy name for file xfers
#define FILE_MOVER "move_file"

struct BEST_APP_VERSION;

// A compilation target, i.e. a architecture/OS combination.
// The core client will be given only applications with the same platform
//
struct PLATFORM {
    int id;
    int create_time;
    char name[256];                 // i.e. "sparc-sun-solaris"
    char user_friendly_name[256];   // i.e. "SPARC Solaris 2.8"
    int deprecated;
    void clear();
};

// An application.
//
struct APP {
    int id;
    int create_time;
    char name[256];         // application name, preferably short
    int min_version;        // don't use app versions before this
    bool deprecated;
    char user_friendly_name[256];
    int homogeneous_redundancy;
    double weight;          // tells the feeder what fraction of results
                            // should come from this app
    bool beta;
    int target_nresults;

    int write(FILE*);
    void clear();
};

// A version of an application.
//
struct APP_VERSION {
    int id;
    int create_time;
    int appid;
    int version_num;
    int platformid;
    char xml_doc[APP_VERSION_XML_BLOB_SIZE];
    // describes app files. format:
    // <file_info>...</file_info>
    // ...
    // <app_version>
    //     <app_name>...</app_name>
    //     <version_num>x</version_num>
    //     <api_version>n.n.n</api_version>
    //     <file_ref>
    //        ...
    //        [<main_program/>]
    //        [<copy_file/>]
    //     </file_ref>
    // </app_version>
    //

    // the following let you handle backwards-incompatible changes to
    // the core client / app interface
    //
    int min_core_version;   // min core version this app will run with
    int max_core_version;   // if <>0, max core version this will run with
    bool deprecated;
    char plan_class[256];

    // the following used by scheduler, not in DB
    //
    BEST_APP_VERSION* bavp;

    int write(FILE*);
    void clear();
};

struct USER {
    int id;
    int create_time;
    char email_addr[256];
    char name[256];
    char authenticator[256];
    char country[256];
    char postal_code[256];
    double total_credit;
    double expavg_credit;           // credit per second, recent average
    double expavg_time;             // when the above was computed
    char global_prefs[BLOB_SIZE];
        // global preferences, within <global_preferences> tag
    char project_prefs[BLOB_SIZE];
        // project preferences; format:
        // <project_preferences>
        //    <resource_share>X</resource_share>
        //    <project_specific>
        //        ...
        //    </project_specific>
        //    <venue name="x">
        //       <resource_share>x</resource_share>
        //       <project_specific>
        //           ...
        //       </project_specific>
        //    </venue>
        //    ...
        // </project_preferences>
    int teamid;                     // team ID if any
    char venue[256];                // home/work/school (default)
    char url[256];                  // user's web page if any
    bool send_email;
    bool show_hosts;
    int posts;                      // number of messages posted (redundant)
        // deprecated as of 9/2004 - forum_preferences.posts is used instead
        // may be used as a temp var
        // WARNING: it's a shortint (16 bits) in the DB

    // The following are specific to SETI@home;
    // they record info about the user's involvement in a prior project
    int seti_id;                    // ID in old DB
    int seti_nresults;              // number of WUs completed
    int seti_last_result_time;      // time of last result (UNIX)
    double seti_total_cpu;          // number of CPU seconds
    char signature[256];
        // deprecated as of 9/2004 - may be used as temp
    bool has_profile;
    char cross_project_id[256];
    char passwd_hash[256];
    bool email_validated;           // deprecated
    int donated;
    void clear();
};

#define TEAM_TYPE_CLUB                  1
#define TEAM_TYPE_COMPANY               2
#define TEAM_TYPE_PRIMARY               3
#define TEAM_TYPE_SECONDARY             4
#define TEAM_TYPE_JUNIOR_COLLEGE        5
#define TEAM_TYPE_UNIVERSITY            6
#define TEAM_TYPE_GOVERNMENT            7

// invariants of teams:
// a team has > 0 members

struct TEAM {
    int id;
    int create_time;
    int userid;             // User ID of team founder
    char name[256];
    char name_lc[256];      // Team name in lowercase (used for searching)
    char url[256];
    int type;               // Team type (see above)
    char name_html[256];
    char description[BLOB_SIZE];
    int nusers;             // UNDEFINED BY DEFAULT
    char country[256];
    double total_credit;
    double expavg_credit;
    double expavg_time;

    int seti_id;            // ID in another DB
        // this is used to identify BOINC-wide teams
    int ping_user;          // user who asked to become founder
    int ping_time;          // when they asked.
        // see html/inc/team.inc for more details

    void clear();
};

struct HOST {
    int id;
    int create_time;
    int userid;             // ID of user running this host
        // If the host is "zombied" during merging of duplicate hosts,
        // this field is set to zero and rpc_seqno is used to
        // store the ID of the new host (kludge, but what the heck)
    int rpc_seqno;          // last seqno received from client
        // also used as a "forwarding ID" for zombied hosts (see above)
    int rpc_time;           // time of last scheduler RPC
    double total_credit;
    double expavg_credit;   // credit per second, recent average
    double expavg_time;     // last time the above was updated

    // all remaining items are assigned by the client
    int timezone;           // local STANDARD time at host - UTC time
                            // (in seconds) 
    char domain_name[256];
    char serialnum[256];    // textual description of coprocessors
    char last_ip_addr[256]; // internal IP address as of last RPC
    int nsame_ip_addr;      // # of RPCs with same IP address

    double on_frac;         // see client/time_stats.h
    double connected_frac;
    double active_frac;
    double cpu_efficiency;  // deprecated as of 6.4 client
    double duration_correction_factor;

    int p_ncpus;            // Number of CPUs on host
    char p_vendor[256];     // Vendor name of CPU
    char p_model[256];      // Model of CPU
    double p_fpops;         // measured floating point ops/sec of CPU
    double p_iops;          // measured integer ops/sec of CPU
    double p_membw;         // measured memory bandwidth (bytes/sec) of CPU
                            // The above are per CPU, not total

    char os_name[256];      // Name of operating system
    char os_version[256];   // Version of operating system

    double m_nbytes;        // Size of memory in bytes
    double m_cache;         // Size of CPU cache in bytes (L1 or L2?)
    double m_swap;          // Size of swap space in bytes

    double d_total;         // Total disk space on volume containing
                            // the BOINC client directory.
    double d_free;          // how much is free on that volume

    // the following 2 items are reported in scheduler RPCs
    // from clients w/ source Oct 4 2005 and later.
    // NOTE: these items plus d_total and d_free are sufficient
    // to avoid exceeding BOINC's limit on total disk space.
    // But they are NOT sufficient to do resource-share-based
    // disk space allocation.
    // This needs to thought about.
    //
    double d_boinc_used_total;
                            // disk space being used in BOINC client dir,
                            // including all projects and BOINC itself
    double d_boinc_used_project;
                            // amount being used for this project

    // The following item is not used.
    // It's redundant (server can compute based on other params and prefs)
    //
    double d_boinc_max;     // max disk space that BOINC is allowed to use,
                            // reflecting user preferences
    double n_bwup;          // Average upload bandwidth, bytes/sec
    double n_bwdown;        // Average download bandwidth, bytes/sec
                            // The above are derived from actual
                            // file upload/download times, and may reflect
                            // factors other than network bandwidth

    // The following is derived (by server) from other fields
    double credit_per_cpu_sec;

    char venue[256];        // home/work/school
    int nresults_today;     // results sent since midnight
    double avg_turnaround;  // recent average result turnaround time
    char host_cpid[256];    // host cross-project ID
    char external_ip_addr[256]; // IP address seen by scheduler
    int max_results_day;
        // MRD is dynamically adjusted to limit work sent to bad hosts.
        // The maximum # of results sent per day is
        // max_results_day * (NCPUS + NCUDA * cuda_multiplier).
        // 0 means uninitialized; set to config.daily_result_quota
        // -1 means this host is blacklisted - don't return results
        // or accept results or trickles; just send it an error message
        // Otherwise it lies in the range 0 .. config.daily_result_quota
    double error_rate;      // dynamic estimate of fraction of results
                            // that fail validation

    // the following not stored in DB
    //
    double claimed_credit_per_cpu_sec;
    char p_features[256];

    int parse(FILE*);
    int parse_time_stats(FILE*);
    int parse_net_stats(FILE*);
    int parse_disk_usage(FILE*);
    void fix_nans();
    void clear();
};

// values for file_delete state
#define FILE_DELETE_INIT        0
#define FILE_DELETE_READY       1
    // set to this value only when we believe all files are uploaded
#define FILE_DELETE_DONE        2
    // means the files were successfully deleted
#define FILE_DELETE_ERROR       3
    // Any error was returned while attempting to delete the file

// values for assimilate_state
#define ASSIMILATE_INIT         0
#define ASSIMILATE_READY        1
#define ASSIMILATE_DONE         2

// NOTE: there is no overall state for a WU (like done/not done)
// There's just a bunch of independent substates
// (file delete, assimilate, and states of results, error flags)

// bit fields of error_mask
#define WU_ERROR_COULDNT_SEND_RESULT            1
#define WU_ERROR_TOO_MANY_ERROR_RESULTS         2
#define WU_ERROR_TOO_MANY_SUCCESS_RESULTS       4
#define WU_ERROR_TOO_MANY_TOTAL_RESULTS         8
#define WU_ERROR_CANCELLED                      16
#define WU_ERROR_NO_CANONICAL_RESULT            32

struct WORKUNIT {
    int id;
    int create_time;
    int appid;                  // associated app
    char name[256];
    char xml_doc[BLOB_SIZE];
    int batch;
    double rsc_fpops_est;       // estimated # of FP operations
        // used to estimate how long a result will take on a host
    double rsc_fpops_bound;     // upper bound on # of FP ops
        // used to calculate an upper bound on the CPU time for a result
        // before it is aborted.
    double rsc_memory_bound;    // upper bound on RAM working set (bytes)
        // currently used only by scheduler to screen hosts
        // At some point, could use as runtime limit
    double rsc_disk_bound;      // upper bound on amount of disk needed (bytes)
        // (including input, output and temp files, but NOT the app)
        // used for 2 purposes:
        // 1) for scheduling (don't send this WU to a host w/ insuff. disk)
        // 2) abort task if it uses more than this disk
    double rsc_bandwidth_bound;
        // send only to hosts with at least this much download bandwidth
    bool need_validate;         // this WU has at least 1 result in
                                // validate state = NEED_CHECK
    int canonical_resultid;     // ID of canonical result, or zero
    double canonical_credit;    // credit that all correct results get
    int transition_time;        // when should transition_handler
                                // next check this WU?
                                // MAXINT if no need to check
    int delay_bound;            // determines result deadline,
                                // timeout check time
    int error_mask;             // bitmask of errors (see above)
    int file_delete_state;
    int assimilate_state;
    int hr_class;               // homogeneous redundancy class
        // used to send redundant copies only to "similar" hosts
        // (in terms of numerics, performance, or both)
    double opaque;              // project-specific; usually external ID
    int min_quorum;             // minimum quorum size
    int target_nresults;        // try to get this many successful results
                                // may be > min_quorum to get consensus
                                // quicker or reflect loss rate
    int max_error_results;      // WU error if < #error results
    int max_total_results;      // WU error if < #total results
        // (need this in case results are never returned)
    int max_success_results;    // WU error if < #success results
        // without consensus (i.e. WU is nondeterministic)
    char result_template_file[64];
    int priority;
    char mod_time[16];

    // the following not used in the DB
    char app_name[256];
    void clear();
};

struct CREDITED_JOB {
    int userid;
    double workunitid;

    // the following not used in the DB
    void clear();
};

// WARNING: be Very careful about changing any values,
// especially for a project already running -
// the database will become inconsistent

//#define RESULT_SERVER_STATE_INACTIVE       1
#define RESULT_SERVER_STATE_UNSENT         2
#define RESULT_SERVER_STATE_IN_PROGRESS    4
#define RESULT_SERVER_STATE_OVER           5
    // we received a reply, timed out, or decided not to send.
    // Note: we could get a reply even after timing out.

#define RESULT_OUTCOME_INIT             0
#define RESULT_OUTCOME_SUCCESS          1
#define RESULT_OUTCOME_COULDNT_SEND     2
#define RESULT_OUTCOME_CLIENT_ERROR     3
    // an error happened on the client
#define RESULT_OUTCOME_NO_REPLY         4
#define RESULT_OUTCOME_DIDNT_NEED       5
    // we created the result but didn't need to send it because we already
    // got a canonical result for the WU
#define RESULT_OUTCOME_VALIDATE_ERROR   6
    // The outcome was initially SUCCESS,
    // but the validator had a permanent error reading a result file,
    // or the result file had a syntax error
#define RESULT_OUTCOME_CLIENT_DETACHED  7
    // we believe that the client detached

#define VALIDATE_STATE_INIT         0
#define VALIDATE_STATE_VALID        1
#define VALIDATE_STATE_INVALID      2
#define VALIDATE_STATE_NO_CHECK     3
    // WU had error, so we'll never get around to validating its results
    // This lets us avoid showing the claimed credit as "pending"
#define VALIDATE_STATE_INCONCLUSIVE 4
    // the validator looked this result (as part of a check_set() call)
    // but didn't find a canonical result.
    // This needs to be distinct from INIT for the transitioner to decide
    // whether to trigger the validator
#define VALIDATE_STATE_TOO_LATE     5
    // The result arrived after the canonical result's files were deleted,
    // so we can't determine if it's valid

// values for ASSIGNMENT.target_type
#define ASSIGN_NONE     0
#define ASSIGN_HOST     1
#define ASSIGN_USER     2
#define ASSIGN_TEAM     3

struct RESULT {
    int id;
    int create_time;
    int workunitid;
    int server_state;               // see above
    int outcome;                    // see above; defined if server state OVER
    int client_state;               // phase that client contacted us in.
                                    // if UPLOADED then outcome is success.
                                    // error details are in stderr_out.
                                    // The values for this field are defined
                                    // in lib/result_state.h
    int hostid;                     // host processing this result
    int userid;                     // user processing this result
    int report_deadline;            // deadline for receiving result
    int sent_time;                  // when result was sent to host
    int received_time;              // when result was received from host
    char name[256];
    double cpu_time;                // CPU time used to complete result
        // NOTE: with the current scheduler and client,
        // this is elapsed time, not the CPU time
    char xml_doc_in[BLOB_SIZE];     // descriptions of output files
    char xml_doc_out[BLOB_SIZE];    // MD5s of output files
    char stderr_out[BLOB_SIZE];     // stderr output, if any
    int batch;
    int file_delete_state;          // see above; values for file_delete_state
    int validate_state;
    double claimed_credit;          // CPU time times host credit/sec
    double granted_credit;          // == canonical credit of WU
    double opaque;                  // project-specific; usually external ID
    int random;                     // determines send order
    int app_version_num;            // version# of app (not core client)
    int appid;                      // copy of WU's appid
    int exit_status;                // application exit status, if any
    int teamid;
    int priority;
    char mod_time[16];

    // the following used by the scheduler, but not stored in the DB
    //
    char wu_name[256];
    double fpops_per_cpu_sec;
    double fpops_cumulative;
    double intops_per_cpu_sec;
    double intops_cumulative;
    int units;      // used for granting credit by # of units processed
    int parse_from_client(FILE*);
    char platform_name[256];
    BEST_APP_VERSION* bavp;
    void clear();
    int write_to_client(FILE*);
};

struct MSG_FROM_HOST {
    int id;
    int create_time;
    int hostid;
    char variety[256];              // project-defined; what kind of msg
    bool handled;                   // message handler has processed this
    char xml[MSG_FROM_HOST_BLOB_SIZE];
    void clear();
};

struct MSG_TO_HOST {
    int id;
    int create_time;
    int hostid;
    char variety[256];              // project-defined; what kind of msg
    bool handled;                   // scheduler has sent this
    char xml[MSG_TO_HOST_BLOB_SIZE];      // text to include in sched reply
    void clear();
};

struct ASSIGNMENT {
    int id;
    int create_time;
    int target_id;
    int target_type;
    int multi;
    int workunitid;
    int resultid;
    void clear();
};

struct TRANSITIONER_ITEM {
    int id; // WARNING: this is the WU ID
    char name[256];
    int appid;
    int min_quorum;
    bool need_validate;
    int canonical_resultid;
    int transition_time;
    int delay_bound;
    int error_mask;
    int max_error_results;
    int max_total_results;
    int file_delete_state;
    int assimilate_state;
    int target_nresults;
    char result_template_file[64];
    int priority;
    int hr_class;
    int batch;
    int res_id; // This is the RESULT ID
    char res_name[256];
    int res_report_deadline;
    int res_server_state;
    int res_outcome;
    int res_validate_state;
    int res_file_delete_state;
    int res_sent_time;
    int res_hostid;
    int res_received_time;

    void clear();
    void parse(MYSQL_ROW&);
};

struct CREDIT_MULTIPLIER {
    int id;
    int appid;
    int _time;
    double multiplier;

    void clear();
};
    
struct DB_CREDIT_MULTIPLIER : public DB_BASE, public CREDIT_MULTIPLIER {
    DB_CREDIT_MULTIPLIER(DB_CONN* p=0);
    int get_id();
    void db_print(char *);
    void db_parse(MYSQL_ROW &row);
    void get_nearest(int appid, int time);
};

struct STATE_COUNTS {
    int appid; 
    int last_update_time;   
    int result_server_state_2;       
    int result_server_state_4;       
    int result_file_delete_state_1;  
    int result_file_delete_state_2;  
    int result_server_state_5_and_file_delete_state_0;   
    int workunit_need_validate_1;    
    int workunit_assimilate_state_1; 
    int workunit_file_delete_state_1; 
    int workunit_file_delete_state_2;

    void clear();
};

struct DB_STATE_COUNTS : public DB_BASE, public STATE_COUNTS {
    DB_STATE_COUNTS(DB_CONN* p=0);
    int get_id();
    void db_print(char *);
    void db_parse(MYSQL_ROW &row);
};

struct VALIDATOR_ITEM {
    WORKUNIT wu;
    RESULT res;
 
    void clear();
    void parse(MYSQL_ROW&);
};

    

class DB_PLATFORM : public DB_BASE, public PLATFORM {
public:
    DB_PLATFORM(DB_CONN* p=0);
    int get_id();
    void db_print(char*);
    void db_parse(MYSQL_ROW &row);
};

class DB_APP : public DB_BASE, public APP {
public:
    DB_APP(DB_CONN* p=0);
    int get_id();
    void db_print(char*);
    void db_parse(MYSQL_ROW &row);
};

class DB_APP_VERSION : public DB_BASE, public APP_VERSION {
public:
    DB_APP_VERSION(DB_CONN* p=0);
    int get_id();
    void db_print(char*);
    void db_parse(MYSQL_ROW &row);
};

class DB_USER : public DB_BASE, public USER {
public:
    DB_USER(DB_CONN* p=0);
    int get_id();
    void db_print(char*);
    void db_parse(MYSQL_ROW &row);
    void operator=(USER& r) {USER::operator=(r);}
};

class DB_TEAM : public DB_BASE, public TEAM {
public:
    DB_TEAM(DB_CONN* p=0);
    int get_id();
    void db_print(char*);
    void db_parse(MYSQL_ROW &row);
};

class DB_HOST : public DB_BASE, public HOST {
public:
    DB_HOST(DB_CONN* p=0);
    int get_id();
    int update_diff(HOST&);
    void db_print(char*);
    void db_parse(MYSQL_ROW &row);
    void operator=(HOST& r) {HOST::operator=(r);}
};

class DB_RESULT : public DB_BASE, public RESULT {
public:
    DB_RESULT(DB_CONN* p=0);
    int get_id();
    int mark_as_sent(int old_server_state);
    void db_print(char*);
    void db_print_values(char*);
    void db_parse(MYSQL_ROW &row);
    void operator=(RESULT& r) {RESULT::operator=(r);}
};

class DB_WORKUNIT : public DB_BASE, public WORKUNIT {
public:
    DB_WORKUNIT(DB_CONN* p=0);
    int get_id();
    void db_print(char*);
    void db_parse(MYSQL_ROW &row);
    void operator=(WORKUNIT& w) {WORKUNIT::operator=(w);}
};

class DB_CREDITED_JOB : public DB_BASE, public CREDITED_JOB {
public:
    DB_CREDITED_JOB(DB_CONN* p=0);
    void db_print(char*);
    void db_parse(MYSQL_ROW &row);
    void operator=(CREDITED_JOB& wh) {CREDITED_JOB::operator=(wh);}
};

class DB_MSG_FROM_HOST : public DB_BASE, public MSG_FROM_HOST {
public:
    DB_MSG_FROM_HOST(DB_CONN* p=0);
    int get_id();
    void db_print(char*);
    void db_parse(MYSQL_ROW &row);
};

class DB_MSG_TO_HOST : public DB_BASE, public MSG_TO_HOST {
public:
    DB_MSG_TO_HOST(DB_CONN* p=0);
    int get_id();
    void db_print(char*);
    void db_parse(MYSQL_ROW &row);
};

class DB_ASSIGNMENT : public DB_BASE, public ASSIGNMENT {
public:
    DB_ASSIGNMENT(DB_CONN* p=0);
    int get_id();
    void db_print(char*);
    void db_parse(MYSQL_ROW& row);
};

// The transitioner uses this to get (WU, result) pairs efficiently.
// Each call to enumerate() returns a list of the pairs for a single WU
//
class DB_TRANSITIONER_ITEM_SET : public DB_BASE_SPECIAL {
public:
    DB_TRANSITIONER_ITEM_SET(DB_CONN* p=0);
    TRANSITIONER_ITEM last_item;
    int nitems_this_query;

    int enumerate(
        int transition_time,
        int nresult_limit,
        int wu_id_modulus,
        int wu_id_remainder,
        std::vector<TRANSITIONER_ITEM>& items
    );
    int update_result(TRANSITIONER_ITEM&);
    int update_workunit(TRANSITIONER_ITEM&, TRANSITIONER_ITEM&);
};

// The validator uses this to get (WU, result) pairs efficiently.
// Each call to enumerate() returns a list of the pairs for a single WU
//
class DB_VALIDATOR_ITEM_SET : public DB_BASE_SPECIAL {
public:
    DB_VALIDATOR_ITEM_SET(DB_CONN* p=0);
    VALIDATOR_ITEM last_item;
    int nitems_this_query;

    int enumerate(
        int appid,
        int nresult_limit,
        int wu_id_modulus,
        int wu_id_remainder,
        std::vector<VALIDATOR_ITEM>& items
    );
    int update_result(RESULT&);
    int update_workunit(WORKUNIT&);
};


// used by the feeder and scheduler for outgoing work
//
struct WORK_ITEM {
    int res_id;
    int res_priority;
    WORKUNIT wu;
    void parse(MYSQL_ROW& row);
};

class DB_WORK_ITEM : public WORK_ITEM, public DB_BASE_SPECIAL {
    int start_id;
        // when enumerate_all is used, keeps track of which ID to start from
public:
    DB_WORK_ITEM(DB_CONN* p=0);
    int enumerate(
        int limit, const char* select_clause, const char* order_clause
    );
        // used by feeder
    int enumerate_all(
        int limit, const char* select_clause
    );
        // used by feeder when HR is used.
        // Successive calls cycle through all results.
    int read_result();
        // used by scheduler to read result server state
    int update();
        // used by scheduler to update WU transition time
        // and various result fields
};

// Used by the scheduler to send <result_abort> or <result_abort_if_not_started>
// messages if the result is no longer needed.
//
struct IN_PROGRESS_RESULT {
	char result_name[256];
	int assimilate_state;
	int error_mask;
	int server_state;
	int outcome;
    void parse(MYSQL_ROW& row);
};

class DB_IN_PROGRESS_RESULT : public IN_PROGRESS_RESULT, public DB_BASE_SPECIAL {
public:
    DB_IN_PROGRESS_RESULT(DB_CONN* p=0);
    int enumerate(int hostid, const char* result_names);
};

// Used by the scheduler to handle results reported by clients
// The read and the update of these results are combined
// into single SQL queries.

struct SCHED_RESULT_ITEM {
    char queried_name[256];     // name as reported by client
    int id;
    char name[256];
    int workunitid;
    int appid;
    int server_state;
    int client_state;
    int validate_state;
    int outcome;
    int hostid;
    int userid;
    int teamid;
    int sent_time;
    int received_time;
    double cpu_time;
    double claimed_credit;
    char xml_doc_out[BLOB_SIZE];
    char stderr_out[BLOB_SIZE];
    int app_version_num;
    int exit_status;
    int file_delete_state;

    void clear();
    void parse(MYSQL_ROW& row);
};

class DB_SCHED_RESULT_ITEM_SET : public DB_BASE_SPECIAL {
public:
    DB_SCHED_RESULT_ITEM_SET(DB_CONN* p=0);
    std::vector<SCHED_RESULT_ITEM> results;

    int add_result(char* result_name);

    int enumerate();
        // using a single SQL query, look up all the reported results,
        // (based on queried_name)
        // and fill in the rest of the entries in the results vector

    int lookup_result(char* result_name, SCHED_RESULT_ITEM** result);

    int update_result(SCHED_RESULT_ITEM& result);
    int update_workunits();
};

#endif
