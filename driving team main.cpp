
#define USING_USB_CABLE //comment out when not using USB Cable
#include "mbed.h"
#include <cstdio>
#include <Ticker.h>

// Constants
#define sample_time 0.001f          // 1kHz sample time
#define KP 2.665f            // Proportional gain
#define KI 1.674f             // Integral gain
#define Vbat 9.0f           // Battery voltage
#define targetSpeed 0.3f // Target wheel speed in rad/sec

AnalogIn feedback_sensor(A1);     // Sensor reading feedback
AnalogIn referance_sensor(A3);    // User input for reference signal (via potentiometer)
PwmOut control_output(D6);        // PWM output to control system

float output = 0;
float bound_output = 0;
float current_speed = 0;
float error_current = 0;

// Function to calculate the integral term using Euler forward approximation
float calculateIntegral(float error_current, float& area_prior) {
   // Integral approximation: area_current = area_prior + (error_current * DT)
   float area_current = area_prior + (error_current * sample_time);
   
   // Update the area prior variable for the next iteration
   area_prior = area_current;
   
   return area_current;
}

// Function to calculate the derivative term using a first-order Taylor series approximation
float calculateDerivative(float error_current, float error_prior) {
   // Derivative approximation: (error_current - error_prior) / DT
   return (error_current - error_prior) / sample_time;
}

// Function to calculate the PID output
float calculatePIDOutput(float error_current, float error_prior, float& area_prior, float kp, float ki) {
   // Calculate integral and derivative terms
   float area_current = calculateIntegral(error_current, area_prior);
   float error_change = calculateDerivative(error_current, error_prior);
   
   // PID control action: 
   float action = (kp * error_current) + (ki * area_current);
   
   return action;
}

// Main control function called by the ticker
void controlUpdate() {
   // Static variables to store values between function calls
   static float area_prior = 0.0f;
   static float error_prior = 0.0f;
   
   // Read sensor feedback and user-defined reference
   current_speed = feedback_sensor.read();
   float reference = targetSpeed;
   
   // Calculate the current error
   error_current = reference - current_speed;
   
   // Calculate the PID controller output
   output = calculatePIDOutput(error_current, error_prior, area_prior, KP, KI);
   
   // Bound the output to the PWM range [0.0, 1.0]
   bound_output = (fmax(0.0f, fmin(output, 1.0f)));
   
   // Set the PWM duty cycle
   control_output.write(bound_output);
   
   // Update prior error for the next iteration
   error_prior = error_current;
}

int main() {


   while (true) {
    controlUpdate();
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"); 
    printf("PI Output: %f\n", output); 
    printf("Bounded Output: %f\n", bound_output);  
    printf("Measured Speed: %f\n", current_speed);
    printf("Error: %f\n", error_current); 
 
    }
}
