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


void init_uros();


extern int num_of_action_clients;

class RosComm {
public:
    RosComm(const char *action_name, size_t ros_domain);                  // Konstruktor
    ~RosComm();

    void request_ready_for_start();
    bool is_ready_for_start();
    void run_action();
    void start_race();
    void send_cancel_request();

    int32_t get_current_position();
    bool is_race_aborted();


private:
    static void goal_request_callback(rclc_action_goal_handle_t * goal_handle, bool accepted, void * context);
    
    static void feedback_callback(rclc_action_goal_handle_t * goal_handle, void * ros_feedback, void * context);
    
    static void result_request_callback(
    rclc_action_goal_handle_t * goal_handle, void * ros_result_response,
    void * context);

    static void cancel_request_callback(
    rclc_action_goal_handle_t * goal_handle, bool cancelled,
    void * context);


    const char *current_action_name;

    bool goal_completed;
    bool request_accepted;
    int32_t curr_position;

    rcl_allocator_t allocator;
    rclc_support_t support;
    rcl_node_t node;
    rclc_action_client_t action_client;
    rclc_executor_t executor;

    rclc_action_goal_handle_t *current_goal_handle;

    race_action_interface__action__Race_FeedbackMessage ros_feedback;
    race_action_interface__action__Race_GetResult_Response ros_result_response;
    race_action_interface__action__Race_SendGoal_Request ros_goal_request;

};



#endif