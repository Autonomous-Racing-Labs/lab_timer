#ifndef UROSACTION_H_
#define UROSACTION_H_

#include <Arduino.h>
#include <micro_ros_platformio.h>

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <race_action_interface/action/race.h>

#define RCCHECK(fn) { \
    rcl_ret_t temp_rc = fn; \
    if ((temp_rc != RCL_RET_OK)) { \
        Serial.printf( \
            "Failed status on line %d: %d. Aborting.\nFehler bei der Initialisierung: %s\n", \
            __LINE__, \
            (int)temp_rc, \
            rcl_get_error_string().str \
        ); \
        delay(100); \
        return; \
    } \
}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){Serial.printf( \
        "Failed status on line %d: %d. Continuing.\n", __LINE__, (int)temp_rc);}}


void init_uros(size_t ros_domain);
void run_uros();
void finish_init();

extern int num_of_action_clients;

extern rcl_allocator_t allocator;
extern rclc_support_t support;
extern rcl_node_t node;
extern rclc_executor_t executor;

class RosComm;

struct action_client_s
{
    const char* _action_name;
    rclc_action_client_t* _client;
    race_action_interface__action__Race_GetResult_Response* _ros_result_response;
    race_action_interface__action__Race_FeedbackMessage* _ros_feedback;
    RosComm* p_instance;
};

extern std::vector<action_client_s*> action_clients_v;

class RosComm {
public:
    RosComm(const char *action_name);                  // Konstruktor
    ~RosComm();

    void request_ready_for_start();
    bool is_ready_for_start();
    void start_race();
    void send_cancel_request();

    int32_t get_current_position();
    bool is_race_aborted();

    static void goal_request_callback(rclc_action_goal_handle_t * goal_handle, bool accepted, void * context);
    
    static void feedback_callback(rclc_action_goal_handle_t * goal_handle, void * ros_feedback, void * context);
    
    static void result_request_callback(
    rclc_action_goal_handle_t * goal_handle, void * ros_result_response,
    void * context);

    static void cancel_request_callback(
    rclc_action_goal_handle_t * goal_handle, bool cancelled,
    void * context);

private:
    const char *current_action_name;

    action_client_s this_client;
    bool goal_completed;
    bool request_accepted;
    int32_t curr_position;

    rclc_action_client_t action_client;

    rclc_action_goal_handle_t *current_goal_handle;

    race_action_interface__action__Race_FeedbackMessage ros_feedback;
    race_action_interface__action__Race_GetResult_Response ros_result_response;
    race_action_interface__action__Race_SendGoal_Request ros_goal_request;

};


#endif