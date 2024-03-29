/***********************************************************************************
 * File: ref_sys.cpp
 * 
 * Author: Alex Mueller
 * Date: 20221113
 * 
 * Description: This is a implementation file for functions relating to the ref_sys class
 *              and its methods. It is meant to be implemented with a Robomaster referee system.
 * *********************************************************************************/

#include <Arduino.h>
#include "ref_sys.h"

/***********************************************************************************
 * Function: ref_sys()
 * Description:
 *      This is a default constructor for the ref_sys class. Currently it only begins Serial2 transmission.
 * Parameters: N/A
 * Return: N/A
 ************************************************************************************/

ref_sys::ref_sys() {

    Serial2.begin(115200);
}

/***********************************************************************************
 * Function: read_serial()
 * Description:
 *      This is a function mean to decode the incoming serial transmission (UART). Takes in information
 *      byte by byte and then splits it up depending on meaning. It then assigns values to members of the 
 *      ref_data struct.
 * Parameters: N/A
 * Return: true when the Serial transmission successfully gets decoded.
 ************************************************************************************/

bool ref_sys::read_serial(){

    byte enter_code, temp;          //These lines initialize all our temporary variables
    uint16_t data_length, cmd_id;  
    uint8_t seq, crc, comp_stat, warning_level;
    uint32_t temp_var_32;

    Serial.println("in read serial");

    // ref_sys curr_ref;           //Setting up an instance of our ref_sys class
   
    // put your main code here, to run repeatedly:
    while(Serial2.available()==0){Serial.println("in loop not available loop");};
    if(Serial2.available()>=1){      //The first instance to check if serial data is available

        bool gotStartByte = false;
        //Serial.println("Serial available");

        while(Serial2.available()>=1){
            enter_code = Serial2.read();    //It will read in the first byte of data
            Serial.println(enter_code);
            
            if(enter_code == 0xA5){         //It looks for this value that signifies that there is about to be a transmission of data

                gotStartByte = true;

                //Serial.println("Enter code received");

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                data_length = temp_var_32;

                //Serial.println("After the first read combine");

                //Serial.println(data_length);

                temp_var_32 = read_combine_n_bytes(temp_var_32,1);
                seq = temp_var_32;

                
                temp_var_32 = read_combine_n_bytes(temp_var_32,1);
                crc = temp_var_32;

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                cmd_id = temp_var_32;

                Serial.println("\n");
                Serial.println(cmd_id, HEX);
                Serial.println("\n");

                if(cmd_id == 1){  //stage 1

                //Serial.println("received cmd_id inside 1"); 

                Serial2.readBytes(&temp, 1);       //This waits till another byte of data is available

                comp_stat = temp;     //Reading in a byte of data and bit shifting it 8 bits to the left
                comp_stat = comp_stat &= 0b00000111;

                if(comp_stat == 0){
                    run_data.curr_stage = 'P';  //pre comp stage
                    //Serial.println("pre comp");
                }

                else if(comp_stat == 1){
                    run_data.curr_stage = 'S';   //Setup
                }

                else if(comp_stat == 2){
                    run_data.curr_stage = 'I';    //Init stage
                }

                else if(comp_stat == 3){
                    run_data.curr_stage = 'F';   //5 sec countdown
                }

                else if(comp_stat == 4){
                    run_data.curr_stage = 'C';   //In combat
                }

                else if(comp_stat == 5){
                    run_data.curr_stage = 'R';   //calc comp results
                }

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                run_data.rem_time = temp_var_32;

                return true;
                                    
                }else if(cmd_id == 3){   //everyone hp

                    //Serial.println("received cmd_id inside 3"); 

                Serial2.readBytes(&temp, 1);        //This waits till another byte of data is available
                Serial2.readBytes(&temp, 1);        //This waits till another byte of data is available
                /////////////////////////////////////////////////////////////////
                Serial2.readBytes(&temp, 1);        //This waits till another byte of data is available
                Serial2.readBytes(&temp, 1);        //This waits till another byte of data is available
                ///////////////////////////////////////////////////////////////
                Serial2.readBytes(&temp, 1);        //This waits till another byte of data is available
                Serial2.readBytes(&temp, 1);        //This waits till another byte of data is available
                ///////////////////////////////////////////////////////////////
                Serial2.readBytes(&temp, 1);
                Serial2.readBytes(&temp, 1);
                ///////////////////////////////////////////////////////////////
                Serial2.readBytes(&temp, 1);
                Serial2.readBytes(&temp, 1);
                //////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                run_data.red_sentry_hp = temp_var_32;

                ///////////////////////////////////////////////////////////////
                Serial2.readBytes(&temp, 1);
                Serial2.readBytes(&temp, 1);
                ///////////////////////////////////////////////////////////////
                Serial2.readBytes(&temp, 1);
                Serial2.readBytes(&temp, 1);
                //////////////////////////////////////////////////////////////////
                Serial2.readBytes(&temp, 1);        //This waits till another byte of data is available
                Serial2.readBytes(&temp, 1);        //This waits till another byte of data is available
                /////////////////////////////////////////////////////////////////
                Serial2.readBytes(&temp, 1);
                Serial2.readBytes(&temp, 1);
                ///////////////////////////////////////////////////////////////
                Serial2.readBytes(&temp, 1);        //This waits till another byte of data is available
                Serial2.readBytes(&temp, 1);        //This waits till another byte of data is available
                ///////////////////////////////////////////////////////////////
                Serial2.readBytes(&temp, 1);
                Serial2.readBytes(&temp, 1);
                ///////////////////////////////////////////////////////////////
                Serial2.readBytes(&temp, 1);
                Serial2.readBytes(&temp, 1);

                //////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                run_data.blue_sentry_hp = temp_var_32;
               
                ///////////////////////////////////////////////////////////////
                return true;

                }else if(cmd_id == 260){ //ref warning

                //Serial.println("received cmd_id inside 260"); 


                while(Serial2.readBytes(&temp, 1) != 1){  
                }        //This waits till another byte of data is available

                warning_level = temp;

                if(warning_level == 1){

                    run_data.ref_warning = 'Y';
                    
                }else if(warning_level == 2){

                    run_data.ref_warning = 'R';
                    
                }else if(warning_level == 3){

                    run_data.ref_warning = 'F';

                }

                temp_var_32 = read_combine_n_bytes(temp_var_32,1);

                run_data.foul_robot_id = temp_var_32;

                return true;

                }else if(cmd_id == 201){ //robo stat

                //Serial.println("received cmd_id inside 201"); 

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,1);

                run_data.curr_robot_id = temp_var_32;

                temp_var_32 = read_combine_n_bytes(temp_var_32,1);

                run_data.curr_robot_level = temp_var_32;

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                run_data.curr_robot_hp = temp_var_32;

                ////////////////////////////////////////////////////////////////////////////
                
                while(Serial2.readBytes(&temp, 1) != 1){
                }        //This waits till another byte of data is available

                while(Serial2.readBytes(&temp, 1) != 1){                            //skipping two bytes of data
                }        //This waits till another byte of data is available

                ////////////////////////////////////////////////////////////////////////////
                
                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                run_data.curr_robot_17_cool_val = temp_var_32;

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                run_data.curr_robot_17_heat_lim = temp_var_32;

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                run_data.curr_robot_17_speed_lim = temp_var_32;

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                if(run_data.curr_robot_id  ==  3 ||run_data.curr_robot_id  ==  4 ||run_data.curr_robot_id  ==  5 ||run_data.curr_robot_id  ==  7 ||
                run_data.curr_robot_id  ==  103 ||run_data.curr_robot_id  ==  104 ||run_data.curr_robot_id  ==  105 ||run_data.curr_robot_id  ==  107){
                
                run_data.second_wpn_cool_val = temp_var_32;

                }

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                if(run_data.curr_robot_id  ==  3 ||run_data.curr_robot_id  ==  4 ||run_data.curr_robot_id  ==  5 ||run_data.curr_robot_id  ==  7 ||
                run_data.curr_robot_id  ==  103 ||run_data.curr_robot_id  ==  104 ||run_data.curr_robot_id  ==  105 ||run_data.curr_robot_id  ==  107){
                
                run_data.second_wpn_heat_lim = temp_var_32;

                }

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                if(run_data.curr_robot_id  ==  3 ||run_data.curr_robot_id  ==  4 ||run_data.curr_robot_id  ==  5 ||run_data.curr_robot_id  ==  7 ||
                run_data.curr_robot_id  ==  103 ||run_data.curr_robot_id  ==  104 ||run_data.curr_robot_id  ==  105 ||run_data.curr_robot_id  ==  107){
                
                run_data.second_wpn_speed_lim = temp_var_32;

                }

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                if(run_data.curr_robot_id  ==  1 ||run_data.curr_robot_id  ==  101){

                run_data.second_wpn_cool_val = temp_var_32;

                }

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                if(run_data.curr_robot_id  ==  1 ||run_data.curr_robot_id  ==  101){

                run_data.second_wpn_heat_lim = temp_var_32;

                }

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                if(run_data.curr_robot_id  ==  1 ||run_data.curr_robot_id  ==  101){

                run_data.second_wpn_speed_lim = temp_var_32;

                }

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                run_data.robot_power_lim = temp_var_32;

                /////////////////////////////////////////////////////////////////////////////

                return true;
              
                }
                else if(cmd_id == 514){   //power, current and voltage   
                    
                //Serial.println("received cmd_id, inside 514"); 

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                run_data.chasis_volt = temp_var_32;    

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                run_data.chasis_current = temp_var_32;

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,4);

                run_data.chasis_power = temp_var_32;

                /////////////////////////////////////////////////////////////////
                while(Serial2.readBytes(&temp, 1) != 1){
                }        

                while(Serial2.readBytes(&temp, 1) != 1){                               //skipping 2 bytes of data
                }        

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                run_data.curr_robot_barr_heat = temp_var_32;

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

               if(run_data.curr_robot_id  ==  3 ||run_data.curr_robot_id  ==  4 ||run_data.curr_robot_id  ==  5 ||run_data.curr_robot_id  ==  7 ||
                run_data.curr_robot_id  ==  103 ||run_data.curr_robot_id  ==  104 ||run_data.curr_robot_id  ==  105 ||run_data.curr_robot_id  ==  107){
                
                run_data.curr_robot_second_barr_heat = temp_var_32;
                }

                ////////////////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,2);

                if(run_data.curr_robot_id  ==  1 ||run_data.curr_robot_id  ==  101){

                run_data.curr_robot_second_barr_heat = temp_var_32;

                }

                ////////////////////////////////////////////////////////////////////////////

                return true;
                                    
                }else if(cmd_id == 519){  //RT launch info

                //Serial.println("received cmd_id inside 519"); 

                ///////////////////////////////////////////////////////////////////

                while(Serial2.readBytes(&temp, 1) != 1){ 
                }        //This waits till another byte of data is available        //Skipping 1 byte of data

                ///////////////////////////////////////////////////////////////////

                while(Serial2.readBytes(&temp, 1) != 1){ 
                }        //This waits till another byte of data is available

                temp = temp >> 8;
                comp_stat = temp & 0b00000111;

                if(comp_stat ==  1){

                    run_data.launch_id = 1;

                }else if(comp_stat == 2){

                    run_data.launch_id = 2;

                }else if(comp_stat ==3){

                    run_data.launch_id = 3;

                }

                ///////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,1);

                run_data.launch_freq = temp_var_32;

                //////////////////////////////////////////////////////////////////

                temp_var_32 = read_combine_n_bytes(temp_var_32,4);

                /////////////////////////////////////////////////////////////////

                run_data.launch_speed = temp_var_32;

                return true;
                
                }
        
        }

        }
        
  
  }
}

/***********************************************************************************
 * Function: read_combine_n_bytes()
 * Description:
 *      This function takes in a reference to a 32 bit variable and reads n numbers of bytes from
 *      the serial 2 FIFO, splices the individual bytes together, and then assigns it to the
 *      int passed in via reference.
 * Parameters: A reference to the 32 bit pointer that we want to fill and the integer value representing
 *             how many bytes we need to read.
 * Return: N/A
 ************************************************************************************/

uint32_t ref_sys::read_combine_n_bytes(uint32_t func_input, int n){

    uint8_t temp;
    func_input = 0;


    Serial2.readBytes(&temp, 1);       //This waits till another byte of data is available

        func_input = func_input | temp;
 
    if(n != 1){
        Serial2.readBytes(&temp, 1);       //This waits till another byte of data is available

        func_input = (func_input<<8) | temp;
    }   
    if(n != 1&&n != 2){
        Serial2.readBytes(&temp, 1);        //This waits till another byte of data is available

        func_input = (func_input<<8) | temp;
    }        
    if(n != 1&&n != 2&&n != 3){
        Serial2.readBytes(&temp, 1);        //This waits till another byte of data is available

        func_input = (func_input<<8) | temp;
    }      

    return func_input;
}

/***********************************************************************************
 * Function: hid_buff_write()
 * Description:
 *      This function dumps the values from our run_data struct into the HID buffer.
 * Parameters: A 64 byte array for testing purposes (will change when other teams implement HID official)
 * Return: true once the function finishes
 ************************************************************************************/

bool ref_sys::hid_buff_write(byte arr[]){

    arr[0] = 3;
    arr[1] = run_data.curr_stage;
    arr[2] = run_data.rem_time;
    arr[3] = run_data.ref_warning;
    arr[4] = run_data.blue_sentry_hp;
    arr[5] = run_data.red_sentry_hp;
    arr[6] = run_data.foul_robot_id;
    arr[7] = run_data.curr_robot_id;
    arr[8] = run_data.curr_robot_level;
    arr[9] = run_data.curr_robot_hp;
    arr[10] = run_data.curr_robot_17_cool_val;
    arr[11] = run_data.curr_robot_17_heat_lim;
    arr[12] = run_data.second_wpn_cool_val;
    arr[13] = run_data.second_wpn_heat_lim;
    arr[14] = run_data.second_wpn_speed_lim;
    arr[15] = run_data.chasis_volt;
    arr[16] = run_data.chasis_current;
    arr[17] = run_data.robot_power_lim;
    arr[18] = run_data.chasis_power;
    arr[19] = run_data.curr_robot_barr_heat;
    arr[20] = run_data.curr_robot_second_barr_heat;
    arr[21] = run_data.launch_id;
    arr[22] = run_data.launch_speed;
    arr[23] = run_data.launch_freq;

    return true;
}
