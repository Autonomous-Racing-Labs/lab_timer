#ifndef CARCOM_H_
#define CARCOM_H_

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#include <unordered_map>

#define SERVER_NAME "http://10.134.137.%d:8000/webhook"

enum car_state
{
    NOT_AVAILABLE,
    NOT_READY_TO_RACE,
    GETTING_READY_TO_RACE,
    READY_TO_RACE,
    DRIVING_VERY_FAST
};

typedef struct
{
    uint16_t id;
    car_state current_status;
    int16_t current_position;
    char * server_name;
} car_struct;

typedef struct
{
    bool get_ready;
    bool start_race;
    bool abort;
} request_payload;

class CarCom
{
public:
    void handle_car_com();
    CarCom(); // Konstruktor
    ~CarCom();

    void add_car_id(uint16_t car_id);

    void request_ready_for_start();
    void start_race();
    void send_cancel_request();

    car_state get_status(uint16_t car_id);
    int16_t get_current_position(uint16_t car_id);
    bool is_race_aborted();
    bool are_cars_ready();

    void reset();
    // static void goal_request_callback(rclc_action_goal_handle_t * goal_handle, bool accepted, void * context);

    // static void feedback_callback(rclc_action_goal_handle_t * goal_handle, void * ros_feedback, void * context);

    // static void result_request_callback(
    // rclc_action_goal_handle_t * goal_handle, void * ros_result_response,
    // void * context);

    // static void cancel_request_callback(
    // rclc_action_goal_handle_t * goal_handle, bool cancelled,
    // void * context);

private:
    WebServer server;

    const char *ssid;
    const char *password;

    // Set your Static IP address
    IPAddress local_IP;
    // Set your Gateway IP address
    IPAddress gateway;

    IPAddress subnet;


    std::unordered_map<uint16_t, car_struct *> car_dict;

    void send_request(const request_payload &payload);
    String request_struct_to_string(const request_payload &payload);

    // Variable to store the HTTP request
    String header;

    void handle_webhook();
};

#endif