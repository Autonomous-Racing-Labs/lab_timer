#include "car_com.h"

CarCom::CarCom() : // init variables
                   server(8000),

                   ssid("ITSE Lab"),
                   password("ITSELaboratory"),

                   // Set your Static IP address
                   local_IP(10, 134, 137, 20),
                   // Set your Gateway IP address
                   gateway(10, 134, 137, 1),

                   subnet(255, 255, 0, 0)

{
    // Configures static IP address
    if (!WiFi.config(local_IP, gateway, subnet))
    {
        Serial.println("STA Failed to configure");
    }
    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    unsigned long timeout = 2000;
    unsigned long start_try = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        if(timeout < millis()-start_try){
            esp_restart();
        }
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());

    Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");

    server.on("/webhook", HTTP_POST, [&]() {
        handle_webhook();
    });

    server.begin();
}

void CarCom::handle_car_com(){
    server.handleClient();
}

void CarCom::handle_webhook()
{
    server.send(200, "text/plain", "ok"); // Send response back to the sender

    String payload = server.arg("plain"); // Extract payload from the request
    // Process the payload as needed
    Serial.println("Received payload: " + payload);


    // JSON-Dokument erstellen (anpassen der Größe nach Bedarf)
    DynamicJsonDocument doc(1024);

    // JSON-Nachricht parsen
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }

    // Id extrahieren (muss immer vorhanden sein)
    uint16_t id = doc["car_id"];
    car_struct * car = car_dict[id];

    // Beispielhafte Verarbeitung der Keys, falls vorhanden
    if (doc.containsKey("ready")) {
        bool is_ready = doc["ready"];
        if(is_ready == true){
            car->current_status = READY_TO_RACE;
        }else{
            car->current_status = NOT_AVAILABLE;
        }
    }

    if (doc.containsKey("track_position")) {
        int track_pos = doc["track_position"];
        car->current_position = track_pos;
    }

    if (doc.containsKey("abort")) {
        bool is_canceled = doc["abort"];
        if(is_canceled == true)
            car->current_status = NOT_READY_TO_RACE;
    }

}

void CarCom::add_car_id(uint16_t car_id)
{
    if(car_id == -1){
        return;
    }
    car_struct *p_new_car = new car_struct;

    p_new_car->current_position = 0;
    p_new_car->id = car_id;
    p_new_car->current_status = NOT_AVAILABLE;

    int buf_size = 50;
    char server_name[buf_size];
    int retVal = snprintf(server_name, buf_size, SERVER_NAME, car_id);

    if (retVal > 0 && retVal < buf_size)
    {
        // Allocate memory for the server name and copy the string
        p_new_car->server_name = new char[retVal + 1];
        strcpy(p_new_car->server_name, server_name);
    }
    else
    {
        // Handle error, maybe set p_new_car->server_name to nullptr or some default value
        p_new_car->server_name = nullptr;
    }

    car_dict[car_id] = p_new_car;
}
void CarCom::reset(){
        std::for_each(car_dict.begin(), car_dict.end(),
                  [&](const std::pair<const uint16_t, car_struct *> &entry) {
                      car_struct *car = entry.second;
                        car->current_position = 0;
                        car->current_status = NOT_AVAILABLE;
                  });
}
void CarCom::request_ready_for_start()
{
    request_payload payload = {
        true,  // get_ready
        false, // start_race
        false, // abort
    };
    send_request(payload);
}

void CarCom::start_race()
{
    request_payload payload = {
        false, // get_ready
        true,  // start_race
        false, // abort
    };
    send_request(payload);
}

void CarCom::send_cancel_request()
{
    request_payload payload = {
        false, // get_ready
        false, // start_race
        true,  // abort
    };
    send_request(payload);
}

String CarCom::request_struct_to_string(const request_payload &payload) {
    // Create a JSON document
    DynamicJsonDocument doc(200);

    // Add the values to the JSON document
    doc["get_ready"] = payload.get_ready;
    doc["go"] = payload.start_race;
    doc["abort"] = payload.abort;

    // Serialize the JSON document to a string
    String jsonString;
    serializeJson(doc, jsonString);

    // Return the JSON string
    return jsonString;
}

void CarCom::send_request(const request_payload &payload)
{
    std::for_each(car_dict.begin(), car_dict.end(),
                  [&](const std::pair<const uint16_t, car_struct *> &entry) {
                      car_struct *car = entry.second;
                      
                      if(payload.start_race == true && car->current_status != READY_TO_RACE){
                        return;
                      }
                      WiFiClient client;
                      HTTPClient http;

                      // Your Domain name with URL path or IP address with path
                      http.begin(client, car->server_name);
                      // Specify content-type header
                      http.addHeader("Content-Type", "application/json");

                      // Data to send with HTTP POST
                      String httpRequestData = request_struct_to_string(payload);
                      // Send HTTP POST request
                      http.setTimeout(0);
                      int httpResponseCode = http.POST(httpRequestData);

                      Serial.print("HTTP Response code: ");
                      Serial.println(httpResponseCode);

                      // Free resources
                      http.end();
                  });
}

car_state CarCom::get_status(uint16_t car_id)
{
    car_struct *p_car_of_interest = car_dict[car_id];
    return p_car_of_interest->current_status;
}

int16_t CarCom::get_current_position(uint16_t car_id)
{
    car_struct *p_car_of_interest = car_dict[car_id];
    return p_car_of_interest->current_position;
}

bool CarCom::are_cars_ready(){
    return std::any_of(car_dict.begin(), car_dict.end(),
                       [](const std::pair<const uint16_t, car_struct *> &entry) {
                           car_struct *car = entry.second;
                           return car->current_status == READY_TO_RACE;
                       });
}

bool CarCom::is_race_aborted()
{
    return std::any_of(car_dict.begin(), car_dict.end(),
                       [](const std::pair<const uint16_t, car_struct *> &entry) {
                           car_struct *car = entry.second;
                           return car->current_status == NOT_READY_TO_RACE;
                       });
    return false;
}