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

// transitioner - handle transitions in the state of a WU
//    - a result has become DONE (via timeout or client reply)
//    - the WU error mask is set (e.g. by validater)
//    - assimilation is finished
//
// cmdline:
//   [ -one_pass ]          do one pass, then exit
//   [ -d x ]               debug level x
//   [ -mod n i ]           process only WUs with (id mod n) == i
//   [ -sleep_interval x ]  sleep x seconds if nothing to do

#include "config.h"
#include <vector>
#include <unistd.h>
#include <cstring>
#include <climits>
#include <cstdlib>
#include <string>
#include <signal.h>
#include <sys/time.h>

#include "boinc_db.h"
#include "util.h"
#include "backend_lib.h"
#include "common_defs.h"
#include "error_numbers.h"
#include "str_util.h"

#include "sched_config.h"
#include "sched_util.h"
#include "sched_msgs.h"
#ifdef GCL_SIMULATOR
#include "gcl_simulator.h"
#endif

#define LOCKFILE                "transitioner.out"
#define PIDFILE                 "transitioner.pid"

#define SELECT_LIMIT    1000

#define DEFAULT_SLEEP_INTERVAL  5

int startup_time;
R_RSA_PRIVATE_KEY key;
int mod_n, mod_i;
bool do_mod = false;
bool one_pass = false;
int sleep_interval = DEFAULT_SLEEP_INTERVAL;

void signal_handler(int) {
    log_messages.printf(MSG_NORMAL, "Signaled by simulator\n");
    return;
}

int result_suffix(char* name) {
    char* p = strrchr(name, '_');
    if (p) return atoi(p+1);
    return 0;
}

// A result just timed out.
// Update the host's avg_turnaround and max_results_day.
//
int penalize_host(int hostid, double delay_bound) {
    DB_HOST host;
    char buf[256];
    int retval = host.lookup_id(hostid);
    if (retval) return retval;
    compute_avg_turnaround(host, delay_bound);
    if (host.max_results_day == 0 || host.max_results_day > config.daily_result_quota) {
        host.max_results_day = config.daily_result_quota;
    }
    host.max_results_day -= 1;
    if (host.max_results_day < 1) {
        host.max_results_day = 1;
    }
    sprintf(buf,
        "avg_turnaround=%f, max_results_day=%d",
        host.avg_turnaround, host.max_results_day
    );
    return host.update_field(buf);
}

int handle_wu(
    DB_TRANSITIONER_ITEM_SET& transitioner,
    std::vector<TRANSITIONER_ITEM>& items
) {
    int ntotal, nerrors, retval, ninprogress, nsuccess;
    int nunsent, ncouldnt_send, nover, ndidnt_need, nno_reply;
    int canonical_result_index, j;
    char suffix[256];
    time_t now = time(0), x;
    bool all_over_and_validated, have_new_result_to_validate, do_delete;
    unsigned int i;

    TRANSITIONER_ITEM& wu_item = items[0];
    TRANSITIONER_ITEM wu_item_original = wu_item;

    // "assigned" WUs aren't supposed to pass through the transitioner.
    // If we get one, it's an error
    //
    if (config.enable_assignment && strstr(wu_item.name, ASSIGNED_WU_STR)) {
        DB_WORKUNIT wu;
        char buf[256];

        wu.id = wu_item.id;
        log_messages.printf(MSG_CRITICAL,
            "Assigned WU %d unexpectedly found by transitioner\n", wu.id
        );
        sprintf(buf, "transition_time=%d", INT_MAX);
        retval = wu.update_field(buf);
        if (retval) {
            log_messages.printf(MSG_CRITICAL,
                "update_field failed %d\n", retval
            );
        }
        return 0;
    }

    // count up the number of results in various states,
    // and check for timed-out results
    //
    ntotal = 0;
    nunsent = 0;
    ninprogress = 0;
    nover = 0;
    nerrors = 0;
    nsuccess = 0;
    ncouldnt_send = 0;
    nno_reply = 0;
    ndidnt_need = 0;
    have_new_result_to_validate = false;
    int rs, max_result_suffix = -1;

    // Scan the WU's results, and find the canonical result if there is one
    //
    canonical_result_index = -1;
    if (wu_item.canonical_resultid) {
        for (i=0; i<items.size(); i++) {
            TRANSITIONER_ITEM& res_item = items[i];
            if (!res_item.res_id) continue;
            if (res_item.res_id == wu_item.canonical_resultid) {
                canonical_result_index = i;
            }
        }
    }

    if (wu_item.canonical_resultid && (canonical_result_index == -1)) {
        log_messages.printf(MSG_CRITICAL,
            "[WU#%d %s] can't find canonical result\n",
            wu_item.id, wu_item.name
        );
    }

    // if there is a canonical result, see if its file are deleted
    //
    bool canonical_result_files_deleted = false;
    if (canonical_result_index >= 0) {
        TRANSITIONER_ITEM& cr = items[canonical_result_index];
        if (cr.res_file_delete_state == FILE_DELETE_DONE) {
            canonical_result_files_deleted = true;
        }
    }

    // Scan this WU's results, and
    // 1) count those in various server states;
    // 2) identify time-out results and update their server state and outcome
    // 3) find the max result suffix (in case need to generate new ones)
    // 4) see if we have a new result to validate
    //    (outcome SUCCESS and validate_state INIT)
    //
    for (i=0; i<items.size(); i++) {
        TRANSITIONER_ITEM& res_item = items[i];

        if (!res_item.res_id) continue;
        ntotal++;

        rs = result_suffix(res_item.res_name);
        if (rs > max_result_suffix) max_result_suffix = rs;

        switch (res_item.res_server_state) {
        case RESULT_SERVER_STATE_UNSENT:
            nunsent++;
            break;
        case RESULT_SERVER_STATE_IN_PROGRESS:
            if (res_item.res_report_deadline < now) {
                log_messages.printf(MSG_NORMAL,
                    "[WU#%d %s] [RESULT#%d %s] result timed out (%d < %d) server_state:IN_PROGRESS=>OVER; outcome:NO_REPLY\n",
                    wu_item.id, wu_item.name, res_item.res_id, res_item.res_name,
                    res_item.res_report_deadline, (int)now
                );
                res_item.res_server_state = RESULT_SERVER_STATE_OVER;
                res_item.res_outcome = RESULT_OUTCOME_NO_REPLY;
                retval = transitioner.update_result(res_item);
                if (retval) {
                    log_messages.printf(MSG_CRITICAL,
                        "[WU#%d %s] [RESULT#%d %s] update_result(): %d\n",
                        wu_item.id, wu_item.name, res_item.res_id,
                        res_item.res_name, retval
                    );
                }
                penalize_host(res_item.res_hostid, (double)wu_item.delay_bound);
                nover++;
                nno_reply++;
            } else {
                ninprogress++;
            }
            break;
        case RESULT_SERVER_STATE_OVER:
            nover++;
            switch (res_item.res_outcome) {
            case RESULT_OUTCOME_COULDNT_SEND:
                log_messages.printf(MSG_NORMAL,
                    "[WU#%d %s] [RESULT#%d %s] result couldn't be sent\n",
                    wu_item.id, wu_item.name, res_item.res_id, res_item.res_name
                );
                ncouldnt_send++;
                break;
            case RESULT_OUTCOME_SUCCESS:
                if (res_item.res_validate_state == VALIDATE_STATE_INIT) {
                    if (canonical_result_files_deleted) {
                        res_item.res_validate_state = VALIDATE_STATE_TOO_LATE;
                        retval = transitioner.update_result(res_item);
                        log_messages.printf(MSG_NORMAL,
                            "[WU#%d %s] [RESULT#%d %s] validate_state:INIT=>TOO_LATE retval %d\n",
                            wu_item.id, wu_item.name, res_item.res_id,
                            res_item.res_name, retval
                        );
                    } else {
                        have_new_result_to_validate = true;
                    }
                }
                nsuccess++;
                break;
            case RESULT_OUTCOME_CLIENT_ERROR:
            case RESULT_OUTCOME_VALIDATE_ERROR:
                nerrors++;
                break;
            case RESULT_OUTCOME_CLIENT_DETACHED:
            case RESULT_OUTCOME_NO_REPLY:
                nno_reply++;
                break;
            case RESULT_OUTCOME_DIDNT_NEED:
                ndidnt_need++;
                break;
            }
            break;
        }
    }

    log_messages.printf(MSG_DEBUG,
        "[WU#%d %s] %d results: unsent %d, in_progress %d, over %d (success %d, error %d, couldnt_send %d, no_reply %d, didnt_need %d)\n",
        wu_item.id, wu_item.name, ntotal, nunsent, ninprogress, nover,
        nsuccess, nerrors, ncouldnt_send, nno_reply, ndidnt_need
    );

    // if there's a new result to validate, trigger validation
    //
    if (have_new_result_to_validate && (nsuccess >= wu_item.min_quorum)) {
        wu_item.need_validate = true;
        log_messages.printf(MSG_NORMAL,
            "[WU#%d %s] need_validate:=>true\n", wu_item.id, wu_item.name
        );
    }

    // check for WU error conditions
    // NOTE: check on max # of success results is done in validater
    //
    if (ncouldnt_send > 0) {
        wu_item.error_mask |= WU_ERROR_COULDNT_SEND_RESULT;
    }

    // if WU has results with errors and no success yet,
    // reset homogeneous redundancy class to give other platforms a try
    //
    if (nerrors & !(nsuccess || ninprogress)) {
        wu_item.hr_class = 0;
    }

    if (nerrors > wu_item.max_error_results) {
        log_messages.printf(MSG_NORMAL,
            "[WU#%d %s] WU has too many errors (%d errors for %d results)\n",
            wu_item.id, wu_item.name, nerrors, (int)items.size()
        );
        wu_item.error_mask |= WU_ERROR_TOO_MANY_ERROR_RESULTS;
    }
    if ((int)items.size() > wu_item.max_total_results) {
        log_messages.printf(MSG_NORMAL,
            "[WU#%d %s] WU has too many total results (%d)\n",
            wu_item.id, wu_item.name, (int)items.size()
        );
        wu_item.error_mask |= WU_ERROR_TOO_MANY_TOTAL_RESULTS;
    }

    // if this WU had an error, don't send any unsent results,
    // and trigger assimilation if needed
    //
    if (wu_item.error_mask) {
        for (i=0; i<items.size(); i++) {
            TRANSITIONER_ITEM& res_item = items[i];
            if (res_item.res_id) {
                bool update_result = false;
                switch(res_item.res_server_state) {
                case RESULT_SERVER_STATE_UNSENT:
                    log_messages.printf(MSG_NORMAL,
                        "[WU#%d %s] [RESULT#%d %s] server_state:UNSENT=>OVER; outcome:=>DIDNT_NEED\n",
                        wu_item.id, wu_item.name, res_item.res_id, res_item.res_name
                    );
                    res_item.res_server_state = RESULT_SERVER_STATE_OVER;
                    res_item.res_outcome = RESULT_OUTCOME_DIDNT_NEED;
                    update_result = true;
                    break;
                case RESULT_SERVER_STATE_OVER:
                    switch (res_item.res_outcome) {
                    case RESULT_OUTCOME_SUCCESS:
                        switch(res_item.res_validate_state) {
                        case VALIDATE_STATE_INIT:
                        case VALIDATE_STATE_INCONCLUSIVE:
                            res_item.res_validate_state = VALIDATE_STATE_NO_CHECK;
                            update_result = true;
                            break;
                        }
                    }
                }
                if (update_result) {
                    retval = transitioner.update_result(res_item);
                    if (retval) {
                        log_messages.printf(MSG_CRITICAL,
                            "[WU#%d %s] [RESULT#%d %s] result.update() == %d\n",
                            wu_item.id, wu_item.name, res_item.res_id, res_item.res_name, retval
                        );
                    }
                }
            }
        }
        if (wu_item.assimilate_state == ASSIMILATE_INIT) {
            wu_item.assimilate_state = ASSIMILATE_READY;
            log_messages.printf(MSG_NORMAL,
                "[WU#%d %s] error_mask:%d assimilate_state:INIT=>READY\n",
                wu_item.id, wu_item.name, wu_item.error_mask
            );
        }
    } else if (wu_item.canonical_resultid == 0) {
        // Here if no WU-level error.
        // Generate new results if needed.
        // NOTE: n must be signed
        //
        int n = wu_item.target_nresults - nunsent - ninprogress - nsuccess;
        std::string values;
        char value_buf[MAX_QUERY_LEN];
        if (n > 0) {
            log_messages.printf(
                MSG_NORMAL,
                "[WU#%d %s] Generating %d more results (%d target - %d unsent - %d in progress - %d success)\n",
                wu_item.id, wu_item.name, n, wu_item.target_nresults, nunsent, ninprogress, nsuccess
            );
            for (j=0; j<n; j++) {
                sprintf(suffix, "%d", max_result_suffix+j+1);
                const char *rtfpath = config.project_path("%s", wu_item.result_template_file);
                int priority_increase = 0;
                if (nover && config.reliable_priority_on_over) {
                    priority_increase += config.reliable_priority_on_over;
                } else if (nover && !nerrors && config.reliable_priority_on_over_except_error) {
                    priority_increase += config.reliable_priority_on_over_except_error;
                }
                retval = create_result_ti(
                    wu_item, (char *)rtfpath, suffix, key, config, value_buf, priority_increase
                );
                if (retval) {
                    log_messages.printf(MSG_CRITICAL,
                        "[WU#%d %s] create_result_ti() %d\n",
                        wu_item.id, wu_item.name, retval
                    );
                    return retval;
                }
                if (j==0) {
                    values = value_buf;
                } else {
                    values += ",";
                    values += value_buf;
                }
            }
            DB_RESULT r;
            retval = r.insert_batch(values);
            if (retval) {
                log_messages.printf(MSG_CRITICAL,
                    "[WU#%d %s] insert_batch() %d\n",
                    wu_item.id, wu_item.name, retval
                );
                return retval;
            }
        }
    }

    // scan results:
    //  - see if all over and validated
    //
    all_over_and_validated = true;
    bool all_over_and_ready_to_assimilate = true; // used for the defer assmilation
	int most_recently_returned = 0;
    for (i=0; i<items.size(); i++) {
        TRANSITIONER_ITEM& res_item = items[i];
        if (res_item.res_id) {
            if (res_item.res_server_state == RESULT_SERVER_STATE_OVER) {
            	if ( res_item.res_received_time > most_recently_returned ) {
            		most_recently_returned = res_item.res_received_time;
            	}
                if (res_item.res_outcome == RESULT_OUTCOME_SUCCESS) {
                    if (res_item.res_validate_state == VALIDATE_STATE_INIT) {
                        all_over_and_validated = false;
                        all_over_and_ready_to_assimilate = false;
                    }
                } else if ( res_item.res_outcome == RESULT_OUTCOME_NO_REPLY ) {
                	if ( ( res_item.res_report_deadline + config.grace_period_hours*60*60 ) > now ) {
                		all_over_and_validated = false;
                	}
                }
            } else {
                all_over_and_validated = false;
                all_over_and_ready_to_assimilate = false;
            }
        }
    }

    // If we are defering assimilation until all results are over
    // and validated then when that happens we need to make sure
    // that it gets advanced to assimilate ready
    // the items.size is a kludge
    //
    if (all_over_and_ready_to_assimilate == true && wu_item.assimilate_state == ASSIMILATE_INIT && items.size() > 0 && wu_item.canonical_resultid > 0
    ) {
    	wu_item.assimilate_state = ASSIMILATE_READY;
        log_messages.printf(MSG_NORMAL,
            "[WU#%d %s] Deferred assimililation now set to ASSIMILATE_STATE_READY\n",
            wu_item.id, wu_item.name
        );
    }
    // if WU is assimilated, trigger file deletion
    //
    if (wu_item.assimilate_state == ASSIMILATE_DONE && ((most_recently_returned + config.delete_delay_hours*60*60) < now)) {
        // can delete input files if all results OVER
        //
        if (all_over_and_validated && wu_item.file_delete_state == FILE_DELETE_INIT) {
            wu_item.file_delete_state = FILE_DELETE_READY;
            log_messages.printf(MSG_DEBUG,
                "[WU#%d %s] ASSIMILATE_DONE: file_delete_state:=>READY\n",
                wu_item.id, wu_item.name
            );
        }

        // output of error results can be deleted immediately;
        // output of success results can be deleted if validated
        //
        for (i=0; i<items.size(); i++) {
            TRANSITIONER_ITEM& res_item = items[i];

            // can delete canonical result outputs only if all successful
            // results have been validated
            //
            if (((int)i == canonical_result_index) && !all_over_and_validated) {
                continue;
            }

            if (res_item.res_id) {
                do_delete = false;
                switch(res_item.res_outcome) {
                case RESULT_OUTCOME_CLIENT_ERROR:
                    do_delete = true;
                    break;
                case RESULT_OUTCOME_SUCCESS:
                    do_delete = (res_item.res_validate_state != VALIDATE_STATE_INIT);
                    break;
                }
                if (do_delete && res_item.res_file_delete_state == FILE_DELETE_INIT) {
                    log_messages.printf(MSG_NORMAL,
                        "[WU#%d %s] [RESULT#%d %s] file_delete_state:=>READY\n",
                        wu_item.id, wu_item.name, res_item.res_id, res_item.res_name
                    );
                    res_item.res_file_delete_state = FILE_DELETE_READY;

                    retval = transitioner.update_result(res_item);
                    if (retval) {
                        log_messages.printf(MSG_CRITICAL,
                            "[WU#%d %s] [RESULT#%d %s] result.update() == %d\n",
                            wu_item.id, wu_item.name, res_item.res_id, res_item.res_name, retval
                        );
                    }
                }
            }
        }
    } else if ( wu_item.assimilate_state == ASSIMILATE_DONE ) {
		log_messages.printf(MSG_DEBUG,
            "[WU#%d %s] not checking for items to be ready for delete because the deferred delete time has not expired.  That will occur in %d seconds\n",
            wu_item.id,
            wu_item.name,
            most_recently_returned + config.delete_delay_hours*60*60-(int)now
        );
    }

    // compute next transition time = minimum timeout of in-progress results
    //
    if (wu_item.canonical_resultid) {
        wu_item.transition_time = INT_MAX;
    } else {
        // If there is no canonical result,
        // make sure that the transitioner will 'see' this WU again.
        // In principle this is NOT needed, but it is one way to make
        // the BOINC back-end more robust.
        //
        const int ten_days = 10*86400;
        int long_delay = (int)(1.5*wu_item.delay_bound);
        wu_item.transition_time = (long_delay > ten_days) ? long_delay : ten_days;
        wu_item.transition_time += time(0);
    }
    int max_grace_or_delay_time = 0;  
    for (i=0; i<items.size(); i++) {
        TRANSITIONER_ITEM& res_item = items[i];
        if (res_item.res_id) {
            if (res_item.res_server_state == RESULT_SERVER_STATE_IN_PROGRESS) {
                // In cases where a result has been RESENT to a host, the
                // report deadline time may be EARLIER than
                // sent_time + delay_bound
                // because the sent_time has been updated with the later
                // "resend" time.
                //
                // x = res_item.res_sent_time + wu_item.delay_bound;
                x = res_item.res_report_deadline;
                if (x < wu_item.transition_time) {
                    wu_item.transition_time = x;
                }
            } else if ( res_item.res_server_state == RESULT_SERVER_STATE_OVER  ) {
            	if ( res_item.res_outcome == RESULT_OUTCOME_NO_REPLY ) {
            		// Transition again after the grace period has expired
                	if ( ( res_item.res_report_deadline + config.grace_period_hours*60*60 ) > now ) {
                		x = res_item.res_report_deadline + config.grace_period_hours*60*60;
    					if (x > max_grace_or_delay_time) {
                    		max_grace_or_delay_time = x;
            			}
        			}
                } else if ( res_item.res_outcome == RESULT_OUTCOME_SUCCESS || res_item.res_outcome == RESULT_OUTCOME_CLIENT_ERROR || res_item.res_outcome == RESULT_OUTCOME_VALIDATE_ERROR) {
            		// Transition again after deferred delete period has experied
                	if ( (res_item.res_received_time + config.delete_delay_hours*60*60) > now ) {
                		x = res_item.res_received_time + config.delete_delay_hours*60*60;
    					if (x > max_grace_or_delay_time && res_item.res_received_time > 0) {
                    		max_grace_or_delay_time = x;
   					    }
                	}
                }
            }
        }
    }
    // If either of the grace period or delete delay is less than
    // the next transition time then use that value
    //
    if ( max_grace_or_delay_time < wu_item.transition_time && max_grace_or_delay_time > now && ninprogress == 0) {
        wu_item.transition_time = max_grace_or_delay_time;
        log_messages.printf(MSG_NORMAL,
            "[WU#%d %s] Delaying transition due to grace period or delete day.  New transition time = %d sec\n",
            wu_item.id, wu_item.name, wu_item.transition_time
        );
    }
    
    // If transition time is in the past,
    // the system is bogged down and behind schedule.
    // Delay processing of the WU by an amount DOUBLE the amount we are behind,
    // but not less than 60 secs or more than one day.
    //
    if (wu_item.transition_time < now) {
        int extra_delay = 2*(now - wu_item.transition_time);
        if (extra_delay < 60) extra_delay = 60;
        if (extra_delay > 86400) extra_delay = 86400;
        log_messages.printf(MSG_DEBUG,
            "[WU#%d %s] transition time in past: adding extra delay %d sec\n",
            wu_item.id, wu_item.name, extra_delay
        );
        wu_item.transition_time = now + extra_delay;
    }

    log_messages.printf(MSG_DEBUG,
        "[WU#%d %s] setting transition_time to %d\n",
        wu_item.id, wu_item.name, wu_item.transition_time
    );

    retval = transitioner.update_workunit(wu_item, wu_item_original);
    if (retval) {
        log_messages.printf(MSG_CRITICAL,
            "[WU#%d %s] workunit.update() == %d\n",
            wu_item.id, wu_item.name, retval
        );
        return retval;
    }
    return 0;
}

bool do_pass() {
    int retval;
    DB_TRANSITIONER_ITEM_SET transitioner;
    std::vector<TRANSITIONER_ITEM> items;
    bool did_something = false;

    if (!one_pass) check_stop_daemons();

    // loop over entries that are due to be checked
    //
    while (1) {
        retval = transitioner.enumerate(
            (int)time(0), SELECT_LIMIT, mod_n, mod_i, items
        );
        if (retval) {
            if (retval != ERR_DB_NOT_FOUND) {
                log_messages.printf(MSG_CRITICAL,
                    "WU enum error%d; exiting\n", retval
                );
                exit(1);
            }
            break;
        }
        did_something = true;
        TRANSITIONER_ITEM& wu_item = items[0];
        retval = handle_wu(transitioner, items);
        if (retval) {
            log_messages.printf(MSG_CRITICAL,
                "[WU#%d %s] handle_wu: %d; quitting\n",
                wu_item.id, wu_item.name, retval
            );
            exit(1);
        }

        if (!one_pass) check_stop_daemons();
    }
    return did_something;
}

void main_loop() {
    int retval;

    retval = boinc_db.open(config.db_name, config.db_host, config.db_user, config.db_passwd);
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "boinc_db.open: %d\n", retval);
        exit(1);
    }

    while (1) {
        log_messages.printf(MSG_DEBUG, "doing a pass\n");
        if (!do_pass()) {
            if (one_pass) break;
#ifdef GCL_SIMULATOR
            continue_simulation("transitioner");
            signal(SIGUSR2, simulator_signal_handler);
            pause();
#else
            log_messages.printf(MSG_DEBUG, "sleeping %d\n", sleep_interval);
            sleep(sleep_interval);
#endif
        }
    }
}

int main(int argc, char** argv) {
    int i, retval;
    char path[256];

    startup_time = time(0);
    for (i=1; i<argc; i++) {
        if (!strcmp(argv[i], "-one_pass")) {
            one_pass = true;
        } else if (!strcmp(argv[i], "-d")) {
            log_messages.set_debug_level(atoi(argv[++i]));
        } else if (!strcmp(argv[i], "-mod")) {
            mod_n = atoi(argv[++i]);
            mod_i = atoi(argv[++i]);
            do_mod = true;
        } else if (!strcmp(argv[i], "-sleep_interval")) {
            sleep_interval = atoi(argv[++i]);
        }
    }
    if (!one_pass) check_stop_daemons();

    retval = config.parse_file();
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "Can't parse config.xml: %s\n", boincerror(retval));
        exit(1);
    }

    sprintf(path, "%s/upload_private", config.key_dir);
    retval = read_key_file(path, key);
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "can't read key\n");
        exit(1);
    }

    log_messages.printf(MSG_NORMAL, "Starting\n");

    install_stop_signal_handler();

    main_loop();
}

const char *BOINC_RCSID_be98c91511 = "$Id: transitioner.cpp 18617 2009-07-17 16:13:51Z davea $";
