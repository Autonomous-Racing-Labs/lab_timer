#include <uros_action.h>

char ssid[] = "ITSE Lab";
char psk[] = "ITSELaboratory";

IPAddress agent_ip(10, 134, 137, 227);
size_t agent_port = 8888;

rcl_allocator_t allocator;
rclc_support_t support;
rcl_node_t node;
rclc_executor_t executor;

std::vector<action_client_s*> action_clients_v;

int num_of_action_clients = 0;

void init_uros(size_t ros_domain)
{
    set_microros_wifi_transports(ssid, psk, agent_ip, agent_port);

    allocator = rcl_get_default_allocator();

    // create init_options
    rcl_init_options_t init_ops = rcl_get_zero_initialized_init_options();
    RCCHECK(rcl_init_options_init(&init_ops, allocator));
    RCCHECK(rcl_init_options_set_domain_id(&init_ops, ros_domain));

    RCCHECK(rclc_support_init_with_options(&support, 0, NULL, &init_ops , &allocator));

    // create node
    node = rcl_get_zero_initialized_node();
    RCCHECK(rclc_node_init_default(&node, "race_action_node", "", &support));

}

void run_uros(){
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
    usleep(100000);
}

void finish_init(){
    std::for_each(action_clients_v.begin(), action_clients_v.end(),
        [](action_client_s * client){
            // Create action client
            RCCHECK(
                rclc_action_client_init_default(
                    client->_client,
                    &node,
                    ROSIDL_GET_ACTION_TYPE_SUPPORT(race_action_interface, Race),
                    client->_action_name));
        }
    );

    // Create executor
    rclc_executor_init(&executor, &support.context, 1, &allocator);

    std::for_each(action_clients_v.begin(), action_clients_v.end(),
        [](action_client_s * client){
        RCCHECK(
            rclc_executor_add_action_client(
                &executor,
                client->_client,
                10,
                client->_ros_result_response,
                client->_ros_feedback,
                RosComm::goal_request_callback,
                RosComm::feedback_callback,
                RosComm::result_request_callback,
                RosComm::cancel_request_callback,
                (void *)client->p_instance)
                
            );
        }
    );

}

RosComm::RosComm(const char *action_name) : goal_completed(false), request_accepted(false), curr_position(-1)
{
    this_client = {
        action_name, 
        &action_client, 
        &ros_result_response,
        &ros_feedback,    
        this};
    action_clients_v.push_back(&this_client);

    Serial.println(num_of_action_clients);

    // Allocate msg memory
    ros_feedback.feedback.curr_positon = 0;

    ros_result_response.result.end_status = 0;

    num_of_action_clients += 1;
}

RosComm::~RosComm()
{
    RCCHECK(rclc_action_client_fini(&action_client, &node))
    num_of_action_clients -= 1;
    if (num_of_action_clients == 0)
    {
        RCCHECK(rcl_node_fini(&node))
    }
}

void RosComm::request_ready_for_start()
{
    // Send goal request
    ros_goal_request.goal.race_type = 1;
    RCCHECK(rclc_action_send_goal_request(&action_client, &ros_goal_request, NULL));
}

void RosComm::send_cancel_request(){
    rclc_action_send_cancel_request(current_goal_handle);
}

bool RosComm::is_ready_for_start()
{
    return request_accepted;
}

int32_t RosComm::get_current_position(){
    return curr_position;
}

bool RosComm::is_race_aborted(){
    return goal_completed;
}

void RosComm::start_race(){
    RCCHECK(rclc_action_send_result_request(current_goal_handle));
}

void RosComm::goal_request_callback(rclc_action_goal_handle_t *goal_handle, bool accepted, void *context)
{
    (void)context;
    RosComm *self = static_cast<RosComm *>(context);
    self->current_goal_handle = goal_handle;

    race_action_interface__action__Race_SendGoal_Request *request =
        (race_action_interface__action__Race_SendGoal_Request *)goal_handle->ros_goal_request;
    Serial.printf(
        "%s, Goal request (race_type: %d): %s\n", 
        self->current_action_name,
        request->goal.race_type,
        accepted ? "Accepted" : "Rejected");

    self->request_accepted = accepted;

    if (!accepted)
    {
        self->goal_completed = true;
    }
}

void RosComm::feedback_callback(rclc_action_goal_handle_t *goal_handle, void *ros_feedback, void *context)
{
    (void)context;
    RosComm *self = static_cast<RosComm *>(context);

    race_action_interface__action__Race_SendGoal_Request *request =
        (race_action_interface__action__Race_SendGoal_Request *)goal_handle->ros_goal_request;

    Serial.printf(
        "%s, Goal Feedback (race_type: %d)",
        self->current_action_name,
        request->goal.race_type);

    race_action_interface__action__Race_FeedbackMessage *feedback =
        (race_action_interface__action__Race_FeedbackMessage *)ros_feedback;

    int32_t res = feedback->feedback.curr_positon;
    self->curr_position = res;
    Serial.printf("Feedback: %d\n", res);
}

void RosComm::result_request_callback(
    rclc_action_goal_handle_t *goal_handle, void *ros_result_response,
    void *context)
{
    (void)context;
    RosComm *self = static_cast<RosComm *>(context);

    race_action_interface__action__Race_SendGoal_Request *request =
        (race_action_interface__action__Race_SendGoal_Request *)goal_handle->ros_goal_request;

    Serial.printf(
        "%s, Goal Result (race_type: %d) [",
        self->current_action_name,
        request->goal.race_type);

    race_action_interface__action__Race_GetResult_Response *result =
        (race_action_interface__action__Race_GetResult_Response *)ros_result_response;

    if (result->status == GOAL_STATE_SUCCEEDED)
    {
        Serial.printf("%d ", result->result.end_status);
    }
    else if (result->status == GOAL_STATE_CANCELED)
    {
        Serial.printf("CANCELED ");
    }
    else
    {
        Serial.printf("ABORTED ");
    }

    Serial.printf("\b]\n");

    self->goal_completed = true;
}

void RosComm::cancel_request_callback(
    rclc_action_goal_handle_t *goal_handle, bool cancelled,
    void *context)
{
    (void)context;
    RosComm *self = static_cast<RosComm *>(context);

    race_action_interface__action__Race_SendGoal_Request *request =
        (race_action_interface__action__Race_SendGoal_Request *)goal_handle->ros_goal_request;

    Serial.printf(
        "%s, Goal cancel request (race_type: %d): %s\n",
        self->current_action_name,
        request->goal.race_type,
        cancelled ? "Accepted" : "Rejected");

    if (cancelled)
    {
        self->goal_completed = true;
    }
}